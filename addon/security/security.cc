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


#include <utils.h>
#include <artik_log.h>

#include <iostream>
#include <string>
#include <unordered_map>

#include "security/security.h"
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
using v8::Array;
using v8::Handle;
using v8::Int32;
using v8::Boolean;
using v8::HandleScope;
using v8::Context;
using v8::Script;

Persistent<Function> SecurityWrapper::constructor;

const std::unordered_map<std::string, see_hash_mode>
SecurityWrapper::hash_mode = {
  { "sha1", HASH_SHA1_160 },
  { "sha256", HASH_SHA2_256 },
  { "sha384", HASH_SHA2_384 },
  { "sha512", HASH_SHA2_512 }
};

const std::unordered_map<std::string, see_aes_mode>
SecurityWrapper::aes_mode = {
  { "aes_ecb_nopad", AES_ECB_NOPAD },
  { "aes_ecb_pkcs7", AES_ECB_PKCS7 },
  { "aes_cbc_nopad", AES_CBC_NOPAD },
  { "aes_cbc_pkcs7", AES_CBC_PKCS7 },
  { "aes_ctr_nopad", AES_CTR_NOPAD }
};

const std::unordered_map<std::string, see_rsa_mode>
SecurityWrapper::rsa_mode = {
  { "rsaes_1024_pkcs1_v1_5", RSAES_1024_PKCS1_V1_5 },
  { "rsaes_2048_pkcs1_v1_5", RSAES_2048_PKCS1_V1_5 },
  { "rsassa_1024_pkcs1_v1_5_sha160", RSASSA_1024_PKCS1_V1_5_SHA160 },
  { "rsassa_1024_pkcs1_v1_5_sha256", RSASSA_1024_PKCS1_V1_5_SHA256 },
  { "rsassa_1024_pkcs1_v1_5_sha384", RSASSA_1024_PKCS1_V1_5_SHA384 },
  { "rsassa_1024_pkcs1_v1_5_sha512", RSASSA_1024_PKCS1_V1_5_SHA512 },
  { "rsassa_2048_pkcs1_v1_5_sha160", RSASSA_2048_PKCS1_V1_5_SHA160 },
  { "rsassa_2048_pkcs1_v1_5_sha256", RSASSA_2048_PKCS1_V1_5_SHA256 },
  { "rsassa_2048_pkcs1_v1_5_sha384", RSASSA_2048_PKCS1_V1_5_SHA384 },
  { "rsassa_2048_pkcs1_v1_5_sha512", RSASSA_2048_PKCS1_V1_5_SHA512 },
  { "rsassa_1024_pkcs1_pss_mgf1_sha160", RSASSA_1024_PKCS1_PSS_MGF1_SHA160 },
  { "rsassa_1024_pkcs1_pss_mgf1_sha256", RSASSA_1024_PKCS1_PSS_MGF1_SHA256 },
  { "rsassa_1024_pkcs1_pss_mgf1_sha384", RSASSA_1024_PKCS1_PSS_MGF1_SHA384 },
  { "rsassa_1024_pkcs1_pss_mgf1_sha512", RSASSA_1024_PKCS1_PSS_MGF1_SHA512 },
  { "rsassa_2048_pkcs1_pss_mgf1_sha160", RSASSA_2048_PKCS1_PSS_MGF1_SHA160 },
  { "rsassa_2048_pkcs1_pss_mgf1_sha256", RSASSA_2048_PKCS1_PSS_MGF1_SHA256 },
  { "rsassa_2048_pkcs1_pss_mgf1_sha384", RSASSA_2048_PKCS1_PSS_MGF1_SHA384 },
  { "rsassa_2048_pkcs1_pss_mgf1_sha512", RSASSA_2048_PKCS1_PSS_MGF1_SHA512 },
};

