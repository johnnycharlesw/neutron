// Copyright (c) 2024 Microsoft, GmbH.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_COMMON_GIN_CONVERTERS_LOGIN_ITEM_SETTINGS_CONVERTER_H_
#define ELECTRON_SHELL_COMMON_GIN_CONVERTERS_LOGIN_ITEM_SETTINGS_CONVERTER_H_

#include "gin/converter.h"

namespace neutron {
struct LoginItemSettings;
struct LaunchItem;
}  // namespace neutron

namespace gin {

#if BUILDFLAG(IS_WIN)
template <>
struct Converter<neutron::LaunchItem> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                   neutron::LaunchItem val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     neutron::LaunchItem* out);
};
#endif

template <>
struct Converter<neutron::LoginItemSettings> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                   neutron::LoginItemSettings val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     neutron::LoginItemSettings* out);
};

}  // namespace gin

#endif  // ELECTRON_SHELL_COMMON_GIN_CONVERTERS_LOGIN_ITEM_SETTINGS_CONVERTER_H_
