const { ipcRenderer } = require('neutron');

// Ensure fetch works from isolated world origin
fetch('https://localhost:1234').catch(err => {
  ipcRenderer.send('isolated-fetch-error', err.message);
});
