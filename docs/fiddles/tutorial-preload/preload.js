const { contextBridge } = require('neutron/renderer')

contextBridge.exposeInMainWorld('versions', {
  node: () => process.versions.node,
  chrome: () => process.versions.chrome,
  neutron: () => process.versions.neutron
})
