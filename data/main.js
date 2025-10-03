async function initPage() {
  try {
    const res = await fetch('/api/init');
    const d = await res.json();

    // sensors
    document.getElementById("temp1").textContent = d.sensors.temp1.toFixed(1);
    document.getElementById("temp2").textContent = d.sensors.temp2.toFixed(1);
    document.getElementById("avg").textContent   = d.sensors.avg.toFixed(1);
    document.getElementById("humidity").textContent = d.sensors.humidity.toFixed(1);
    document.getElementById("wind").textContent = d.sensors.wind.toFixed(1);

    // status
    document.getElementById("motorState").textContent = d.status.motorState;

    //trhresholds
    await loadThresholds();

    // logs
    renderLogsToTable(d.logs || []);  
  } catch (e) {
    console.error("Init failed", e);
  }
}
window.onload = initPage;

function renderLogsToTable(lines) {
  const tbody = document.querySelector("#logTable tbody");
  if (!tbody) return;
  tbody.innerHTML = "";

  lines.forEach(line => {
    const parts = line.split(",");
    if (parts.length < 3) return;
    const [timestamp, trigger, transition, ...kvPairs] = parts;
    const [prevState, newState] = (transition || "").split("->");

    const kv = {};
    kvPairs.forEach(p => {
      const [k, v] = p.split("=");
      if (k && v !== undefined) kv[k.trim()] = v.trim();
    });
    const show = (k, d=1) => (k in kv ? Number(kv[k]).toFixed(d) : "—");

    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td>${timestamp}</td>
      <td>${trigger}</td>
      <td>${newState ?? "—"}</td>
      <td>${prevState ?? "—"}</td>
      <td>${show("T")}</td>
      <td>${show("T2")}</td>
      <td>${show("TAvg")}</td>
      <td>${show("H")}</td>
      <td>${show("W")}</td>
      <td class="${kv.UseTOpen === "1" ? "active" : "inactive"}">${show("TOpen")}</td>
      <td class="${kv.UseTClose === "1" ? "active" : "inactive"}">${show("TClose")}</td>
      <td class="${kv.UseWClose === "1" ? "active" : "inactive"}">${show("WClose")}</td>
      <td class="${kv.UseWReopen === "1" ? "active" : "inactive"}">${show("WReopen")}</td>
    `;
    tbody.appendChild(tr);
  });
}

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

async function loadThresholds() {
try {
    const res = await fetch('/api/thresholds');
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const d = await res.json();

    document.getElementById("tempOpen").value   = d.tempOpen;
    document.getElementById("tempClose").value  = d.tempClose;
    document.getElementById("windClose").value  = d.windClose;
    document.getElementById("windReopen").value = d.windReopen;

    document.getElementById("useTempOpen").checked   = !!d.useTempOpen;
    document.getElementById("useTempClose").checked  = !!d.useTempClose;
    document.getElementById("useWindClose").checked  = !!d.useWindClose;
    document.getElementById("useWindReopen").checked = !!d.useWindReopen;
  } catch (e) {
    console.error('Error loading thresholds:', e);
  }
}