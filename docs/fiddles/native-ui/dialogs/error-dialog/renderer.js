const errorBtn = document.getElementById('error-dialog')

errorBtn.addEventListener('click', () => {
  window.neutronAPI.openErrorDialog()
})
