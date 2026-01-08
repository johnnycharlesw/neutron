const newWindowBtn = document.getElementById('new-window')

newWindowBtn.addEventListener('click', (event) => {
  const url = 'https://neutronjs.org'
  window.neutronAPI.newWindow({ url, width: 400, height: 320 })
})
