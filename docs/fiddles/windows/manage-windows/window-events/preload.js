const { contextBridge, ipcRenderer } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronAPI', {
  showDemoWindow: () => ipcRenderer.send('show-demo-window'),
  focusDemoWindow: () => ipcRenderer.send('focus-demo-window'),
  onWindowFocus: (callback) => ipcRenderer.on('window-focus', () => callback()),
  onWindowBlur: (callback) => ipcRenderer.on('window-blur', () => callback()),
  onWindowClose: (callback) => ipcRenderer.on('window-close', () => callback())
})
