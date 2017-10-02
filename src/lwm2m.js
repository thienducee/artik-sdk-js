var events = require('events');
var util = require('util');
var lwm2m = require('../build/Release/artik-sdk.node').lwm2m;

var Lwm2m = function(){
    events.EventEmitter.call(this);
    this.lwm2m = new lwm2m();
    setImmediate(function(self) {
        self.emit('started');
    }, this);
}

util.inherits(Lwm2m, events.EventEmitter);

module.exports = Lwm2m;

Lwm2m.prototype.client_request = function(id, uri, name, lifetime, objects, psk_id, psk_key, certificate_mode_config) {
    var _ = this;
    return this.lwm2m.client_request(id, uri, name, lifetime,
            objects, psk_id, psk_key, certificate_mode_config,
            function(err) {
                _.emit('error', err);
            },
            function(uri) {
                _.emit('execute', uri);
            },
            function(uri) {
                _.emit('changed', uri);
            });
}

Lwm2m.prototype.client_release = function() {
    return this.lwm2m.client_release();
}

Lwm2m.prototype.client_connect = function() {
    return this.lwm2m.client_connect();
}

Lwm2m.prototype.client_disconnect = function() {
    return this.lwm2m.client_disconnect();
}

Lwm2m.prototype.client_write_resource = function(uri, buffer) {
    return this.lwm2m.client_write_resource(uri, buffer);
}

Lwm2m.prototype.client_read_resource = function(uri) {
    return this.lwm2m.client_read_resource(uri);
}

Lwm2m.prototype.serialize_tlv_int = function(array_data) {
    return this.lwm2m.serialize_tlv_int(array_data);
}

Lwm2m.prototype.serialize_tlv_string = function(array_data) {
    return this.lwm2m.serialize_tlv_string(array_data);
}
