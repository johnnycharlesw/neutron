const { app } = require('neutron');

process.on('message', () => {
  console.log('Notified to quit');
  app.quit();
});
