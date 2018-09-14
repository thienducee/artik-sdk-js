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

#include "base/ssl_config_converter.h"

#include <utils.h>
#include <artik_log.h>

#include <string>
#include <iostream>
#include <utility>

static std::array<const char*, 3> ssl_verifies = {
  "none",
  "optional",
  "required" };

namespace artik {

using v8::Exception;
using v8::Isolate;
using v8::Local;
using v8::String;
using v8::Value;

const std::unordered_map<std::string, see_algorithm>
SSLConfigConverter::see_algo = {
  { "aes128", AES_128 },
  { "aes192", AES_192 },
  { "aes256", AES_256 },
  { "rsa1024", RSA_1024 },
  { "rsa2018", RSA_2048 },
  { "ecc_brainpool", ECC_BRAINPOOL_ALGORITHM },
  { "ecc_brainpool_p256r1", ECC_BRAINPOOL_P256R1 },
  { "ecc_sec", ECC_SEC_ALGORITHM },
  { "ecc_sec_p256r1", ECC_SEC_P256R1 },
  { "ecc_sec_p384r1", ECC_SEC_P384R1 },
  { "ecc_sec_p521r1", ECC_SEC_P521R1 },
  { "hmac", HMAC_ALGORITHM },
  { "dh_1024", DH_1024 },
  { "dh_1024_5114", DH_1024_5114 },
  { "dh_2048", DH_2048 },
  { "dh_2048_5114", DH_2048_5114 }
};

std::array<const char*, 3> SSLConfigConverter::security_cert_type_t = {
  "ARTIK_SECURITY_CERT_TYPE_PEM",
  "ARTIK_SECURITY_CERT_TYPE_DER",
  "ARTIK_SECURITY_CERT_TYPE_INVALID" };

std::pair<bool, see_algorithm> SSLConfigConverter::convert_see_algo(
  std::string str) {
  std::pair<bool, see_algorithm> algorithm { false, AES_ALGORITHM };

  auto it = see_algo.find(str);
  if (it != see_algo.end()) {
    algorithm.second = it->second;
    algorithm.first = true;
  }

  return algorithm;
}

std::unique_ptr<artik_secure_element_config>
SSLConfigConverter::convert_se_config(Isolate *isolate, Local<Value> val) {
  std::unique_ptr<artik_secure_element_config> se_config(
    new artik_secure_element_config);

  memset(se_config.get(), 0, sizeof(artik_secure_element_config));

  auto key_id = js_object_attribute_to_cpp<std::string>(val,
                                            "key_id");
  if (!key_id) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
      "Wrong definition of se_config:"
      " certificate_identifier is not a string.")));
    return nullptr;
  }

  auto str_see_algo = js_object_attribute_to_cpp<std::string>(val,
    "key_algo");

  if (!str_see_algo) {
    std::string msg = "Wrong definition of str_see_algo.";
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(
        isolate, msg.c_str())));
    return nullptr;
  }

  auto see_algo = convert_see_algo(str_see_algo.value());
  if (!see_algo.first) {
    isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
      "Wrong definition of see_algo.")));
  }
  se_config->key_id = strdup(key_id.value().c_str());
  se_config->key_algo = see_algo.second;

  return se_config;
}

ArtikSslConfigProxy SSLConfigConverter::convert(
  Isolate *isolate, Local<Value> val) {
  std::unique_ptr<artik_ssl_config> ssl_config(new artik_ssl_config);
  std::unique_ptr<artik_secure_element_config> se_config = nullptr;

  memset(ssl_config.get(), 0, sizeof(artik_ssl_config));

  /* se_config parameter */
  log_dbg("Parse ssl_config");

  auto se_config_js = js_object_attribute_to_cpp<Local<Value>>(val,
    "se_config");

  if (se_config_js) {
    log_dbg("Parse artik secure element config");
    se_config  = convert_se_config(isolate, se_config_js.value());
    if (!se_config) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong definition of se_config_js")));
      return ArtikSslConfigProxy(nullptr);
    }
    ssl_config->se_config = se_config.get();
  }

  auto ca_cert = js_object_attribute_to_cpp<Local<Value>>(val, "ca_cert");

  if (ca_cert) {
    if (!node::Buffer::HasInstance(ca_cert.value())) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong definition of ca_cert")));
      return ArtikSslConfigProxy(nullptr);
    }

    char *val = reinterpret_cast<char *>(node::Buffer::Data(ca_cert.value()));
    size_t len = node::Buffer::Length(ca_cert.value());

    ssl_config->ca_cert.data = strdup(val);
    ssl_config->ca_cert.len = len;
  }

  /* client_cert parameter */
  auto client_cert = js_object_attribute_to_cpp<Local<Value>>(val,
    "client_cert");

  if (client_cert) {
    if (!node::Buffer::HasInstance(client_cert.value())) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong definition of client_cert")));
      return ArtikSslConfigProxy(nullptr);
    }
    char *val = reinterpret_cast<char *>(node::Buffer::Data(
      client_cert.value()));
    size_t len = node::Buffer::Length(client_cert.value());

    ssl_config->client_cert.data = strdup(val);
    ssl_config->client_cert.len = len;
  }

  /* client_key parameter */
  auto client_key = js_object_attribute_to_cpp<Local<Value>>(val, "client_key");

  if (client_key) {
    if (!node::Buffer::HasInstance(client_key.value())) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong definition of client_key")));
      return ArtikSslConfigProxy(nullptr);
    }

    char *val = reinterpret_cast<char *>(node::Buffer::Data(
      client_key.value()));
    size_t len = node::Buffer::Length(client_key.value());

    ssl_config->client_key.data = strdup(val);
    ssl_config->client_key.len = len;
  }

  /* verify_cert parameter */
  auto verify_cert_str = js_object_attribute_to_cpp<std::string>(
    val,
    "verify_cert");
  if (verify_cert_str) {
    auto verify_cert =
      to_artik_parameter<artik_ssl_verify_t>(
        ssl_verifies,
        verify_cert_str.value().c_str());

    if (!verify_cert) {
      isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate,
        "Wrong value for verify_cert : expect 'none',"
        "'optional' or 'required'.")));
      return ArtikSslConfigProxy(nullptr);
    }

    ssl_config->verify_cert = verify_cert.value();
  }

  return ArtikSslConfigProxy(std::move(ssl_config), std::move(se_config));
}

}  // namespace artik
