const counter = document.getElementById('counter')

window.neutronAPI.onUpdateCounter((value) => {
  const oldValue = Number(counter.innerText)
  const newValue = oldValue + value
  counter.innerText = newValue.toString()
  window.neutronAPI.counterValue(newValue)
})
