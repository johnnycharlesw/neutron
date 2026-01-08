import { BrowserWindow } from 'neutron/main';

import { expect } from 'chai';

import { closeAllWindows } from './lib/window-helpers';

describe('process._linkedBinding', () => {
  describe('in the main process', () => {
    it('can access neutron_browser bindings', () => {
      process._linkedBinding('neutron_browser_app');
    });

    it('can access neutron_common bindings', () => {
      process._linkedBinding('neutron_common_v8_util');
    });

    it('cannot access neutron_renderer bindings', () => {
      expect(() => {
        process._linkedBinding('neutron_renderer_ipc');
      }).to.throw(/No such binding was linked: neutron_renderer_ipc/);
    });
  });

  describe('in the renderer process', () => {
    afterEach(closeAllWindows);

    it('cannot access neutron_browser bindings', async () => {
      const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
      w.loadURL('about:blank');
      await expect(w.webContents.executeJavaScript('void process._linkedBinding(\'neutron_browser_app\')'))
        .to.eventually.be.rejectedWith(/Script failed to execute/);
    });

    it('can access neutron_common bindings', async () => {
      const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
      w.loadURL('about:blank');
      await w.webContents.executeJavaScript('void process._linkedBinding(\'neutron_common_v8_util\')');
    });

    it('can access neutron_renderer bindings', async () => {
      const w = new BrowserWindow({ show: false, webPreferences: { nodeIntegration: true, contextIsolation: false } });
      w.loadURL('about:blank');
      await w.webContents.executeJavaScript('void process._linkedBinding(\'neutron_renderer_ipc\')');
    });
  });
});
