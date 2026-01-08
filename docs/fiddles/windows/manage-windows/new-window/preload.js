const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  newWindow: (args) => ipcRenderer.send('new-window', args)
})
