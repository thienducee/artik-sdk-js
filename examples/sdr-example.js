var cloud = require('../src/cloud');
var opt = require('getopt');

var artik_cloud = new cloud();

var dtid = '';
var vid = '';

try{
    opt.setopt("d:v:");
} catch (e){
   switch (e.type) {
        case "unknown":
            console.log("Unknown option: -%s", e.opt);
            console.log("Usage: node sdr-example.js [-d <device type ID>] [-v <vendor id>]");
            break;
        case "required":
            console.log("Required parameter for option: -%s", e.opt);
            break;
        default:
            console.dir(e);
    }
    process.exit(0);
}

opt.getopt(function (o, p){
    switch(o){
    case 'd':
        dtid = String(p);
        break;
    case 'v':
        vid = String(p);
        break;
    default:
        console.log("Usage: node sdr-example.js [-d <device type ID>] [-v <vendor id>]");
        process.exit(0);
    }
});

var regId = '';
var regNonce = '';

var se_config = {
        key_id: "ARTIK/0",
        key_algo: "ecc_sec_p256r1"
    }

function getRegistrationStatus(err, response) {
    if (err) {
        console.log("Error: " + err);
        process.exit(-1);
    }

    var regStatus = response.data.status;
    if (regStatus == "PENDING_USER_CONFIRMATION") {
        setTimeout(function () {
            artik_cloud.sdr_registration_status(se_config, regId, getRegistrationStatus);
        }, 1000);
    } else if (regStatus == "PENDING_DEVICE_COMPLETION") {
        artik_cloud.sdr_complete_registration(se_config, regId, regNonce, function(err, response) {
            if (err) {
                console.log("Error: " + err);
                process.exit(-1);
            }

            console.log('Response: ' + JSON.stringify(response));
            process.exit(0);
        });
    }
}

artik_cloud.sdr_start_registration(se_config, dtid, vid, function(err, response) {
    if (err) {
        console.log("Error: " + err);
        process.exit(-1);
    }

    regId = response.data.rid;
    regNonce = response.data.nonce;

    console.log('Enter pin ' + response.data.pin + ' in the ARTIK Cloud portal');

    artik_cloud.sdr_registration_status(se_config, regId, getRegistrationStatus);
});
