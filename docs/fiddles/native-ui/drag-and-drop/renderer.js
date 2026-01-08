const dragFileLink = document.getElementById('drag-file-link')

dragFileLink.addEventListener('dragstart', event => {
  event.preventDefault()
  window.neutronAPI.dragStart()
})
