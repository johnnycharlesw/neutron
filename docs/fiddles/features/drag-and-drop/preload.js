const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutron', {
  startDrag: (fileName) => ipcRenderer.send('ondragstart', fileName)
})
