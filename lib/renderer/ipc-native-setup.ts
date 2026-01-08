import { ipcRendererInternal } from '@neutron/internal/renderer/ipc-renderer-internal';

import { ipcRenderer } from 'neutron/renderer';

const v8Util = process._linkedBinding('neutron_common_v8_util');

// ElectronApiServiceImpl will look for the "ipcNative" hidden object when
// invoking the 'onMessage' callback.
v8Util.setHiddenValue(globalThis, 'ipcNative', {
  onMessage (internal: boolean, channel: string, ports: MessagePort[], args: any[]) {
    const sender = internal ? ipcRendererInternal : ipcRenderer;
    sender.emit(channel, { sender, ports }, ...args);
  }
});
