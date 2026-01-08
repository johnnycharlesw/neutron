const exLinksBtn = document.getElementById('open-ex-links')
const fileManagerBtn = document.getElementById('open-file-manager')

fileManagerBtn.addEventListener('click', (event) => {
  window.neutronAPI.openHomeDir()
})

exLinksBtn.addEventListener('click', (event) => {
  window.neutronAPI.openExternal('https://neutronjs.org')
})
