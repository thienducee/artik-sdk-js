/*
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 */

#include "coap/coap.h"

#include <unistd.h>
#include <stdint.h>
#include <artik_log.h>

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>

#include "base/ssl_config_converter.h"

static std::array<const char*, 4> coap_msg_type = {
  "CON",
  "NON",
  "ACK",
  "RST"
};

int n = 1;

#define ARCH_LITTLE_ENDIAN (*(char *)&n == 1)

static void swap_bytes(unsigned int *val, int length) {
  if (length == 1 || length == 0)
    return;
  else if (length == 2)
    *val = ((((*val >> 8) & 0x00FF) | ((*val << 8) & 0xFF00)));
  else if (length == 3)
    *val = (((*val >> 16) & 0x0000FF) | (*val & 0x00FF00) |
      (*val << 16 & 0xFF0000));
  else
    *val = ((((*val) >> 24) & 0x000000FF) | (((*val) >> 8) & 0x0000FF00) |
      (((*val) << 8) & 0x00FF0000) | (((*val) << 24) & 0xFF000000));
}

namespace artik {

using v8::ArrayBuffer;
using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Integer;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Array;
using v8::Handle;
using v8::HandleScope;
using v8::Int32;
using v8::Context;
using Nan::MaybeLocal;

Persistent<Function> CoapWrapper::constructor;

Resource::Resource() {
  this->m_get_resource_cb = NULL;
  this->m_post_resource_cb = NULL;
  this->m_put_resource_cb = NULL;
  this->m_delete_resource_cb = NULL;
}

void Resource::setGetResourceCb(Isolate *isolate, Local<Value> cb) {
  this->m_get_resource_cb = new Persistent<Function>();
  this->m_get_resource_cb->Reset(isolate, Local<Function>::Cast(cb));
}

void Resource::setPostResourceCb(Isolate *isolate, Local<Value> cb) {
  this->m_post_resource_cb = new Persistent<Function>();
  this->m_post_resource_cb->Reset(isolate, Local<Function>::Cast(cb));
}

void Resource::setPutResourceCb(Isolate *isolate, Local<Value> cb) {
  this->m_put_resource_cb = new Persistent<Function>();
  this->m_put_resource_cb->Reset(isolate, Local<Function>::Cast(cb));
}

void Resource::setDeleteResourceCb(Isolate *isolate, Local<Value> cb) {
  this->m_delete_resource_cb = new Persistent<Function>();
  this->m_delete_resource_cb->Reset(isolate, Local<Function>::Cast(cb));
}

Resource::~Resource() {
  if (this->m_get_resource_cb)
    delete(this->m_get_resource_cb);

  if (this->m_post_resource_cb)
    delete(this->m_post_resource_cb);

  if (this->m_put_resource_cb)
    delete(this->m_put_resource_cb);

  if (this->m_delete_resource_cb)
    delete(this->m_delete_resource_cb);
}

static Local<Object> coap_option_object(Isolate *isolate,
  artik_coap_option *coap_opt) {
  Local<Object> js_property = Object::New(isolate);

  js_property->Set(String::NewFromUtf8(isolate, "key"),
                      Integer::NewFromUnsigned(isolate, coap_opt->key));

  switch (coap_opt->key) {
  case ARTIK_COAP_OPTION_URI_HOST:
  case ARTIK_COAP_OPTION_LOCATION_PATH:
  case ARTIK_COAP_OPTION_URI_PATH:
  case ARTIK_COAP_OPTION_URI_QUERY:
  case ARTIK_COAP_OPTION_LOCATION_QUERY:
  case ARTIK_COAP_OPTION_PROXY_URI:
  case ARTIK_COAP_OPTION_PROXY_SCHEME:
    js_property->Set(String::NewFromUtf8(isolate, "data"),
                        String::NewFromUtf8(isolate,
                        reinterpret_cast<const char *>(coap_opt->data),
                        v8::String::NewStringType::kNormalString,
                        coap_opt->data_len - 1));
    break;
  case ARTIK_COAP_OPTION_IF_MATCH:
  case ARTIK_COAP_OPTION_ETAG:
    js_property->Set(String::NewFromUtf8(isolate, "data"),
                      Nan::NewBuffer(reinterpret_cast<char *>(coap_opt->data),
                      coap_opt->data_len).ToLocalChecked());
    break;
  case ARTIK_COAP_OPTION_CONTENT_FORMAT:
  case ARTIK_COAP_OPTION_URI_PORT:
  case ARTIK_COAP_OPTION_MAXAGE:
  case ARTIK_COAP_OPTION_ACCEPT:
  case ARTIK_COAP_OPTION_SIZE1:
  case ARTIK_COAP_OPTION_OBSERVE:
  case ARTIK_COAP_OPTION_BLOCK2: {
    unsigned int val = 0;

    memcpy(&val, coap_opt->data, coap_opt->data_len);

    js_property->Set(String::NewFromUtf8(isolate, "data"),
                      Integer::New(isolate, val));
    break;
  }
  default:
    break;
  }

  return js_property;
}

static Local<Object> coap_message_object(Isolate *isolate,
    const artik_coap_msg *coap_msg) {

  Local<Object> js_property = Object::New(isolate);
  std::string msg_type;
  std::vector<Local<Object>> options_vec;
  Local<Array> options_array;

  switch (coap_msg->msg_type) {
  case ARTIK_COAP_MSG_CON:
    msg_type = "CON";
    break;
  case ARTIK_COAP_MSG_NON:
    msg_type = "NON";
    break;
  case ARTIK_COAP_MSG_ACK:
    msg_type = "ACK";
    break;
  case ARTIK_COAP_MSG_RST:
    msg_type = "RST";
    break;
  }

  js_property->Set(String::NewFromUtf8(isolate, "msg_type"),
                      String::NewFromUtf8(isolate, msg_type.c_str()));

  js_property->Set(String::NewFromUtf8(isolate, "token"),
                      ArrayBuffer::New(isolate, coap_msg->token,
                                                coap_msg->token_len));

  js_property->Set(String::NewFromUtf8(isolate, "msg_id"),
                      Number::New(isolate, coap_msg->msg_id));

  js_property->Set(String::NewFromUtf8(isolate, "code"),
                      Integer::NewFromUnsigned(isolate, coap_msg->code));

  js_property->Set(String::NewFromUtf8(isolate, "data"),
                      ArrayBuffer::New(isolate, coap_msg->data,
                                                coap_msg->data_len));

  artik_coap_option *opt = coap_msg->options;
  artik_coap_option *end = opt + coap_msg->num_options;

  for (; opt != end; opt++) {
    Local<Object> option_js = coap_option_object(isolate, opt);

    options_vec.push_back(option_js);
  }

  options_array = Array::New(isolate, options_vec.size());

  for (unsigned int i = 0; i < options_vec.size(); i++)
    options_array->Set(i, options_vec.at(i));

  js_property->Set(String::NewFromUtf8(isolate, "options"), options_array);

  return js_property;
}

std::unique_ptr<artik_coap_psk_param> psk_params_converter(Isolate *isolate,
                    Local<Value> val) {
  std::unique_ptr<artik_coap_psk_param> psk_config(new artik_coap_psk_param);

  memset(psk_config.get(), 0, sizeof(artik_coap_psk_param));

  /* identity parameter */
  auto identity = js_object_attribute_to_cpp<Local<Value>>(val, "identity");

  if (identity) {
    if (!node::Buffer::HasInstance(identity.value())) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong definition of identity")));
      return nullptr;
    }
    char *val = reinterpret_cast<char *>(node::Buffer::Data(
      identity.value()));
    size_t len = node::Buffer::Length(identity.value());

    psk_config->identity = strndup(val, len);
  }