static void security_cb_return(artik_error result,
  artik_time * signing_time_out,
  const v8::Persistent<v8::Function>& return_cb) {
  Isolate * isolate = Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);
  char str[128] = {0};

  if (return_cb.IsEmpty())
    return;

  if (signing_time_out != NULL) {
    snprintf(str, sizeof(str),
    "new Date(Date.UTC(%d, %.2d, %.2d, %.2d, %.2d, %.2d))",
    signing_time_out->year, signing_time_out->month-1, signing_time_out->day,
    signing_time_out->hour, signing_time_out->minute, signing_time_out->second);

    Handle<String> src = String::NewFromUtf8(isolate,
      reinterpret_cast<char*>(str));
    Handle<Script> script = Script::Compile(src);
    Handle<Value> pkcs7_signing_date = script->Run();
    Handle<Value> argv[] = {
      Handle<Value>(v8::Integer::New(isolate, result)),
      Handle<Value>(String::NewFromUtf8(isolate, error_msg(result))),
      pkcs7_signing_date
    };

    Local<Function>::New(isolate, return_cb)->Call(
      isolate->GetCurrentContext()->Global(), 3, argv);
  } else {
    Handle<Value> argv[] = {
      Handle<Value>(v8::Integer::New(isolate, result)),
      Handle<Value>(String::NewFromUtf8(isolate, error_msg(result))),
    };

    Local<Function>::New(isolate, return_cb)->Call(
      isolate->GetCurrentContext()->Global(), 2, argv);
  }
}

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
  NODE_SET_PROTOTYPE_METHOD(modal, "get_certificate_sn", get_certificate_sn);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_certificate_pem_chain",
    get_certificate_pem_chain);
  NODE_SET_PROTOTYPE_METHOD(modal, "set_certificate", set_certificate);
  NODE_SET_PROTOTYPE_METHOD(modal, "remove_certificate", remove_certificate);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_hash", get_hash);
  NODE_SET_PROTOTYPE_METHOD(modal, "generate_key", generate_key);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_hmac", get_hmac);
  NODE_SET_PROTOTYPE_METHOD(modal, "remove_key", remove_key);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_rsa_signature", get_rsa_signature);
  NODE_SET_PROTOTYPE_METHOD(modal, "verify_rsa_signature",
    verify_rsa_signature);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_ecdsa_signature", get_ecdsa_signature);
  NODE_SET_PROTOTYPE_METHOD(modal, "verify_ecdsa_signature",
    verify_ecdsa_signature);
  NODE_SET_PROTOTYPE_METHOD(modal, "set_key", set_key);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_publickey", get_publickey);
  NODE_SET_PROTOTYPE_METHOD(modal, "write_secure_storage",
    write_secure_storage);
  NODE_SET_PROTOTYPE_METHOD(modal, "read_secure_storage", read_secure_storage);
  NODE_SET_PROTOTYPE_METHOD(modal, "remove_secure_storage",
    remove_secure_storage);
  NODE_SET_PROTOTYPE_METHOD(modal, "aes_encryption", aes_encryption);
  NODE_SET_PROTOTYPE_METHOD(modal, "aes_decryption", aes_decryption);
  NODE_SET_PROTOTYPE_METHOD(modal, "rsa_encryption", rsa_encryption);
  NODE_SET_PROTOTYPE_METHOD(modal, "rsa_decryption", rsa_decryption);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_ec_pubkey_from_cert",
    get_ec_pubkey_from_cert);
  NODE_SET_PROTOTYPE_METHOD(modal, "get_random_bytes", get_random_bytes);
  NODE_SET_PROTOTYPE_METHOD(modal, "verify_signature_init",
    verify_signature_init);
  NODE_SET_PROTOTYPE_METHOD(modal, "verify_signature_update",
    verify_signature_update);
  NODE_SET_PROTOTYPE_METHOD(modal, "verify_signature_final",
    verify_signature_final);
  NODE_SET_PROTOTYPE_METHOD(modal, "convert_pem_to_der",
    convert_pem_to_der);
  NODE_SET_PROTOTYPE_METHOD(modal, "generate_dhm_params",
    generate_dhm_params);
  NODE_SET_PROTOTYPE_METHOD(modal, "set_dhm_params",
    set_dhm_params);
  NODE_SET_PROTOTYPE_METHOD(modal, "compute_dhm_params",
    compute_dhm_params);
  NODE_SET_PROTOTYPE_METHOD(modal, "generate_ecdh_params",
    generate_ecdh_params);
  NODE_SET_PROTOTYPE_METHOD(modal, "compute_ecdh_params",
    compute_ecdh_params);

  constructor.Reset(isolate, modal->GetFunction());
  exports->Set(v8::String::NewFromUtf8(isolate, "security"),
         modal->GetFunction());
}

