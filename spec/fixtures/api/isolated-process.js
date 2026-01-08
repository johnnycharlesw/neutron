const { ipcRenderer } = require('neutron');

ipcRenderer.send('context-isolation', process.contextIsolated);
