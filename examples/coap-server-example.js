var artik = require('../src');
var opt = require('getopt');

const artik_coap = require('../src/coap');

var coap = new artik.coap();

coap.create_server();

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

coap.init_resources(resources);

coap.start_server();

process.on('SIGINT', function () {
	coap.stop_server();
	coap.destroy_server();

	process.exit(0);
});