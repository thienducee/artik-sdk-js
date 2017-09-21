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

#ifndef ADDON_CLOUD_CLOUD_H_
#define ADDON_CLOUD_CLOUD_H_

#include <node.h>
#include <node_object_wrap.h>
#include <uv.h>
#include <artik_cloud.hh>

#include <utils.h>

namespace artik {

struct CloudAsyncWork {
  uv_work_t request;
  v8::Persistent<v8::Function> callback;
  Cloud *cloud;
  artik_error ret;

  /* Function parameters */
  char* device_id;
  char* device_type_id;
  char* reg_id;
  char* vendor_id;
  char* nonce;
  char* app_id;
  char* user_id;
  char* action;
  char* message;
  char* name;
  char* data;
  bool properties;
  bool shared;
  bool timestamp;
  int count;
  int offset;
  char* response;
  artik_ssl_config *ssl_config;
};

class CloudWrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

  Cloud* getObj() { return m_cloud; }
  v8::Persistent<v8::Function>* getReceiveCb() { return m_receive_cb; }
  v8::Persistent<v8::Function>* getConnectionCb() { return m_connection_cb; }

 private:
  explicit CloudWrapper(const char* token);
  ~CloudWrapper();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  static void send_message(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void send_action(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_current_user_profile(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_user_devices(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_user_device_types(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_user_application_properties(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_device(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_device_token(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void add_device(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void update_device_token(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void delete_device_token(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void delete_device(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get_device_properties(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void set_device_server_properties(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void sdr_start_registration(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void sdr_registration_status(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void sdr_complete_registration(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void websocket_open_stream(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void websocket_send_message(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void websocket_close_stream(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  Cloud* m_cloud;
  CloudAsyncWork* m_work;
  v8::Persistent<v8::Function>* m_connection_cb;
  v8::Persistent<v8::Function>* m_receive_cb;
};

}  // namespace artik


#endif  // ADDON_CLOUD_CLOUD_H_
