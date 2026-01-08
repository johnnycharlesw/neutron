const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  takeScreenshot: () => ipcRenderer.invoke('take-screenshot', window.devicePixelRatio)
})
