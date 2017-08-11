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

#include "http/http.h"

#include <unistd.h>
#include <node_buffer.h>
#include <artik_log.h>

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
using v8::Array;
using v8::Handle;
using v8::Int32;
using v8::Context;

Persistent<Function> HttpWrapper::constructor;

static void updateSSLConfig(Isolate *isolate, Local<Value> val,
    artik_ssl_config **ssl_config) {

  artik_ssl_config * config = reinterpret_cast<artik_ssl_config*>(malloc(
      sizeof(artik_ssl_config)));

  if (!config) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Not enough memory")));
  }

  memset(config, 0, sizeof(artik_ssl_config));

  /* use_se parameter */
  auto use_se = js_object_attribute_to_cpp<bool>(val, "use_se");

  if (use_se)
    config->use_se = use_se.value();

  /* ca_cert parameter */
  auto ca_cert = js_object_attribute_to_cpp<Local<Value>>(val, "ca_cert");

  if (ca_cert) {
    if (node::Buffer::HasInstance(ca_cert.value())) {
      char *val = reinterpret_cast<char *>(node::Buffer::Data(ca_cert.value()));
      size_t len = node::Buffer::Length(ca_cert.value());

      config->ca_cert.data = strndup(val, len);
      config->ca_cert.len = len;
    } else {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong definition of ca_cert")));
    }
  }

  /* client_cert parameter */
  auto client_cert = js_object_attribute_to_cpp<Local<Value>>(val,
      "client_cert");

  if (client_cert) {
    if (node::Buffer::HasInstance(client_cert.value())) {
      char *val = reinterpret_cast<char *>(node::Buffer::Data(
          client_cert.value()));
      size_t len = node::Buffer::Length(client_cert.value());

      config->client_cert.data = strndup(val, len);
      config->client_cert.len = len;
    } else {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong definition of client_cert")));
    }
  }

  /* client_key parameter */
  auto client_key = js_object_attribute_to_cpp<Local<Value>>(val, "client_key");

  if (client_key) {
    if (node::Buffer::HasInstance(client_key.value())) {
      char *val = reinterpret_cast<char *>(node::Buffer::Data(
          client_key.value()));
      size_t len = node::Buffer::Length(client_key.value());

      config->client_key.data = strndup(val, len);
      config->client_key.len = len;
    } else {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong definition of client_key")));
    }
  }

  /* verify_cert parameter */
  auto verify_cert = js_object_attribute_to_cpp<std::string>(val,
      "verify_cert");

  if (verify_cert) {
    if (verify_cert.value() == "none")
      config->verify_cert = ARTIK_SSL_VERIFY_NONE;
    else if (verify_cert.value() == "optional")
      config->verify_cert = ARTIK_SSL_VERIFY_OPTIONAL;
    else if (verify_cert.value() == "required")
      config->verify_cert = ARTIK_SSL_VERIFY_REQUIRED;
    else
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong definition of verify_cert : expect 'none', 'optional' "
          "or 'required'.")));
  }

  *ssl_config = config;
}

static void cleanup_work(HttpAsyncWork* work) {
  artik_http_headers* headers = work->headers;

  log_dbg("");

  /*
    * properly release any memory allocation performed
    * during headers construction
    */
  if (headers && headers->fields) {
    for (int i=0; i < headers->num_fields; i++) {
      if (headers->fields[i].name)
        free(headers->fields[i].name);
      if (headers->fields[i].data)
        free(headers->fields[i].data);
    }

    free(headers->fields);
    free(headers);
    headers = NULL;
  }

  if (work->body)
    free(work->body);
  if (work->response)
    free(work->response);
  if (work->url)
    free(work->url);

  delete work;
  work = NULL;
}

static int on_http_data(char *data, unsigned int len, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  HttpWrapper* wrap = reinterpret_cast<HttpWrapper*>(user_data);

  log_dbg("");

  if (!wrap->getDataCb())
    return 0;

  Handle<Value> argv[] = {
    Handle<Value>(Nan::CopyBuffer(data, len).ToLocalChecked())
  };

  Local<Function>::New(isolate, *wrap->getDataCb())->Call(
    isolate->GetCurrentContext()->Global(), 1, argv);

  return len;
}

static void on_http_error(artik_error result, void *user_data) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  HttpWrapper* wrap = reinterpret_cast<HttpWrapper*>(user_data);

  log_dbg("");

  if (!wrap->getErrorCb())
    return;

  Handle<Value> argv[] = {
    Handle<Value>(String::NewFromUtf8(isolate, error_msg(result))),
  };

  Local<Function>::New(isolate, *wrap->getErrorCb())->Call(
    isolate->GetCurrentContext()->Global(), 1, argv);
}

