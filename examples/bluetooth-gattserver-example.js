var GattServer = require("../src/bluetooth").GattServer;

var gatt_server = new GattServer();
var val = 100;

var refId = null;
var countDown = function(updateValueCallback)
{
	val--;
	if (val <= 0) {
		clearInterval(refId);
	}
	updateValueCallback(new Buffer([val]));
}

gatt_server.start_advertising({
	type: "peripheral",
	serviceUuids: [ "00001802-0000-1000-8000-00805f9b34fb", "0000180f-0000-1000-8000-00805f9b34fb" ],
});

var descriptor = new GattServer.Descriptor({
	uuid: "ffffffff-ffff-ffff-ffff-abcdabcdabcd",
	properties: ["read", "write"],
	value: new Buffer([0x12]),
	onWriteRequest: function(buf, callback) {
		callback("ok");
	}
});

var characteristic = new GattServer.Characteristic({
	uuid: "00002a19-0000-1000-8000-00805f9b34fb",
	properties: ["read", "notify", "write"],
	value: new Buffer([0x66]),
	descriptors: [descriptor],
	onSubscribe: function(updateValueCallback) {
		refId = setInterval(countDown, 1000, updateValueCallback);
	},
	onWriteRequest: function(buf, callback) {
		callback("ok");
	}
});

var service = new GattServer.Service({
	uuid: "0000180f-0000-1000-8000-00805f9b34fb",
	characteristics: [
		characteristic
	]
});
