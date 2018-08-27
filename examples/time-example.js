/*
 * Time module test. Note that any running NTP service must be disabled prio
 * to running this test. On Fedora based ARTIK boards, use the following command:
 *
 *   $ systemctl stop systemd-timesyncd
 */

var opt = require('getopt');
var time = require('../src/time.js');

var module  = new time();
var alarm_1 = null;
var alarm_2 = null;
var hostname = "fr.pool.ntp.org";
var ntp_timeout_ms = 3000;

try {
    opt.setopt("t:h:?::");
} catch (e) {
    switch (e.type) {
        case "unknown":
            console.log("Unknown option: -%s", e.opt);
            break;
        case "required":
            console.log("Required parameter for option: -%s",  e.opt);
            break;
        default:
            console.dir(e);
            break;
    }
    process.exit(0);
}

opt.getopt(function (o, p) {
    switch(o){
    case 't':
        ntp_timeout_ms = parseInt(p);
        break;
    case 'h':
        hostname = String(p);
        break;
    case '?':
        printf("Usage: time-test -t <ntp timeout ms> -h <ntp name server>");
        process.exit(0);
    default:
        printf("Usage: time-test -t <ntp timeout ms> -h <ntp name server>");
        printf("-help: give this help list\n");
        process.exit(0);
    }
});

console.log("Synchronizing with NTP server " + hostname);
console.log(module.sync_ntp(hostname, ntp_timeout_ms) == 0 ? "Sync successful": "Sync failed");

var date = module.get_time_str('%I:%M:%S-%w-%d/%m/%Y', 2);
var curr_date = module.get_time();
var alarm1_date = module.get_time();
var alarm2_date = module.get_time();
var time_zone = module.ARTIK_TIME_GMT1;
var timestamp = Math.floor(Date.now()/1000); // Get timestamp in seconds
alarm1_date.setUTCSeconds(alarm1_date.getUTCSeconds() + 5);
alarm2_date.setUTCSeconds(alarm2_date.getUTCSeconds() + 20);

console.log("Date in format: " + date);
console.log("Current time is " + curr_date.toUTCString());
console.log("Timestamp of current date is " + module.convert_time_to_timestamp(curr_date) + " s");
console.log("Current timestamp is " + timestamp + " s");
console.log("Date of current timestamps is " + module.convert_timestamp_to_time(timestamp).toUTCString());
console.log("Set alarm 1 to trigger at " + alarm1_date.toUTCString());
console.log("Set alarm 2 to trigger at " + alarm2_date.toUTCString());

try {
    alarm_1 = module.create_alarm_date(time_zone, alarm1_date, function() {
        console.log("Alarm 1 triggered, it is " + module.get_time().toUTCString());
    });

} catch (err) {
    console.log("[ERROR] create_alarm : " + err);
    process.exit(-1);
}

try {
    alarm_2 = module.create_alarm_second(time_zone, 20, function() {
        console.log("Alarm 2 triggered, it is " + module.get_time().toUTCString());
        process.exit(0);
    });
} catch (err) {
    console.log("[ERROR] create_alarm : " + err);
    process.exit(-1);
}

var delay = alarm_1.get_delay();
console.log("The delay for alarm 1 is " + delay + " s.");

delay = alarm_2.get_delay();
console.log("The delay for alarm 2 is " + delay + " s.");

process.on('SIGINT', () => {
    process.exit(-1);
});