void SecurityWrapper::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 0) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                     isolate, "Wrong number of arguments")));
    return;
  }
  if (args.IsConstructCall()) {
    SecurityWrapper* obj = NULL;

    try {
      obj = new SecurityWrapper();
    } catch (artik::ArtikException &e) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
      return;
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
  char *cert_name = NULL;
  unsigned char *cert = NULL;
  unsigned int cert_size = 0;
  artik_error res = S_OK;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param0(args[0]->ToString());
  String::Utf8Value param1(args[1]->ToString());

  auto cert_type =
    to_artik_parameter<artik_security_cert_type_t>(
      SSLConfigConverter::security_cert_type_t, *param1);
  if (!cert_type) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
      "Wrong value of cert_type.")));
    return;
  }

  cert_name = *param0;

  try {
    res = obj->get_certificate(cert_name, cert_type.value(), &cert, &cert_size);
    if (res != S_OK) {
      isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, error_msg(res))));
      return;
    }
    args.GetReturnValue().Set(Nan::CopyBuffer(
      reinterpret_cast<const char *>(cert), cert_size).ToLocalChecked());
    delete(cert);
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
    return;
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

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  unsigned int len = ARTIK_CERT_SN_MAXLEN;
  unsigned char sn[len];
  char *pem_cert = NULL;
  artik_error res = S_OK;
  String::Utf8Value param0(args[0]->ToString());

  pem_cert = *param0;

  try {
    res = obj->get_certificate_sn(pem_cert, sn, &len);

    if (res != S_OK) {
      isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, error_msg(res))));
      return;
    }

    args.GetReturnValue().Set(
        Nan::CopyBuffer(reinterpret_cast<char*>(sn), len).ToLocalChecked());
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
  }
}

void SecurityWrapper::get_certificate_pem_chain(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  unsigned int i = 0;

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  char *cert_name = NULL;
  artik_list *chain = NULL;

  artik_error res = S_OK;
  String::Utf8Value param0(args[0]->ToString());

  cert_name = *param0;

  res = obj->get_certificate_pem_chain(cert_name, &chain);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  Local<Array> array = Nan::New<v8::Array>();

  for (i = 0; i < artik_list_size(chain); i++) {
    artik_list *cert = artik_list_get_by_pos(chain, i);
    array->Set(i, String::NewFromUtf8(isolate, (const char *)cert->data));
  }

  args.GetReturnValue().Set(array);

  artik_list_delete_all(&chain);
}

void SecurityWrapper::set_certificate(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  char *cert_name = NULL;
  unsigned char *sample_cert = NULL;

  artik_error res = S_OK;
  String::Utf8Value param0(args[0]->ToString());
  String::Utf8Value param1(args[1]->ToString());

  cert_name = *param0;
  sample_cert = (unsigned char *)*param1;

  res = obj->set_certificate(cert_name, sample_cert,
    strlen(reinterpret_cast<char *>(sample_cert)));

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::remove_certificate(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  char *cert_name = NULL;

  artik_error res = S_OK;
  String::Utf8Value param0(args[0]->ToString());

  cert_name = *param0;

  res = obj->remove_certificate(cert_name);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::get_hash(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  unsigned char *hash = NULL;
  unsigned int hash_size = 0;
  unsigned int sample_size = 0;
  see_hash_mode hash_algorithm;
  unsigned char *sample_key = NULL;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param0(args[0]->ToString());

  auto it = hash_mode.find(*param0);
  if (it != hash_mode.end()) {
    hash_algorithm = it->second;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  sample_key = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[1]));
  sample_size = node::Buffer::Length(args[1]);

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  res = obj->get_hash(hash_algorithm, sample_key, sample_size, &hash,
    &hash_size);
  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(hash), hash_size).ToLocalChecked());
  delete(hash);
}

