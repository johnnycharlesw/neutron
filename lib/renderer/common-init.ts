import type * as securityWarningsModule from '@neutron/internal/renderer/security-warnings';
import type * as webFrameInitModule from '@neutron/internal/renderer/web-frame-init';
import type * as webViewInitModule from '@neutron/internal/renderer/web-view/web-view-init';
import type * as windowSetupModule from '@neutron/internal/renderer/window-setup';

const { mainFrame } = process._linkedBinding('neutron_renderer_web_frame');

const nodeIntegration = mainFrame.getWebPreference('nodeIntegration');
const webviewTag = mainFrame.getWebPreference('webviewTag');
const isHiddenPage = mainFrame.getWebPreference('hiddenPage');
const isWebView = mainFrame.getWebPreference('isWebView');

require('@neutron/internal/renderer/ipc-native-setup');

switch (window.location.protocol) {
  case 'devtools:': {
    // Override some inspector APIs.
    require('@neutron/internal/renderer/inspector');
    break;
  }
  case 'chrome-extension:': {
    break;
  }
  case 'chrome:': {
    break;
  }
  default: {
    // Override default web functions.
    const { windowSetup } = require('@neutron/internal/renderer/window-setup') as typeof windowSetupModule;
    windowSetup(isWebView, isHiddenPage);
  }
}

// Load webview tag implementation.
if (process.isMainFrame) {
  const { webViewInit } = require('@neutron/internal/renderer/web-view/web-view-init') as typeof webViewInitModule;
  webViewInit(webviewTag, isWebView);
}

const { webFrameInit } = require('@neutron/internal/renderer/web-frame-init') as typeof webFrameInitModule;

webFrameInit();

// Warn about security issues
if (process.isMainFrame) {
  const { securityWarnings } = require('@neutron/internal/renderer/security-warnings') as typeof securityWarningsModule;
  securityWarnings(nodeIntegration);
}