  /* psk parameter */
  auto psk = js_object_attribute_to_cpp<Local<Value>>(val,
    "psk");

  if (psk) {
    if (!node::Buffer::HasInstance(psk.value())) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong definition of psk")));
      return nullptr;
    }
    char *val = reinterpret_cast<char *>(node::Buffer::Data(psk.value()));
    size_t len = node::Buffer::Length(psk.value());

    psk_config.get()->psk = reinterpret_cast<char *>(malloc(len*sizeof(char*)));
    memcpy(psk_config->psk, val, len);
    psk_config->psk_len = len;
  }

  return psk_config;
}

std::unique_ptr<artik_coap_option> coap_option_converter(Isolate *isolate,
                    Local<Value> val) {
  std::unique_ptr<artik_coap_option> coap_opt(new artik_coap_option);

  memset(coap_opt.get(), 0, sizeof(artik_coap_option));

  log_dbg("");

  // key parameter
  auto key = js_object_attribute_to_cpp<unsigned int>(val, "key");

  if (key) {
    coap_opt->key = (artik_coap_option_key)key.value();
  } else {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate,
      "Key parameter missing")));
    return nullptr;
  }

  // data parameter
  auto data = js_object_attribute_to_cpp<Local<Value>>(val, "data");

  if (data) {
    Local<Value> data_val = data.value();
    switch (coap_opt->key) {
    case ARTIK_COAP_OPTION_URI_HOST:
    case ARTIK_COAP_OPTION_LOCATION_PATH:
    case ARTIK_COAP_OPTION_URI_PATH:
    case ARTIK_COAP_OPTION_URI_QUERY:
    case ARTIK_COAP_OPTION_LOCATION_QUERY:
    case ARTIK_COAP_OPTION_PROXY_URI:
    case ARTIK_COAP_OPTION_PROXY_SCHEME: {
      String::Utf8Value param0(data_val->ToString());
      coap_opt->data = reinterpret_cast<unsigned char *>(
                                            strndup(*param0, strlen(*param0)));
      coap_opt->data_len = strlen(*param0);
      break;
    }
    case ARTIK_COAP_OPTION_IF_MATCH:
    case ARTIK_COAP_OPTION_ETAG: {
      unsigned char *val = reinterpret_cast<unsigned char *>(node::Buffer::Data(
        data_val));
      size_t len = node::Buffer::Length(data_val);

      coap_opt.get()->data = reinterpret_cast<unsigned char *>(
                                          malloc(len*sizeof(unsigned char *)));
      memcpy(coap_opt->data, val, len);
      coap_opt->data_len = len;
      break;
    }
    case ARTIK_COAP_OPTION_CONTENT_FORMAT:
    case ARTIK_COAP_OPTION_URI_PORT:
    case ARTIK_COAP_OPTION_ACCEPT: {
      unsigned int val = data_val->Uint32Value();
      size_t len;

      if (val < UINT8_MAX) {
        len = 1;
      } else if (val > UINT8_MAX && val < UINT16_MAX) {
        len = 2;
      } else {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Value too high for data option")));
        return nullptr;
      }

      coap_opt.get()->data = reinterpret_cast<unsigned char *>(
                                          malloc(len*sizeof(unsigned char *)));
      if (ARCH_LITTLE_ENDIAN)
          swap_bytes(&val, len);
      memcpy(coap_opt->data, &val, len);
      coap_opt->data_len = len;
      break;
    }
    case ARTIK_COAP_OPTION_MAXAGE:
    case ARTIK_COAP_OPTION_SIZE1:
    case ARTIK_COAP_OPTION_BLOCK2: {
      unsigned int val = data_val->Uint32Value();
      size_t len;

      if (val < UINT8_MAX) {
        len = 1;
      } else if (val > UINT8_MAX && val < UINT16_MAX) {
        len = 2;
      } else if (val > UINT16_MAX && val < UINT32_MAX) {
        len = 4;
      } else {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Value too high for data option")));
        return nullptr;
      }

      coap_opt.get()->data = reinterpret_cast<unsigned char *>(
                                          malloc(len*sizeof(unsigned char *)));
      if (ARCH_LITTLE_ENDIAN)
          swap_bytes(&val, len);
      memcpy(coap_opt->data, &val, len);
      coap_opt->data_len = len;
      break;
    }
    default:
      break;
    }
  }

  return coap_opt;
}

