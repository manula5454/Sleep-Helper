// These three UUIDs must exactly match the ESP32's BLE service/characteristic UUIDs.
const SERVICE_UUID = "12345678-1234-1234-1234-1234567890ab";
const DATA_CHAR_UUID = "12345678-1234-1234-1234-1234567890ac";
const RELAY_CHAR_UUID = "12345678-1234-1234-1234-1234567890ad";

let dataCharacteristic = null;
let relayCharacteristic = null;
let relayIsOn = false;

const envChart = new Chart(document.getElementById('envChart'), {
  type: 'line',
  data: {
    labels: [],
    datasets: [
      { label: 'Temp (°C)', data: [], borderColor: '#e2725b', tension: 0.3 },
      { label: 'Humidity (%)', data: [], borderColor: '#3a7bd5', tension: 0.3 },
    ]
  },
  options: {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      x: {
        ticks: { color: '#9a9a95', maxTicksLimit: 6 },
        title: { display: true, text: 'Time', color: '#9a9a95' }
      },
      y: {
        ticks: { color: '#9a9a95' },
        title: { display: true, text: 'Value', color: '#9a9a95' }
      }
    },
    plugins: {
      legend: { labels: { color: '#f2f2f0' } },
      zoom: {
        pan: { enabled: true, mode: 'x' },
        zoom: {
          pinch: { enabled: true },
          wheel: { enabled: true },
          mode: 'x'
        }
      },
      annotation: {
        annotations: {
          tempBaseline: {
            type: 'line',
            yMin: 19, yMax: 19, // commonly recommended sleep temp, ~19°C
            borderColor: '#e2725b',
            borderWidth: 1,
            borderDash: [6, 4],
            label: { display: true, content: 'Ideal temp ~19°C', position: 'end', color: '#e2725b', font: { size: 10 } }
          },
          humidityBaseline: {
            type: 'line',
            yMin: 50, yMax: 50, // middle of commonly recommended 40-60% range
            borderColor: '#3a7bd5',
            borderWidth: 1,
            borderDash: [6, 4],
            label: { display: true, content: 'Ideal humidity ~50%', position: 'start', color: '#3a7bd5', font: { size: 10 } }
          }
        }
      }
    }
  }
});

const activityChart = new Chart(document.getElementById('activityChart'), {
  type: 'line',
  data: {
    labels: [],
    datasets: [
      { label: 'Light (raw)', data: [], borderColor: '#f2c14e', tension: 0.3 },
      { label: 'Sound (raw)', data: [], borderColor: '#6fcf97', tension: 0.3 },
    ]
  },
  options: {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      x: {
        ticks: { color: '#9a9a95', maxTicksLimit: 6 },
        title: { display: true, text: 'Time', color: '#9a9a95' }
      },
      y: {
        ticks: { color: '#9a9a95' },
        title: { display: true, text: 'Reading', color: '#9a9a95' }
      }
    },
    plugins: {
      legend: { labels: { color: '#f2f2f0' } },
      zoom: {
        pan: { enabled: true, mode: 'x' },
        zoom: {
          pinch: { enabled: true },
          wheel: { enabled: true },
          mode: 'x'
        }
      },
      annotation: {
        annotations: {
          lightBaseline: {
            type: 'line',
            yMin: 10, yMax: 10, // measured: your LDR reads ~10 at absolute darkness
            borderColor: '#f2c14e',
            borderWidth: 1,
            borderDash: [6, 4],
            label: { display: true, content: 'Dark room ~10', position: 'end', color: '#f2c14e', font: { size: 10 } }
          },
          soundBaseline: {
            type: 'line',
            // PLACEHOLDER: replace 100 with your own measured "quiet room" reading once you have one
            yMin: 100, yMax: 100,
            borderColor: '#6fcf97',
            borderWidth: 1,
            borderDash: [6, 4],
            label: { display: true, content: 'Quiet room (placeholder)', position: 'start', color: '#6fcf97', font: { size: 10 } }
          }
        }
      }
    }
  }
});