static void HttpWorkAsyncGet(uv_work_t *req) {
  HttpAsyncWork* work = static_cast<HttpAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->http->get(work->url, work->headers, &work->response,
      &work->status, work->ssl);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void HttpWorkAsyncPost(uv_work_t *req) {
  HttpAsyncWork* work = static_cast<HttpAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->http->post(work->url, work->headers, work->body,
      &work->response, &work->status, work->ssl);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void HttpWorkAsyncPut(uv_work_t *req) {
  HttpAsyncWork* work = static_cast<HttpAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->http->put(work->url, work->headers, work->body,
      &work->response, &work->status, work->ssl);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void HttpWorkAsyncDel(uv_work_t *req) {
  HttpAsyncWork* work = static_cast<HttpAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->http->del(work->url, work->headers, &work->response,
      &work->status, work->ssl);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void HttpWorkAsyncComplete(uv_work_t *req, int status) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  HttpAsyncWork* work = static_cast<HttpAsyncWork*>(req->data);

  log_dbg("");

  if (!work)
    return;

  /* If an error occurred, return error message as the response */
  if (work->ret != S_OK)
    work->response = strndup(error_msg(work->ret), MAX_ERRR_MSG_LEN);

  /* Prepare the return values */
  Handle<Value> argv[] = {
    Handle<Value>(String::NewFromUtf8(isolate, work->response)),
    Handle<Value>(Int32::New(isolate, work->status))
  };

  /* Call the callback function */
  Local<Function>::New(isolate, work->callback)->Call(
      isolate->GetCurrentContext()->Global(), 2, argv);

  /* Clean up */
  cleanup_work(work);
}

HttpWrapper::HttpWrapper() {
  m_http = new Http();
}

HttpWrapper::~HttpWrapper() {
  delete m_http;
}

void HttpWrapper::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "http"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototypes
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_stream", get_stream);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get", get);
  NODE_SET_PROTOTYPE_METHOD(tpl, "post", post);
  NODE_SET_PROTOTYPE_METHOD(tpl, "put", put);
  NODE_SET_PROTOTYPE_METHOD(tpl, "del", del);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "http"),
      tpl->GetFunction());
}

void HttpWrapper::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.IsConstructCall()) {
    HttpWrapper* obj = new HttpWrapper();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    Local<Context> context = isolate->GetCurrentContext();
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(
        cons->NewInstance(context, 0, NULL).ToLocalChecked());
  }
}

