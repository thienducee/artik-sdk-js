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

#include "mqtt/mqtt.h"

#include <unistd.h>
#include <node_buffer.h>
#include <nan.h>
#include <artik_log.h>
#include <artik_error.hh>

#include <iostream>
#include <string>

namespace artik {

using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Handle;
using v8::Context;

#define CHECK_ARGUMENT_STRING(x)  (x->IsUndefined() || !x->IsString())
#define CHECK_ARGUMENT_NUMBER(x)  (x->IsUndefined() || !x->IsNumber())
#define CHECK_ARGUMENT_BOOLEAN(x) (x->IsUndefined() || !x->IsBoolean())
#define CHECK_ARGUMENT_DEFINED(x) (x->IsUndefined())
#define CHECK_ARGUMENT_OBJECT(x) (x->IsUndefined() || !x->IsObject())

Persistent<Function> MqttWrapper::constructor;

static void updateSSLConfig(Isolate *isolate, Local<Value> val,
    artik_ssl_config **ssl_config) {
  *ssl_config = reinterpret_cast<artik_ssl_config*>(malloc(
      sizeof(artik_ssl_config)));

  memset(*ssl_config, 0, sizeof(artik_ssl_config));

  // use_se parameter
  auto use_se = js_object_attribute_to_cpp<bool>(val, "use_se");

  if (use_se)
    (*ssl_config)->use_se = use_se.value();

  // ca_cert parameter
  auto ca_cert = js_object_attribute_to_cpp<Local<Value>>(val, "ca_cert");

  if (ca_cert) {
    if (node::Buffer::HasInstance(ca_cert.value())) {
      char *val = reinterpret_cast<char *>(node::Buffer::Data(ca_cert.value()));
      size_t len = node::Buffer::Length(ca_cert.value());

      (*ssl_config)->ca_cert.data = strdup(val);
      (*ssl_config)->ca_cert.len = len;
    } else {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong definition of ca_cert")));
    }
  }

  // client_cert parameter
  auto client_cert = js_object_attribute_to_cpp<Local<Value>>(val,
      "client_cert");

  if (client_cert) {
    if (node::Buffer::HasInstance(client_cert.value())) {
      char *val = reinterpret_cast<char *>(node::Buffer::Data(
          client_cert.value()));
      size_t len = node::Buffer::Length(client_cert.value());

      (*ssl_config)->client_cert.data = strdup(val);
      (*ssl_config)->client_cert.len = len;
    } else {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong definition of client_cert")));
    }
  }

  // client_key parameter
  auto client_key = js_object_attribute_to_cpp<Local<Value>>(val, "client_key");

  if (client_key) {
    if (node::Buffer::HasInstance(client_key.value())) {
      char *val = reinterpret_cast<char *>(node::Buffer::Data(
          client_key.value()));
      size_t len = node::Buffer::Length(client_key.value());

      (*ssl_config)->client_key.data = strdup(val);
      (*ssl_config)->client_key.len = len;
    } else {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong definition of client_key")));
    }
  }

  // verify_cert parameter
  auto verify_cert = js_object_attribute_to_cpp<std::string>(val,
      "verify_cert");

  if (verify_cert) {
    if (verify_cert.value() == "none")
      (*ssl_config)->verify_cert = ARTIK_SSL_VERIFY_NONE;
    else if (verify_cert.value() == "optional")
      (*ssl_config)->verify_cert = ARTIK_SSL_VERIFY_OPTIONAL;
    else if (verify_cert.value() == "required")
      (*ssl_config)->verify_cert = ARTIK_SSL_VERIFY_REQUIRED;
    else
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong definition of verify_cert : expect 'none', 'optional' "
          "or 'required'.")));
  }
}

static void on_mqtt_connect(artik_mqtt_config *client_config, void *user_data,
    artik_error result) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  MqttWrapper* wrap = reinterpret_cast<MqttWrapper*>(client_config->handle);

  log_dbg("");

  if (!wrap->getConnectCb())
    return;

  Handle<Value> argv[] = {
    Handle<Value>(String::NewFromUtf8(isolate, error_msg(result)))
  };

  Local<Function>::New(isolate, *wrap->getConnectCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);
}

static void on_mqtt_disconnect(artik_mqtt_config *client_config,
    void *data_user, artik_error result) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  MqttWrapper* wrap = reinterpret_cast<MqttWrapper*>(client_config->handle);

  log_dbg("");

  if (!wrap->getDisconnectCb())
    return;

  Handle<Value> argv[] = {
    Handle<Value>(String::NewFromUtf8(isolate, error_msg(result)))
  };

  Local<Function>::New(isolate, *wrap->getDisconnectCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);
}