std::unique_ptr<artik_coap_msg> coap_message_converter(Isolate *isolate,
                    Local<Value> val) {
  std::unique_ptr<artik_coap_msg> coap_msg(new artik_coap_msg);
  std::unique_ptr<artik_coap_option> coap_opt;

  memset(coap_msg.get(), 0, sizeof(artik_coap_msg));

  log_dbg("");

  // msg_type parameter
  auto msg_type_str = js_object_attribute_to_cpp<std::string>(val, "msg_type");

  if (msg_type_str) {
    auto msg_type = to_artik_parameter<artik_coap_msg_type>(coap_msg_type,
                                                  msg_type_str.value().c_str());

    if (!msg_type) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong value for msg_type : expect 'CON',"
        "'NON', 'ACK' or 'RST'.")));
      return nullptr;
    }

    coap_msg->msg_type = msg_type.value();
  }

  // token parameter
  auto token = js_object_attribute_to_cpp<Local<Value>>(val, "token");

  if (token) {
    unsigned char *val = reinterpret_cast<unsigned char *>(node::Buffer::Data(
      token.value()));
    size_t len = node::Buffer::Length(token.value());

    coap_msg.get()->token = reinterpret_cast<unsigned char *>(
                                          malloc(len*sizeof(unsigned char *)));
    memcpy(coap_msg->token, val, len);
    coap_msg->token_len = len;
  }

  // msg_id parameter
  auto msg_id = js_object_attribute_to_cpp<unsigned int>(val, "msg_id");

  if (msg_id) {
    coap_msg->msg_id = msg_id.value();
  }

  // code parameter
  auto code = js_object_attribute_to_cpp<unsigned int>(val, "code");

  if (code) {
    coap_msg->code = (artik_coap_code)code.value();
  }

  // data parameter
  auto data = js_object_attribute_to_cpp<Local<Value>>(val, "data");

  if (data) {
    unsigned char *val = reinterpret_cast<unsigned char *>(node::Buffer::Data(
      data.value()));
    size_t len = node::Buffer::Length(data.value());

    coap_msg.get()->data = reinterpret_cast<unsigned char *>(
                                          malloc(len*sizeof(unsigned char *)));
    memcpy(coap_msg->data, val, len);
    coap_msg->data_len = len;
  }

  // options parameter
  auto option_js = js_object_attribute_to_cpp<Local<Array>>(val, "options");

  if (option_js) {
    Local<Array> options = option_js.value();

    if (options->Length() > 0) {
      coap_msg.get()->options = reinterpret_cast<artik_coap_option*>(
        malloc(options->Length()*sizeof(artik_coap_option)));
      for (unsigned int i = 0; i < options->Length(); i++) {
        Local<Value> option = options->Get(i);
        coap_opt = coap_option_converter(isolate, option);
        if (coap_opt) {
          memcpy(&coap_msg.get()->options[i], coap_opt.get(),
            sizeof(artik_coap_option));
        }
      }
      coap_msg->num_options = options->Length();
    }
  }

  return coap_msg;
}

