const versionInfoBtn = document.getElementById('version-info')

versionInfoBtn.addEventListener('click', () => {
  const message = `This app is using Electron version: ${window.neutronVersion}`
  document.getElementById('got-version-info').innerHTML = message
})
