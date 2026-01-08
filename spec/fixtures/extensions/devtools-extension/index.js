/* global chrome */
chrome.devtools.inspectedWindow.eval('require("neutron").ipcRenderer.send("winning")', (result, exc) => {
  console.log(result, exc);
});