void SecurityWrapper::generate_key(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  char *const_key_name = NULL;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param0(args[0]->ToString());

  auto optio_algo =  SSLConfigConverter::convert_see_algo(*param0);
  if (!optio_algo.first) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  const_key_name = *param1;

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  res = obj->generate_key(optio_algo.second, const_key_name, NULL);
  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::get_hmac(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  unsigned char *hmac = NULL;
  unsigned int hmac_size = 0;
  unsigned int sample_size = 0;
  see_hash_mode hmac_algorithm;
  unsigned char *sample_key = NULL;
  char *hmac_key_name = NULL;

  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param0(args[0]->ToString());

  auto it = hash_mode.find(*param0);
  if (it != hash_mode.end()) {
    hmac_algorithm = it->second;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  hmac_key_name = *param1;
  sample_key = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  sample_size = node::Buffer::Length(args[2]);

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  res = obj->get_hmac(hmac_algorithm, hmac_key_name, sample_key, sample_size,
    &hmac, &hmac_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(hmac), hmac_size).ToLocalChecked());
  delete(hmac);
}

void SecurityWrapper::remove_key(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  char *const_key_name = NULL;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto optio_algo =  SSLConfigConverter::convert_see_algo(*param0);
  if (!optio_algo.first) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  const_key_name = *param1;

  res = obj->remove_key(optio_algo.second, const_key_name);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::get_rsa_signature(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  char *rsa_key_name = NULL;
  unsigned char *hash = NULL;
  unsigned int hash_size = 0;
  unsigned char *rsa_sig = NULL;
  unsigned int rsa_sig_size = 0;
  see_rsa_mode rsa_key_algorithm;

  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto it = rsa_mode.find(*param0);
  if (it != rsa_mode.end()) {
    rsa_key_algorithm = it->second;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  rsa_key_name = *param1;
  hash = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  hash_size = node::Buffer::Length(args[2]);

  res = obj->get_rsa_signature(rsa_key_algorithm, rsa_key_name, hash, hash_size,
    0, &rsa_sig, &rsa_sig_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(rsa_sig), rsa_sig_size).ToLocalChecked());
  delete(rsa_sig);
}

void SecurityWrapper::verify_rsa_signature(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  char *rsa_key_name = NULL;
  unsigned char *hash = NULL;
  unsigned int hash_size = 0;
  unsigned char *rsa_sig = NULL;
  unsigned int rsa_sig_size = 0;
  see_rsa_mode rsa_key_algorithm;

  if (args.Length() != 4) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[3]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto it = rsa_mode.find(*param0);
  if (it != rsa_mode.end()) {
    rsa_key_algorithm = it->second;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  rsa_key_name = *param1;
  hash = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  hash_size = node::Buffer::Length(args[2]);
  rsa_sig = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[3]));
  rsa_sig_size = node::Buffer::Length(args[3]);

  res = obj->verify_rsa_signature(rsa_key_algorithm, rsa_key_name, hash,
    hash_size, 0, rsa_sig, rsa_sig_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::get_ecdsa_signature(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  char *ecdsa_key_name = NULL;
  unsigned char *hash = NULL;
  unsigned int hash_size = 0;
  unsigned char *ecdsa_sig = NULL;
  unsigned int ecdsa_sig_size = 0;

  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto optio_algo =  SSLConfigConverter::convert_see_algo(*param0);
  if (!optio_algo.first) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  ecdsa_key_name = *param1;
  hash = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  hash_size = node::Buffer::Length(args[2]);

  res = obj->get_ecdsa_signature(optio_algo.second, ecdsa_key_name, hash,
    hash_size, &ecdsa_sig, &ecdsa_sig_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(ecdsa_sig),
    ecdsa_sig_size).ToLocalChecked());
  delete(ecdsa_sig);
}

void SecurityWrapper::verify_ecdsa_signature(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  char *ecdsa_key_name = NULL;
  unsigned char *hash = NULL;
  unsigned int hash_size = 0;
  unsigned char *ecdsa_sig = NULL;
  unsigned int ecdsa_sig_size = 0;

  if (args.Length() != 4) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[3]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto optio_algo =  SSLConfigConverter::convert_see_algo(*param0);
  if (!optio_algo.first) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  ecdsa_key_name = *param1;
  hash = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  hash_size = node::Buffer::Length(args[2]);
  ecdsa_sig = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[3]));
  ecdsa_sig_size = node::Buffer::Length(args[3]);

  res = obj->verify_ecdsa_signature(optio_algo.second, ecdsa_key_name, hash,
    hash_size, ecdsa_sig, ecdsa_sig_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::set_key(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  char * key_name = NULL;
  unsigned int sample_size = 0;
  unsigned char *sample_key = NULL;

  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto optio_algo =  SSLConfigConverter::convert_see_algo(*param0);
  if (!optio_algo.first) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  key_name = *param1;
  sample_key = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  sample_size = node::Buffer::Length(args[2]);

  res = obj->set_key(optio_algo.second, key_name, sample_key, sample_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::get_publickey(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  char * key_name = NULL;
  unsigned int publickey_size = 0;
  unsigned char *publickey = NULL;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto optio_algo =  SSLConfigConverter::convert_see_algo(*param0);
  if (!optio_algo.first) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  key_name = *param1;

  res = obj->get_publickey(optio_algo.second, key_name, &publickey,
    &publickey_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(publickey),
    publickey_size).ToLocalChecked());
  delete(publickey);
}

void SecurityWrapper::write_secure_storage(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  char *name_storage = NULL;
  unsigned int offset_storage = 0;
  unsigned char *data_write_storage = NULL;
  unsigned int data_write_size = 0;

  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());
  name_storage = *param0;
  offset_storage = args[1]->IntegerValue();
  data_write_storage = reinterpret_cast<unsigned char*>
  (node::Buffer::Data(args[2]));
  data_write_size = node::Buffer::Length(args[2]);

  res = obj->write_secure_storage(name_storage, offset_storage,
    data_write_storage, data_write_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::read_secure_storage(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  char *name_storage = NULL;
  unsigned int offset_storage = 0;
  unsigned int data_read_size = 0;
  unsigned char *read_buf = 0;
  unsigned int read_size = 0;

  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());
  name_storage = *param0;
  offset_storage = args[1]->IntegerValue();
  data_read_size = args[2]->IntegerValue();

  res = obj->read_secure_storage(name_storage, offset_storage, data_read_size,
    &read_buf, &read_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(read_buf), read_size).ToLocalChecked());
  delete(read_buf);
}

void SecurityWrapper::remove_secure_storage(
  const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  char *name_storage = NULL;

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());
  name_storage = *param0;

  res = obj->remove_secure_storage(name_storage);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(String::NewFromUtf8(isolate, error_msg(res)));
}

void SecurityWrapper::aes_encryption(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  see_aes_mode aes_enc_mode;
  char *aes_enc_name = NULL;
  unsigned char *iv = NULL;
  unsigned int iv_size = 0;
  unsigned char *sample_key = NULL;
  unsigned int sample_key_size = 0;
  unsigned char *aes_enc_data = NULL;
  unsigned int aes_enc_data_size = 0;

  if (args.Length() != 4) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[3]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto it = aes_mode.find(*param0);
  if (it != aes_mode.end()) {
    aes_enc_mode = it->second;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  aes_enc_name = *param1;

  iv = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  iv_size = node::Buffer::Length(args[2]);

  sample_key = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[3]));
  sample_key_size = node::Buffer::Length(args[3]);

  res = obj->aes_encryption(aes_enc_mode, aes_enc_name, iv, iv_size, sample_key,
    sample_key_size, &aes_enc_data, &aes_enc_data_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(aes_enc_data),
    aes_enc_data_size).ToLocalChecked());
  delete(aes_enc_data);
}

