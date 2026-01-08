const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('shell', {
  open: () => ipcRenderer.send('shell:open')
})
