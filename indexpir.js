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
        const data = await res.json();
        setLatest(data);
        pushLog(JSON.stringify(data));
      } catch (e) {
        pushLog("Error: " + e.message);
      }
    }, 2000);

    return () => clearInterval(iv);
  }, []);

  return (
    <div style={{ maxWidth: 900, margin: "30px auto", padding: 16 }}>
      <h2>ESP32 + PIR Sensor → Next.js Dashboard</h2>

      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 16 }}>
        
        {/* Data */}
        <div style={{ border: "1px solid #ddd", borderRadius: 12, padding: 16 }}>
          <h3>Data Terbaru</h3>
          {latest ? (
            <ul>
              <li><b>device</b>: {latest.device}</li>
              <li><b>PIR</b>: {latest.pir === 1 ? "Gerakan Terdeteksi" : "Tidak Ada Gerakan"}</li>
              <li><b>Relay</b>: {latest.relayStatus ? "ON" : "OFF"}</li>
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
