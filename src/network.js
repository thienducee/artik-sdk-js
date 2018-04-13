var events = require('events');
var util = require('util');
var network = require('../build/Release/artik-sdk.node').network;
var Network = function() {
    this.network = new network();
}

var NetworkWatcher = function(addr, interval, timeout) {
    events.EventEmitter.call(this);
    this.addr = addr;
    this.interval = interval;
    this.timeout = timeout;
}

util.inherits(NetworkWatcher, events.EventEmitter);
module.exports = Network;
module.exports.network_watcher = NetworkWatcher;

Network.prototype.add_watch_online_status = function(network_watcher) {
    this.network.add_watch_online_status(
        network_watcher,
        function(val) { network_watcher.emit('connectivity-change', val); });
    return network_watcher;
}

Network.prototype.remove_watch_online_status = function(watcher) {
    return this.network.remove_watch_online_status(watcher);
}

Network.prototype.set_network_config = function set_network_config(net_config, interface) {
    return this.network.set_network_config(net_config, interface);
}

Network.prototype.get_network_config = function get_network_config(interface) {
    return this.network.get_network_config(interface);
}

Network.prototype.get_current_public_ip = function get_current_public_ip() {
    return this.network.get_current_public_ip();
}

Network.prototype.dhcp_client_start = function dhcp_client_start(interface) {
    return this.network.dhcp_client_start(interface);
}

Network.prototype.dhcp_client_stop = function dhcp_client_stop() {
    return this.network.dhcp_client_stop();
}

Network.prototype.dhcp_server_start = function dhcp_server_start(dhcp_server_config) {
    return this.network.dhcp_server_start(dhcp_server_config);
}

Network.prototype.dhcp_server_stop = function dhcp_server_stop() {
    return this.network.dhcp_server_stop();
}

Network.prototype.get_online_status = function get_online_status(addr, timeout) {
    return this.network.get_online_status(addr, timeout);
}
