// pages/index.js
import { useEffect, useState } from "react";

export default function Home() {
  const [latest, setLatest] = useState(null);
  const [log, setLog] = useState([]);

  const pushLog = (m) =>
    setLog((p) => [`[${new Date().toLocaleTimeString()}] ${m}`, ...p].slice(0, 200));

  useEffect(() => {
    pushLog("Polling /api/latest tiap 2 detik...");

    const iv = setInterval(async () => {
      try {
        const res = await fetch("/api/latest");
        if (!res.ok) throw new Error(res.statusText);
        const data = await res.json();
        setLatest(data);
        if (data?.ts) pushLog("Update: " + JSON.stringify(data));
      } catch (e) {
        pushLog("REST error: " + e.message);
      }
    }, 2000);

    return () => clearInterval(iv);
  }, []);

  return (
    <div style={{ maxWidth: 900, margin: "30px auto", padding: 16 }}>
      <h2>ESP32 → Next.js (REST Dashboard)</h2>

      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 16 }}>
        
        {/* Data */}
        <div style={{ border: "1px solid #ddd", borderRadius: 12, padding: 16 }}>
          <h3>Data Sensor</h3>
          {latest ? (
            <ul>
              <li><b>device</b>: {latest.device}</li>
              <li><b>tempDHT</b>: {latest.tempDHT} °C</li>
              <li><b>humidity</b>: {latest.humidity} %</li>
              <li><b>relay</b>: {latest.relayStatus ? "ON" : "OFF"}</li>
              <li><b>timestamp</b>: {new Date(latest.ts).toLocaleString()}</li>
            </ul>
          ) : <p>Menunggu data...</p>}
        </div>

        {/* Log */}
        <div style={{ border: "1px solid #ddd", borderRadius: 12, padding: 16, maxHeight: 360, overflow: "auto" }}>
          <h3>Log</h3>
          <pre>{log.join("\n")}</pre>
        </div>
      </div>
    </div>
  );
}