async function connectBLE() {
  try {
    setStatus('Scanning...', false);

    const device = await navigator.bluetooth.requestDevice({
      filters: [{ services: [SERVICE_UUID] }]
    });

    setStatus('Connecting...', false);
    const server = await device.gatt.connect();
    const service = await server.getPrimaryService(SERVICE_UUID);

    dataCharacteristic = await service.getCharacteristic(DATA_CHAR_UUID);
    relayCharacteristic = await service.getCharacteristic(RELAY_CHAR_UUID);

    await dataCharacteristic.startNotifications();
    dataCharacteristic.addEventListener('characteristicvaluechanged', onDataReceived);

    setStatus('Connected to ' + device.name, true);
    document.getElementById('relayBtn').disabled = false;

  } catch (error) {
    setStatus('Connection failed: ' + error.message, false);
  }
}

let lastChartUpdate = 0;
const CHART_UPDATE_INTERVAL = 60000/4; // only plot a new point once per minute, even though data arrives more often

function onDataReceived(event) {
  const decoder = new TextDecoder('utf-8');
  const text = decoder.decode(event.target.value);
  // Expected format from ESP32: "temp,humidity,light,sound,relayState"
  const parts = text.split(',');
  if (parts.length < 5) return;

  const temp = parseFloat(parts[0]);
  const hum = parseFloat(parts[1]);
  const light = parseFloat(parts[2]);
  const sound = parseFloat(parts[3]);
  const relayState = parts[4] === "1";

  // Live numbers update immediately, every single message
  document.getElementById('tempVal').textContent = temp.toFixed(1);
  document.getElementById('humVal').textContent = hum.toFixed(1);
  document.getElementById('lightVal').textContent = light.toFixed(0);
  document.getElementById('soundVal').textContent = sound.toFixed(0);

  // Keep the button in sync with the relay's real physical state,
  // including when the ESP32 switches it automatically (e.g. temp > threshold)
  relayIsOn = relayState;
  updateRelayButton();

  const time = new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });

  // Save every reading, regardless of chart throttling, so nothing is lost overnight
  saveReading(time, temp, hum, light, sound);

  // Chart only gets a new point once per CHART_UPDATE_INTERVAL, so an overnight
  // session stays readable instead of accumulating thousands of points
  const now = Date.now();
  if (now - lastChartUpdate < CHART_UPDATE_INTERVAL) return;
  lastChartUpdate = now;

  envChart.data.labels.push(time);
  envChart.data.datasets[0].data.push(temp);
  envChart.data.datasets[1].data.push(hum);

  activityChart.data.labels.push(time);
  activityChart.data.datasets[0].data.push(light);
  activityChart.data.datasets[1].data.push(sound);

  envChart.update();
  activityChart.update();
}

// ---- Persistence: survives refresh, tab close, browser restart ----

function saveReading(time, temp, hum, light, sound) {
  const existing = JSON.parse(localStorage.getItem('sleepData') || '[]');
  existing.push({ time, temp, hum, light, sound });
  localStorage.setItem('sleepData', JSON.stringify(existing));
}

function loadSavedData() {
  const existing = JSON.parse(localStorage.getItem('sleepData') || '[]');
  existing.forEach(r => {
    envChart.data.labels.push(r.time);
    envChart.data.datasets[0].data.push(r.temp);
    envChart.data.datasets[1].data.push(r.hum);
    activityChart.data.labels.push(r.time);
    activityChart.data.datasets[0].data.push(r.light);
    activityChart.data.datasets[1].data.push(r.sound);
  });
  envChart.update();
  activityChart.update();
}

function clearSavedData() {
  localStorage.removeItem('sleepData');
  location.reload();
}

// ---- Relay control ----

async function toggleRelay() {
  if (!relayCharacteristic) return;

  const desiredState = !relayIsOn;

  const encoder = new TextEncoder();
  const value = encoder.encode(desiredState ? "1" : "0");

  try {
    await relayCharacteristic.writeValue(value);
    relayIsOn = desiredState;
    updateRelayButton();
  } catch (error) {
    setStatus('Relay command failed: ' + error.message, false);
  }
}

function updateRelayButton() {
  const btn = document.getElementById('relayBtn');
  btn.textContent = 'Fan: ' + (relayIsOn ? 'ON' : 'OFF');
  btn.className = relayIsOn ? 'on' : '';
}

function setStatus(text, connected) {
  const el = document.getElementById('status');
  el.textContent = text;
  el.className = connected ? 'connected' : '';
}

// Restore any data saved from a previous session as soon as the page loads
loadSavedData();