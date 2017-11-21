var events = require('events');
var util = require('util');
var coap = require('../build/Release/artik-sdk.node').coap;

var Coap = function(coap_config) {
	events.EventEmitter.call(this);
	this.coap = new coap(coap_config);
}

util.inherits(Coap, events.EventEmitter);

module.exports = Coap;

Coap.prototype.create_client = function() {
	return this.coap.create_client();
}

Coap.prototype.destroy_client = function() {
	return this.coap.destroy_client();
}

Coap.prototype.connect = function() {
	return this.coap.connect();
}

Coap.prototype.disconnect = function() {
	return this.coap.disconnect();
}

Coap.prototype.create_server = function() {
	return this.coap.create_server();
}

Coap.prototype.destroy_server = function() {
	return this.coap.destroy_server();
}

Coap.prototype.start_server = function(verify_psk_cb) {
	return this.coap.start_server(verify_psk_cb);
}

Coap.prototype.stop_server = function() {
	return this.coap.stop_server();
}

Coap.prototype.send_message = function(path, coap_msg, send_cb) {
	return this.coap.send_message(path, coap_msg, send_cb);
}

Coap.prototype.observe = function(path, msg_type, observe_cb, options, token) {
	return this.coap.observe(path, msg_type, observe_cb, options, token);
}

Coap.prototype.cancel_observe = function(path, token, send_cb) {
	return this.coap.cancel_observe(path, token, send_cb);
}

Coap.prototype.init_resources = function(resources) {
	return this.coap.init_resources(resources);
}

Coap.prototype.notify_resource_changed = function(path) {
	return this.coap.notify_resource_changed(path);
}

module.exports.CONTENT_FORMAT = {
	'PLAIN': 	0,
	'LINK_FORMAT': 	40,
	'XML': 		41,
	'OCTET_STREAM': 42,
	'EXI': 		47,
	'JSON': 	50,
	'CBOR': 	60
}

module.exports.OPTION_KEY = {
	'IF_MATCH': 		1,
	'URI_HOST': 		3,
	'ETAG': 		4,
	'IF_NONE_MATCH': 	5,
	'OBSERVE': 		6,
	'URI_PORT': 		7,
	'LOCATION_PATH': 	8,
	'URI_PATH': 		11,
	'CONTENT_FORMAT': 	12,
	'CONTENT_TYPE': 	12,
	'MAXAGE': 		14,
	'URI_QUERY': 		15,
	'ACCEPT': 		17,
	'LOCATION_QUERY': 	20,
	'BLOCK2':		23,
	'PROXY_URI': 		35,
	'PROXY_SCHEME': 	39,
	'SIZE1': 		60
};

module.exports.CODE = {
	'EMPTY': 			0,
	'GET': 				1,
	'POST': 			2,
	'PUT': 				3,
	'DELETE': 			4,
	'CREATED': 			65,
	'DELETED': 			66,
	'VALID': 			67,
	'CHANGED': 			68,
	'CONTENT': 			69,
	'BAD_REQUEST': 			128,
	'UNAUTHORIZED': 		129,
	'BAD_OPTION': 			130,
	'FORBIDDEN': 			131,
	'NOT_FOUND': 			132,
	'METHOD_NOT_ALLOWED': 		133,
	'NOT_ACCEPTABLE': 		134,
	'PRECONDITION_FAILED': 		140,
	'REQ_ENTITY_TOO_LARGE':  	141,
	'UNSUPPORTED_CONTENT_FORMAT':  	143,
	'INTERNAL_SERVER_ERROR': 	160,
	'NOT_IMPLEMENTED': 		161,
	'BAD_GATEWAY': 			162,
	'SERVICE_UNAVAILABLE': 		163,
	'GATEWAY_TIMEOUT': 		165,
	'PROXY_NOT_SUPPORTED': 		166
};

module.exports.RESOURCE_NOTIF_TYPE = {
	'NON': 0x0,
	'CON': 0x2
}