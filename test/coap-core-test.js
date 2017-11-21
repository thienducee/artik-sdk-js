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

var config = {
	uri: "coap://californium.eclipse.org/"
}

var coap = new artik.coap(config);

testCase("CoAP", function() {
	preEach(function() {
		coap.create_client();
		coap.connect();
	});

	testCase("#GET test", function() {

		assertions('Send a GET request', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("test", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
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

	testCase("#DELETE test", function() {

		assertions('Send a DELETE request', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.DELETE,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("test", msg, function(resp, error) {

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

	testCase("#PUT test", function() {

		assertions('Send a PUT request', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("1234", 'hex'),
				data: Buffer.from("Hello World"),
				options:[
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				expect(resp.code).to.satisfy(function(code) {
					if ((code == artik_coap.CODE.CREATED) ||
						(code == artik_coap.CODE.CHANGED))
						return true;
					else
						return false;
				});
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase("#POST test", function() {

		assertions('Send a POST request', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.POST,
				token: Buffer.from("1234", 'hex'),
				data: Buffer.from("Hello World"),
				options:[
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				expect(resp.code).to.satisfy(function(code) {
					if ((code == artik_coap.CODE.CREATED) ||
						(code == artik_coap.CODE.CHANGED))
						return true;
					else
						return false;
				});
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase("#GET test (NON mode)", function() {

		assertions('Send a GET request in NON mode', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "NON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("test", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.msg_type, "NON");
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.notEqual(resp.msg_id, 0);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_option = true;
				}
				assert.equal(found_option, true);
				done();
			});
		});
	});

	testCase("#DELETE test (NON mode)", function() {

		assertions('Send a DELETE request in NON mode', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "NON",
				msg_id: 1234,
				code: artik_coap.CODE.DELETE,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("test", msg, function(resp, error) {

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(resp.msg_type, "NON");
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.DELETED);
				assert.notEqual(resp.msg_id, 0);
				done();
			});
		});
	});

	testCase("#PUT test (NON mode)", function() {

		assertions('Send a PUT request in NON mode', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "NON",
				msg_id: 1234,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("1234", 'hex'),
				data: Buffer.from("Hello World"),
				options:[
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(resp.msg_type, "NON");
				expect(msg.token).to.eql(Buffer.from(resp.token));
				expect(resp.code).to.satisfy(function(code) {
					if ((code == artik_coap.CODE.CREATED) ||
						(code == artik_coap.CODE.CHANGED))
						return true;
					else
						return false;
				});
				assert.notEqual(resp.msg_id, 0);
				done();
			});
		});
	});

	testCase("#POST test (NON mode)", function() {

		assertions('Send a POST request in NON mode', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "NON",
				msg_id: 1234,
				code: artik_coap.CODE.POST,
				token: Buffer.from("1234", 'hex'),
				data: Buffer.from("Hello World"),
				options:[
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(resp.msg_type, "NON");
				expect(msg.token).to.eql(Buffer.from(resp.token));
				expect(resp.code).to.satisfy(function(code) {
					if ((code == artik_coap.CODE.CREATED) ||
						(code == artik_coap.CODE.CHANGED))
						return true;
					else
						return false;
				});
				assert.notEqual(resp.msg_id, 0);
				done();
			});
		});
	});

	testCase("#GET separate", function() {

		assertions('Send a GET request', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("separate", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_option = true;
				}
				assert.equal(found_option, true);
				assert.notEqual(resp.msg_id, 0);
				done();
			});
		});
	});

	testCase("#GET test (empty token)", function() {

		assertions('Send a GET request with no token', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET
			}

			coap.send_message("test", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				assert.equal(resp.token.length, undefined);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_option = true;
				}
				assert.equal(found_option, true);
				done();
			});
		});
	});

	testCase("#GET Uri-Path options", function() {

		assertions('Send a GET request with Uri-Path options', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("seg1/seg2/seg3", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
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

	testCase("#GET query", function() {

		assertions('Send a GET request with Uri-Query options', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("query?first=1&second=2", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
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

	testCase("#GET separate (NON mode)", function() {

		assertions('Send a GET request in NON mode', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "NON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("separate", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_option = true;
				}
				assert.equal(found_option, true);
				assert.notEqual(resp.msg_id, 0);
				done();
			});
		});
	});

	testCase("#POST test Location-Path options", function() {

		assertions('Send a POST request with Location-Path options', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.POST,
				token: Buffer.from("1234", 'hex'),
				data: Buffer.from("Hello World"),
				options:[
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("test", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				expect(resp.code).to.satisfy(function(code) {
					if ((code == artik_coap.CODE.CREATED) ||
						(code == artik_coap.CODE.CHANGED))
						return true;
					else
						return false;
				});
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.LOCATION_PATH &&
							i + 2 < resp.options.length) {
						assert.equal(resp.options[i].data, "location1");
						assert.equal(resp.options[i+1].data, "location2");
						assert.equal(resp.options[i+2].data, "location3");
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase("#POST Location-Query options", function() {

		assertions('Send a POST request with Location-Query options', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.POST,
				token: Buffer.from("1234", 'hex'),
				data: Buffer.from("Hello World"),
				options:[
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("location-query?first=1&second=2", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				expect(resp.code).to.satisfy(function(code) {
					if (code == artik_coap.CODE.CREATED)
						return true;
					else
						return false;
				});
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.LOCATION_QUERY &&
							i + 1 < resp.options.length) {
						assert.equal(resp.options[i].data, "first=1");
						assert.equal(resp.options[i+1].data, "second=2");
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase("#GET Accept option", function() {

		assertions('Send a GET request with Accept option (text/plain)', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex'),
				options:[
					{
						key: artik_coap.OPTION_KEY.ACCEPT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("multi-format", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT) {
						assert.equal(resp.options[i].data, artik_coap.CONTENT_FORMAT.PLAIN);
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				done();
			});
		});

		assertions('Send a GET request with Accept option (xml)', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 5678,
				code: artik_coap.CODE.GET,
				token: Buffer.from("5678", 'hex'),
				options:[
					{
						key: artik_coap.OPTION_KEY.ACCEPT,
						data: artik_coap.CONTENT_FORMAT.XML
					}
				]
			}

			coap.send_message("multi-format", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT) {
						assert.equal(resp.options[i].data, artik_coap.CONTENT_FORMAT.XML);
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				done();
			});
		});
	});

	testCase("#GET Etag option", function() {

		var etag_value;

		assertions('Send a GET request for retrieving an Etag', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("validate", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.ETAG) {
						assert.isNotNull(resp.options[i].data);
						etag_value = Buffer.from(resp.options[i].data);
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				done();
			});
		});

		assertions('Send a GET request for verifying if the Etag value is still valid', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 5678,
				code: artik_coap.CODE.GET,
				token: Buffer.from("5678", 'hex'),
				options:[
					{
						key: artik_coap.OPTION_KEY.ETAG,
						data: etag_value
					}
				]
			}

			coap.send_message("validate", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.VALID);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.ETAG) {
						expect(Buffer.from(resp.options[i].data)).to.eql(etag_value);
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				done();
			});
		});

		assertions('Send a GET request for verifying if the Etag value is no longer valid', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 9012,
				code: artik_coap.CODE.GET,
				token: Buffer.from("9012", 'hex'),
				options:[
					{
						key: artik_coap.OPTION_KEY.ETAG,
						data: etag_value
					}
				]
			}

			coap.send_message("validate", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.ETAG) {
						expect(Buffer.from(resp.options[i].data)).to.not.eql(etag_value);
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				done();
			});
		})

	});

	testCase("#GET Etag and If-Match options", function() {

		var etag_value;

		assertions('Send a GET request for single update', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.GET,
				token: Buffer.from("1234", 'hex')
			}

			coap.send_message("validate", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.ETAG) {
						assert.isNotNull(resp.options[i].data);
						etag_value = Buffer.from(resp.options[i].data);
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				done();
			});
		});

		assertions('Send a PUT request for single update', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 5678,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("5678", 'hex'),
				data: Buffer.from("Hello World"),
				options:[
					{
						key: artik_coap.OPTION_KEY.IF_MATCH,
						data: etag_value
					},
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("validate", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CHANGED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});

		assertions('Send a GET request for concurrent update 1', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 9012,
				code: artik_coap.CODE.GET,
				token: Buffer.from("9012", 'hex')
			}

			coap.send_message("validate", msg, function(resp, error) {
				var found_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(Buffer.from(resp.data), "Hello World");
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				assert.equal(msg.msg_id, resp.msg_id);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.ETAG) {
						expect(Buffer.from(resp.options[i].data)).to.not.eql(etag_value);
						etag_value = Buffer.from(resp.options[i].data);
						found_option = true;
					}
				}
				assert.equal(found_option, true);
				done();
			});
		});

		assertions('Send a PUT request for concurrent update 1', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 3456,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("3456", 'hex'),
				data: Buffer.from("dlroW olleH"),
				options:[
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("validate", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CHANGED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});

		assertions('Send a PUT request for concurrent update 2', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 7890,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("7890", 'hex'),
				data: Buffer.from("World Hello"),
				options:[
					{
						key: artik_coap.OPTION_KEY.IF_MATCH,
						data: etag_value
					},
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					}
				]
			}

			coap.send_message("validate", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.PRECONDITION_FAILED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase("#PUT If-None-Match option", function() {

		var etag_value;

		assertions('Send a PUT request for single creation', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("1234", 'hex'),
				data: Buffer.from("Hello World"),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					},
					{
						key: artik_coap.OPTION_KEY.IF_NONE_MATCH
					}
				]
			}

			coap.send_message("create1", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CREATED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});

		assertions('Send a PUT request for concurrent creation', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 5678,
				code: artik_coap.CODE.PUT,
				token: Buffer.from("5678", 'hex'),
				data: Buffer.from("Hello World"),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: artik_coap.CONTENT_FORMAT.PLAIN
					},
					{
						key: artik_coap.OPTION_KEY.IF_NONE_MATCH
					}
				]
			}

			coap.send_message("create1", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(msg.token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.PRECONDITION_FAILED);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	testCase("Ping", function() {

		assertions('Send a PING request', function(done) {
			this.timeout(10000);

			var msg = {
				msg_type: "CON",
				msg_id: 1234,
				code: artik_coap.CODE.EMPTY
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "RST");
				assert.isNotNull(resp);
				assert.equal(resp.msg_type, "RST");
				assert.equal(resp.code, artik_coap.CODE.EMPTY);
				assert.equal(msg.msg_id, resp.msg_id);
				done();
			});
		});
	});

	postEach(function() {
		coap.disconnect();
		coap.destroy_client();
	});
});