std::unique_ptr<artik_coap_config> coap_config_converter(
                    Isolate *isolate, Local<Value> val) {
  ArtikSslConfigProxy ssl_config(nullptr);
  std::unique_ptr<artik_coap_psk_param> psk_config;
  std::unique_ptr<artik_coap_config> coap_config(new artik_coap_config);

  memset(coap_config.get(), 0, sizeof(artik_coap_config));

  // uri parameter
  auto uri = js_object_attribute_to_cpp<std::string>(val, "uri");

  if (uri) {
    coap_config->uri = strdup(uri.value().c_str());
  }

  // port parameter
  auto port = js_object_attribute_to_cpp<int>(val, "port");

  if (port) {
    coap_config->port = port.value();
  }

  // ssl parameter
  auto ssl = js_object_attribute_to_cpp<Local<Value>>(val, "ssl");

  if (ssl) {
    ssl_config = SSLConfigConverter::convert(isolate, ssl.value());
    if (ssl_config) {
      coap_config->ssl = reinterpret_cast<artik_ssl_config*>(
                            malloc(sizeof(artik_ssl_config)));
      memcpy(coap_config->ssl, ssl_config.get(), sizeof(artik_ssl_config));
    }
  }

  log_dbg("");

  // psk parameter
  auto psk = js_object_attribute_to_cpp<Local<Value>>(val, "psk");

  if (psk) {
    psk_config = psk_params_converter(isolate, psk.value());
    if (psk_config) {
      coap_config->psk = reinterpret_cast<artik_coap_psk_param*>(
                            malloc(sizeof(artik_coap_psk_param)));
      memcpy(coap_config->psk, psk_config.get(), sizeof(artik_coap_psk_param));
    }
  }

  // verify_psk parameter
  auto verify_psk = js_object_attribute_to_cpp<bool>(val, "verify_psk");

  if (verify_psk) {
    coap_config->enable_verify_psk = verify_psk.value();
  }

  return coap_config;
}

std::unique_ptr<artik_coap_attr> coap_attribute_converter(
                    Isolate *isolate, Local<Value> val) {
  std::unique_ptr<artik_coap_attr> coap_attribute(new artik_coap_attr);

  memset(coap_attribute.get(), 0, sizeof(artik_coap_attr));

  // name parameter
  auto name = js_object_attribute_to_cpp<std::string>(val, "name");

  if (name) {
    coap_attribute->name = reinterpret_cast<unsigned char*>(
                            strdup(name.value().c_str()));
    coap_attribute->name_len = strlen(name.value().c_str());
  }

  // value parameter
  auto value = js_object_attribute_to_cpp<Local<Value>>(val, "val");

  if (value) {
    if (!node::Buffer::HasInstance(value.value())) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong definition of identity")));
      return nullptr;
    }
    unsigned char *c_val = reinterpret_cast<unsigned char *>(node::Buffer::Data(
      value.value()));
    size_t len = node::Buffer::Length(value.value());

    coap_attribute->val = reinterpret_cast<unsigned char*>(
                            malloc(len*sizeof(unsigned char *)));
    void *_val = static_cast<void*>(const_cast<unsigned char*>(
                                                        coap_attribute->val));
    memcpy(_val, c_val, len);
    coap_attribute->val_len = len;
  }

  return coap_attribute;
}

static void get_resource_callback(const artik_coap_msg *request,
                    artik_coap_msg *response, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  Resource* res = reinterpret_cast<Resource*>(user_data);
  MaybeLocal<Value> ret;
  std::unique_ptr<artik_coap_msg> coap_response;

  log_dbg("");

  if (res && !res->getGetResourceCb())
    return;

  log_dbg("");

  Local<Object> js_coap_request = coap_message_object(isolate, request);

  Handle<Value> argv[] = {
    js_coap_request
  };

  log_dbg("");

  ret = Local<Function>::New(isolate, *res->getGetResourceCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);

  log_dbg("");

  if (ret.ToLocalChecked()->IsUndefined()) {
    log_err("No object returned");
    return;
  }

  log_dbg("");

  if (!ret.ToLocalChecked()->IsObject()) {
    log_err("Wrong type");
    return;
  } else {
    coap_response = coap_message_converter(isolate, ret.ToLocalChecked());
    if (coap_response) {
      memcpy(response, coap_response.get(), sizeof(artik_coap_msg));
    }
  }
}

static void post_resource_callback(const artik_coap_msg *request,
                    artik_coap_msg *response, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  Resource* res = reinterpret_cast<Resource*>(user_data);
  MaybeLocal<Value> ret;
  std::unique_ptr<artik_coap_msg> coap_response;

  log_dbg("");

  if (res && !res->getPostResourceCb())
    return;

  Local<Object> js_coap_request = coap_message_object(isolate, request);

  Handle<Value> argv[] = {
    js_coap_request
  };

  ret = Local<Function>::New(isolate, *res->getPostResourceCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);

  if (ret.ToLocalChecked()->IsUndefined()) {
    log_err("No object returned");
    return;
  }

  if (!ret.ToLocalChecked()->IsObject()) {
    log_err("Wrong type");
    return;
  } else {
    coap_response = coap_message_converter(isolate, ret.ToLocalChecked());
    if (coap_response) {
      memcpy(response, coap_response.get(), sizeof(artik_coap_msg));
    }
  }
}

