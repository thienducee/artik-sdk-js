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
	"MIIBizCCATACCQC3RBCjNEJVjjAKBggqhkjOPQQDAjBNMQswCQYDVQQGEwJGUjEM\n" +
	"MAoGA1UECAwDSWRGMQ4wDAYDVQQHDAVQYXJpczEQMA4GA1UECgwHU2Ftc3VuZzEO\n" +
	"MAwGA1UECwwFQXJ0aWswHhcNMTcxMTEwMTgwNzE0WhcNMTgxMTEwMTgwNzE0WjBN\n" +
	"MQswCQYDVQQGEwJGUjEMMAoGA1UECAwDSWRGMQ4wDAYDVQQHDAVQYXJpczEQMA4G\n" +
	"A1UECgwHU2Ftc3VuZzEOMAwGA1UECwwFQXJ0aWswWTATBgcqhkjOPQIBBggqhkjO\n" +
	"PQMBBwNCAASRKOwF3JR+GyOdhrDHcQy+8FP33/hABG8gpuf/FSwG9QO3uE6nILFC\n" +
	"1m0v585Bwihk1k4L6x5NbGM8Mwj3J8eEMAoGCCqGSM49BAMCA0kAMEYCIQCJwCXP\n" +
	"VZqRKpANYaActLJWzel5x82rJCtDzlyaB9gDKwIhAJPtbI2d8V8VjqUjk7+jmgne\n" +
	"TRwNTtCcfpAJJKHoeIVw\n" +
	"-----END CERTIFICATE-----\n";

var client_key =
	"-----BEGIN EC PRIVATE KEY-----\n" +
	"MHcCAQEEILcWZVHXR1jPZin57hI7hKg83irXAAx2tnOJRPgckZDBoAoGCCqGSM49\n" +
	"AwEHoUQDQgAEkSjsBdyUfhsjnYawx3EMvvBT99/4QARvIKbn/xUsBvUDt7hOpyCx\n" +
	"QtZtL+fOQcIoZNZOC+seTWxjPDMI9yfHhA==\n" +
	"-----END EC PRIVATE KEY-----\n";

var server_cert =
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

var server_key =
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

var resourceBuf = "Hello World";

var config_server = {
	port: 8085
}

var config_psk_server = {
	port: 8090,
	psk: {
		identity: Buffer.from("password"),
		psk: Buffer.from("sesame")
	}
}

var config_ecdsa_server = {
	port: 8090,
	ssl: {
		client_cert: Buffer.from(server_cert),
		client_key: Buffer.from(server_key)
	}
}

var config_client = {
	uri: "coap://127.0.0.1:8085"
}

var config_psk_client = {
	uri: "coaps://127.0.0.1:8090",
	psk: {
		identity: Buffer.from("password"),
		psk: Buffer.from("sesame")
	}
}

var config_ecdsa_client = {
	uri: "coaps://127.0.0.1:8090",
	ssl: {
		client_cert: Buffer.from(client_cert),
		client_key: Buffer.from(client_key)
	}
}

var coap_server;
var coap_client;

