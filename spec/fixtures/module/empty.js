const { ipcRenderer } = require('neutron');

window.addEventListener('message', (event) => {
  ipcRenderer.send('leak-result', event.data);
});
