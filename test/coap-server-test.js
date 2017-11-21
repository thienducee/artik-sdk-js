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

var resourceBuf = "Hello World";

var resources = [
	{
		path: "info",
		notif_type: artik_coap.RESOURCE_NOTIF_TYPE.NON,
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
		notif_type: artik_coap.RESOURCE_NOTIF_TYPE.NON,
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
				coap.notify_resource_changed("test");
				return response;
			} else {
				response.code = artik_coap.CODE.CREATED;
				resourceBuf = Buffer.from(request.data).toString();
				coap.notify_resource_changed("test");
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
				coap.notify_resource_changed("test");
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

			coap.notify_resource_changed("test");

			return response;
		}
	}
]

var coap = new artik.coap();

testCase("CoAP", function() {

	testCase('#create_server()', function() {
		assertions("Create a CoAP server", function(done) {
			coap.create_server();
			done();
		});
	});

	testCase('#init_resources()', function() {
		assertions("Initialize resources", function(done) {
			coap.init_resources(resources);
			done();
		});
	});

	testCase('#start_server(), #stop_server()', function() {
		assertions("Start and stop the server", function(done) {

			this.timeout(31000);

			setTimeout(function() {
				coap.stop_server();
				done();
			}, 30000);

			coap.start_server();
		});
	});

	testCase('#destroy_server()', function() {
		assertions("Destroy the server", function(done) {
			coap.destroy_server();
			done();
		});
	});
});