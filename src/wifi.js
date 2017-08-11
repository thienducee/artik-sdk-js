var events = require('events');
var util = require('util');
var wifi = require('../build/Release/artik-sdk.node').wifi;

var Wifi = function(){
	events.EventEmitter.call(this);
	this.wifi = new wifi();
	setImmediate(function(self) {
		self.emit('started');
	}, this);
}

util.inherits(Wifi, events.EventEmitter);

module.exports = Wifi;

Wifi.prototype.WIFI_ENCRYPTION_OPEN = 0x00000000;
Wifi.prototype.WIFI_ENCRYPTION_WEP = 0x00000001;
Wifi.prototype.WIFI_ENCRYPTION_WPA = 0x00000002;
Wifi.prototype.WIFI_ENCRYPTION_WPA2 = 0x00000004;
Wifi.prototype.WIFI_ENCRYPTION_WPA2_PERSONAL = 0x00010000;
Wifi.prototype.WIFI_ENCRYPTION_WPA2_ENTERPRISE = 0x00020000;

Wifi.prototype.scan_request = function() {
	var _ = this;
	return this.wifi.scan_request(function(list) {
		_.emit('scan', list);
	});
};

Wifi.prototype.connect = function(ssid, password, is_persistent) {
	var _ = this;
	return this.wifi.connect(ssid, password, is_persistent, function() {
		_.emit('connected');
	});
};

Wifi.prototype.disconnect = function() {
	return this.wifi.disconnect();
};

Wifi.prototype.get_scan_result = function() {
	return this.wifi.get_scan_result();
};

Wifi.prototype.start_ap = function(ssid, password, channel, encryption) {
	return this.wifi.start_ap(ssid, password, channel, encryption);
};
