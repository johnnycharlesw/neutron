const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  dragStart: () => ipcRenderer.send('ondragstart')
})
