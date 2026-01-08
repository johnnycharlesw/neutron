const { app, ipcMain } = require('neutron');

app.whenReady().then(() => {
  process.stdout.write(JSON.stringify(ipcMain.eventNames()));
  process.stdout.end();

  app.quit();
});