void SecurityWrapper::aes_decryption(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  see_aes_mode aes_enc_mode;
  char *aes_enc_name = NULL;
  unsigned char *iv = NULL;
  unsigned int iv_size = 0;
  unsigned char *aes_enc_data = NULL;
  unsigned int aes_enc_data_size = 0;
  unsigned char *aes_dec_data = NULL;
  unsigned int aes_dec_data_size = 0;

  if (args.Length() != 4) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[3]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto it = aes_mode.find(*param0);
  if (it != aes_mode.end()) {
    aes_enc_mode = it->second;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  aes_enc_name = *param1;

  iv = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  iv_size = node::Buffer::Length(args[2]);

  aes_enc_data = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[3]));
  aes_enc_data_size = node::Buffer::Length(args[3]);

  res = obj->aes_decryption(aes_enc_mode, aes_enc_name, iv, iv_size,
    aes_enc_data, aes_enc_data_size, &aes_dec_data, &aes_dec_data_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(aes_dec_data),
    aes_dec_data_size).ToLocalChecked());
  delete(aes_dec_data);
}

void SecurityWrapper::rsa_encryption(const FunctionCallbackInfo<Value>& args) {
Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  see_rsa_mode rsa_enc_mode;
  char *rsa_enc_name = NULL;
  unsigned char *sample_key = NULL;
  unsigned int sample_key_size = 0;
  unsigned char *rsa_enc_data = NULL;
  unsigned int rsa_enc_data_size = 0;

  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto it = rsa_mode.find(*param0);
  if (it != rsa_mode.end()) {
    rsa_enc_mode = it->second;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  rsa_enc_name = *param1;

  sample_key = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  sample_key_size = node::Buffer::Length(args[2]);

  res = obj->rsa_encryption(rsa_enc_mode, rsa_enc_name, sample_key,
    sample_key_size, &rsa_enc_data, &rsa_enc_data_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(rsa_enc_data),
    rsa_enc_data_size).ToLocalChecked());
  delete(rsa_enc_data);
}

