line=$(osascript ./itunesTitle.scpt | tail -n 1)
curl  --data-urlencode text="$line"  -G "http://192.168.1.26/lcd"  --request GET > /dev/null