static void put_resource_callback(const artik_coap_msg *request,
                    artik_coap_msg *response, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  Resource* res = reinterpret_cast<Resource*>(user_data);
  MaybeLocal<Value> ret;
  std::unique_ptr<artik_coap_msg> coap_response;

  log_dbg("");

  if (res && !res->getPutResourceCb())
    return;

  Local<Object> js_coap_request = coap_message_object(isolate, request);

  Handle<Value> argv[] = {
    js_coap_request
  };

  ret = Local<Function>::New(isolate, *res->getPutResourceCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);

  if (ret.ToLocalChecked()->IsUndefined()) {
    log_err("No object returned");
    return;
  }

  if (!ret.ToLocalChecked()->IsObject()) {
    log_err("Wrong type");
    return;
  } else {
    coap_response = coap_message_converter(isolate, ret.ToLocalChecked());
    if (coap_response) {
      memcpy(response, coap_response.get(), sizeof(artik_coap_msg));
    }
  }
}

static void delete_resource_callback(const artik_coap_msg *request,
                    artik_coap_msg *response, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  Resource* res = reinterpret_cast<Resource*>(user_data);
  MaybeLocal<Value> ret;
  std::unique_ptr<artik_coap_msg> coap_response;

  log_dbg("");

  if (res && !res->getDeleteResourceCb())
    return;

  Local<Object> js_coap_request = coap_message_object(isolate, request);

  Handle<Value> argv[] = {
    js_coap_request
  };

  ret = Local<Function>::New(isolate, *res->getDeleteResourceCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);

  if (ret.ToLocalChecked()->IsUndefined()) {
    log_err("No object returned");
    return;
  }

  if (!ret.ToLocalChecked()->IsObject()) {
    log_err("Wrong type");
    return;
  } else {
    coap_response = coap_message_converter(isolate, ret.ToLocalChecked());
    if (coap_response) {
      memcpy(response, coap_response.get(), sizeof(artik_coap_msg));
    }
  }
}

std::unique_ptr<artik_coap_resource> coap_resource_converter(
                    Isolate *isolate, CoapWrapper *wrap, Local<Value> val) {
  std::unique_ptr<artik_coap_resource> coap_resource(new artik_coap_resource);
  std::unique_ptr<artik_coap_attr> coap_attr;

  memset(coap_resource.get(), 0, sizeof(artik_coap_resource));

  wrap->getResources()->push_back(new Resource());

  // path parameter
  auto path = js_object_attribute_to_cpp<std::string>(val, "path");

  if (path) {
    coap_resource->path = strdup(path.value().c_str());
    coap_resource->path_len = strlen(path.value().c_str());
  }

  log_dbg("");

  // notif_type parameter
  auto notif_type = js_object_attribute_to_cpp<unsigned int>(val, "notif_type");

  if (notif_type) {
    coap_resource->default_notification_type =
                (artik_coap_resource_notification_type)notif_type.value();
  }

  log_dbg("");

  // observable parameter
  auto observable = js_object_attribute_to_cpp<bool>(val, "observable");

  if (observable) {
    coap_resource->observable = observable.value();
  }

  log_dbg("");

  // attributes parameter
  auto attributes_js = js_object_attribute_to_cpp<Local<Array>>(val,
                                                                "attributes");

  if (attributes_js) {
    Local<Array> attributes = attributes_js.value();

    if (attributes->Length() > 0) {
      coap_resource.get()->attributes = reinterpret_cast<artik_coap_attr*>(
                         malloc(attributes->Length()*sizeof(artik_coap_attr)));
      for (unsigned int i = 0; i < attributes->Length(); i++) {
        Local<Value> attribute = attributes->Get(i);
        coap_attr = coap_attribute_converter(isolate, attribute);
        if (coap_attr) {
          memcpy(&coap_resource.get()->attributes[i], coap_attr.get(),
                                           sizeof(artik_coap_attr));
        }
      }
      coap_resource->num_attributes = attributes->Length();
    }
  }

  log_dbg("");

  // get_resource parameter
  auto get_resource = js_object_attribute_to_cpp<Local<Value>>(val,
                                                              "get_resource");

  if (get_resource) {
    wrap->getResources()->back()->setGetResourceCb(isolate,
                                                          get_resource.value());

    coap_resource->resource_cb[0] = get_resource_callback;
    coap_resource->resource_data[0] = reinterpret_cast<void*>(
                                                wrap->getResources()->back());
  }

  log_dbg("");

  // post_resource parameter
  auto post_resource = js_object_attribute_to_cpp<Local<Value>>(val,
                                                              "post_resource");

  if (post_resource) {
    wrap->getResources()->back()->setPostResourceCb(isolate,
                                                        post_resource.value());

    coap_resource->resource_cb[1] = post_resource_callback;
    coap_resource->resource_data[1] = reinterpret_cast<void*>(
                                                wrap->getResources()->back());
  }

  log_dbg("");

  // put_resource parameter
  auto put_resource = js_object_attribute_to_cpp<Local<Value>>(val,
                                                                "put_resource");

  if (put_resource) {
    wrap->getResources()->back()->setPutResourceCb(isolate,
                                                          put_resource.value());

    coap_resource->resource_cb[2] = put_resource_callback;
    coap_resource->resource_data[2] = reinterpret_cast<void*>(
                                                wrap->getResources()->back());
  }

  log_dbg("");

  // delete_resource parameter
  auto delete_resource = js_object_attribute_to_cpp<Local<Value>>(val,
                                                            "delete_resource");

  if (delete_resource) {
    wrap->getResources()->back()->setDeleteResourceCb(isolate,
                                                      delete_resource.value());

    coap_resource->resource_cb[3] = delete_resource_callback;
    coap_resource->resource_data[3] = reinterpret_cast<void*>(
                                                wrap->getResources()->back());
  }

  return coap_resource;
}

