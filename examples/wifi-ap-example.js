var exec = require('child_process').execSync;
var wifi = new (require('../src/wifi'))();

var ssid = "SimpleTestAP"; //'<enter a SSID here>';
var pwd = "test1234"; //'<passphrase of the SSID>';
var channel = 1; //'<Number channel of the AP>';
var openmode = false; //'<En/Disable wpa2 authentification>'

if (process.argv.length >= 3)
	ssid = process.argv[2];
if (process.argv.length >= 4)
	pwd = process.argv[3];
if (process.argv.length >= 5)
	channel = Number(process.argv[4]);
if (process.argv.length >= 6)
	openmode = (process.argv[5] == "true");

wifi.on('started', function() {
	console.log("Starting Wi-Fi Access Point");
	console.log("SSID : '" + ssid + "'");
	console.log("Pass : '" + pwd + "'");
	console.log("Channel : '" + channel + "'");
	console.log("OpenMode : '" + openmode + "'");
	var res = wifi.start_ap(ssid, pwd, channel, openmode ?
			wifi.WIFI_ENCRYPTION_OPEN : wifi.WIFI_ENCRYPTION_WPA2);
	console.log("Result => '" + res + "'");
	process.exit(0);
});
