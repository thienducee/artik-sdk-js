var artik = require('../src');
var opt = require('getopt');
var fs = require('fs');

const artik_coap = require('../src/coap');

var device_token = "";
var payload = "";
var get_method = true;

var messages_path = "v1.1/messages/";
var actions_path = "v1.1/actions/";

var client_cert =
	"-----BEGIN CERTIFICATE-----\n" +
	"MIIB4TCCAYegAwIBAgIJAJvNMfZLercmMAoGCCqGSM49BAMCME0xCzAJBgNVBAYT\n" +
	"AkZSMQwwCgYDVQQIDANJZEYxDjAMBgNVBAcMBVBhcmlzMRAwDgYDVQQKDAdTYW1z\n" +
	"dW5nMQ4wDAYDVQQLDAVBcnRpazAeFw0xODA0MjMxNTM0MjVaFw0xOTA0MjMxNTM0\n" +
	"MjVaME0xCzAJBgNVBAYTAkZSMQwwCgYDVQQIDANJZEYxDjAMBgNVBAcMBVBhcmlz\n" +
	"MRAwDgYDVQQKDAdTYW1zdW5nMQ4wDAYDVQQLDAVBcnRpazBZMBMGByqGSM49AgEG\n" +
	"CCqGSM49AwEHA0IABKXl5NswY/mFN+kOslUjIJCMLMleTxu6cOZmphceJhtn+9a7\n" +
	"0kukS38y3JivmUYQ1sD6lghw5pxUJlL4GIbczZmjUDBOMB0GA1UdDgQWBBTg5euL\n" +
	"zcnBcY1SHoT+bq9lkOnPqDAfBgNVHSMEGDAWgBTg5euLzcnBcY1SHoT+bq9lkOnP\n" +
	"qDAMBgNVHRMEBTADAQH/MAoGCCqGSM49BAMCA0gAMEUCIE0Y8P1HpQ05RmnLqW5+\n" +
	"qVpvTzbQiQZALLvrPQLcwlSDAiEAiLYz3WTQeGjYmT/7F+c3vwWQp5XWjR8JGNjL\n" +
	"oC39p5Q=\n" +
	"-----END CERTIFICATE-----\n";

var client_key =
	"-----BEGIN EC PARAMETERS-----\n" +
	"BggqhkjOPQMBBw==\n" +
	"-----END EC PARAMETERS-----\n" +
	"-----BEGIN EC PRIVATE KEY-----\n" +
	"MHcCAQEEIIo1HWeuNp2tfYlrvEZo70rXvUJblFhkx0bizV/NLkieoAoGCCqGSM49\n" +
	"AwEHoUQDQgAEpeXk2zBj+YU36Q6yVSMgkIwsyV5PG7pw5mamFx4mG2f71rvSS6RL\n" +
	"fzLcmK+ZRhDWwPqWCHDmnFQmUvgYhtzNmQ==\n" +
	"-----END EC PRIVATE KEY-----\n";

function pad(d) {
	return (d < 10) ? '0' + d.toString() : d.toString();
}

function buf2hex(buffer) {
	return Array.prototype.map.call(new Uint8Array(buffer), x => ('00' + x.toString(16)).slice(-2)).join('');
}

function print_response(msg) {
	console.log("==[ CoAP Response ]============================================");
	console.log("MID    : " + msg.msg_id);
	console.log("Token  : " + buf2hex(msg.token));
	console.log("Type   : " + msg.msg_type);
	console.log("Status : " + (msg.code >> 5) + "." + (pad(msg.code & 0x1f)));

	if (msg.options && msg.options.length > 0) {
		process.stdout.write("Options: ");
		var options_str = "";
		for (var i = 0; i < msg.options.length; i++) {
			switch(msg.options[i].key) {
			case artik_coap.OPTION_KEY.OBSERVE:
				options_str += ("\"Observe\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.CONTENT_FORMAT:
				options_str += ("\"Content-Format\":" + msg.options[i].data);
				break;
			default:
				break;
			}
			if (i + 1 != msg.options.length)
				options_str += ", ";
		}
		console.log("{" + options_str + "}");
	}

	console.log("Payload: " + Buffer.from(msg.data).length + " Bytes");

	if (Buffer.from(msg.data) && Buffer.from(msg.data).length > 0) {
		console.log("---------------------------------------------------------------");
		console.log("" + Buffer.from(msg.data));
	}

	console.log("===============================================================");
}

try {
	opt.setopt("t:m:d:");
} catch (e) {
	switch (e.type) {
	case "unknown":
		console.log("Unknown option: -%s", e.opt);
		console.log("Usage: node coap-cloud-example.js" +
			" [-m method (get/GET or post/POST, GET by default)]" +
			" [-t <device_token>]" +
			" [-d <data (in POST request)>");
		break;
	case "required":
		console.log("Required parameter for option: -%s", e.opt);
		console.log("Usage: node coap-cloud-example.js" +
			" [-m method (get/GET or post/POST, GET by default)]" +
			" [-t <device_token>]" +
			" [-d <data (in POST request)>");
		break;
	default:
		console.dir(e);
	}
	process.exit(0);
}

var coap_config = {
	uri: "coaps://coaps-api.artik.cloud/",
	ssl: {
		client_cert: Buffer.from(client_cert),
		client_key: Buffer.from(client_key)
	}
}

opt.getopt(function(o, p){
	switch(o) {
	case 't':
		device_token = String(p);
		break;
	case 'm':
		var method = String(p);
		if (method == "get" || method == "GET")
			get_method = true;
		else if (method == "post" || method == "POST")
			get_method = false;
		else {
			console.log("Unknown method (select get/GET or post/POST)");
			process.exit(-1);
		}
		break;
	case 'd':
		payload = String(p);
		break;
	default:
		console.log("Usage: node coap-cloud-example.js" +
			" [-m method (get/GET or post/POST, GET by default)]" +
			" [-t <device_token>]" +
			" [-d <data (in POST request)>");
		process.exit(0);
	}
});

if (device_token == "") {
	console.log("Missing device token");
	process.exit(-1);
}

if (!get_method && Buffer.from(payload) == "") {
	console.log("Missing data");
	process.exit(-1);
}

var coap = new artik.coap(coap_config);

coap.create_client();

coap.connect();

if (!get_method) {
	var path_cloud = messages_path + device_token;

	var coap_msg = {
		msg_type: "CON",
		code: artik_coap.CODE.POST,
		data: Buffer.from(payload)
	}

	coap.send_message(path_cloud, coap_msg, function(resp, error) {
		if (error != "NONE" || resp == undefined) {
			console.log("Fail to receive message");
			process.exit(-1);
		}

		print_response(resp);
	});
} else {
	var path_cloud = actions_path + device_token;

	coap.observe(path_cloud, "CON", function(resp, error) {
		if (error != "NONE" || resp == undefined) {
			console.log("Fail to observe");
			process.exit(-1);
		}

		print_response(resp);
	});
}

process.on('SIGINT', function () {
	if (get_method) {
		var path_cloud = actions_path + device_token;

		coap.cancel_observe(path_cloud);
	}

	coap.disconnect();
	coap.destroy_client();

	process.exit(0);
});