void HttpWrapper::get_stream(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HttpWrapper* obj = ObjectWrap::Unwrap<HttpWrapper>(args.Holder());
  Http* http = obj->getObj();
  artik_http_headers* headers = NULL;
  artik_ssl_config* ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  /* Copy headers */
  if (args[1]->IsArray()) {
    Local<Array> array = Local<Array>::Cast(args[1]);

    if (array->Length() > 2) {
      int num_headers = array->Length()/2;
      artik_http_header_field* fields =
        reinterpret_cast<artik_http_header_field*>(malloc(
        num_headers*sizeof(artik_http_header_field)));
      if (!fields) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      for (int i=0; i < num_headers; i++) {
        v8::String::Utf8Value name(array->Get(i*2)->ToString());
        v8::String::Utf8Value data(array->Get((i*2)+1)->ToString());

        fields[i].name = strndup(*name, MAX_HEADER_SIZE);
        fields[i].data = strndup(*data, MAX_HEADER_SIZE);
      }

      headers = reinterpret_cast<artik_http_headers*>(
        malloc(sizeof(artik_http_headers)));
      if (!headers) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      headers->num_fields = num_headers;
      headers->fields = fields;
    }
  }

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* Data Callback */
  if (!args[3]->IsUndefined()) {
    obj->m_data_cb = new v8::Persistent<v8::Function>();
    obj->m_data_cb->Reset(isolate, Local<Function>::Cast(args[3]));
  }

  /* Error Callback */
  if (!args[4]->IsUndefined()) {
    obj->m_error_cb = new v8::Persistent<v8::Function>();
    obj->m_error_cb->Reset(isolate, Local<Function>::Cast(args[4]));
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  const char *url = *param0;
  artik_error ret = http->get_stream(url, headers, NULL, on_http_data,
    reinterpret_cast<void*>(obj), ssl_config);

  on_http_error(ret, reinterpret_cast<void*>(obj));

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void HttpWrapper::get(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HttpWrapper* obj = ObjectWrap::Unwrap<HttpWrapper>(args.Holder());
  Http* http = obj->getObj();
  artik_http_headers* headers = NULL;
  artik_ssl_config* ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  /* Copy headers */
  if (args[1]->IsArray()) {
    Local<Array> array = Local<Array>::Cast(args[1]);

    if (array->Length() > 2) {
      int num_headers = array->Length() / 2;
      artik_http_header_field *fields =
          reinterpret_cast<artik_http_header_field*>(malloc(
          num_headers * sizeof(artik_http_header_field)));
      if (!fields) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      for (int i = 0; i < num_headers; i++) {
        v8::String::Utf8Value name(array->Get(i*2)->ToString());
        v8::String::Utf8Value data(array->Get((i*2)+1)->ToString());

        fields[i].name = strndup(*name, MAX_HEADER_SIZE);
        fields[i].data = strndup(*data, MAX_HEADER_SIZE);
      }

      headers = reinterpret_cast<artik_http_headers*>(malloc(
          sizeof(artik_http_headers)));
      if (!headers) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      headers->num_fields = num_headers;
      headers->fields = fields;
    }
  }

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[3]->IsUndefined()) {
    v8::String::Utf8Value param0(args[0]->ToString());

    obj->m_work = new HttpAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->http = http;
    obj->m_work->headers = headers;
    obj->m_work->url = strndup(*param0, strlen(*param0));
    obj->m_work->body = NULL;
    obj->m_work->ssl = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[3]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request), HttpWorkAsyncGet,
        HttpWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));

  } else { /* Otherwise make the call directly */
    v8::String::Utf8Value param0(args[0]->ToString());
    const char *url = *param0;
    char *response = NULL;
    artik_error ret = http->get(url, headers, &response, NULL, ssl_config);

    if (ret != S_OK)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void HttpWrapper::post(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HttpWrapper* obj = ObjectWrap::Unwrap<HttpWrapper>(args.Holder());
  Http* http = obj->getObj();
  artik_http_headers* headers = NULL;
  artik_ssl_config* ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  /* Copy headers */
  if (args[1]->IsArray()) {
    Local<Array> array = Local<Array>::Cast(args[1]);

    if (array->Length() > 2) {
      int num_headers = array->Length()/2;
      artik_http_header_field* fields =
          reinterpret_cast<artik_http_header_field*>(malloc(
          num_headers * sizeof(artik_http_header_field)));
      if (!fields) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      for (int i = 0; i < num_headers; i++) {
        v8::String::Utf8Value name(array->Get(i*2)->ToString());
        v8::String::Utf8Value data(array->Get((i*2)+1)->ToString());

        fields[i].name = strndup(*name, MAX_HEADER_SIZE);
        fields[i].data = strndup(*data, MAX_HEADER_SIZE);
      }

      headers = reinterpret_cast<artik_http_headers*>(malloc(
          sizeof(artik_http_headers)));
      if (!headers) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      headers->num_fields = num_headers;
      headers->fields = fields;
    }
  }

  /* SSL Configuration */
  if (!args[3]->IsUndefined() && args[3]->IsObject())
    updateSSLConfig(isolate, args[3], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[4]->IsUndefined()) {
    v8::String::Utf8Value param0(args[0]->ToString());

    obj->m_work = new HttpAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->http = http;
    obj->m_work->headers = headers;
    obj->m_work->url = strndup(*param0, strlen(*param0));
    obj->m_work->ssl = ssl_config;

    /* copy body data if provided */
    if (!args[2]->IsUndefined() && args[2]->IsString()) {
      v8::String::Utf8Value param2(args[2]->ToString());
      obj->m_work->body = strndup(*param2, strlen(*param2));
    } else {
      obj->m_work->body = NULL;
    }

    Local<Function> callback = Local<Function>::Cast(args[4]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        HttpWorkAsyncPost, HttpWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));

  } else { /* Otherwise make the call directly */
    v8::String::Utf8Value param0(args[0]->ToString());
    const char *url = *param0;
    const char *body = NULL;
    char *response = NULL;
    artik_error ret = S_OK;

    /* copy body data if provided */
    if (!args[2]->IsUndefined() && args[2]->IsString()) {
      v8::String::Utf8Value param2(args[2]->ToString());
      body = strndup(*param2, strlen(*param2));
    }

    ret = http->post(url, headers, body, &response, NULL, ssl_config);

    if (ret != S_OK)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void HttpWrapper::put(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HttpWrapper* obj = ObjectWrap::Unwrap<HttpWrapper>(args.Holder());
  Http* http = obj->getObj();
  artik_http_headers* headers = NULL;
  artik_ssl_config* ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  /* Copy headers */
  if (args[1]->IsArray()) {
    Local<Array> array = Local<Array>::Cast(args[1]);

    if (array->Length() > 2) {
      int num_headers = array->Length()/2;
      artik_http_header_field* fields =
          reinterpret_cast<artik_http_header_field*>(malloc(
          num_headers * sizeof(artik_http_header_field)));
      if (!fields) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      for (int i = 0; i < num_headers; i++) {
        v8::String::Utf8Value name(array->Get(i*2)->ToString());
        v8::String::Utf8Value data(array->Get((i*2)+1)->ToString());

        fields[i].name = strndup(*name, MAX_HEADER_SIZE);
        fields[i].data = strndup(*data, MAX_HEADER_SIZE);
      }

      headers = reinterpret_cast<artik_http_headers*>(malloc(
          sizeof(artik_http_headers)));
      if (!headers) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      headers->num_fields = num_headers;
      headers->fields = fields;
    }
  }

  /* SSL Configuration */
  if (!args[3]->IsUndefined() && args[3]->IsObject())
    updateSSLConfig(isolate, args[3], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[4]->IsUndefined()) {
    v8::String::Utf8Value param0(args[0]->ToString());

    obj->m_work = new HttpAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->http = http;
    obj->m_work->headers = headers;
    obj->m_work->url = strndup(*param0, strlen(*param0));
    obj->m_work->ssl = ssl_config;

    /* copy body data if provided */
    if (!args[2]->IsUndefined() && args[2]->IsString()) {
      v8::String::Utf8Value param2(args[2]->ToString());
      obj->m_work->body = strndup(*param2, strlen(*param2));
    } else {
      obj->m_work->body = NULL;
    }

    Local<Function> callback = Local<Function>::Cast(args[4]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        HttpWorkAsyncPut, HttpWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));

  } else { /* Otherwise make the call directly */
    v8::String::Utf8Value param0(args[0]->ToString());
    const char *url = *param0;
    const char *body = NULL;
    char *response = NULL;
    artik_error ret = S_OK;

    /* copy body data if provided */
    if (!args[2]->IsUndefined() && args[2]->IsString()) {
      v8::String::Utf8Value param2(args[2]->ToString());
      body = strndup(*param2, strlen(*param2));
    }

    ret = http->put(url, headers, body, &response, NULL, ssl_config);

    if (ret != S_OK)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void HttpWrapper::del(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  HttpWrapper* obj = ObjectWrap::Unwrap<HttpWrapper>(args.Holder());
  Http* http = obj->getObj();
  artik_http_headers* headers = NULL;
  artik_ssl_config* ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  /* Copy headers */
  if (args[1]->IsArray()) {
    Local<Array> array = Local<Array>::Cast(args[1]);

    if (array->Length() > 2) {
      int num_headers = array->Length()/2;
      artik_http_header_field *fields =
          reinterpret_cast<artik_http_header_field*>(malloc(
          num_headers * sizeof(artik_http_header_field)));
      if (!fields) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      for (int i = 0; i < num_headers; i++) {
        v8::String::Utf8Value name(array->Get(i*2)->ToString());
        v8::String::Utf8Value data(array->Get((i*2)+1)->ToString());

        fields[i].name = strndup(*name, MAX_HEADER_SIZE);
        fields[i].data = strndup(*data, MAX_HEADER_SIZE);
      }

      headers = reinterpret_cast<artik_http_headers*>(malloc(
          sizeof(artik_http_headers)));
      if (!headers) {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
          isolate, "Memory allocation error")));
        return;
      }

      headers->num_fields = num_headers;
      headers->fields = fields;
    }
  }

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[3]->IsUndefined()) {
    v8::String::Utf8Value param0(args[0]->ToString());

    obj->m_work = new HttpAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->http = http;
    obj->m_work->headers = headers;
    obj->m_work->url = strndup(*param0, strlen(*param0));
    obj->m_work->body = NULL;
    obj->m_work->ssl = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[3]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        HttpWorkAsyncDel, HttpWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));

  } else { /* Otherwise make the call directly */
    v8::String::Utf8Value param0(args[0]->ToString());
    const char *url = *param0;
    char *response = NULL;
    artik_error ret = S_OK;

    ret = http->del(url, headers, &response, NULL, ssl_config);

    if (ret != S_OK)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

}  // namespace artik