void SecurityWrapper::rsa_decryption(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  see_rsa_mode rsa_enc_mode;
  char *rsa_enc_name = NULL;
  unsigned char *rsa_enc_data = NULL;
  unsigned int rsa_enc_data_size = 0;
  unsigned char *rsa_dec_data = NULL;
  unsigned int rsa_dec_data_size = 0;

  if (args.Length() != 3) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[2]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());

  auto it = rsa_mode.find(*param0);
  if (it != rsa_mode.end()) {
    rsa_enc_mode = it->second;
  } else {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  rsa_enc_name = *param1;

  rsa_enc_data = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[2]));
  rsa_enc_data_size = node::Buffer::Length(args[2]);

  res = obj->rsa_decryption(rsa_enc_mode, rsa_enc_name, rsa_enc_data,
    rsa_enc_data_size, &rsa_dec_data, &rsa_dec_data_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(rsa_dec_data),
    rsa_dec_data_size).ToLocalChecked());
  delete(rsa_dec_data);
}


void SecurityWrapper::get_ec_pubkey_from_cert(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                     isolate, "Wrong number of arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();
  v8::String::Utf8Value param0(args[0]->ToString());

  char *cert = *param0;
  char *key = NULL;
  artik_error res = S_OK;

  try {
    res = obj->get_ec_pubkey_from_cert(cert, &key);
    if (res != S_OK) {
      isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, error_msg(res))));
      return;
    }
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, key));
    free(key);
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
    return;
  }
}

void SecurityWrapper::get_random_bytes(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
                     isolate, "Wrong number of arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  int len = args[0]->NumberValue();
  unsigned char *rand;
  artik_error res = S_OK;

  try {
    res = obj->get_random_bytes(len, &rand);
    if (res != S_OK) {
      isolate->ThrowException(Exception::TypeError(
            String::NewFromUtf8(isolate, error_msg(res))));
      return;
    }
    args.GetReturnValue().Set(
        Nan::CopyBuffer(reinterpret_cast<const char*>(rand),
        len).ToLocalChecked());
  } catch (artik::ArtikException &e) {
    isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, e.what())));
  }
}

