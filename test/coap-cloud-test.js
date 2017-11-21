/* Global Includes */
var testCase   = require('mocha').describe;
var pre        = require('mocha').before;
var preEach    = require('mocha').beforeEach;
var post       = require('mocha').after;
var postEach   = require('mocha').afterEach;
var assertions = require('mocha').it;
var assert     = require('chai').assert;
var expect     = require('chai').expect;
var validator  = require('validator');
var exec       = require('child_process').execSync;

var artik = require('../src');
var fs    = require('fs');

const artik_coap = require('../src/coap');

var device_token 	= process.env.COAP_DEVICE_TOKEN;
var payload 		= process.env.COAP_DATA;
var messages_path 	= "v1.1/messages/";
var actions_path 	= "v1.1/actions/";

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

var config = {
	uri: "coaps://coaps-api.artik.cloud/",
	ssl: {
		client_cert: Buffer.from(client_cert),
		client_key: Buffer.from(client_key)
	}
}

var coap = new artik.coap(config);

testCase('CoAP', function() {

	preEach(function() {
		coap.create_client();
	});

	testCase("#observe()", function() {

		assertions('Connect and observe a resource', function(done) {
			this.timeout(16000);

			coap.connect();

			var path_cloud = actions_path + device_token;
			var disconnected = false;

			coap.observe(path_cloud, "CON", function(msg, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(msg);
				print_response(msg);
				expect(msg.code).to.satisfy(function(code) {
					if ((code == artik_coap.CODE.VALID) ||
						(code == artik_coap.CODE.CONTENT))
						return true;
					else {
						coap.cancel_observe(path_cloud);
						coap.disconnect();
						disconnected = true;
						return false;
					}
				});
			});

			setTimeout(function() {
				if (!disconnected) {
					coap.cancel_observe(path_cloud);
					coap.disconnect();
					done();
				}
			}, 15000);

		});

	});

	testCase('#send_message()', function() {

		assertions('Connect and post on a resource', function(done) {
			this.timeout(10000);

			coap.connect();

			var path_cloud = messages_path + device_token;

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.POST,
				data: Buffer.from(payload)
			}

			coap.send_message(path_cloud, msg, function(msg, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(msg);
				print_response(msg);
				assert.equal(msg.code, artik_coap.CODE.CREATED);
				coap.disconnect();
				done();
			});
		});

	});

	postEach(function() {
		coap.destroy_client();
	})

});