static void on_mqtt_subscribe(artik_mqtt_config *client_config, void *data_user,
    int mid, int qos_count, const int *granted_qos) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  MqttWrapper* wrap = reinterpret_cast<MqttWrapper*>(client_config->handle);

  log_dbg("");

  if (!wrap->getSubscribeCb())
    return;

  Handle<Value> argv[] = {
    Handle<Value>(v8::Integer::New(isolate, mid))
  };

  Local<Function>::New(isolate, *wrap->getSubscribeCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);
}

static void on_mqtt_unsubscribe(artik_mqtt_config *client_config,
    void *data_user, int mid) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  MqttWrapper* wrap = reinterpret_cast<MqttWrapper*>(client_config->handle);

  log_dbg("");

  if (!wrap->getUnsubscribeCb())
    return;

  Handle<Value> argv[] = {
    Handle<Value>(v8::Integer::New(isolate, mid))
  };

  Local<Function>::New(isolate, *wrap->getUnsubscribeCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);
}

static void on_mqtt_publish(artik_mqtt_config *client_config, void *data_user,
    int mid) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  MqttWrapper* wrap = reinterpret_cast<MqttWrapper*>(client_config->handle);

  log_dbg("");

  if (!wrap->getPublishCb())
    return;

  Handle<Value> argv[] = {
    Handle<Value>(v8::Integer::New(isolate, mid))
  };

  Local<Function>::New(isolate, *wrap->getPublishCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);
}

static void on_mqtt_message(artik_mqtt_config *client_config, void *data_user,
    artik_mqtt_msg *msg) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  MqttWrapper* wrap = reinterpret_cast<MqttWrapper*>(client_config->handle);

  log_dbg("");

  if (!wrap->getMessageCb())
    return;

  Handle<Value> argv[] = {
    Handle<Value>(v8::Integer::New(isolate, msg->msg_id)),
    Handle<Value>(String::NewFromUtf8(isolate, msg->topic)),
    Handle<Value>(Nan::CopyBuffer(reinterpret_cast<char*>(msg->payload),
        msg->payload_len).ToLocalChecked()),
    Handle<Value>(v8::Integer::New(isolate, msg->qos)),
    Handle<Value>(v8::Boolean::New(isolate, msg->retain)),
  };

  Local<Function>::New(isolate, *wrap->getMessageCb())->Call(
      isolate->GetCurrentContext()->Global(), 5, argv);
}

MqttWrapper::MqttWrapper(artik_mqtt_config const &config) {
  Isolate* isolate = Isolate::GetCurrent();

  try {
    m_mqtt = new Mqtt(config);
    m_mqtt->config().handle = this;
    m_mqtt->set_connect(on_mqtt_connect, NULL);
    m_mqtt->set_disconnect(on_mqtt_disconnect, NULL);
    m_mqtt->set_subscribe(on_mqtt_subscribe, NULL);
    m_mqtt->set_unsubscribe(on_mqtt_unsubscribe, NULL);
    m_mqtt->set_publish(on_mqtt_publish, NULL);
    m_mqtt->set_message(on_mqtt_message, NULL);
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, e.what())));
  }
}

MqttWrapper::~MqttWrapper() {
  delete m_mqtt;
}

void MqttWrapper::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();
  Local<FunctionTemplate> modal = FunctionTemplate::New(isolate, New);

  log_dbg("");

  modal->SetClassName(String::NewFromUtf8(isolate, "mqtt"));
  modal->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototypes
  NODE_SET_PROTOTYPE_METHOD(modal, "set_willmsg", set_willmsg);
  NODE_SET_PROTOTYPE_METHOD(modal, "free_willmsg", free_willmsg);
  NODE_SET_PROTOTYPE_METHOD(modal, "clear_willmsg", clear_willmsg);
  NODE_SET_PROTOTYPE_METHOD(modal, "connect", connect);
  NODE_SET_PROTOTYPE_METHOD(modal, "disconnect", disconnect);
  NODE_SET_PROTOTYPE_METHOD(modal, "subscribe", subscribe);
  NODE_SET_PROTOTYPE_METHOD(modal, "unsubscribe", unsubscribe);
  NODE_SET_PROTOTYPE_METHOD(modal, "publish", publish);


  MqttWrapper::constructor.Reset(isolate, modal->GetFunction());
  exports->Set(v8::String::NewFromUtf8(isolate, "mqtt"), modal->GetFunction());
}

