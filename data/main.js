const slider = document.getElementById("myRange");
const btnRange = document.getElementById("btnRange");

async function initPage() {
  try {
    const res = await fetch("/api/init");
    const d = await res.json();

    // sensors
    document.getElementById("temp1").textContent = d.sensors.temp1.toFixed(1);
    document.getElementById("temp2").textContent = d.sensors.temp2.toFixed(1);
    document.getElementById("avg").textContent = d.sensors.avg.toFixed(1);
    document.getElementById("humidity").textContent =
      d.sensors.humidity.toFixed(1);
    document.getElementById("wind").textContent = d.sensors.wind.toFixed(1);

    // status
    document.getElementById("motorState").textContent =
      d.status.motorState + d.status.motorPercent + "%";

    slider.value = d.status.motorPercent;
    btnRange.textContent = d.status.motorPercent + "%";

    //trhresholds
    await loadThresholds();

    // logs
    renderLogsToTable(d.logs || []);
  } catch (e) {
    console.error("Init failed", e);
  }
}
window.onload = initPage;

function renderLogsToTable(entries) {
  const tbody = document.querySelector("#logTable tbody");
  if (!tbody) return;
  tbody.innerHTML = "";

  entries.forEach((e) => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td>${e.timestamp}</td>
      <td>${e.trigger}</td>
      <td>${e.state?.toFixed(0) + "%" ?? "—"}</td>
      <td>${e.sensors?.T?.toFixed(1) + " &deg;C" ?? "—"}</td>
      <td>${e.sensors?.T2?.toFixed(1) + " &deg;C" ?? "—"}</td>
      <td>${e.sensors?.TAvg?.toFixed(1) + " &deg;C" ?? "—"}</td>
      <td>${e.sensors?.H?.toFixed(1) + "%" ?? "—"}</td>
      <td>${e.sensors?.W?.toFixed(1) + " m/s" ?? "—"}</td>
      <td class="${e.thresholds?.UseTOpen ? "active" : "inactive"}">${
      e.thresholds?.TOpen ?? "-"
    }</td>
      <td class="${e.thresholds?.UseTClose ? "active" : "inactive"}">${
      e.thresholds?.TClose ?? "-"
    }</td>
      <td class="${e.thresholds?.UseWClose ? "active" : "inactive"}">${
      e.thresholds?.WClose ?? "-"
    }</td>
      <td class="${e.thresholds?.UseWReopen ? "active" : "inactive"}">${
      e.thresholds?.WReopen ?? "-"
    }</td>
    `;
    tbody.appendChild(tr);
  });
}

function sendMotorCommand(direction) {
  fetch("/api/motor", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ direction }),
  })
    .then((res) => res.text())
    .then((response) => console.log(response))
    .catch((err) => console.error("Error sending motor command:", err));
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

  fetch("/api/thresholds", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify(data),
  })
    .then((res) => {
      if (!res.ok) throw new Error("Ni mogoče poslati pragov");
      alert("Pragi shranjeni!");
    })
    .catch((err) => {
      console.error(err);
      alert("Napaka pri pošiljanju pragov");
    });
}

async function loadThresholds() {
  try {
    const res = await fetch("/api/thresholds");
    if (!res.ok) throw new Error("HTTP " + res.status);
    const d = await res.json();

    document.getElementById("tempOpen").value = d.tempOpen;
    document.getElementById("tempClose").value = d.tempClose;
    document.getElementById("windClose").value = d.windClose;
    document.getElementById("windReopen").value = d.windReopen;

    document.getElementById("useTempOpen").checked = !!d.useTempOpen;
    document.getElementById("useTempClose").checked = !!d.useTempClose;
    document.getElementById("useWindClose").checked = !!d.useWindClose;
    document.getElementById("useWindReopen").checked = !!d.useWindReopen;
  } catch (e) {
    console.error("Error loading thresholds:", e);
  }
}

btnRange.onclick = function () {
  const value = parseInt(slider.value, 10);
  sendMotorCommand(value);
};

// Update button text when slider moves
slider.oninput = function () {
  btnRange.textContent = this.value + "%";
};