var resources = [
	{
		path: "info",
		notif_type: artik_coap.RESOURCE_NOTIF_TYPE.CON,
		attributes: [
			{
				name: "ct",
				val: Buffer.from("0")
			},
			{
				name: "title",
				val: Buffer.from("\"General Info\"")
			}
		],
		get_resource: function(request) {
			var response = {
				code: artik_coap.CODE.CONTENT,
				data: Buffer.from("Welcome from Artik CoAP" +
					" Server Test"),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			};

			return response;
		}
	},
	{
		path: "test",
		notif_type: artik_coap.RESOURCE_NOTIF_TYPE.CON,
		observable: true,
		attributes: [
			{
				name: "ct",
				val: Buffer.from("0")
			},
			{
				name: "title",
				val: Buffer.from("\"Internal Buffer\"")
			},
			{
				name: "rt",
				val: Buffer.from("\"Data\"")
			},
			{
				name: "if",
				val: Buffer.from("\"buffer\"")
			}
		],
		get_resource: function(request) {
			var response = {
				code: artik_coap.CODE.CONTENT,
				data: Buffer.from(resourceBuf, 'utf8'),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			};

			return response;
		},
		post_resource: function(request) {
			var response = {};

			if (Buffer.from(request.data).length > 256) {
				response.code = artik_coap.CODE.UNAUTHORIZED;
				response.data = Buffer.from("The length must be inferior" +
						" to 256");
				return response;
			}

			if (resourceBuf.length > 0) {
				response.code = artik_coap.CODE.CHANGED;
				resourceBuf = Buffer.from(request.data).toString();
				coap_server.notify_resource_changed("test");
				return response;
			} else {
				response.code = artik_coap.CODE.CREATED;
				resourceBuf = Buffer.from(request.data).toString();
				coap_server.notify_resource_changed("test");
				return response;
			}
		},
		put_resource: function(request) {
			var response = {};

			if (Buffer.from(request.data).length > 256) {
				response.code = artik_coap.CODE.UNAUTHORIZED;
				response.data = Buffer.from("The length must be inferior" +
						" to 256");
				return response;
			}

			if (resourceBuf.length > 0) {
				response.code = artik_coap.CODE.CHANGED;
				resourceBuf = Buffer.from(request.data).toString();
				coap_server.notify_resource_changed("test");
				return response;
			} else {
				response.code = artik_coap.CODE.UNAUTHORIZED;
				response.data = Buffer.from("The resource is not created" +
						" (do POST before)");
				return response;
			}
		},
		delete_resource: function(request) {
			var response = {
				code: artik_coap.CODE.DELETED,
			}

			resourceBuf = "";
			coap_server.notify_resource_changed("test");
			return response;
		}
	}
]

