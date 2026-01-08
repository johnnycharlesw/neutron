const { ipcRenderer } = require('neutron');

ipcRenderer.send('pong');
