const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  createFramelessWindow: (args) => ipcRenderer.send('create-frameless-window', args)
})