void MqttWrapper::New(const FunctionCallbackInfo<Value>& args) {
  Isolate *isolate = args.GetIsolate();
  MqttWrapper* obj = NULL;

  log_dbg("");

  if ((args.Length() < 6 || args.Length() > 13) ||
      CHECK_ARGUMENT_STRING(args[0])   ||  // Client ID
      CHECK_ARGUMENT_STRING(args[1])   ||  // User Name
      CHECK_ARGUMENT_STRING(args[2])   ||  // User Password
      CHECK_ARGUMENT_BOOLEAN(args[3])  ||  // Clean session flag
      CHECK_ARGUMENT_NUMBER(args[4])   ||  // Keep-Alive time
      CHECK_ARGUMENT_BOOLEAN(args[5])) {   // Block flag
    isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Mqtt Wrong arguments")));
  } else if (args.IsConstructCall()) {
    v8::String::Utf8Value client_id(args[0]->ToString());
    v8::String::Utf8Value user_name(args[1]->ToString());
    v8::String::Utf8Value user_pwd(args[2]->ToString());
    artik_mqtt_config config;

    memset(&config, 0, sizeof(config));
    config.client_id = strndup(*client_id, strlen(*client_id));
    config.user_name = strndup(*user_name, strlen(*user_name));
    config.pwd = strndup(*user_pwd, strlen(*user_pwd));
    config.clean_session = args[3]->BooleanValue();
    config.keep_alive_time = args[4]->NumberValue();
    config.block = args[5]->BooleanValue();

    if (!args[6]->IsUndefined() && args[6]->IsObject())
      updateSSLConfig(isolate, args[6], &(config.tls));

    obj = new MqttWrapper(config);
    obj->Wrap(args.This());

    if (args.Length() >= 8 &&
        !args[7]->IsNull() &&
        !args[7]->IsUndefined()) {
      obj->m_connect_cb = new v8::Persistent<v8::Function>();
      obj->m_connect_cb->Reset(isolate, Local<Function>::Cast(args[7]));
    }
    if (args.Length() >= 9 &&
        !args[8]->IsNull() &&
        !args[8]->IsUndefined()) {
      obj->m_disconnect_cb = new v8::Persistent<v8::Function>();
      obj->m_disconnect_cb->Reset(isolate, Local<Function>::Cast(args[8]));
    }
    if (args.Length() >= 10 &&
        !args[9]->IsNull() &&
        !args[9]->IsUndefined()) {
      obj->m_subscribe_cb = new v8::Persistent<v8::Function>();
      obj->m_subscribe_cb->Reset(isolate, Local<Function>::Cast(args[9]));
    }
    if (args.Length() >= 11 &&
        !args[10]->IsNull() &&
        !args[10]->IsUndefined()) {
      obj->m_unsubscribe_cb = new v8::Persistent<v8::Function>();
      obj->m_unsubscribe_cb->Reset(isolate, Local<Function>::Cast(args[10]));
    }
    if (args.Length() >= 12 &&
        !args[11]->IsNull() &&
        !args[11]->IsUndefined()) {
      obj->m_publish_cb = new v8::Persistent<v8::Function>();
      obj->m_publish_cb->Reset(isolate, Local<Function>::Cast(args[11]));
    }
    if (args.Length() == 13 &&
        !args[12]->IsNull() &&
        !args[12]->IsUndefined()) {
      obj->m_message_cb = new v8::Persistent<v8::Function>();
      obj->m_message_cb->Reset(isolate, Local<Function>::Cast(args[12]));
    }
    args.GetReturnValue().Set(args.This());
  } else {
    int argc = args.Length();
    Local<Value> argv[argc];

    for (int i = 0; i < argc; i++)
      argv[i] = args[i];

    Local<Context> context = isolate->GetCurrentContext();
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(
        cons->NewInstance(context, argc, argv).ToLocalChecked());
  }
}

void MqttWrapper::set_willmsg(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  MqttWrapper* wrap = ObjectWrap::Unwrap<MqttWrapper>(args.Holder());
  Mqtt* obj = wrap->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  // Check Arguments
  if ((args.Length() < 4)              ||
      CHECK_ARGUMENT_STRING(args[0])   ||  // Will topic
      CHECK_ARGUMENT_STRING(args[1])   ||  // Will message
      CHECK_ARGUMENT_NUMBER(args[2])   ||  // QoS
      CHECK_ARGUMENT_BOOLEAN(args[3])) {   // Retain flag
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                      isolate, "Wrong arguments")));
  }

  v8::String::Utf8Value will_topic(args[0]->ToString());
  v8::String::Utf8Value will_msg(args[1]->ToString());

  ret = obj->set_willmsg(*will_topic, *will_msg, args[2]->NumberValue(),
      args[3]->BooleanValue());

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void MqttWrapper::free_willmsg(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  MqttWrapper* wrap = ObjectWrap::Unwrap<MqttWrapper>(args.Holder());
  Mqtt* obj = wrap->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  ret = obj->free_willmsg();

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void MqttWrapper::clear_willmsg(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  MqttWrapper* wrap = ObjectWrap::Unwrap<MqttWrapper>(args.Holder());
  Mqtt* obj = wrap->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  ret = obj->clear_willmsg();

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void MqttWrapper::connect(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  MqttWrapper* wrap = ObjectWrap::Unwrap<MqttWrapper>(args.Holder());
  Mqtt* obj = wrap->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  // Check Arguments
  if ((args.Length() < 2 || args.Length() > 3) ||
      CHECK_ARGUMENT_STRING(args[0])   ||  // Host
      CHECK_ARGUMENT_NUMBER(args[1])) {    // Port
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                      isolate, "Wrong arguments")));
  }

  v8::String::Utf8Value host(args[0]->ToString());
  if (args.Length() == 3 && !args[3]->IsNull() && !args[3]->IsUndefined()) {
    wrap->m_connect_cb = new v8::Persistent<v8::Function>();
    wrap->m_connect_cb->Reset(isolate, Local<Function>::Cast(args[2]));
  }

  ret = obj->connect(*host, args[1]->NumberValue());
  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void MqttWrapper::disconnect(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  MqttWrapper* wrap = ObjectWrap::Unwrap<MqttWrapper>(args.Holder());
  Mqtt* obj = wrap->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  if (args.Length() == 1 && !args[0]->IsNull() && !args[0]->IsUndefined()) {
    wrap->m_disconnect_cb = new v8::Persistent<v8::Function>();
    wrap->m_disconnect_cb->Reset(isolate, Local<Function>::Cast(args[0]));
  }

  ret = obj->disconnect();

  args.GetReturnValue().Set(Number::New(isolate, ret));
}

