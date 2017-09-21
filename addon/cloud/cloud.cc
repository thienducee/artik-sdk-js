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

#include "cloud/cloud.h"

#include <unistd.h>
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

Persistent<Function> CloudWrapper::constructor;

static void updateSSLConfig(Isolate *isolate, Local<Value> val,
    artik_ssl_config **ssl_config) {
  *ssl_config = reinterpret_cast<artik_ssl_config*>(malloc(
      sizeof(artik_ssl_config)));

  memset(*ssl_config, 0, sizeof(artik_ssl_config));

  /* use_se parameter */
  auto use_se = js_object_attribute_to_cpp<bool>(val, "use_se");

  if (use_se)
    (*ssl_config)->use_se = use_se.value();

  /* ca_cert parameter */
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

  /* client_cert parameter */
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

  /* client_key parameter */
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

  /* verify_cert parameter */
  auto verify_cert = js_object_attribute_to_cpp<std::string>(val,
      "verify_cert");

  if (verify_cert) {
    if (verify_cert.value() == "none") {
      (*ssl_config)->verify_cert = ARTIK_SSL_VERIFY_NONE;
    } else if (verify_cert.value() == "optional") {
      (*ssl_config)->verify_cert = ARTIK_SSL_VERIFY_OPTIONAL;
    } else if (verify_cert.value() == "required") {
      (*ssl_config)->verify_cert = ARTIK_SSL_VERIFY_REQUIRED;
    } else {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
          "Wrong value for verify_cert : expect 'none',"
          "'optional' or 'required'.")));
    }
  }
}

static void cleanup_work(CloudAsyncWork* work) {
  log_dbg("");

  if (work->device_id)
    free(work->device_id);
  if (work->device_type_id)
    free(work->device_type_id);
  if (work->reg_id)
    free(work->reg_id);
  if (work->vendor_id)
    free(work->vendor_id);
  if (work->nonce)
    free(work->nonce);
  if (work->app_id)
    free(work->app_id);
  if (work->user_id)
    free(work->user_id);
  if (work->action)
    free(work->action);
  if (work->message)
    free(work->message);
  if (work->response)
    free(work->response);
  if (work->name)
    free(work->name);
  if (work->data)
    free(work->data);
  if (work->ssl_config)
    free(work->ssl_config);

  delete work;
  work = NULL;
}

