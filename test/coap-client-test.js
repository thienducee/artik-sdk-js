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

var token = process.env.COAP_TOKEN;
var payload = process.env.COAP_DATA;
var content_format = process.env.COAP_CONTENT_FORMAT;

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
			case artik_coap.OPTION_KEY.LOCATION_PATH:
				options_str += ("\"Location-Path\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.CONTENT_FORMAT:
				options_str += ("\"Content-Format\":" + msg.options[i].data);
				break;
			case artik_coap.OPTION_KEY.MAXAGE:
				options_str += ("\"Max-Age\":" + msg.options[i].data);
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
	uri: "coap://californium.eclipse.org/"
}

var coap = new artik.coap(config);

testCase("CoAP", function() {

	preEach(function() {
		coap.create_client();
	});

	testCase("#GET request", function() {

		assertions('Send a GET request', function(done) {
			this.timeout(10000);

			coap.connect();

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.GET,
				token: Buffer.from(token, 'hex')
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.isNotNull(resp.data);
				assert.equal(token, buf2hex(resp.token));
				assert.equal(resp.code, artik_coap.CODE.CONTENT);
				coap.disconnect();
				done();
			});
		});
	});

	testCase("#POST request", function() {

		assertions('Send a POST request', function(done) {
			this.timeout(10000);

			coap.connect();

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.POST,
				token: Buffer.from(token, 'hex'),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: content_format
					}
				],
				data: Buffer.from(payload)
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(token, buf2hex(resp.token));
				expect(resp.code).to.satisfy(function(code) {
					if ((code == artik_coap.CODE.CREATED) ||
						(code == artik_coap.CODE.CHANGED))
						return true;
					else
						return false;
				});
				coap.disconnect();
				done();
			});
		});
	});

	testCase("#PUT request", function() {

		assertions('Send a PUT request', function(done) {
			this.timeout(10000);

			coap.connect();

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.PUT,
				token: Buffer.from(token, 'hex'),
				options: [
					{
						key: artik_coap.OPTION_KEY.CONTENT_FORMAT,
						data: content_format
					}
				],
				data: Buffer.from(payload)
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(token, buf2hex(resp.token));
				expect(resp.code).to.satisfy(function(code) {
					if ((code == artik_coap.CODE.CREATED) ||
						(code == artik_coap.CODE.CHANGED))
						return true;
					else
						return false;
				});
				coap.disconnect();
				done();
			});
		});
	});

	testCase("#DELETE request", function() {

		assertions('Send a DELETE request', function(done) {
			this.timeout(10000);

			coap.connect();

			var msg = {
				msg_type: "CON",
				code: artik_coap.CODE.DELETE,
				token: Buffer.from(token, 'hex')
			}

			coap.send_message("test", msg, function(resp, error) {
				assert.equal(error, "NONE");
				assert.isNotNull(resp);
				print_response(resp);
				assert.equal(token, buf2hex(resp.token));
				assert.equal(resp.code, artik_coap.CODE.DELETED);
				coap.disconnect();
				done();
			});
		});
	});

	postEach(function() {
		coap.destroy_client();
	});
});