void MqttWrapper::subscribe(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  MqttWrapper* wrap = ObjectWrap::Unwrap<MqttWrapper>(args.Holder());
  Mqtt* obj = wrap->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  // Check Arguments
  if ((args.Length() < 2 || args.Length() > 4) ||
      CHECK_ARGUMENT_NUMBER(args[0])   ||  // QoS
      CHECK_ARGUMENT_STRING(args[1])) {    // Message topic
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                      isolate, "Wrong arguments")));
  }

  v8::String::Utf8Value msg_topic(args[1]->ToString());

  if (args.Length() >= 3 && !args[2]->IsNull() && !args[2]->IsUndefined()) {
    wrap->m_subscribe_cb = new v8::Persistent<v8::Function>();
    wrap->m_subscribe_cb->Reset(isolate, Local<Function>::Cast(args[2]));
  }
  if (args.Length() == 4 && !args[3]->IsNull() && !args[3]->IsUndefined()) {
    wrap->m_message_cb = new v8::Persistent<v8::Function>();
    wrap->m_message_cb->Reset(isolate, Local<Function>::Cast(args[3]));
  }

  ret = obj->subscribe(args[0]->BooleanValue(), *msg_topic);

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void MqttWrapper::unsubscribe(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  MqttWrapper* wrap = ObjectWrap::Unwrap<MqttWrapper>(args.Holder());
  Mqtt* obj = wrap->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  // Check Arguments
  if ((args.Length() < 1 || args.Length() > 2) ||
      CHECK_ARGUMENT_STRING(args[0])) {  // Message topic
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                      isolate, "Wrong arguments")));
  }

  if (args.Length() == 2 && !args[1]->IsNull() && !args[1]->IsUndefined()) {
    wrap->m_unsubscribe_cb = new v8::Persistent<v8::Function>();
    wrap->m_unsubscribe_cb->Reset(isolate, Local<Function>::Cast(args[1]));
  }

  v8::String::Utf8Value msg_topic(args[0]->ToString());

  ret = obj->unsubscribe(*msg_topic);

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void MqttWrapper::publish(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  MqttWrapper* wrap = ObjectWrap::Unwrap<MqttWrapper>(args.Holder());
  Mqtt* obj = wrap->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  // Check Arguments
  if ((args.Length() < 4 || args.Length() > 5) ||
      CHECK_ARGUMENT_NUMBER(args[0])   ||  // QoS
      CHECK_ARGUMENT_BOOLEAN(args[1])  ||  // Retain flag
      CHECK_ARGUMENT_STRING(args[2])   ||  // Message topic
      CHECK_ARGUMENT_OBJECT(args[3])) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                      isolate, "Wrong arguments")));
  }

  if (!node::Buffer::HasInstance(args[3]))
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                      isolate, "Argument should be a Buffer.")));

  v8::String::Utf8Value msg_topic(args[2]->ToString());

  char *buffer = node::Buffer::Data(args[3]);
  size_t length = node::Buffer::Length(args[3]);

  if (args.Length() == 5 && !args[4]->IsNull() && !args[4]->IsUndefined()) {
    wrap->m_publish_cb = new v8::Persistent<v8::Function>();
    wrap->m_publish_cb->Reset(isolate, Local<Function>::Cast(args[4]));
  }

  ret = obj->publish(args[0]->NumberValue(), args[1]->BooleanValue(),
                     *msg_topic, length, buffer);

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

}  // namespace artik