static void coap_send_callback(const artik_coap_msg *msg,
                    artik_coap_error error, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  CoapWrapper* wrap = reinterpret_cast<CoapWrapper*>(user_data);

  log_dbg("");

  if (!wrap->getSendCb())
    return;

  std::string s_error;

  switch (error) {
  case ARTIK_COAP_ERROR_NONE:
    s_error = "NONE";
    break;
  case ARTIK_COAP_ERROR_TOO_MANY_RETRIES:
    s_error = "TOO MANY RETRIES";
    break;
  case ARTIK_COAP_ERROR_NOT_DELIVERABLE:
    s_error = "NOT DELIVERABLE";
    break;
  case ARTIK_COAP_ERROR_RST:
    s_error = "RST";
    break;
  case ARTIK_COAP_ERROR_TLS_FAILED:
    s_error = "TLS FAILED";
    break;
  }

  Local<Object> js_coap_msg = coap_message_object(isolate, msg);

  Handle<Value> argv[] = {
    js_coap_msg,
    Handle<Value>(String::NewFromUtf8(isolate, s_error.c_str()))
  };

  Local<Function>::New(isolate, *wrap->getSendCb())->Call(
      isolate->GetCurrentContext()->Global(), 2, argv);
}

static void coap_observe_callback(const artik_coap_msg *msg,
                    artik_coap_error error, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  CoapWrapper* wrap = reinterpret_cast<CoapWrapper*>(user_data);

  log_dbg("");

  if (!wrap->getObserveCb())
    return;

  std::string s_error;

  switch (error) {
  case ARTIK_COAP_ERROR_NONE:
    s_error = "NONE";
    break;
  case ARTIK_COAP_ERROR_TOO_MANY_RETRIES:
    s_error = "TOO MANY RETRIES";
    break;
  case ARTIK_COAP_ERROR_NOT_DELIVERABLE:
    s_error = "NOT DELIVERABLE";
    break;
  case ARTIK_COAP_ERROR_RST:
    s_error = "RST";
    break;
  case ARTIK_COAP_ERROR_TLS_FAILED:
    s_error = "TLS FAILED";
    break;
  }

  Local<Object> js_coap_msg = coap_message_object(isolate, msg);

  Handle<Value> argv[] = {
    js_coap_msg,
    Handle<Value>(String::NewFromUtf8(isolate, s_error.c_str()))
  };

  Local<Function>::New(isolate, *wrap->getObserveCb())->Call(
      isolate->GetCurrentContext()->Global(), 2, argv);
}

static int coap_verify_psk_callback(const unsigned char *identity,
                    unsigned char **key, int key_len, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  HandleScope handleScope(isolate);
  CoapWrapper* wrap = reinterpret_cast<CoapWrapper*>(user_data);
  MaybeLocal<Value> ret;

  log_dbg("");

  if (!wrap->getVerifyPskCb())
    return 0;

  char *_identity = reinterpret_cast<char*>(const_cast<unsigned char*>(
                                                                identity));

  Handle<Value> argv[] = {
    Handle<Value>(Nan::CopyBuffer(_identity,
                                  strlen(_identity)).ToLocalChecked()),
    Handle<Value>(Integer::New(isolate, key_len))
  };

  ret = Local<Function>::New(isolate, *wrap->getVerifyPskCb())->Call(
      isolate->GetCurrentContext()->Global(), 2, argv);

  log_dbg("");

  if (ret.ToLocalChecked()->IsUndefined()) {
    log_err("No object returned");
    return 0;
  }

  log_dbg("");

  if (!node::Buffer::HasInstance(ret.ToLocalChecked())) {
    log_err("Wrong type");
    return 0;
  } else {
    unsigned char *val = reinterpret_cast<unsigned char *>(node::Buffer::Data(
      ret.ToLocalChecked()));
    size_t len = node::Buffer::Length(ret.ToLocalChecked());

    memcpy(*key, val, len);

    return len;
  }

  return 0;
}

CoapWrapper::CoapWrapper() {
  m_coap = new Coap();
  m_loop = GlibLoop::Instance();
  m_resources = new std::vector<Resource*>;
  m_loop->attach();
}

CoapWrapper::CoapWrapper(artik_coap_config *config) {
  m_coap = new Coap(config);
  m_loop = GlibLoop::Instance();
  m_resources = new std::vector<Resource*>;
  m_loop->attach();
}

CoapWrapper::~CoapWrapper() {
  delete m_coap;
  m_loop->detach();
  for (std::vector<Resource*>::iterator it = m_resources->begin();
                                  it != m_resources->end(); ++it) {
    delete (*it);
  }
  if (!m_resources->empty())
    m_resources->clear();
  delete m_resources;
}

