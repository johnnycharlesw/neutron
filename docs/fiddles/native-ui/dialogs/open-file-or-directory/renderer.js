const selectDirBtn = document.getElementById('select-directory')

selectDirBtn.addEventListener('click', async () => {
  const path = await window.neutronAPI.openFileDialog()
  document.getElementById('selected-file').innerHTML = `You selected: ${path}`
})
