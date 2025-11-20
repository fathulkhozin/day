// pages/api/ingest.js
export const config = { api: { bodyParser: true } };

if (!global._iotLatest) global._iotLatest = {};

export default async function handler(req, res) {
  if (req.method !== "POST")
    return res.status(405).json({ ok: false, msg: "Use POST" });

  const device = (req.query.device || "unknown").toString();
  
  const { tempDHT, humidity, relayStatus } = req.body || {};

  const payload = {
    device,
    tempDHT: Number(tempDHT),
    humidity: Number(humidity),
    relayStatus: Number(relayStatus),
    ts: Date.now()
  };

  global._iotLatest = payload;

  // broadcast jika pakai websocket
  if (global._wsServer?.clients) {
    for (const c of global._wsServer.clients) {
      try { c.send(JSON.stringify(payload)); } catch {}
    }
  }

  return res.status(200).json({ ok: true, saved: payload });
}
