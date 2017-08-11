var events = require('events');
var util = require('util');
var http = require('../build/Release/artik-sdk.node').http;

var Readable = require('stream').Readable;

var Http = function() {
	events.EventEmitter.call(this);
	this.http = new http();
}

util.inherits(Http, events.EventEmitter);

module.exports = Http;

Http.prototype.get_stream = function(url, headers, ssl_config) {

	var inStream = new Readable();

	this.http.get_stream(url, headers, ssl_config,
		function(data){
			inStream.push(data);
		},
		function(err){
			inStream.push(null);

			if (err == "OK")
				inStream.emit('end');
			else
				inStream.emit('error', new Error("Stream - " + err));
		});

	return inStream;
}

Http.prototype.get = function(url, headers, ssl_config, func) {
	return this.http.get(url, headers, ssl_config, func);
}

Http.prototype.post = function(url, headers, body, ssl_config, func) {
	return this.http.post(url, headers, body, ssl_config, func);
};

Http.prototype.put = function(url, headers, body, ssl_config, func) {
	return this.http.put(url, headers, body, ssl_config, func);
}

Http.prototype.del = function(url, headers, ssl_config, func) {
	return this.http.del(url, headers, ssl_config, func);
}