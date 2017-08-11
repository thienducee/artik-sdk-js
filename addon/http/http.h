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

#ifndef ADDON_HTTP_HTTP_H_
#define ADDON_HTTP_HTTP_H_

#include <node.h>
#include <node_object_wrap.h>
#include <uv.h>

#include <artik_http.hh>

#include <utils.h>

namespace artik {

struct HttpAsyncWork {
  uv_work_t                     request;
  v8::Persistent<v8::Function>  callback;
  artik_error                   ret;
  Http*                         http;
  artik_http_headers*           headers;
  char*                         url;
  char*                         response;
  char*                         body;
  int                           status;
  artik_ssl_config*             ssl;
};

class HttpWrapper : public node::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

  Http* getObj() { return m_http; }
  v8::Persistent<v8::Function>* getDataCb() { return m_data_cb; }
  v8::Persistent<v8::Function>* getErrorCb() { return m_error_cb; }

 private:
  HttpWrapper();
  ~HttpWrapper();

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static v8::Persistent<v8::Function> constructor;

  static void get_stream(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void get(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void post(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void put(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void del(const v8::FunctionCallbackInfo<v8::Value>& args);

  Http* m_http;
  HttpAsyncWork* m_work;
  v8::Persistent<v8::Function>* m_data_cb;
  v8::Persistent<v8::Function>* m_error_cb;
};

}  // namespace artik


#endif  // ADDON_HTTP_HTTP_H_
