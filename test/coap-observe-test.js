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

	testCase("#OBSERVE obs", function() {

		assertions('Send an OBSERVE request', function(done) {
			this.timeout(60000);

			var token = Buffer.from("1234", 'hex');

			var count = 5;

			coap.observe("obs", "CON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				if (count == 5)
					assert.equal(resp.msg_type, "ACK");
				else
					assert.equal(resp.msg_type, "CON");
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
				assert.isNotNull(resp.data);
				count--;

				if (count == 0)
					done();
			}, [], token);
		});
	});

	testCase("#OBSERVE obs (NON mode)", function() {

		assertions('Send an OBSERVE request in NON mode', function(done) {
			this.timeout(60000);

			var token = Buffer.from("5678", 'hex');

			var count = 5;

			coap.observe("obs", "NON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				/* Note : The server could still occasionally send a confirmable message,
	 			 * which then needs to be acknowledged by the client.
	 			 */
	 			expect(resp.msg_type).to.satisfy(function(msg_type) {
					if ((msg_type == "NON") ||
						(msg_type == "CON"))
						return true;
					else
						return false;
				});
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
				assert.isNotNull(resp.data);
				count--;

				if (count == 0)
					done();
			}, [], token);
		});
	});

	testCase("#OBSERVE obs (explicit RST)", function() {

		assertions('Send an OBSERVE request', function(done) {
			this.timeout(60000);

			var token = Buffer.from("9012", 'hex');

			var count = 5;

			coap.observe("obs", "CON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				expect(token).to.eql(Buffer.from(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				if (count == 5)
					assert.equal(resp.msg_type, "ACK");
				else
					assert.equal(resp.msg_type, "CON");
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
				assert.isNotNull(resp.data);
				count--;

				if (count == 0)
					done();
			}, [], token);
		});

		assertions('Send a RST request', function(done) {
			this.timeout(10000);

			var token = Buffer.from("9012", 'hex');

			var count = 5;

			var msg = {
				msg_type: "RST",
				code: artik_coap.CODE.EMPTY,
				token: token
			}

			coap.send_message("obs", msg);

			done();
		});
	});

	testCase("#OBSERVE obs with a DELETE request", function() {

		assertions('Send an OBSERVE request with a DELETE request from another client', function(done) {
			this.timeout(60000);

			var coap_del = new artik.coap(config);

			coap_del.create_client();
			coap_del.connect();

			var token = Buffer.from("3456", 'hex');

			var count = 5;

			coap.observe("obs", "CON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);

				if (count != 2) {
					assert.equal(resp.code, artik_coap.CODE.CONTENT);

					if (count == 5)
						assert.equal(resp.msg_type, "ACK");
					else
						assert.equal(resp.msg_type, "CON");

					expect(token).to.eql(Buffer.from(resp.token));

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

					assert.isNotNull(resp.data);
					count--;

					if (count == 2) {
						var msg = {
							code: artik_coap.CODE.DELETE,
							msg_type: "CON"
						}

						coap_del.send_message("obs", msg);

						coap_del.disconnect();
						coap_del.destroy_client();
					}
				} else {
					assert.equal(resp.code, artik_coap.CODE.NOT_FOUND);
					assert.equal(resp.msg_type, "CON");

					expect(token).to.eql(Buffer.from(resp.token));

					for (var i = 0; i < resp.options.length; i++) {
						if (resp.options[i].key == artik_coap.OPTION_KEY.OBSERVE)
							found_observe_option = true;
					}

					assert.equal(found_observe_option, false);

					count--;
				}

				if (count == 1)
					done();
			}, [], token);
		});
	});

	testCase("#OBSERVE obs with change of content-format", function() {

		assertions('Send an OBSERVE request with a PUT request from another client', function(done) {
			this.timeout(60000);

			var coap_del = new artik.coap(config);

			coap_del.create_client();
			coap_del.connect();

			var token = Buffer.from("7890", 'hex');

			var count = 5;

			coap.observe("obs", "CON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);

				if (count != 2) {
					assert.equal(resp.code, artik_coap.CODE.CONTENT);

					if (count == 5)
						assert.equal(resp.msg_type, "ACK");
					else
						assert.equal(resp.msg_type, "CON");

					expect(token).to.eql(Buffer.from(resp.token));

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

					assert.isNotNull(resp.data);
					count--;

					if (count == 2) {
						var msg = {
							code: artik_coap.CODE.PUT,
							msg_type: "CON",
							data: Buffer.from("{\"data\":\"Hello World\"}"),
							options: [
								{
									key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
									data: artik_coap.CONTENT_FORMAT.JSON
								}
							]
						}

						coap_del.send_message("obs", msg);

						coap_del.disconnect();
						coap_del.destroy_client();
					}
				} else {
					assert.equal(resp.code, artik_coap.CODE.NOT_ACCEPTABLE);
					assert.equal(resp.msg_type, "CON");

					expect(token).to.eql(Buffer.from(resp.token));

					for (var i = 0; i < resp.options.length; i++) {
						if (resp.options[i].key == artik_coap.OPTION_KEY.OBSERVE)
							found_observe_option = true;
					}

					assert.equal(found_observe_option, false);

					count--;
				}

				if (count == 1)
					done();
			}, [], token);
		});
	});

	testCase("#OBSERVE obs with change of payload", function() {

		assertions('Send an OBSERVE request with a PUT request from another client', function(done) {
			this.timeout(60000);

			var coap_del = new artik.coap(config);

			coap_del.create_client();
			coap_del.connect();

			var token = Buffer.from("1234", 'hex');

			var count = 5;

			coap.observe("obs", "CON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);

				if (count != 2) {
					assert.equal(resp.code, artik_coap.CODE.CONTENT);

					if (count == 5)
						assert.equal(resp.msg_type, "ACK");
					else
						assert.equal(resp.msg_type, "CON");

					expect(token).to.eql(Buffer.from(resp.token));

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

					assert.isNotNull(resp.data);
					count--;

					if (count == 2) {
						var msg = {
							code: artik_coap.CODE.PUT,
							msg_type: "CON",
							data: Buffer.from("World Hello"),
							options: [
								{
									key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
									data: artik_coap.CONTENT_FORMAT.PLAIN
								}
							]
						}

						coap_del.send_message("obs", msg);

						coap_del.disconnect();
						coap_del.destroy_client();
					}
				} else {
					assert.equal(resp.code, artik_coap.CODE.CONTENT);
					assert.equal(resp.msg_type, "CON");

					expect(token).to.eql(Buffer.from(resp.token));

					for (var i = 0; i < resp.options.length; i++) {
						if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT) {
							var content_format = 0;

							content_format = resp.options[i].data;

							assert.equal(content_format, 0);

							found_content_format_option = true;
						}
						if (resp.options[i].key == artik_coap.OPTION_KEY.OBSERVE) {
							var observe = 0;

							observe = resp.options[i].data;

							assert.notEqual(observe, 0);

							found_observe_option = true;
						}
					}

					assert.equal(found_content_format_option, true);
					assert.equal(found_observe_option, true);

					assert.equal(Buffer.from(resp.data), "World Hello");

					count--;
				}

				if (count == 1)
					done();
			}, [], token);
		});
	});

	testCase("#OBSERVE obs (no cancellation)", function() {

		assertions('Send an OBSERVE request', function(done) {
			this.timeout(60000);

			var token = Buffer.from("5678", 'hex');

			var count = 12;

			function response_cb(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				var new_token = Buffer.from("beef", 'hex');

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);

				if (count != 7) {
					assert.equal(resp.code, artik_coap.CODE.CONTENT);

					if (count == 12)
						assert.equal(resp.msg_type, "ACK");
					else
						assert.equal(resp.msg_type, "CON");

					expect(token).to.eql(Buffer.from(resp.token));

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

					count--;

					assert.isNotNull(resp.data);

					if (count == 7) {
						var msg = {
							code: artik_coap.CODE.GET,
							msg_type: "CON",
							token: new_token
						}

						coap.send_message("obs", msg, response_cb);
					}
				} else {
					assert.equal(resp.code, artik_coap.CODE.CONTENT);

					assert.equal(resp.msg_type, "ACK");

					expect(new_token).to.eql(Buffer.from(resp.token));

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
					assert.equal(found_observe_option, false);

					count --;
				}

				if (count == 1)
					done();
			}

			coap.observe("obs", "CON", response_cb, [], token);
		});
	});

	testCase("#OBSERVE obs (cancellation)", function() {

		assertions('Send an OBSERVE request', function(done) {
			this.timeout(60000);

			var token = Buffer.from("8901", 'hex');

			var count = 5;

			function response_cb(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);

				if (count != 2) {
					assert.equal(resp.code, artik_coap.CODE.CONTENT);

					if (count == 5)
						assert.equal(resp.msg_type, "ACK");
					else
						assert.equal(resp.msg_type, "CON");

					expect(token).to.eql(Buffer.from(resp.token));

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

					count--;

					assert.isNotNull(resp.data);

					if (count == 2)
						coap.cancel_observe("obs", token, response_cb);

				} else {
					assert.equal(resp.code, artik_coap.CODE.CONTENT);

					assert.equal(resp.msg_type, "ACK");

					expect(token).to.eql(Buffer.from(resp.token));

					for (var i = 0; i < resp.options.length; i++) {
						if (resp.options[i].key == artik_coap.OPTION_KEY.OBSERVE) {
							var observe = 0;

							observe = resp.options[i].data;

							assert.notEqual(observe, 0);

							found_observe_option = true;
						}
					}

					assert.equal(found_observe_option, false);

					count --;
				}

				if (count == 1)
					done();
			}

			coap.observe("obs", "CON", response_cb, [], token);
		});
	});

	testCase("#OBSERVE obs-large", function() {

		assertions('Send an OBSERVE request', function(done) {
			this.timeout(60000);

			var token = Buffer.from("2345", 'hex');

			var count = 5;
			var number = 0;
			var final_payload = "";

			coap.observe("obs-large", "CON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;
				var found_block2_option = false;

				var num = 0;
				var more = 0;
				var size = 0;
				var szx = 0;
				var value = 0;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);

				assert.equal(resp.code, artik_coap.CODE.CONTENT);

				expect(token).to.eql(Buffer.from(resp.token));

				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_content_format_option = true;
					if (resp.options[i].key == artik_coap.OPTION_KEY.OBSERVE) {
						var observe = 0;

						observe = resp.options[i].data;

						assert.notEqual(observe, 0);

						found_observe_option = true;
					}
					if (resp.options[i].key == artik_coap.OPTION_KEY.BLOCK2) {
						value = resp.options[i].data;

						size = value & 0x07;
						size += 4;

						num = Math.trunc(value/16);

						more = value & 0x08;
						more >>= 3;

						szx = 1 << size;

						assert.equal(num, number++);

						if (num == 2)
							assert.equal(more, 0);
						else
							assert.equal(more, 1);

						assert.equal(szx, 64);

						found_block2_option = true;
					}
				}

				assert.equal(found_block2_option, true);
				assert.equal(found_content_format_option, true);

				if (number == 0)
					assert.equal(found_observe_option, true);

				final_payload += Buffer.from(resp.data).toString();

				if (!more) {
					count--;
					number = 0;

					console.log("Final payload: \n" + final_payload);
					final_payload = "";
				}

				if (count == 0)
					done();
			}, [], token);
		});
	});

	testCase("#OBSERVE obs-pumping", function() {

		assertions('Send an OBSERVE request', function(done) {
			this.timeout(60000);

			var token = Buffer.from("6789", 'hex');

			var count = 5;
			var number = 0;
			var final_payload = "";

			coap.observe("obs-pumping", "CON", function(resp, error) {
				var found_content_format_option = false;
				var found_observe_option = false;
				var found_block2_option = false;

				var num = 0;
				var more = 0;
				var size = 0;
				var szx = 0;
				var value = 0;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);

				assert.equal(resp.code, artik_coap.CODE.CONTENT);

				expect(token).to.eql(Buffer.from(resp.token));

				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.CONTENT_FORMAT)
						found_content_format_option = true;
					if (resp.options[i].key == artik_coap.OPTION_KEY.OBSERVE) {
						var observe = 0;

						observe = resp.options[i].data;

						assert.notEqual(observe, 0);

						found_observe_option = true;
					}
					if (resp.options[i].key == artik_coap.OPTION_KEY.BLOCK2) {
						value = resp.options[i].data;

						size = value & 0x07;
						size += 4;

						num = Math.trunc(value/16);

						more = value & 0x08;
						more >>= 3;

						szx = 1 << size;

						assert.equal(num, number++);

						assert.equal(szx, 64);

						found_block2_option = true;
					}
				}

				assert.equal(found_content_format_option, true);

				if (number == 0)
					assert.equal(found_observe_option, true);

				if (found_block2_option)
					final_payload += Buffer.from(resp.data).toString();

				if (found_block2_option && !more) {
					count--;
					number = 0;

					console.log("Final payload: \n" + final_payload);
					final_payload = "";
				}

				if (count == 0)
					done();
			}, [], token);
		});
	});

	postEach(function() {
		coap.disconnect();
		coap.destroy_client();
	});
});