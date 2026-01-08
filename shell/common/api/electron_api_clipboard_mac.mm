// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "base/strings/sys_string_conversions.h"
#include "shell/common/api/neutron_api_clipboard.h"
#include "ui/base/cocoa/find_pasteboard.h"

namespace neutron::api {

void Clipboard::WriteFindText(const std::u16string& text) {
  NSString* text_ns = base::SysUTF16ToNSString(text);
  [[FindPasteboard sharedInstance] setFindText:text_ns];
}

std::u16string Clipboard::ReadFindText() {
  return base::SysNSStringToUTF16([[FindPasteboard sharedInstance] findText]);
}

}  // namespace neutron::api