static void CloudWorkAsyncSdrCompleteRegistration(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->sdr_complete_registration(work->reg_id, work->nonce,
      &work->response);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncSdrRegistrationStatus(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->sdr_registration_status(work->reg_id,
      &work->response);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncSdrStartRegistration(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->sdr_start_registration(work->device_type_id,
      work->vendor_id, &work->response);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncDeleteDeviceToken(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->delete_device_token(work->device_id, &work->response,
      work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncUpdateDeviceToken(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->update_device_token(work->device_id, &work->response,
      work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncGetDeviceToken(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->get_device_token(work->device_id, &work->response,
      work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncGetDevice(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->get_device(work->device_id, work->properties,
      &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncGetUserApplicationProperties(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->get_user_application_properties(work->device_id,
      work->app_id, &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncGetUserDeviceTypes(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->get_user_device_types(work->count, work->shared,
      work->offset, work->user_id, &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncGetUserDevices(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->get_user_devices(work->count, work->properties,
      work->offset, work->user_id, &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncGetCurrentUserProfile(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->get_current_user_profile(&work->response,
      work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncSendAction(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->send_action(work->device_id, work->action,
      &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncSendMessage(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->send_message(work->device_id, work->message,
      &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncAddDevice(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->add_device(work->user_id, work->device_type_id,
      work->name, &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncDeleteDevice(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->delete_device(work->device_id, &work->response,
      work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncGetDeviceProps(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->get_device_properties(work->device_id,
      work->timestamp, &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncSetDeviceServerProps(uv_work_t *req) {
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  work->response = NULL;
  work->ret = work->cloud->set_device_server_properties(work->device_id,
      work->data, &work->response, work->ssl_config);
  if (work->ret == E_INTERRUPTED) {
    cleanup_work(work);
    return;
  }
}

static void CloudWorkAsyncComplete(uv_work_t *req, int status) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  CloudAsyncWork* work = static_cast<CloudAsyncWork*>(req->data);

  log_dbg("");

  if (!work)
    return;

  /* If an error occurred, return error message as the response */
  if (work->ret != S_OK && !work->response)
    work->response = strndup(error_msg(work->ret), MAX_ERRR_MSG_LEN);

  /* Prepare the return values */
  Handle<Value> argv[] = {
    Handle<Value>(String::NewFromUtf8(isolate, work->response)),
  };

  /* Call the callback function */
  Local<Function>::New(isolate, work->callback)->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);

  /* Clean up */
  cleanup_work(work);
}

static void on_receive_callback(void *user_data, void *result) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  CloudWrapper *obj = reinterpret_cast<CloudWrapper *>(user_data);

  log_dbg("");

  if (!obj || !obj->getReceiveCb())
    return;

  Handle<Value> argv[] = {
    Handle<Value>(String::NewFromUtf8(isolate,
        result ? reinterpret_cast<char*>(result) : "null")),
  };

  Local<Function>::New(isolate, *obj->getReceiveCb())->Call(
      isolate->GetCurrentContext()->Global(), 1, argv);
}

static void on_connection_callback(void *user_data, void *result) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  CloudWrapper *obj = reinterpret_cast<CloudWrapper *>(user_data);
  uint32_t ret = reinterpret_cast<uintptr_t>(result);

  log_dbg("");

  if (!obj || !obj->getConnectionCb())
    return;

  if (ret == ARTIK_WEBSOCKET_CLOSED) {
    Handle<Value> argv[] = {
      Handle<Value>(String::NewFromUtf8(isolate, "CLOSED")),
    };
    Local<Function>::New(isolate, *obj->getConnectionCb())->Call(
        isolate->GetCurrentContext()->Global(), 1, argv);
    return;
  } else if (ret == ARTIK_WEBSOCKET_CONNECTED) {
    Handle<Value> argv[] = {
      Handle<Value>(String::NewFromUtf8(isolate, "CONNECTED")),
    };
    Local<Function>::New(isolate, *obj->getConnectionCb())->Call(
        isolate->GetCurrentContext()->Global(), 1, argv);
    return;
  } else if (ret == ARTIK_WEBSOCKET_HANDSHAKE_ERROR) {
    Handle<Value> argv[] = {
      Handle<Value>(String::NewFromUtf8(isolate, "HANDSHAKE ERROR")),
    };
    Local<Function>::New(isolate, *obj->getConnectionCb())->Call(
        isolate->GetCurrentContext()->Global(), 1, argv);
    return;
  }

  log_err("Wrong value for callback result");
}

CloudWrapper::CloudWrapper(const char* token) {
  m_cloud = new Cloud(token);
}

CloudWrapper::~CloudWrapper() {
  delete m_cloud;
}

void CloudWrapper::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  // Prepare constructor template
  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
  tpl->SetClassName(String::NewFromUtf8(isolate, "cloud"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototypes
  NODE_SET_PROTOTYPE_METHOD(tpl, "send_message", send_message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "send_action", send_action);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_current_user_profile",
                            get_current_user_profile);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_user_devices", get_user_devices);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_user_device_types",
                            get_user_device_types);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_user_application_properties",
                            get_user_application_properties);
  NODE_SET_PROTOTYPE_METHOD(tpl, "add_device", add_device);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_device", get_device);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_device_token", get_device_token);
  NODE_SET_PROTOTYPE_METHOD(tpl, "update_device_token", update_device_token);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete_device_token", delete_device_token);
  NODE_SET_PROTOTYPE_METHOD(tpl, "delete_device", delete_device);
  NODE_SET_PROTOTYPE_METHOD(tpl, "get_device_properties",
                            get_device_properties);
  NODE_SET_PROTOTYPE_METHOD(tpl, "set_device_server_properties",
                            set_device_server_properties);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sdr_start_registration",
                            sdr_start_registration);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sdr_registration_status",
                            sdr_registration_status);
  NODE_SET_PROTOTYPE_METHOD(tpl, "sdr_complete_registration",
                            sdr_complete_registration);
  NODE_SET_PROTOTYPE_METHOD(tpl, "websocket_open_stream",
                            websocket_open_stream);
  NODE_SET_PROTOTYPE_METHOD(tpl, "websocket_send_message",
                            websocket_send_message);
  NODE_SET_PROTOTYPE_METHOD(tpl, "websocket_close_stream",
                            websocket_close_stream);

  constructor.Reset(isolate, tpl->GetFunction());
  exports->Set(String::NewFromUtf8(isolate, "cloud"),
               tpl->GetFunction());
}

void CloudWrapper::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  char* token = NULL;
  CloudWrapper* obj = NULL;

  if (args.IsConstructCall()) {
    if (!args[0]->IsUndefined() && args[0]->IsString()) {
        v8::String::Utf8Value param0(args[0]->ToString());
        token = *param0;
        obj = new CloudWrapper(token);
    } else {
      obj = new CloudWrapper(NULL);
    }

    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };

    Local<Context> context = isolate->GetCurrentContext();
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(
        cons->NewInstance(context, args.Length(), argv).ToLocalChecked());
  }
}

void CloudWrapper::send_message(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()  ||
    args[1]->IsUndefined() || !args[1]->IsString() ) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  char *device_id = *param0;
  v8::String::Utf8Value param1(args[1]->ToString());
  char *message = *param1;

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[3]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(device_id, strlen(device_id));
    obj->m_work->message = strndup(message, strlen(message));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[3]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncSendMessage, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    char *response = NULL;
    artik_error ret = cloud->send_message(device_id, message, &response,
        ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::send_action(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()  ||
    args[1]->IsUndefined() || !args[1]->IsString() ) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  char *device_id = *param0;
  v8::String::Utf8Value param1(args[1]->ToString());
  char *action = *param1;

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[3]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(device_id, strlen(device_id));
    obj->m_work->action = strndup(action, strlen(action));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[3]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncSendAction, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    char *response = NULL;
    artik_error ret = cloud->send_action(device_id, action, &response,
        ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::get_current_user_profile(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  /* SSL Configuration */
  if (!args[0]->IsUndefined() && args[0]->IsObject())
    updateSSLConfig(isolate, args[0], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[1]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[1]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncGetCurrentUserProfile, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    char *response = NULL;
    artik_error ret = cloud->get_current_user_profile(&response, ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::get_user_devices(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsNumber()  ||
    args[1]->IsUndefined() || !args[1]->IsBoolean() ||
    args[2]->IsUndefined() || !args[2]->IsNumber()  ||
    args[3]->IsUndefined() || !args[3]->IsString()  ) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
        return;
  }

  int count = args[0]->IntegerValue();
  bool properties = args[1]->BooleanValue();
  int offset = args[2]->IntegerValue();
  v8::String::Utf8Value param3(args[3]->ToString());
  char *user_id = *param3;

  /* SSL Configuration */
  if (!args[4]->IsUndefined() && args[4]->IsObject())
    updateSSLConfig(isolate, args[4], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[5]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->user_id = strndup(user_id, strlen(user_id));
    obj->m_work->properties = properties;
    obj->m_work->count = count;
    obj->m_work->offset = offset;
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[5]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncGetUserDevices, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    char *response = NULL;
    artik_error ret = cloud->get_user_devices(count, properties, offset,
        user_id, &response, ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::get_user_device_types(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsNumber()  ||
    args[1]->IsUndefined() || !args[1]->IsBoolean() ||
    args[2]->IsUndefined() || !args[2]->IsNumber()  ||
    args[3]->IsUndefined() || !args[3]->IsString()  ) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  int count = args[0]->IntegerValue();
  bool shared = args[1]->BooleanValue();
  int offset = args[2]->IntegerValue();
  v8::String::Utf8Value param3(args[3]->ToString());
  char *user_id = *param3;

  /* SSL Configuration */
  if (!args[4]->IsUndefined() && args[4]->IsObject())
    updateSSLConfig(isolate, args[4], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[5]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->user_id = strndup(user_id, strlen(user_id));
    obj->m_work->shared = shared;
    obj->m_work->count = count;
    obj->m_work->offset = offset;
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[5]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncGetUserDeviceTypes, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    char *response = NULL;
    artik_error ret = cloud->get_user_device_types(count, shared, offset,
        user_id, &response, ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::get_user_application_properties(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString() ||
    args[1]->IsUndefined() || !args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  v8::String::Utf8Value param1(args[1]->ToString());

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[3]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(*param0, strlen(*param0));
    obj->m_work->app_id = strndup(*param1, strlen(*param1));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[3]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncGetUserApplicationProperties, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_id = *param0;
    const char *app_id = *param1;
    char *response = NULL;
    artik_error ret = cloud->get_user_application_properties(device_id, app_id,
        &response, ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::get_device(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  bool properties = false;
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());

  if (!args[1]->IsUndefined() && args[1]->IsBoolean())
    properties = args[1]->BooleanValue();

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[3]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(*param0, strlen(*param0));
    obj->m_work->properties = properties;
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[3]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncGetDevice, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_id = *param0;
    char *response = NULL;
    artik_error ret = cloud->get_device(device_id, properties, &response,
        ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::get_device_token(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());

  /* SSL Configuration */
  if (!args[1]->IsUndefined() && args[1]->IsObject())
    updateSSLConfig(isolate, args[1], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[2]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(*param0, strlen(*param0));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[2]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncGetDeviceToken, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_id = *param0;
    char *response = NULL;
    artik_error ret = cloud->get_device_token(device_id, &response, ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::add_device(const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString() ||
    args[1]->IsUndefined() || !args[1]->IsString() ||
    args[2]->IsUndefined() || !args[2]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  v8::String::Utf8Value param1(args[1]->ToString());
  v8::String::Utf8Value param2(args[2]->ToString());

  /* SSL Configuration */
  if (!args[3]->IsUndefined() && args[3]->IsObject())
    updateSSLConfig(isolate, args[3], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[4]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->user_id = strndup(*param0, strlen(*param0));
    obj->m_work->device_type_id = strndup(*param1, strlen(*param1));
    obj->m_work->name = strndup(*param2, strlen(*param2));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[4]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncAddDevice, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *user_id = *param0;
    const char *dt_id = *param1;
    const char *name = *param2;
    char *response = NULL;
    artik_error ret = cloud->add_device(user_id, dt_id, name, &response,
        ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::update_device_token(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());

  /* SSL Configuration */
  if (!args[1]->IsUndefined() && args[1]->IsObject())
    updateSSLConfig(isolate, args[1], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[2]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(*param0, strlen(*param0));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[2]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncUpdateDeviceToken, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_id = *param0;
    char *response = NULL;
    artik_error ret = cloud->update_device_token(device_id, &response,
        ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::delete_device_token(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());

  /* SSL Configuration */
  if (!args[1]->IsUndefined() && args[1]->IsObject())
    updateSSLConfig(isolate, args[1], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[2]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(*param0, strlen(*param0));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[2]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncDeleteDeviceToken, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_id = *param0;
    char *response = NULL;
    artik_error ret = cloud->delete_device_token(device_id, &response,
        ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::delete_device(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());

  /* SSL Configuration */
  if (!args[1]->IsUndefined() && args[1]->IsObject())
    updateSSLConfig(isolate, args[1], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[2]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(*param0, strlen(*param0));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[2]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncDeleteDevice, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_id = *param0;
    char *response = NULL;
    artik_error ret = cloud->delete_device(device_id, &response, ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::get_device_properties(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud *cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());

  if (args[1]->IsUndefined() || !args[1]->IsBoolean()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  bool timestamp = args[1]->BooleanValue();

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[3]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(*param0, strlen(*param0));
    obj->m_work->timestamp = timestamp;
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[3]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncGetDeviceProps, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_id = *param0;
    char *response = NULL;
    artik_error ret = cloud->get_device_properties(device_id, timestamp,
                                                  &response, ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::set_device_server_properties(
      const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud *cloud = obj->getObj();
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());

  if (args[1]->IsUndefined() || !args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param1(args[1]->ToString());

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  /* If callback is provided, make the call asynchronous */
  if (!args[3]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_id = strndup(*param0, strlen(*param0));
    obj->m_work->data = strndup(*param1, strlen(*param1));
    obj->m_work->ssl_config = ssl_config;

    Local<Function> callback = Local<Function>::Cast(args[3]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncSetDeviceServerProps, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_id = *param0;
    const char *data = *param1;
    char *response = NULL;
    artik_error ret = cloud->set_device_server_properties(device_id, data,
                                                  &response, ssl_config);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::sdr_start_registration(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString() ||
    args[1]->IsUndefined() || !args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  v8::String::Utf8Value param1(args[1]->ToString());

  /* If callback is provided, make the call asynchronous */
  if (!args[2]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->device_type_id = strndup(*param0, strlen(*param0));
    obj->m_work->vendor_id = strndup(*param1, strlen(*param1));

    Local<Function> callback = Local<Function>::Cast(args[2]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncSdrStartRegistration, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *device_type_id = *param0;
    const char *vendor_id = *param1;
    char *response = NULL;
    artik_error ret = cloud->sdr_start_registration(device_type_id, vendor_id,
        &response);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::sdr_registration_status(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());

  /* If callback is provided, make the call asynchronous */
  if (!args[1]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->reg_id = strndup(*param0, strlen(*param0));

    Local<Function> callback = Local<Function>::Cast(args[1]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncSdrRegistrationStatus, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *reg_id = *param0;
    char *response = NULL;
    artik_error ret = cloud->sdr_registration_status(reg_id, &response);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::sdr_complete_registration(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString() ||
    args[1]->IsUndefined() || !args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  v8::String::Utf8Value param1(args[1]->ToString());

  /* If callback is provided, make the call asynchronous */
  if (!args[2]->IsUndefined()) {
    obj->m_work = new CloudAsyncWork();
    obj->m_work->request.data = obj->m_work;
    obj->m_work->cloud = cloud;
    obj->m_work->reg_id = strndup(*param0, strlen(*param0));
    obj->m_work->nonce = strndup(*param1, strlen(*param1));

    Local<Function> callback = Local<Function>::Cast(args[2]);
    obj->m_work->callback.Reset(isolate, callback);

    uv_queue_work(uv_default_loop(), &(obj->m_work->request),
        CloudWorkAsyncSdrCompleteRegistration, CloudWorkAsyncComplete);

    args.GetReturnValue().Set(Undefined(isolate));
  } else { /* Otherwise make the call directly */
    const char *reg_id = *param0;
    const char *nonce = *param1;
    char *response = NULL;
    artik_error ret = cloud->sdr_complete_registration(reg_id, nonce,
        &response);

    if (ret != S_OK && !response)
      response = strndup(error_msg(ret), MAX_ERRR_MSG_LEN);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, response));

    free(response);
  }
}

void CloudWrapper::websocket_open_stream(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_error ret = S_OK;
  artik_ssl_config *ssl_config = NULL;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString() ||
    args[1]->IsUndefined() || !args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  v8::String::Utf8Value param1(args[1]->ToString());

  char *token = *param0;
  char *device_id = *param1;

  /* SSL Configuration */
  if (!args[2]->IsUndefined() && args[2]->IsObject())
    updateSSLConfig(isolate, args[2], &ssl_config);

  ret = cloud->websocket_open_stream(token, device_id, ssl_config);

  /* If a callback is provided, use it for notification */
  if (ret == S_OK && !args[3]->IsUndefined()) {
    obj->m_receive_cb = new v8::Persistent<v8::Function>();
    obj->m_receive_cb->Reset(isolate, Local<Function>::Cast(args[3]));
    cloud->websocket_set_receive_callback(on_receive_callback,
        reinterpret_cast<void*>(obj));
  }

  if (ret == S_OK && !args[4]->IsUndefined()) {
    obj->m_connection_cb = new v8::Persistent<v8::Function>();
    obj->m_connection_cb->Reset(isolate, Local<Function>::Cast(args[4]));
    cloud->websocket_set_connection_callback(on_connection_callback,
        reinterpret_cast<void*>(obj));
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void CloudWrapper::websocket_send_message(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  v8::String::Utf8Value param0(args[0]->ToString());
  char *msg = *param0;
  ret = cloud->websocket_send_message(msg);

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

void CloudWrapper::websocket_close_stream(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  Isolate* isolate = args.GetIsolate();
  CloudWrapper* obj = ObjectWrap::Unwrap<CloudWrapper>(args.Holder());
  Cloud* cloud = obj->getObj();
  artik_error ret = S_OK;

  log_dbg("");

  ret = cloud->websocket_close_stream();
  if (ret == S_OK) {
    delete(obj->m_receive_cb);
    obj->m_receive_cb = NULL;
    delete(obj->m_connection_cb);
    obj->m_connection_cb = NULL;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(ret)));
}

}  // namespace artik