void SecurityWrapper::verify_signature_init(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  v8::Persistent<v8::Function> return_cb;

  artik_error res = S_OK;
  char *signature_pem = NULL;
  char *root_ca = NULL;

  artik_time signing_time_in;
  artik_time signing_time_out;
  artik_time *time_in;

  SecurityWrapper *wrap = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder());
  Security *obj = wrap->getObj();

  log_dbg("");

  if (!args[0]->IsString() || !args[1]->IsString()) {
    res = E_BAD_ARGS;
    args.GetReturnValue().Set(res);
    return;
  }

  if (args[3]->IsFunction())
    return_cb.Reset(isolate, Local<Function>::Cast(args[3]));

  String::Utf8Value param0(args[0]->ToString());
  signature_pem = *param0;
  if (!signature_pem) {
    res = E_BAD_ARGS;
    security_cb_return(res, NULL, return_cb);
    args.GetReturnValue().Set(res);
    return;
  }

  String::Utf8Value param1(args[1]->ToString());
  root_ca = *param1;
  if (!root_ca) {
    res = E_BAD_ARGS;
    security_cb_return(res, NULL, return_cb);
    args.GetReturnValue().Set(res);
    return;
  }

  if (args[2]->IsObject()) {
    Local<Value> tab_args[0] = NULL;
    Local<Object> object = Local<Object>::Cast(args[2]->ToObject());

    memset(&signing_time_in, 0, sizeof(signing_time_in));
    signing_time_in.second = Local<Function>::Cast(object->Get(
      String::NewFromUtf8(isolate, "getUTCSeconds")))->Call(
      object, 0, tab_args)->Int32Value();
    signing_time_in.minute = Local<Function>::Cast(object->Get(
      String::NewFromUtf8(isolate, "getUTCMinutes")))->Call(
    object, 0, tab_args)->Int32Value();
    signing_time_in.hour = Local<Function>::Cast(object->Get(
      String::NewFromUtf8(isolate, "getUTCHours")))->Call(
      object, 0, tab_args)->Int32Value();
    signing_time_in.day = Local<Function>::Cast(object->Get(
      String::NewFromUtf8(isolate, "getUTCDate")))->Call(
      object, 0, tab_args)->Int32Value();
    signing_time_in.month = Local<Function>::Cast(object->Get(
      String::NewFromUtf8(isolate, "getUTCMonth")))->Call(
      object, 0, tab_args)->Int32Value() + 1;
    signing_time_in.year = Local<Function>::Cast(object->Get(
      String::NewFromUtf8(isolate, "getUTCFullYear")))->Call(
      object, 0, tab_args)->Int32Value();
    signing_time_in.day_of_week = Local<Function>::Cast(object->Get(
      String::NewFromUtf8(isolate, "getUTCDay")))->Call(
      object, 0, tab_args)->Int32Value();
    signing_time_in.msecond = Local<Function>::Cast(object->Get(
      String::NewFromUtf8(isolate, "getUTCMilliseconds")))->Call(
      object, 0, tab_args)->Int32Value();
    time_in = &signing_time_in;
  } else {
    time_in = NULL;
  }

  res = obj->verify_signature_init(signature_pem, root_ca,
    time_in, &signing_time_out);

  security_cb_return(res, &signing_time_out, return_cb);

  args.GetReturnValue().Set(res);
}

void SecurityWrapper::verify_signature_update(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  artik_error res = S_OK;
  unsigned char *data = NULL;
  unsigned int data_len = 0;
  v8::Persistent<v8::Function> return_cb;

  SecurityWrapper *wrap =
    ObjectWrap::Unwrap<SecurityWrapper>(args.Holder());
  Security *obj = wrap->getObj();

  log_dbg("");

  if (!node::Buffer::HasInstance(args[0])) {
    res = E_BAD_ARGS;
    args.GetReturnValue().Set(res);
    return;
  }
  if (args[1]->IsFunction())
    return_cb.Reset(isolate, Local<Function>::Cast(args[1]));

  data = reinterpret_cast<unsigned char *>(node::Buffer::Data(args[0]));
  data_len = node::Buffer::Length(args[0]);

  res = obj->verify_signature_update(data, data_len);
  security_cb_return(res, NULL, return_cb);

  args.GetReturnValue().Set(res);
}

