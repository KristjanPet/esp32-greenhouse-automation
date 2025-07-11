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