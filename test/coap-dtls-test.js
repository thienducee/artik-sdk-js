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

const artik_coap = require('../src/coap');

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

var MAX_SIZE = 63;

function add_spaces(x) {
	console.log("");
	process.stdout.write(" ".repeat(x));
}

function pad(d) {
	return (d < 10) ? '0' + d.toString() : d.toString();
}

function buf2hex(buffer) {
	return Array.prototype.map.call(new Uint8Array(buffer), x => ('00' + x.toString(16)).slice(-2)).join('');
}

function count_digits(number) {
	return number.toString().length;
}

function print_response(msg) {
	console.log("==[ CoAP Response ]============================================");
	console.log("MID    : " + msg.msg_id);
	console.log("Token  : " + buf2hex(msg.token));
	console.log("Type   : " + msg.msg_type);
	console.log("Status : " + (msg.code >> 5) + "." + (pad(msg.code & 0x1f)));

	if (msg.options && msg.options.length > 0) {
		var c = 10;
		process.stdout.write("Options: ");
		process.stdout.write("{")
		var options_str = "";
		for (var i = 0; i < msg.options.length; i++) {
			switch(msg.options[i].key) {
			case artik_coap.OPTION_KEY.IF_MATCH:
				if (c + 11 + msg.options[i].data.length < MAX_SIZE)
					c += (11 + msg.options[i].data.length);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"If-Match\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.URI_HOST:
				if (c + 11 + msg.options[i].data.byteLength < MAX_SIZE)
					c += (11 + msg.options[i].data.byteLength);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Uri-Host\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.ETAG:
				if (c + 7 + msg.options[i].data.byteLength < MAX_SIZE)
					c += (7 + msg.options[i].data.byteLength);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Etag\":" + buf2hex(msg.options[i].data));
				break;
			case artik_coap.OPTION_KEY.IF_NONE_MATCH:
				if (c + 15 < MAX_SIZE)
					c += 15;
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"If-None-Match\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.OBSERVE:
				if (c + 10 + count_digits(msg.options[i].data) < MAX_SIZE)
					c += (10 + count_digits(msg.options[i].data));
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Observe\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.URI_PORT:
				if (c + 11 + count_digits(msg.options[i].data) < MAX_SIZE)
					c += (11 + count_digits(msg.options[i].data));
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Uri-Port\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.LOCATION_PATH:
				if (c + 16 + msg.options[i].data.length < MAX_SIZE)
					c += (16 + msg.options[i].data.length);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Location-Path\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.URI_PATH:
				if (c + 11 + msg.options[i].data.length < MAX_SIZE)
					c += (11 + msg.options[i].data.length);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Uri-Path\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.CONTENT_FORMAT:
				if (c + 17 + count_digits(msg.options[i].data) < MAX_SIZE)
					c += (17 + count_digits(msg.options[i].data));
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Content-Format\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.MAXAGE:
				if (c + 10 + count_digits(msg.options[i].data) < MAX_SIZE)
					c += (10 + count_digits(msg.options[i].data));
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Max-Age\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.URI_QUERY:
				if (c + 12 + msg.options[i].data.length < MAX_SIZE)
					c += (12 + msg.options[i].data.length);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Uri-Query\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.ACCEPT:
				if (c + 9 + count_digits(msg.options[i].data) < MAX_SIZE)
					c += (9 + count_digits(msg.options[i].data));
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Accept\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.LOCATION_QUERY:
				if (c + 17 + msg.options[i].data.length < MAX_SIZE)
					c += (17 + msg.options[i].data.length);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Location-Query\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.BLOCK2:
				var block2, szx, num, more, size;

				block2 = msg.options[i].data;

				szx = block2 & 0x07;
				szx += 4;

				num = Math.trunc(block2/16)

				more = block2 & 0x08;
				more >>= 3;

				size = 1 << szx;

				if (c + 9 + count_digits(num) + 1
						+ count_digits(more) + 1
						+ count_digits(size) < MAX_SIZE)
					c += 9 + count_digits(num) + 1
						+ count_digits(more) + 1
						+ count_digits(size);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Block2\":" + num + "/" + more + "/" + size);
				break;
			case artik_coap.OPTION_KEY.PROXY_URI:
				if (c + 12 + msg.options[i].data.length < MAX_SIZE)
					c += (12 + msg.options[i].data.length);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Proxy-Uri\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.PROXY_SCHEME:
				if (c + 15 + msg.options[i].data.length < MAX_SIZE)
					c += (15 + msg.options[i].data.length);
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Proxy-Scheme\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.SIZE1:
				if (c + 8 + count_digits(msg.options[i].data) < MAX_SIZE)
					c += (8 + count_digits(msg.options[i].data));
				else {
					c = 9;
					add_spaces(c);
				}
				process.stdout.write("\"Size1\":" + msg.options[i].data);
				break;
			default:
				break;
			}
			if (i + 1 != msg.options.length)
				process.stdout.write(", ");
		}
		console.log("}");
	}

	console.log("Payload: " + Buffer.from(msg.data).length + " Bytes");

	if (Buffer.from(msg.data) && Buffer.from(msg.data).length > 0) {
		console.log("---------------------------------------------------------------");
		console.log("" + Buffer.from(msg.data));
	}

	console.log("===============================================================");
}

