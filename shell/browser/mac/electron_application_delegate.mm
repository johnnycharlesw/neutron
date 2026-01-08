// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#import "shell/browser/mac/neutron_application_delegate.h"

#include <string>

#include "base/allocator/buildflags.h"
#include "base/allocator/partition_allocator/src/partition_alloc/shim/allocator_shim.h"
#include "base/functional/callback.h"
#include "base/mac/mac_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/sys_string_conversions.h"
#include "shell/browser/api/neutron_api_push_notifications.h"
#include "shell/browser/browser.h"
#include "shell/browser/mac/dict_util.h"
#import "shell/browser/mac/neutron_application.h"
#include "shell/common/mac_util.h"

#import <UserNotifications/UserNotifications.h>

static NSDictionary* UNNotificationResponseToNSDictionary(
    UNNotificationResponse* response) {
  if (![response respondsToSelector:@selector(actionIdentifier)] ||
      ![response respondsToSelector:@selector(notification)]) {
    return nil;
  }

  NSMutableDictionary* result = [[NSMutableDictionary alloc] init];
  result[@"actionIdentifier"] = response.actionIdentifier;
  result[@"date"] = @(response.notification.date.timeIntervalSince1970);
  result[@"identifier"] = response.notification.request.identifier;
  result[@"userInfo"] = response.notification.request.content.userInfo;

  // [response isKindOfClass:[UNTextInputNotificationResponse class]]
  if ([response respondsToSelector:@selector(userText)]) {
    result[@"userText"] =
        static_cast<UNTextInputNotificationResponse*>(response).userText;
  }

  return result;
}

@implementation ElectronApplicationDelegate {
  ElectronMenuController* __strong menu_controller_;
}

- (void)setApplicationDockMenu:(neutron::ElectronMenuModel*)model {
  menu_controller_ = [[ElectronMenuController alloc] initWithModel:model
                                             useDefaultAccelerator:NO];
}

- (void)willPowerOff:(NSNotification*)notify {
  [[AtomApplication sharedApplication] willPowerOff:notify];
}

- (void)applicationWillFinishLaunching:(NSNotification*)notify {
  // Don't add the "Enter Full Screen" menu item automatically.
  [[NSUserDefaults standardUserDefaults]
      setBool:NO
       forKey:@"NSFullScreenMenuItemEverywhere"];

  [[[NSWorkspace sharedWorkspace] notificationCenter]
      addObserver:self
         selector:@selector(willPowerOff:)
             name:NSWorkspaceWillPowerOffNotification
           object:nil];

  neutron::Browser::Get()->WillFinishLaunching();
}

// NSUserNotification is deprecated; all calls should be replaced with
// UserNotifications.frameworks API
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

- (void)applicationDidFinishLaunching:(NSNotification*)notify {
  NSObject* user_notification =
      [notify userInfo][NSApplicationLaunchUserNotificationKey];
  NSDictionary* notification_info = nil;

  if (user_notification) {
    if ([user_notification isKindOfClass:[NSUserNotification class]]) {
      notification_info =
          [static_cast<NSUserNotification*>(user_notification) userInfo];
    } else {
      notification_info = UNNotificationResponseToNSDictionary(
          static_cast<UNNotificationResponse*>(user_notification));
    }
  }

  NSAppleEventDescriptor* event =
      NSAppleEventManager.sharedAppleEventManager.currentAppleEvent;
  BOOL launched_as_login_item =
      (event.eventID == kAEOpenApplication &&
       [event paramDescriptorForKeyword:keyAEPropData].enumCodeValue ==
           keyAELaunchedAsLogInItem);
  neutron::Browser::Get()->SetLaunchedAtLogin(launched_as_login_item);

  neutron::Browser::Get()->DidFinishLaunching(
      neutron::NSDictionaryToValue(notification_info));
}

// -Wdeprecated-declarations
#pragma clang diagnostic pop

- (void)applicationDidBecomeActive:(NSNotification*)notification {
  neutron::Browser::Get()->DidBecomeActive();
}

- (void)applicationDidResignActive:(NSNotification*)notification {
  neutron::Browser::Get()->DidResignActive();
}

