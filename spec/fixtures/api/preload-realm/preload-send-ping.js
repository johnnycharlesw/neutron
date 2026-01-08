const { ipcRenderer } = require('neutron');

ipcRenderer.send('ping');
