var artik = require('../src');
var opt = require('getopt');

const artik_coap = require('../src/coap');

var token = "";
var data = "";
var method = artik_coap.CODE.EMPTY;
var content_format = 0;

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

function get_test_californium(token) {
	var config = {
		uri: "coap://californium.eclipse.org/"
	}

	var msg = {
		msg_type: "CON",
		code: artik_coap.CODE.GET,
		token: Buffer.from(token, 'hex')
	}

	var coap = new artik.coap(config);

	coap.create_client();

	coap.connect();

	coap.send_message("test", msg, function(resp, error) {
		if (error != "NONE" || resp == undefined) {
			console.log("Fail to receive message");
			process.exit(-1);
		}

		print_response(resp);
	});

	process.on('SIGINT', function () {
		coap.disconnect();
		coap.destroy_client();

		process.exit(0);
	});
}

function post_test_californium(token, data, content_format) {
	var config = {
		uri: "coap://californium.eclipse.org/"
	}

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
		data: Buffer.from(data)
	}

	var coap = new artik.coap(config);

	coap.create_client();

	coap.connect();

	coap.send_message("test", msg, function(resp, error) {
		if (error != "NONE" || resp == undefined) {
			console.log("Fail to receive message");
			process.exit(-1);
		}

		print_response(resp);
	});

	process.on('SIGINT', function () {
		coap.disconnect();
		coap.destroy_client();

		process.exit(0);
	});
}

function put_test_californium(token, data, content_format) {
	var config = {
		uri: "coap://californium.eclipse.org/"
	}

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
		data: Buffer.from(data)
	}

	var coap = new artik.coap(config);

	coap.create_client();

	coap.connect();

	coap.send_message("test", msg, function(resp, error) {
		if (error != "NONE" || resp == undefined) {
			console.log("Fail to receive message");
			process.exit(-1);
		}

		print_response(resp);
	});

	process.on('SIGINT', function () {
		coap.disconnect();
		coap.destroy_client();

		process.exit(0);
	});
}

function delete_test_californium(token) {
	var config = {
		uri: "coap://californium.eclipse.org/"
	}

	var msg = {
		msg_type: "CON",
		code: artik_coap.CODE.DELETE,
		token: Buffer.from(token, 'hex')
	}

	var coap = new artik.coap(config);

	coap.create_client();

	coap.connect();

	coap.send_message("test", msg, function(resp, error) {
		if (error != "NONE" || resp == undefined) {
			console.log("Fail to receive message");
			process.exit(-1);
		}

		print_response(resp);
	});

	process.on('SIGINT', function () {
		coap.disconnect();
		coap.destroy_client();

		process.exit(0);
	});
}

try {
	opt.setopt("t:m:d:c:");
} catch (e) {
	switch (e.type) {
	case "unknown":
		console.log("Unknown option: -%s", e.opt);
		console.log("Usage: node coap-client-example.js" +
			" [-m method (get/GET, post/POST, put/PUT, delete/DELETE)]" +
			" [-t <token>]" +
			" [-c <content format ID> (e.g: 50 for JSON)]" +
			" [-d <data>]");
		break;
	case "required":
		console.log("Required parameter for option: -%s", e.opt);
		console.log("Usage: node coap-client-example.js" +
			" [-m method (get/GET, post/POST, put/PUT, delete/DELETE)]" +
			" [-t <token>]" +
			" [-c <content format ID> (e.g: 50 for JSON)]" +
			" [-d <data>]");
		break;
	default:
		console.dir(e);
	}
	process.exit(0);
}

opt.getopt(function(o, p){
	switch(o) {
	case 't':
		token = String(p);
		break;
	case 'm':
		var _method = String(p);
		if (_method == "get" || _method == "GET")
			method = artik_coap.CODE.GET;
		else if (_method == "post" || _method == "POST")
			method = artik_coap.CODE.POST;
		else if (_method == "put" || _method == "PUT")
			method = artik_coap.CODE.PUT;
		else if (_method == "delete" || _method == "DELETE")
			method = artik_coap.CODE.DELETE;
		else {
			console.log("Unknown method (get/GET, post/POST, put/PUT, delete/DELETE)");
			process.exit(-1);
		}
		break;
	case 'd':
		data = String(p);
		break;
	case 'c':
		content_format = p;
		break;
	default:
		console.log("Usage: node coap-client-example.js" +
			" [-m method (get/GET, post/POST, put/PUT, delete/DELETE)]" +
			" [-t <token>]" +
			" [-c <content format ID> (e.g: 50 for JSON)]" +
			" [-d <data>]");
		process.exit(0);
	}
});

if (method == artik_coap.CODE.EMPTY) {
	console.log("Missing method (get/GET, post/POST, put/PUT, delete/DELETE)");
	process.exit(-1);
}

switch (method) {
case artik_coap.CODE.GET:
	get_test_californium(token);
	break;
case artik_coap.CODE.POST:
	post_test_californium(token, data, content_format);
	break;
case artik_coap.CODE.PUT:
	put_test_californium(token, data, content_format);
	break;
case artik_coap.CODE.DELETE:
	delete_test_californium(token);
	break;
default:
	break;
}