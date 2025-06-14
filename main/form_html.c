const char *form_html =
"<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
"<style>body { font-family: sans-serif; max-width: 400px; margin: auto; padding: 1rem; }"
"input, select { width: 100%; padding: 8px; margin: 6px 0; box-sizing: border-box; }"
"input[type=submit] { background-color: #4CAF50; color: white; border: none; cursor: pointer; }"
"input[type=submit]:hover { background-color: #45a049; }</style>"
"<script>"
"async function scanNetworks() {"
"  const res = await fetch('/scan');"
"  const data = await res.json();"
"  console.log(data);"
"  const select = document.getElementById('ssid_list');"
"  select.innerHTML = '';"
"  const defaultOpt = document.createElement('option');"
"  defaultOpt.text = '-- Seleziona una rete --';"
"  defaultOpt.value = '';"
"  select.appendChild(defaultOpt);"
"  data.forEach(ssid => {"
"    const opt = document.createElement('option');"
"    opt.value = ssid; opt.text = ssid;"
"    select.appendChild(opt);"
"  });"
"}"
"</script></head><body>"
"<h2>Soil Sensor Setup</h2>"
"<button onclick='scanNetworks()'>Scansiona reti</button><br><br>"
"<form method='POST'>"
"SSID:<br><select id='ssid_list' name='ssid'>"
"<option value=\"\">-- Seleziona una rete --</option>"
"</select><br>"
"Wi-Fi Password:<br><input name='password' type='password'><br>"
"MQTT Host:<br><input name='mqtt_host'><br>"
"MQTT Port:<br><input name='mqtt_port' type='number' value='1883'><br>"
"MQTT User:<br><input name='mqtt_user'><br>"
"MQTT Password:<br><input name='mqtt_pass' type='password'><br>"
"Sleep Interval (minutes):<br><input name='sleep_interval' type='number' value='5'><br><br>"
"<input type='submit' value='Save & Reboot'>"
"</form></body></html>";
