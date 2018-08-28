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

#ifndef ADDON_SECURITY_SECURITY_H_
#define ADDON_SECURITY_SECURITY_H_

#include <node.h>
#include <nan.h>
#include <node_object_wrap.h>

#include <artik_security.hh>

#include <unordered_map>
#include <string>

namespace artik {

class SecurityWrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

  Security* getObj() { return m_security; }

 private:
  SecurityWrapper();
  ~SecurityWrapper();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  static void get_certificate(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_certificate_sn(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_certificate_pem_chain(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void set_certificate(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void remove_certificate(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_hash(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void generate_key(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_hmac(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void remove_key(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_rsa_signature(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void verify_rsa_signature(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_ecdsa_signature(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void verify_ecdsa_signature(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void set_key(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_publickey(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void write_secure_storage(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void read_secure_storage(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void remove_secure_storage(
    const v8::FunctionCallbackInfo<v8::Value>& args);
  static void aes_encryption(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void aes_decryption(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void rsa_encryption(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void rsa_decryption(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_ec_pubkey_from_cert(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_random_bytes(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void verify_signature_init(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void verify_signature_update(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void verify_signature_final(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void convert_pem_to_der(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void generate_dhm_params(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void set_dhm_params(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void compute_dhm_params(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void generate_ecdh_params(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void compute_ecdh_params(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  static const std::unordered_map<std::string, see_hash_mode> hash_mode;
  static const std::unordered_map<std::string, see_aes_mode> aes_mode;
  static const std::unordered_map<std::string, see_rsa_mode> rsa_mode;
  Security * m_security;
};

}  // namespace artik

#endif  // ADDON_SECURITY_SECURITY_H_
