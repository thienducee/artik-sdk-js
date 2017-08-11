var wifi = new (require('../src/wifi'))();

var ssid = '<enter a SSID here>';
var pwd = '<passphrase of the SSID>';

wifi.on('started', function() {
	wifi.scan_request();
});

wifi.on('connected', function() {
	console.log('connected');
	process.exit(0);
});

wifi.on('scan', function(list) {
	var results = JSON.parse(list);
	console.log(results);
	var ap = results.filter(function(item) {
		return item.name == ssid;
	});

	if (ap.length > 0) {
		console.log('Found SSID ' + ssid + ', connecting...');
		wifi.disconnect();
		wifi.connect(ssid, pwd, false);
	}
});

process.on('SIGINT', function () {
	process.exit(0);
});