void CoapWrapper::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "coap"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototypes
  NODE_SET_PROTOTYPE_METHOD(tpl, "create_client", create_client);
  NODE_SET_PROTOTYPE_METHOD(tpl, "destroy_client", destroy_client);
  NODE_SET_PROTOTYPE_METHOD(tpl, "connect", connect);
  NODE_SET_PROTOTYPE_METHOD(tpl, "disconnect", disconnect);
  NODE_SET_PROTOTYPE_METHOD(tpl, "create_server", create_server);
  NODE_SET_PROTOTYPE_METHOD(tpl, "destroy_server", destroy_server);
  NODE_SET_PROTOTYPE_METHOD(tpl, "start_server", start_server);
  NODE_SET_PROTOTYPE_METHOD(tpl, "stop_server", stop_server);
  NODE_SET_PROTOTYPE_METHOD(tpl, "send_message", send_message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "observe", observe);
  NODE_SET_PROTOTYPE_METHOD(tpl, "cancel_observe", cancel_observe);
  NODE_SET_PROTOTYPE_METHOD(tpl, "init_resources", init_resources);
  NODE_SET_PROTOTYPE_METHOD(tpl, "notify_resource_changed",
                                          notify_resource_changed);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "coap"), tpl->GetFunction());
}

void CoapWrapper::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  int lenArg = 1;
  std::unique_ptr<artik_coap_config> coap_config;

  if (args.Length() > lenArg) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  } else if (args.IsConstructCall()) {
    CoapWrapper *obj = NULL;

    if (args[0]->IsObject()) {
      coap_config = coap_config_converter(isolate, args[0]);
    } else if (!args[0]->IsUndefined() && !args[0]->IsObject()) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong arguments")));
      return;
    }

    if (!coap_config) {
      coap_config = std::unique_ptr<artik_coap_config>(new artik_coap_config);
      memset(coap_config.get(), 0, sizeof(artik_coap_config));
    }

    obj = new CoapWrapper(coap_config.get());
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    int argc = args.Length();
    Local<Value> argv[argc];

    for (int i = 0; i < argc; ++i)
      argv[i] = args[i];

    Local<Context> context = isolate->GetCurrentContext();
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(
        cons->NewInstance(context, argc, argv).ToLocalChecked());
  }
}

