const { contextBridge } = require('neutron/renderer')

contextBridge.exposeInMainWorld('neutronVersion', process.versions.neutron)
