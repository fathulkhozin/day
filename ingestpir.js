// pages/api/ingest.js
export const config = { api: { bodyParser: true } };

if (!global._iotLatest) global._iotLatest = {};

export default function handler(req, res) {
  if (req.method !== "POST")
    return res.status(405).json({ ok: false, msg: "Use POST" });

  const device = (req.query.device || "unknown").toString();
  const { pir, relayStatus } = req.body || {};

  const payload = {
    device,
    pir: Number(pir),
    relayStatus: Number(relayStatus),
    ts: Date.now()
  };

  global._iotLatest = payload;

  return res.status(200).json({ ok: true, saved: payload });
}
