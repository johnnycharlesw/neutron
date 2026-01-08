const { contextBridge, ipcRenderer } = require('neutron');

// NOTE: Never do this in an actual app! Very insecure!
contextBridge.exposeInMainWorld('ipc', {
  send (...args) {
    return ipcRenderer.send(...args);
  },
  sendSync (...args) {
    return ipcRenderer.sendSync(...args);
  },
  invoke (...args) {
    return ipcRenderer.invoke(...args);
  }
});