void CoapWrapper::create_client(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();

  if (args.Length() > 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  artik_error ret = obj->create_client();
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to create client")));
    return;
  }

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::destroy_client(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();

  if (args.Length() > 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  artik_error ret = obj->destroy_client();
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to destroy client")));
    return;
  }

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::connect(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();

  if (args.Length() > 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  artik_error ret = obj->connect();
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to connect")));
    return;
  }

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::disconnect(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();

  if (args.Length() > 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  artik_error ret = obj->disconnect();
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to disconnect")));
    return;
  }

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::create_server(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();

  if (args.Length() > 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  artik_error ret = obj->create_server();
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to create server")));
    return;
  }

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::destroy_server(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();

  if (args.Length() > 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  artik_error ret = obj->destroy_server();
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to destroy server")));
    return;
  }

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::start_server(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();

  if (args.Length() > 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsUndefined()) {
    wrap->m_verify_psk_cb = new Persistent<Function>();
    wrap->m_verify_psk_cb->Reset(isolate, Local<Function>::Cast(args[0]));
    obj->set_verify_psk_callback(coap_verify_psk_callback,
        reinterpret_cast<void*>(wrap));
  }

  artik_error ret = obj->start_server();
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to start server")));
    return;
  }

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::stop_server(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();

  if (args.Length() > 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  artik_error ret = obj->stop_server();
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to stop server")));
    return;
  }

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::send_message(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();
  char* path = NULL;
  std::unique_ptr<artik_coap_msg> coap_msg;

  if (args.Length() < 2 || args.Length() > 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  if (args[0]->IsString()) {
    String::Utf8Value param0(args[0]->ToString());
    path = strndup(*param0, strlen(*param0));
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (args[1]->IsObject()) {
    coap_msg = coap_message_converter(isolate, args[1]);
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsUndefined()) {
    wrap->m_send_cb = new Persistent<Function>();
    wrap->m_send_cb->Reset(isolate, Local<Function>::Cast(args[2]));
    obj->set_send_callback(coap_send_callback,
        reinterpret_cast<void*>(wrap));
  } else {
    obj->set_send_callback(NULL, NULL);
  }

  artik_error ret = obj->send_message(path, coap_msg.get());
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to send message")));
    return;
  }

  if (path)
    free(path);

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::observe(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();
  char* path = NULL;
  artik_coap_msg_type msg_type = ARTIK_COAP_MSG_CON;
  artik_coap_option *options = NULL;
  int num_options = 0;
  unsigned char *token = NULL;
  int token_len = 0;
  std::unique_ptr<artik_coap_option> coap_opt;

  if (args.Length() < 2 || args.Length() > 5) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  if (args[0]->IsString()) {
    String::Utf8Value param0(args[0]->ToString());
    path = strndup(*param0, strlen(*param0));
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (args[1]->IsString()) {
    v8::String::Utf8Value param0(args[1]->ToString());

    auto msg_type_val = to_artik_parameter<artik_coap_msg_type>(coap_msg_type,
                                                  *param0);

    if (!msg_type_val) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong value for msg_type : expect 'CON',"
        "'NON', 'ACK' or 'RST'.")));
      return;
    }

    msg_type = msg_type_val.value();
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong argument type")));
    return;
  }

  if (!args[2]->IsUndefined()) {
    wrap->m_observe_cb = new Persistent<Function>();
    wrap->m_observe_cb->Reset(isolate, Local<Function>::Cast(args[2]));
    obj->set_observe_callback(coap_observe_callback,
        reinterpret_cast<void*>(wrap));
  } else {
    obj->set_observe_callback(NULL, NULL);
  }

  if (args[3]->IsObject()) {
    auto option_js = js_type_to_cpp<Local<Array>>(args[3]);

    if (option_js) {
      Local<Array> options_val = option_js.value();

      if (options_val->Length() > 0) {
        options = reinterpret_cast<artik_coap_option*>(malloc(
          options_val->Length()*sizeof(artik_coap_option)));
        for (unsigned int i = 0; i < options_val->Length(); i++) {
          Local<Value> option = options_val->Get(i);
          coap_opt = coap_option_converter(isolate, option);
          if (coap_opt) {
            memcpy(&options[i], coap_opt.get(), sizeof(artik_coap_option));
          }
        }
        num_options = options_val->Length();
      }
    }
  }

  if (node::Buffer::HasInstance(args[4])) {
    unsigned char *val = reinterpret_cast<unsigned char *>(node::Buffer::Data(
      args[4]));
    size_t len = node::Buffer::Length(args[4]);

    token = reinterpret_cast<unsigned char*>(malloc(
                                              len*sizeof(unsigned char *)));
    memcpy(token, val, len);
    token_len = len;
  }


  artik_error ret = obj->observe(path, msg_type, options, num_options,
                                token, token_len);
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to observe")));
    return;
  }

  if (path)
    free(path);

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::cancel_observe(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();
  char* path = NULL;
  unsigned char *token = NULL;
  int token_len = 0;
  std::unique_ptr<artik_coap_option> coap_opt;

  if (args.Length() < 2 || args.Length() > 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  if (args[0]->IsString()) {
    String::Utf8Value param0(args[0]->ToString());
    path = strndup(*param0, strlen(*param0));
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (node::Buffer::HasInstance(args[1])) {
    unsigned char *val = reinterpret_cast<unsigned char *>(node::Buffer::Data(
      args[1]));
    size_t len = node::Buffer::Length(args[1]);

    token = reinterpret_cast<unsigned char *>(malloc(
                                              len*sizeof(unsigned char *)));
    memcpy(token, val, len);
    token_len = len;
  }

  if (!args[2]->IsUndefined()) {
    wrap->m_send_cb = new Persistent<Function>();
    wrap->m_send_cb->Reset(isolate, Local<Function>::Cast(args[2]));
    obj->set_send_callback(coap_send_callback,
        reinterpret_cast<void*>(wrap));
  } else {
    obj->set_send_callback(NULL, NULL);
  }

  artik_error ret = obj->cancel_observe(path, token, token_len);
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to cancel observe")));
    return;
  }

  if (path)
    free(path);

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::init_resources(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();
  std::unique_ptr<artik_coap_resource> coap_resource;
  artik_coap_resource *resources = NULL;
  int num_resources = 0;

  if (args.Length() < 1 || args.Length() > 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  if (args[0]->IsObject()) {
    auto resource_js = js_type_to_cpp<Local<Array>>(args[0]);

    if (resource_js) {
      Local<Array> resources_val = resource_js.value();

      if (resources_val->Length() > 0) {
        resources = reinterpret_cast<artik_coap_resource*>(
          malloc(resources_val->Length()*sizeof(artik_coap_resource)));
        for (unsigned int i = 0; i < resources_val->Length(); i++) {
          Local<Value> resource = resources_val->Get(i);
          coap_resource = coap_resource_converter(isolate, wrap, resource);
          if (coap_resource) {
            memcpy(&resources[i], coap_resource.get(),
                                                sizeof(artik_coap_resource));
          }
        }
        num_resources = resources_val->Length();
      }
    }
  }

  artik_error ret = obj->init_resources(resources, num_resources);
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to init resources")));
    return;
  }

  if (resources)
    free(resources);

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void CoapWrapper::notify_resource_changed(
                                    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CoapWrapper* wrap = ObjectWrap::Unwrap<CoapWrapper>(args.Holder());
  Coap* obj = wrap->getObj();
  char *path = NULL;

  if (args[0]->IsString()) {
    String::Utf8Value param0(args[0]->ToString());
    path = strndup(*param0, strlen(*param0));
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  artik_error ret = obj->notify_resource_changed(path);
  if (ret != S_OK) {
    isolate->ThrowException(Exception::Error(String::NewFromUtf8(
     isolate, "Failed to notify resource changed")));
    return;
  }

  if (path)
    free(path);

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

}  // namespace artik
