// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ELECTRON_SHELL_BROWSER_MAC_ELECTRON_APPLICATION_DELEGATE_H_
#define ELECTRON_SHELL_BROWSER_MAC_ELECTRON_APPLICATION_DELEGATE_H_

#import <Cocoa/Cocoa.h>

#import "shell/browser/ui/cocoa/neutron_menu_controller.h"

@interface NeutronApplicationDelegate : NSObject <NSApplicationDelegate>

// Sets the menu that will be returned in "applicationDockMenu:".
- (void)setApplicationDockMenu:(neutron::NeutronMenuModel*)model;

@end

#endif  // ELECTRON_SHELL_BROWSER_MAC_ELECTRON_APPLICATION_DELEGATE_H_
