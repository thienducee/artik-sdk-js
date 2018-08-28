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


#ifndef ADDON_BASE_SSL_CONFIG_CONVERTER_H_
#define ADDON_BASE_SSL_CONFIG_CONVERTER_H_

#include <artik_ssl.h>
#include <node.h>
#include <node_object_wrap.h>
#include <utils.h>

#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>

namespace artik {

struct ArtikSslConfigProxy {
  explicit ArtikSslConfigProxy(std::nullptr_t): m_ssl(nullptr),
    m_se_config(nullptr) {}
  ArtikSslConfigProxy(std::unique_ptr<artik_ssl_config> ssl,
    std::unique_ptr<artik_secure_element_config> se_config)
    : m_ssl(std::move(ssl)), m_se_config(std::move(se_config)) {}

  artik_ssl_config *get() {
    return m_ssl.get();
  }

  artik_ssl_config *operator->() {
    return get();
  }

  explicit operator bool() const {
    return m_ssl != nullptr;
  }

 private:
  std::unique_ptr<artik_ssl_config> m_ssl;
  std::unique_ptr<artik_secure_element_config> m_se_config;
};

struct SSLConfigConverter {
  static ArtikSslConfigProxy convert(
    v8::Isolate *isolate,
    v8::Local<v8::Value> val);

  static std::array<const char*, 3> security_cert_type_t;
  static std::pair<bool, see_algorithm> convert_see_algo(std::string);
  static std::unique_ptr<artik_secure_element_config> convert_se_config(
    v8::Isolate *isolate, v8::Local<v8::Value> val);

 private:
    static const std::unordered_map<std::string, see_algorithm> see_algo;
};
}  // namespace artik

#endif  // ADDON_BASE_SSL_CONFIG_CONVERTER_H_
