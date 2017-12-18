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

#include "security/security.h"

#include <utils.h>

#include "base/ssl_config_converter.h"

namespace artik {

using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Number;
using v8::Persistent;
using v8::String;
using v8::Value;
using v8::Handle;
using v8::Context;

Persistent<Function> SecurityWrapper::constructor;

SecurityWrapper::SecurityWrapper() {
  m_security = new Security();
}

SecurityWrapper::~SecurityWrapper() {
  delete m_security;
}

void SecurityWrapper::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();
  Local<FunctionTemplate> modal = FunctionTemplate::New(isolate, New);

  modal->SetClassName(String::NewFromUtf8(isolate, "security"));
  modal->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototypes
  NODE_SET_PROTOTYPE_METHOD(modal, "get_certificate", get_certificate);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_ca_chain", get_ca_chain);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_key_from_cert", get_key_from_cert);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_random_bytes", get_random_bytes);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_certificate_sn", get_certificate_sn);

  constructor.Reset(isolate, modal->GetFunction());
  exports->Set(v8::String::NewFromUtf8(isolate, "security"),
         modal->GetFunction());
}

void SecurityWrapper::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 0)
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                     isolate, "Wrong number of arguments")));
  if (args.IsConstructCall()) {
    SecurityWrapper* obj = NULL;

    try {
      obj = new SecurityWrapper();
    } catch (artik::ArtikException &e) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
    }
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    Local<Context> context = isolate->GetCurrentContext();
    Local<Function> cons = Local<Function>::New(isolate, constructor);
    args.GetReturnValue().Set(
        cons->NewInstance(context, 0, NULL).ToLocalChecked());
  }
}

void SecurityWrapper::get_certificate(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();
  char *cert = NULL;
  artik_error res = S_OK;

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param0(args[0]->ToString());
  auto cert_id =
    to_artik_parameter<artik_security_certificate_id>(
      SSLConfigConverter::security_certificate_ids, *param0);
  if (!cert_id) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
      "Wrong value of cert_id. ")));
    return;
  }

  try {
    res = obj->get_certificate(cert_id.value(), &cert);
    if (res != S_OK) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, error_msg(res))));
      return;
    }
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, cert));
    free(cert);
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
  }
}

void SecurityWrapper::get_ca_chain(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

    if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  char *chain = NULL;
  artik_error res = S_OK;
  String::Utf8Value param0(args[0]->ToString());
  auto cert_id =
    to_artik_parameter<artik_security_certificate_id>(
      SSLConfigConverter::security_certificate_ids, *param0);
  if (!cert_id) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
      "Wrong value of cert_id. ")));
    return;
  }

  try {
    res = obj->get_ca_chain(cert_id.value(), &chain);
    if (res != S_OK)
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, error_msg(res))));
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, chain));
    free(chain);
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
  }
}

void SecurityWrapper::get_key_from_cert(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 1)
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                     isolate, "Wrong number of arguments")));

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();
  v8::String::Utf8Value param0(args[0]->ToString());
  char *cert = *param0;
  char *key = NULL;
  artik_error res = S_OK;

  try {
    res = obj->get_key_from_cert(cert, &key);
    if (res != S_OK)
      isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, error_msg(res))));
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, key));
    free(key);
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
  }
}

void SecurityWrapper::get_random_bytes(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 1)
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                     isolate, "Wrong number of arguments")));

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  int len = args[0]->NumberValue();
  unsigned char rand[len];
  artik_error res = S_OK;

  try {
    res = obj->get_random_bytes(rand, len);
    if (res != S_OK)
      isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, error_msg(res))));
    args.GetReturnValue().Set(
        Nan::CopyBuffer((const char*)rand, len).ToLocalChecked());
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
  }
}

void SecurityWrapper::get_certificate_sn(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (args[0]->IsUndefined() || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  unsigned int len = ARTIK_CERT_SN_MAXLEN;
  unsigned char sn[len];
  artik_error res = S_OK;
  String::Utf8Value param0(args[0]->ToString());
  auto cert_id =
    to_artik_parameter<artik_security_certificate_id>(
      SSLConfigConverter::security_certificate_ids, *param0);
  if (!cert_id) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
      "Wrong value of cert_id. ")));
    return;
  }

  try {
    res = obj->get_certificate_sn(cert_id.value(), sn, &len);
    if (res != S_OK)
      isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, error_msg(res))));
    args.GetReturnValue().Set(
        Nan::CopyBuffer(reinterpret_cast<char*>(sn), len).ToLocalChecked());
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
  }
}

}  // namespace artik

