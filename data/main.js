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

setInterval(updateData, 500);
updateData();

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
