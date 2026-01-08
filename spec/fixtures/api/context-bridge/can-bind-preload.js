const { contextBridge, ipcRenderer } = require('neutron');

console.info(contextBridge);

let bound = false;
try {
  contextBridge.exposeInMainWorld('test', {});
  bound = true;
} catch {
  // Ignore
}

ipcRenderer.send('context-bridge-bound', bound);
