const { app, webContents } = require('neutron');

app.whenReady().then(function () {
  webContents.create();

  app.quit();
});
