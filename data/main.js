async function updateData() {
    try {
    const res = await fetch('/api/sensors');
    const data = await res.json();
    document.getElementById('temp1').textContent = data.temp1;
    document.getElementById('temp2').textContent = data.temp2;
    document.getElementById('avg').textContent = data.avg;
    document.getElementById('humidity').textContent = data.humidity;
    document.getElementById('wind').textContent = data.wind;
    } catch (e) {
    console.error("Ni mogoče naložiti podatkov:", e);
    }
}

setInterval(updateData, 1500); // Update every 1.5 seconds
updateData();

async function pollStatus() {
  try {
    const res = await fetch('/api/status');
    const { manual, motorState } = await res.json();
    document.getElementById('motorState').textContent = motorState || '--';

    const btns = document.querySelectorAll('.motor-btn');
    btns.forEach(b => b.disabled = manual);

    const note = document.getElementById('manualLockNote');
    if (note) note.style.display = manual ? 'block' : 'none';
  } catch (e) {
    console.warn('Status check failed', e);
  }
}

setInterval(pollStatus, 1000);
pollStatus(); // run once on load

function sendMotorCommand(direction) {
  fetch('/api/motor', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ direction })
  })
  .then(res => res.text())
  .then(response => console.log(response))
  .catch(err => console.error('Error sending motor command:', err));
}

function sendThresholds() {
  const data = {
    tempOpen: parseFloat(document.getElementById("tempOpen").value),
    tempClose: parseFloat(document.getElementById("tempClose").value),
    windClose: parseFloat(document.getElementById("windClose").value),
    windReopen: parseFloat(document.getElementById("windReopen").value),

    useTempOpen: document.getElementById("useTempOpen").checked,
    useTempClose: document.getElementById("useTempClose").checked,
    useWindClose: document.getElementById("useWindClose").checked,
    useWindReopen: document.getElementById("useWindReopen").checked,
  };

  fetch('/api/thresholds', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify(data)
  }).then(res => {
    if (!res.ok) throw new Error("Ni mogoče poslati pragov");
    alert("Pragi shranjeni!");
  }).catch(err => {
    console.error(err);
    alert("Napaka pri pošiljanju pragov");
  });
}

function loadThresholds() {
  fetch('/api/thresholds')
    .then(res => res.json())
    .then(data => {
      document.getElementById("tempOpen").value = data.tempOpen;
      document.getElementById("tempClose").value = data.tempClose;
      document.getElementById("windClose").value = data.windClose;
      document.getElementById("windReopen").value = data.windReopen;

      document.getElementById("useTempOpen").checked = data.useTempOpen;
      document.getElementById("useTempClose").checked = data.useTempClose;
      document.getElementById("useWindClose").checked = data.useWindClose;
      document.getElementById("useWindReopen").checked = data.useWindReopen;
    })
    .catch(err => console.error('Error loading thresholds:', err));
}

async function loadLogs() {
  try {
    const res = await fetch('/api/logs?n=100');
    const arr = await res.json(); // array of strings
    document.getElementById('logBox').textContent = arr.join('\n');
  } catch(e) {
    console.warn('Log fetch failed', e);
  }
}
setInterval(loadLogs, 3000);
loadLogs();