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

	testCase("#GET early negociation", function() {

		assertions('Send a GET request with early negociation', function(done) {
			this.timeout(20000);

			var num = 0;
			var more = 0;
			var size = 32;
			var block2 = 0;
			var szx = 0;
			var count = 0;

			var n = 0;
			var value = 0;
			var final_payload = "";

			while (size != 1) {
				size >>= 1;
				count++;
			}

			szx = count - 4;

			num <<= 4;
			more <<= 3;
			size = szx & 0x07;

			block2 = num | more | size;

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.GET,
				options: [
					{
						key: artik_coap.OPTION_KEY.BLOCK2,
						data: block2
					}
				]
			}

			coap.send_message("large", msg, function(resp, error) {
				var found_option = false;

				num = 0;
				more = 0;
				size = 0;
				szx = 0;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.BLOCK2) {
						value = resp.options[i].data;

						size = value & 0x07;
						size += 4;

						num = Math.trunc(value/16);

						more = value & 0x08;
						more >>= 3;

						szx = 1 << size;

						assert.equal(num, n++);

						if (num == 39)
							assert.equal(more, 0);
						else
							assert.equal(more, 1);

						assert.isTrue(szx <= 32);

						found_option = true;
					}
				}

				assert.equal(found_option, true);

				final_payload += Buffer.from(resp.data).toString();

				if (!more) {
					console.log("Final payload: \n" + final_payload);
					done();
				}
			});
		});

		assertions('Send a GET request with late negociation', function(done) {
			this.timeout(20000);

			var num = 0;
			var more = 0;
			var size = 0;
			var szx = 0;

			var n = 0;
			var value = 0;
			var final_payload = "";

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.GET
			}

			coap.send_message("large", msg, function(resp, error) {
				var found_option = false;

				num = 0;
				more = 0;
				size = 0;
				szx = 0;

				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				for (var i = 0; i < resp.options.length; i++) {
					if (resp.options[i].key == artik_coap.OPTION_KEY.BLOCK2) {
						value = resp.options[i].data;

						size = value & 0x07;
						size += 4;

						num = Math.trunc(value/16);

						more = value & 0x08;
						more >>= 3;

						szx = 1 << size;

						assert.equal(num, n++);

						if (num == 19)
							assert.equal(more, 0);
						else
							assert.equal(more, 1);

						assert.equal(szx, 64);

						found_option = true;
					}
				}

				assert.equal(found_option, true);

				final_payload += Buffer.from(resp.data).toString();

				if (!more) {
					console.log("Final payload: \n" + final_payload);
					done();
				}
			});
		});
	});

	postEach(function() {
		coap.disconnect();
		coap.destroy_client();
	});

});