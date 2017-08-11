const artik = require('../src');

const name = artik.get_platform_name();

if(name == 'ARTIK 520') {
	console.log("Running SERIAL test on ARTIK 520");
	const a5 = require('../src/platform/artik520');
	var port = a5.ARTIK_A5_SERIAL.SCOM.XSCOM4
} else if(name == 'ARTIK 1020') {
	console.log("Running SERIAL test on ARTIK 1020");
	const a10 = require('../src/platform/artik1020');
	var port = a10.ARTIK_A10_SERIAL.SCOM.XSCOM2;
} else if(name == 'ARTIK 710') {
	console.log("Running SERIAL test on ARTIK 710");
	const a7 = require('../src/platform/artik710');
	var port = a7.ARTIK_A710_SERIAL.UART.UART0;
} else if(name == 'ARTIK 530') {
	console.log("Running SERIAL test on ARTIK 530");
	const a530 = require('../src/platform/artik530');
	var port = a530.ARTIK_A530_SERIAL.UART.UART4;
}

var loopback = new artik.serial(port,
				"serial-loopback",
				115200,
				"none",
				8,
				1,
				"none");

var buf = new Buffer('aabbccddeeff');

loopback.request();

loopback.on('read', function(data) {
    if (data)
		console.log("Rx Data: " + data);
});

var written = loopback.write(buf);
console.log("Wrote " + written + " bytes");

process.on('SIGINT', function () {
    loopback.release();
    process.kill(process.pid);
});
