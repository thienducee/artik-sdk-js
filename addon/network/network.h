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

#ifndef ADDON_NETWORK_NETWORK_H_
#define ADDON_NETWORK_NETWORK_H_

#include <node.h>
#include <node_object_wrap.h>

#include <uv.h>
#include <artik_network.hh>

#include <utils.h>
#include <loop.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace artik {

class NetworkWatcher {
 private:
  artik_watch_online_status_handle m_handle;
  std::unique_ptr<v8::Persistent<v8::Function>> m_val;

 public:
  NetworkWatcher(
      v8::Isolate *isolate,
      artik_watch_online_status_handle handle,
      v8::Local<v8::Value> val)
      : m_handle(handle),
        m_val(new v8::Persistent<v8::Function>(
                        isolate,
                        v8::Local<v8::Function>::Cast(val))) {}

  NetworkWatcher(const NetworkWatcher&) = delete;
  NetworkWatcher(NetworkWatcher&&) = default;

  v8::Local<v8::Function> getCallback(v8::Isolate *isolate) const {
    return v8::Local<v8::Function>::New(isolate, *m_val);
  }

  artik_watch_online_status_handle getHandle() const {
    return m_handle;
  }
};

class NetworkWrapper : public node::ObjectWrap {
  std::unordered_map<std::string, NetworkWatcher> m_watchers;

 public:
  static void Init(v8::Local<v8::Object> exports);

  Network* getObj() { return m_network; }

  Optional<std::reference_wrapper<NetworkWatcher>> findWatcher(
      const std::string& addr);

 private:
  NetworkWrapper();
  ~NetworkWrapper();
  void addWatcher(
      std::string&& addr,
      artik_watch_online_status_handle handle,
      v8::Isolate *isolate,
      v8::Local<v8::Value> func);
  void removeWatcher(const std::string& addr);

  static void New(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  static void set_network_config(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_network_config(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_current_public_ip(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void dhcp_client_start(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void dhcp_client_stop(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void dhcp_server_start(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void dhcp_server_stop(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_online_status(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void add_watch_online_status(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void remove_watch_online_status(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  Network* m_network;
  GlibLoop* m_loop;
};

}  // namespace artik


#endif  // ADDON_NETWORK_NETWORK_H_
