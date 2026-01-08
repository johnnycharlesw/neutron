const backBtn = document.getElementById('backBtn')
const forwardBtn = document.getElementById('forwardBtn')
const backHistoryBtn = document.getElementById('backHistoryBtn')
const forwardHistoryBtn = document.getElementById('forwardHistoryBtn')
const urlInput = document.getElementById('urlInput')
const goBtn = document.getElementById('goBtn')
const historyPanel = document.getElementById('historyPanel')

async function updateButtons () {
  const canGoBack = await window.neutronAPI.canGoBack()
  const canGoForward = await window.neutronAPI.canGoForward()
  backBtn.disabled = !canGoBack
  backHistoryBtn.disabled = !canGoBack

  forwardBtn.disabled = !canGoForward
  forwardHistoryBtn.disabled = !canGoForward
}

async function updateURL () {
  urlInput.value = await window.neutronAPI.getCurrentURL()
}

function transformURL (url) {
  if (!url.startsWith('http://') && !url.startsWith('https://')) {
    const updatedUrl = 'https://' + url
    return updatedUrl
  }
  return url
}

async function navigate (url) {
  const urlInput = transformURL(url)

  await window.neutronAPI.loadURL(urlInput)
}

async function showHistory (forward = false) {
  const history = await window.neutronAPI.getHistory()
  const currentIndex = history.findIndex(entry => entry.url === transformURL(urlInput.value))

  if (!currentIndex) {
    return
  }

  const relevantHistory = forward
    ? history.slice(currentIndex + 1)
    : history.slice(0, currentIndex).reverse()

  historyPanel.innerHTML = ''
  relevantHistory.forEach(entry => {
    const div = document.createElement('div')
    div.textContent = `Title: ${entry.title}, URL: ${entry.url}`
    div.onclick = () => navigate(entry.url)
    historyPanel.appendChild(div)
  })

  historyPanel.style.display = 'block'
}

backBtn.addEventListener('click', () => window.neutronAPI.goBack())
forwardBtn.addEventListener('click', () => window.neutronAPI.goForward())
backHistoryBtn.addEventListener('click', () => showHistory(false))
forwardHistoryBtn.addEventListener('click', () => showHistory(true))
goBtn.addEventListener('click', () => navigate(urlInput.value))

urlInput.addEventListener('keypress', (e) => {
  if (e.key === 'Enter') {
    navigate(urlInput.value)
  }
})

document.addEventListener('click', (e) => {
  if (e.target !== historyPanel && !historyPanel.contains(e.target) &&
    e.target !== backHistoryBtn && e.target !== forwardHistoryBtn) {
    historyPanel.style.display = 'none'
  }
})

window.neutronAPI.onNavigationUpdate(() => {
  updateButtons()
  updateURL()
})

updateButtons()