testCase("CoAP", function() {

	testCase('#Init and start server', function() {

		assertions("Create a CoAP server", function(done) {
			coap_server = new artik.coap(config_server);
			coap_server.create_server();
			done();
		});

		assertions("Initialize ressources", function(done) {
			coap_server.init_resources(resources);
			done();
		});

		assertions("Start server", function(done) {
			coap_server.start_server();
			done();
		});

	});

	testCase('#Establish connection', function() {

		assertions("Create a CoAP client", function(done) {
			coap_client = new artik.coap(config_client);
			coap_client.create_client();
			done();
		});

		assertions("Connect", function(done) {
			coap_client.connect();
			done();
		});
	});

	testCase('#GET info', function() {

		assertions("Send a GET request for path \"info\"", function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap_client.send_message("info", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(Buffer.from(resp.data), "Welcome from Artik CoAP Server Test");
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_option = true;
				}
				assert.equal(found_option, true);
				done();
			});
		});
	});

	testCase('#GET test', function() {

		assertions("Send a GET request for path \"test\"", function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 5678,
				code: artik_coap.CODE.GET,
				token: Buffer.from("5678", 'hex')
			}

			coap_client.send_message("test", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(Buffer.from(resp.data), "Hello World");
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_option = true;
				}
				assert.equal(found_option, true);
				done();
			});
		});
	});

	testCase('#POST test', function() {

		assertions("Send a POST request for path \"test\"", function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 9012,
				code: artik_coap.CODE.POST,
				token: Buffer.from("9012", 'hex'),
				data: Buffer.from("Artik"),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap_client.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CHANGED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase('#PUT test', function() {

		assertions("Send a PUT request for path \"test\"", function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 3456,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("3456", 'hex'),
				data: Buffer.from("Samsung"),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap_client.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CHANGED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase('#DELETE test', function() {

		assertions("Send a DELETE request for path \"test\"", function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 7890,
				code: artik_coap.CODE.DELETE,
				token: Buffer.from("7890", 'hex')
			}

			coap_client.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.DELETED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase('#PUT unauthorized test', function() {

		assertions("Send a PUT request for path \"test\"", function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("1234", 'hex'),
				data: Buffer.from("Samsung"),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap_client.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.UNAUTHORIZED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase('#Subscribe test', function() {

		assertions("Send an OBSERVE request for path \"test\"", function(done) {
			this.timeout(10000);

			var token = Buffer.from("5678", 'hex')

			coap_client.observe("test", "CON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(resp.msg_type, "ACK");
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_content_format_option = true;
					if (resp.options[i].key == artik_coap.OPTION_KEY.OBSERVE) {
						var observe = 0;

						observe = resp.options[i].data;

						assert.notEqual(observe, 0);

						found_observe_option = true;
					}
				}
				assert.equal(found_content_format_option, true);
				assert.equal(found_observe_option, true);

				done();
			}, [], token);
		});
	});

	testCase('#Unsubscribe test', function() {

		assertions("Send a CANCEL request for path \"test\"", function(done) {
			this.timeout(10000);

			var token = Buffer.from("9012", 'hex')

			coap_client.cancel_observe("test", token, function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(resp.msg_type, "ACK");
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_content_format_option = true;
					if (resp.options[i].key == artik_coap.OPTION_KEY.OBSERVE)
						found_observe_option = true;
				}
				assert.equal(found_content_format_option, true);
				assert.equal(found_observe_option, false);
				done();
			});
		});
	});

	testCase('#Disconnect and destroy client', function() {

		assertions("Disconnect", function(done) {
			coap_client.disconnect();
			done();
		});

		assertions("Destroy client", function(done) {
			coap_client.destroy_client();
			done();
		});
	});

	testCase('#Stop and destroy server', function() {

		assertions("Stop server", function(done) {
			coap_server.stop_server();
			done();
		});

		assertions("Destroy client", function(done) {
			coap_server.destroy_server();
			done();
		});
	});

	testCase('#Init and start server with PSK config', function() {

		assertions("Create a CoAP server with PSK config", function(done) {
			coap_server = new artik.coap(config_psk_server);
			coap_server.create_server();
			done();
		});

		assertions("Initialize ressources", function(done) {
			coap_server.init_resources(resources);
			done();
		});

		assertions("Start server", function(done) {
			coap_server.start_server();
			done();
		});

	});

	testCase('#Establish connection with PSK config', function() {

		assertions("Create a CoAP client", function(done) {
			coap_client = new artik.coap(config_psk_client);
			coap_client.create_client();
			done();
		});

		assertions("Connect", function(done) {
			coap_client.connect();
			done();
		});
	});

	testCase('#GET info (PSK)', function() {

		assertions("Send a GET request for path \"info\"", function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap_client.send_message("info", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(Buffer.from(resp.data), "Welcome from Artik CoAP Server Test");
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_option = true;
				}
				assert.equal(found_option, true);
				done();
			});
		});
	});

	testCase('#Disconnect and destroy client', function() {

		assertions("Disconnect", function(done) {
			coap_client.disconnect();
			done();
		});

		assertions("Destroy client", function(done) {
			coap_client.destroy_client();
			done();
		});
	});

	testCase('#Stop and destroy server', function() {

		assertions("Stop server", function(done) {
			coap_server.stop_server();
			done();
		});

		assertions("Destroy client", function(done) {
			coap_server.destroy_server();
			done();
		});
	});

	testCase('#Init and start server with ECDSA config', function() {

		assertions("Create a CoAP server with ECDSA config", function(done) {
			coap_server = new artik.coap(config_ecdsa_server);
			coap_server.create_server();
			done();
		});

		assertions("Initialize ressources", function(done) {
			coap_server.init_resources(resources);
			done();
		});

		assertions("Start server", function(done) {
			coap_server.start_server();
			done();
		});

	});

	testCase('#Establish connection with ECDSA config', function() {

		assertions("Create a CoAP client", function(done) {
			coap_client = new artik.coap(config_ecdsa_client);
			coap_client.create_client();
			done();
		});

		assertions("Connect", function(done) {
			coap_client.connect();
			done();
		});
	});

	testCase('#GET info (ECDSA)', function() {

		assertions("Send a GET request for path \"info\"", function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap_client.send_message("info", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(Buffer.from(resp.data), "Welcome from Artik CoAP Server Test");
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_option = true;
				}
				assert.equal(found_option, true);
				done();
			});
		});
	});

	testCase('#Disconnect and destroy client', function() {

		assertions("Disconnect", function(done) {
			coap_client.disconnect();
			done();
		});

		assertions("Destroy client", function(done) {
			coap_client.destroy_client();
			done();
		});
	});

	testCase('#Stop and destroy server', function() {

		assertions("Stop server", function(done) {
			coap_server.stop_server();
			done();
		});

		assertions("Destroy client", function(done) {
			coap_server.destroy_server();
			done();
		});
	});
});