- (NSMenu*)applicationDockMenu:(NSApplication*)sender {
  return menu_controller_ ? menu_controller_.menu : nil;
}

- (BOOL)application:(NSApplication*)sender openFile:(NSString*)filename {
  std::string filename_str(base::SysNSStringToUTF8(filename));
  return neutron::Browser::Get()->OpenFile(filename_str) ? YES : NO;
}

- (BOOL)applicationShouldHandleReopen:(NSApplication*)theApplication
                    hasVisibleWindows:(BOOL)flag {
  neutron::Browser* browser = neutron::Browser::Get();
  browser->Activate(static_cast<bool>(flag));
  return flag;
}

- (BOOL)application:(NSApplication*)sender
    continueUserActivity:(NSUserActivity*)userActivity
      restorationHandler:
#ifdef MAC_OS_X_VERSION_10_14
          (void (^)(NSArray<id<NSUserActivityRestoring>>* restorableObjects))
#else
          (void (^)(NSArray* restorableObjects))
#endif
              restorationHandler {
  std::string activity_type(base::SysNSStringToUTF8(userActivity.activityType));
  NSURL* url = userActivity.webpageURL;
  NSDictionary* details = url ? @{@"webpageURL" : url.absoluteString} : @{};
  if (!userActivity.userInfo)
    return NO;

  neutron::Browser* browser = neutron::Browser::Get();
  return browser->ContinueUserActivity(
             activity_type,
             neutron::NSDictionaryToValue(userActivity.userInfo),
             neutron::NSDictionaryToValue(details))
             ? YES
             : NO;
}

- (BOOL)application:(NSApplication*)application
    willContinueUserActivityWithType:(NSString*)userActivityType {
  std::string activity_type(base::SysNSStringToUTF8(userActivityType));

  neutron::Browser* browser = neutron::Browser::Get();
  return browser->WillContinueUserActivity(activity_type) ? YES : NO;
}

- (void)application:(NSApplication*)application
    didFailToContinueUserActivityWithType:(NSString*)userActivityType
                                    error:(NSError*)error {
  std::string activity_type(base::SysNSStringToUTF8(userActivityType));
  std::string error_message(
      base::SysNSStringToUTF8(error.localizedDescription));

  neutron::Browser* browser = neutron::Browser::Get();
  browser->DidFailToContinueUserActivity(activity_type, error_message);
}

- (IBAction)newWindowForTab:(id)sender {
  neutron::Browser::Get()->NewWindowForTab();
}

- (void)application:(NSApplication*)application
    didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken {
  // Resolve outstanding APNS promises created during registration attempts
  if (auto* push_notifications = neutron::api::PushNotifications::Get()) {
    std::string encoded =
        base::HexEncode(neutron::util::as_byte_span(deviceToken));
    push_notifications->ResolveAPNSPromiseSetWithToken(
        base::ToLowerASCII(encoded));
  }
}

- (void)application:(NSApplication*)application
    didFailToRegisterForRemoteNotificationsWithError:(NSError*)error {
  std::string error_message(base::SysNSStringToUTF8(
      [NSString stringWithFormat:@"%ld %@ %@", error.code, error.domain,
                                 error.userInfo]));
  neutron::api::PushNotifications* push_notifications =
      neutron::api::PushNotifications::Get();
  if (push_notifications) {
    push_notifications->RejectAPNSPromiseSetWithError(error_message);
  }
}

- (void)application:(NSApplication*)application
    didReceiveRemoteNotification:(NSDictionary*)userInfo {
  neutron::api::PushNotifications* push_notifications =
      neutron::api::PushNotifications::Get();
  if (push_notifications) {
    neutron::api::PushNotifications::Get()->OnDidReceiveAPNSNotification(
        neutron::NSDictionaryToValue(userInfo));
  }
}

// This only has an effect on macOS 12+, and requests any state restoration
// archive to be created with secure encoding. See the article at
// https://sector7.computest.nl/post/2022-08-process-injection-breaking-all-macos-security-layers-with-a-single-vulnerability/
// for more details.
- (BOOL)applicationSupportsSecureRestorableState:(NSApplication*)app {
  return YES;
}

@end
