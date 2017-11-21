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

#ifndef ADDON_COAP_COAP_H_
#define ADDON_COAP_COAP_H_

#include <node.h>
#include <node_object_wrap.h>

#include <artik_coap.hh>

#include <utils.h>
#include <loop.h>
#include <stdint.h>

#include <vector>

namespace artik {

class Resource {
 public:
  Resource();
  ~Resource();

  v8::Persistent<v8::Function>* getGetResourceCb() { return m_get_resource_cb; }
  v8::Persistent<v8::Function>* getPostResourceCb()
                                                  { return m_post_resource_cb; }
  v8::Persistent<v8::Function>* getPutResourceCb() { return m_put_resource_cb; }
  v8::Persistent<v8::Function>* getDeleteResourceCb()
                                                { return m_delete_resource_cb; }

  void setGetResourceCb(v8::Isolate *isolate, v8::Local<v8::Value> cb);
  void setPostResourceCb(v8::Isolate *isolate, v8::Local<v8::Value> cb);
  void setPutResourceCb(v8::Isolate *isolate, v8::Local<v8::Value> cb);
  void setDeleteResourceCb(v8::Isolate *isolate, v8::Local<v8::Value> cb);

 private:
  v8::Persistent<v8::Function>* m_get_resource_cb;
  v8::Persistent<v8::Function>* m_post_resource_cb;
  v8::Persistent<v8::Function>* m_put_resource_cb;
  v8::Persistent<v8::Function>* m_delete_resource_cb;
};

class CoapWrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

  Coap* getObj() { return m_coap; }
  v8::Persistent<v8::Function>* getSendCb() { return m_send_cb; }
  v8::Persistent<v8::Function>* getObserveCb() { return m_observe_cb; }
  v8::Persistent<v8::Function>* getVerifyPskCb() { return m_verify_psk_cb; }
  std::vector<Resource*>* getResources() { return m_resources; }

 private:
  CoapWrapper();
  explicit CoapWrapper(artik_coap_config *config);
  ~CoapWrapper();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;
  static void create_client(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void destroy_client(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void connect(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void disconnect(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void create_server(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void destroy_server(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void start_server(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void stop_server(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void send_message(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void observe(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void cancel_observe(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void init_resources(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void notify_resource_changed(
                              const v8::FunctionCallbackInfo<v8::Value>& args);

  Coap* m_coap;
  v8::Persistent<v8::Function>* m_send_cb;
  v8::Persistent<v8::Function>* m_observe_cb;
  v8::Persistent<v8::Function>* m_verify_psk_cb;
  std::vector<Resource*>* m_resources;
  GlibLoop* m_loop;
};

}  // namespace artik

#endif  // ADDON_COAP_COAP_H_
