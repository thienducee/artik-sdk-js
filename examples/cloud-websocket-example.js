var cloud = require('../src/cloud');
var artik = require('../src/');
var opt = require('getopt');
var fs = require('fs');

var access_token = "";
var device_id = "";
var test_message = '';
var security = new artik.security.Security();
var conn = new cloud();

try{
    opt.setopt("t:d:m:r:sv");
} catch (e){
    switch (e.type) {
        case "unknown":
            console.log("Unknown option: -%s", e.opt);
            console.log("Usage: node cloud-websocket-example.js [-t <access token>] [-d <device id>] [-m <JSON type test message>]" +
                "[-s for enabling SDR (Secure Device Registered) test] [-v for verifying root certificate] [-r <CA root file>]");
            break;
        case "required":
            console.log("Required parameter for option: -%s", e.opt);
            break;
        default:
            console.dir(e);
    }
    process.exit(0);
}

var ssl_config = {
    ca_cert: Buffer.from(""),
    verify_cert: "none",
    se_config : {
        key_id: "ARTIK/0",
        key_algo: "ecc_sec_p256r1"
    }
}

ssl_config.client_cert = security.get_certificate('ARTIK/0', 'ARTIK_SECURITY_CERT_TYPE_PEM');
ssl_config.client_key = security.get_publickey("ecc_sec_p256r1", 'ARTIK/0');

var ping_period = 20000;
var pong_timeout = 5000;

opt.getopt(function (o, p){
    switch(o){
        case 't':
            access_token = String(p);
            break;
        case 'd':
            device_id = String(p);
            break;
        case 'm':
            test_message = String(p);
            break;
        case 'r':
            var data = fs.readFileSync(String(p));
            ssl_config.ca_cert = Buffer.from(data);
            break;
        case 's':
        /*    ssl_config.se_config.key_id = fs.readFileSync(String(p));
            ssl_config.se_config.key_algo = fs.readFileSync(String(p));*/
            break;
        case 'v':
            ssl_config.verify_cert = "required";
            break;
        default:
            console.log("Unknown option: -%s", e.opt);
            console.log("Usage: node cloud-websocket-example.js [-t <access token>] [-d <device id>] [-m <JSON type test message>]" +
                "[-s for enabling SDR (Secure Device Registered) test] [-v for verifying root certificate] [-r <CA root file>]");
            process.exit(0);
    }
});

conn.on('connected', function(result) {
    console.log("Connect result: " + result);

    if (result != "CONNECTED")
        process.exit(0);
});

conn.on('receive', function(message) {
    console.log("Received: " + message);
});

conn.websocket_open_stream(access_token, device_id, ping_period, pong_timeout, ssl_config);

process.on('SIGINT', function () {
    console.log("Close stream");
    conn.websocket_close_stream();
    process.exit(0);
});

setInterval(function () {
    console.log("Writing: " + test_message);
    conn.websocket_send_message(test_message);
}, 1000);

setTimeout(function() {
    console.log("Time out, close stream");
    conn.websocket_close_stream();
    process.exit(0);
}, 5500);
