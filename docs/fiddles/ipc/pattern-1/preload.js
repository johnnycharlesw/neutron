const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  setTitle: (title) => ipcRenderer.send('set-title', title)
})