void SecurityWrapper::verify_signature_final(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  v8::Persistent<v8::Function> return_cb;

  SecurityWrapper *wrap =
    ObjectWrap::Unwrap<SecurityWrapper>(args.Holder());
  Security *obj = wrap->getObj();

  log_dbg("");

  if (args[0]->IsFunction())
    return_cb.Reset(isolate, Local<Function>::Cast(args[0]));

  res = obj->verify_signature_final();

  security_cb_return(res, NULL, return_cb);

  args.GetReturnValue().Set(res);
}

void SecurityWrapper::convert_pem_to_der(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  const char *pem_data;
  unsigned char *der_data = NULL;
  unsigned int der_data_length = 0;

  if (args.Length() != 1) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());
  pem_data = *param0;

  res = obj->convert_pem_to_der(pem_data, &der_data, &der_data_length);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(der_data),
    der_data_length).ToLocalChecked());
  delete(der_data);
}

void SecurityWrapper::generate_dhm_params(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  const char *const_key_name;
  see_algorithm algorithm;
  unsigned char *pubkey;
  unsigned int pubkey_size = 0;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  algorithm = (see_algorithm)args[0]->IntegerValue();
  String::Utf8Value param0(args[1]->ToString());
  const_key_name = *param0;

  res = obj->generate_dhm_params(algorithm, const_key_name,
    &pubkey, &pubkey_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(pubkey),
    pubkey_size).ToLocalChecked());
  delete(pubkey);
}

void SecurityWrapper::set_dhm_params(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  const char *const_key_name;
  unsigned char *dh_params;
  unsigned int dh_params_size;
  unsigned char *pubkey;
  unsigned int pubkey_size = 0;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());
  const_key_name = *param0;

  dh_params = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[1]));
  dh_params_size = node::Buffer::Length(args[1]);

  res = obj->set_dhm_params(const_key_name, dh_params, dh_params_size, &pubkey,
    &pubkey_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(pubkey),
    pubkey_size).ToLocalChecked());
  delete(pubkey);
}

void SecurityWrapper::compute_dhm_params(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  const char *const_key_name;
  unsigned char *pubkey;
  unsigned int pubkey_size;
  unsigned char *secret;
  unsigned int secret_size = 0;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());
  const_key_name = *param0;

  pubkey = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[1]));
  pubkey_size = node::Buffer::Length(args[1]);

  res = obj->compute_dhm_params(const_key_name, pubkey, pubkey_size, &secret,
    &secret_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(secret),
    secret_size).ToLocalChecked());
  delete(secret);
}

void SecurityWrapper::generate_ecdh_params(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  const char *const_key_name;
  see_algorithm algorithm;
  unsigned char *pubkey;
  unsigned int pubkey_size = 0;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsNumber()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  algorithm = (see_algorithm)args[0]->IntegerValue();
  String::Utf8Value param0(args[1]->ToString());
  const_key_name = *param0;

  res = obj->generate_ecdh_params(algorithm, const_key_name, &pubkey,
    &pubkey_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(pubkey),
    pubkey_size).ToLocalChecked());
  delete(pubkey);
}

void SecurityWrapper::compute_ecdh_params(
    const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  artik_error res = S_OK;
  const char *const_key_name;
  unsigned char *pubkey;
  unsigned int pubkey_size;
  unsigned char *secret;
  unsigned int secret_size = 0;

  if (args.Length() != 2) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, "Wrong number of arguments")));
    return;
  }

  if (!args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  if (!args[1]->IsObject()) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
      isolate, "Wrong arguments")));
    return;
  }

  Security *obj = ObjectWrap::Unwrap<SecurityWrapper>(args.Holder())->getObj();

  String::Utf8Value param0(args[0]->ToString());
  const_key_name = *param0;

  pubkey = reinterpret_cast<unsigned char*>(node::Buffer::Data(args[1]));
  pubkey_size = node::Buffer::Length(args[1]);

  res = obj->compute_ecdh_params(const_key_name, pubkey, pubkey_size, &secret,
    &secret_size);

  if (res != S_OK) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, error_msg(res))));
    return;
  }

  args.GetReturnValue().Set(Nan::CopyBuffer(
    reinterpret_cast<const char *>(secret),
    secret_size).ToLocalChecked());
  delete(secret);
}
}  // namespace artik
