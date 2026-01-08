const { WebContentsView, app } = require('neutron');

app.whenReady().then(function () {
  // eslint-disable-next-line no-new
  new WebContentsView();

  app.quit();
});