var config_psk = {
	uri: "coaps://californium.eclipse.org/",
	psk: {
		identity: Buffer.from("password"),
		psk: Buffer.from("sesame")
	}
}

var config_wrong_psk = {
	uri: "coaps://californium.eclipse.org/",
	psk: {
		identity: Buffer.from("password"),
		psk: Buffer.from("wrong")
	}
}

var config_ssl = {
	uri: "coaps://californium.eclipse.org/",
	ssl: {
		client_cert: Buffer.from(client_cert),
		client_key: Buffer.from(client_key)
	}
}

var coap_psk = new artik.coap(config_psk);
var coap_wrong_psk = new artik.coap(config_wrong_psk);
var coap_ssl = new artik.coap(config_ssl);

testCase("CoAP", function() {

	testCase("#GET secure - PSK", function() {

		preEach(function() {
			coap_psk.create_client();
			coap_psk.connect();
		});

		assertions('Send a GET request in PSK config', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}
			coap_psk.send_message("secure", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.NOT_FOUND);
				done();
			});
		});

		postEach(function() {
			coap_psk.disconnect();
			coap_psk.destroy_client();
		});
	});

	testCase("#GET test - PSK", function() {

		preEach(function() {
			coap_psk.create_client();
			coap_psk.connect();
		});

		assertions('Send a GET request in PSK config', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap_psk.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				done();
			});
		});

		postEach(function() {
			coap_psk.disconnect();
			coap_psk.destroy_client();
		});
	});

	testCase("#GET test - Wrong PSK", function() {

		preEach(function() {
			coap_wrong_psk.create_client();
			coap_wrong_psk.connect();
		});

		assertions('Send a GET request in wrong PSK config', function(done) {
			this.timeout(30000);

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap_wrong_psk.send_message("test", msg, function(resp, error) {
				assert.equal(error, "TLS FAILED");
				console.log("TLS failed");
				done();
			});
		});

		postEach(function() {
			coap_wrong_psk.destroy_client();
		});
	});

	testCase("#GET secure - RPK", function() {

		preEach(function() {
			coap_ssl.create_client();
			coap_ssl.connect();
		});

		assertions('Send a GET request in RPK config', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.GET,
				token: Buffer.from("5678", 'hex')
			}
			coap_ssl.send_message("secure", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.NOT_FOUND);
				done();
			});
		});

		postEach(function() {
			coap_ssl.disconnect();
			coap_ssl.destroy_client();
		});
	});

	testCase("#GET test - RPK", function() {

		preEach(function() {
			coap_ssl.create_client();
			coap_ssl.connect();
		});

		assertions('Send a GET request in RPK config', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.GET,
				token: Buffer.from("5678", 'hex')
			}

			coap_ssl.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				done();
			});
		});

		postEach(function() {
			coap_ssl.disconnect();
			coap_ssl.destroy_client();
		});
	});
});