const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  createDemoWindow: () => ipcRenderer.send('create-demo-window'),
  onBoundsChanged: (callback) => ipcRenderer.on('bounds-changed', () => callback())
})
