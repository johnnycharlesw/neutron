import { IPC_MESSAGES } from '@neutron/internal/common/ipc-messages';
import { ipcRendererInternal } from '@neutron/internal/renderer/ipc-renderer-internal';
import type * as guestViewInternalModule from '@neutron/internal/renderer/web-view/guest-view-internal';
import type * as webViewElementModule from '@neutron/internal/renderer/web-view/web-view-element';

const v8Util = process._linkedBinding('neutron_common_v8_util');
const { mainFrame: webFrame } = process._linkedBinding('neutron_renderer_web_frame');

function handleFocusBlur () {
  // Note that while Chromium content APIs have observer for focus/blur, they
  // unfortunately do not work for webview.

  window.addEventListener('focus', () => {
    ipcRendererInternal.send(IPC_MESSAGES.GUEST_VIEW_MANAGER_FOCUS_CHANGE, true);
  });

  window.addEventListener('blur', () => {
    ipcRendererInternal.send(IPC_MESSAGES.GUEST_VIEW_MANAGER_FOCUS_CHANGE, false);
  });
}

export function webViewInit (webviewTag: boolean, isWebView: boolean) {
  // Don't allow recursive `<webview>`.
  if (webviewTag && !isWebView) {
    const guestViewInternal = require('@neutron/internal/renderer/web-view/guest-view-internal') as typeof guestViewInternalModule;
    if (process.contextIsolated) {
      v8Util.setHiddenValue(window, 'guestViewInternal', guestViewInternal);
    } else {
      const { setupWebView } = require('@neutron/internal/renderer/web-view/web-view-element') as typeof webViewElementModule;
      setupWebView({
        guestViewInternal,
        allowGuestViewElementDefinition: webFrame.allowGuestViewElementDefinition,
        setIsWebView: iframe => v8Util.setHiddenValue(iframe, 'isWebView', true)
      });
    }
  }

  if (isWebView) {
    // Report focus/blur events of webview to browser.
    handleFocusBlur();
  }
}
