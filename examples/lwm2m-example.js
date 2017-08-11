var readline = require('readline');
var lwm2m = new(require('../src/lwm2m.js'))();

var server_id = 123;
var server_uri = 'coaps://coaps-api.artik.cloud:5686';
var lifetime = 30;
var dtls_psk_id = '< DM enabled ARTIK Cloud Device ID >';
var dtls_psk_key = '< DM enabled ARTIK Cloud Device Token >';

var objects = {
    device: {
        manufacturer: 'Samsung',
        model: 'ARTIK',
        serial: '1234567890',
        fwVersion: '1.0',
        hwVersion: '1.0',
        swVersion: '1.0',
        deviceType: 'Hub',
        powerSource: 0,
        powerVoltage: 5000,
        powerCurrent: 1500,
        batteryLevel: 100,
        memoryTotal: 1000000,
        memoryFree: 200000,
        timeZone: 'Europe/Paris',
        utcOffset: '+01:00',
        bindingModes: 'U'
    }
};

rl = readline.createInterface(process.stdin, process.stdout);
rl.setPrompt('lwm2m>');
rl.prompt();

function usage() {
    console.log('Commands:');
    console.log('\tread <uri>');
    console.log('\twrite <uri> <value>');
    console.log('\tquit');
}

rl.on('line', function(line) {
    if (line.trim().startsWith('read')) {
        var opts = line.trim().split(' ');
        if (opts.length != 2)
            usage();
        else {
            var buf = lwm2m.client_read_resource(opts[1].toString());
            console.log('URI: ' + opts[1] + ' - Value: ' + buf);
        }
    } else if (line.trim().startsWith('write')) {
        var opts = line.trim().split(' ');
        if (opts.length != 3)
            usage();
        else {
            var buf = new Buffer(opts[2]);
            lwm2m.client_write_resource(opts[1].toString(), buf);
        }
    } else if (line.trim().startsWith('quit')) {
        lwm2m.client_disconnect();
        process.exit(0);
    } else {
        usage();
    }
    rl.prompt();
});


lwm2m.on('started', function() {
    lwm2m.client_connect(server_id, server_uri, dtls_psk_id, lifetime, objects,
        dtls_psk_id, dtls_psk_key);
});

lwm2m.on('error', function(err) {
    console.log('\r\nLWM2M error: ' + err);
    rl.prompt();
});

lwm2m.on('execute', function(uri) {
    console.log('\r\nLWM2M execute: ' + uri);
    rl.prompt();
});

lwm2m.on('changed', function(uri) {
    console.log('\r\nLWM2M changed: ' + uri);
    rl.prompt();
});

process.on('SIGINT', function () {
    lwm2m.client_disconnect();
    process.exit(0);
});


