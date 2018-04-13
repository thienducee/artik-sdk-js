const artik = require('../src')
var opt = require('getopt');

const net = artik.network;
const network = new net();

var net_config = {
	ip_addr: "",
	netmask: "",
	gw_addr: "",
	dns_addr: []
}

var interface = "wifi";

var enable_set_config = false;

try{
	opt.setopt("i:n:g:a:b:s:l:e");
} catch (e){
   switch (e.type) {
	case "unknown":
		console.log("Unknown option: -%s", e.opt);
		console.log("Usage: node network-example.js [-i IP address of server] " +
			"[-n netmask] [-g gateway address] " +
			"[-a DNS address 1] [-b DNS address 2] " +
			"[-e for ethernet] (wifi by default)");
		break;
	case "required":
		console.log("Required parameter for option: -%s", e.opt);
		break;
	default:
		console.dir(e);
    }
	process.exit(0);
}

opt.getopt(function (o, p){
	switch(o){
	case 'i':
		net_config.ip_addr = String(p);
		enable_set_config = true;
		break;
	case 'n':
		net_config.netmask = String(p);
		enable_set_config = true;
		break;
	case 'g':
		net_config.gw_addr = String(p);
		enable_set_config = true;
		break;
	case 'a':
		net_config.dns_addr.push(String(p));
		enable_set_config = true;
		break;
	case 'b':
		net_config.dns_addr.push(String(p));
		enable_set_config = true;
		break;
	case 'e':
		interface = "ethernet";
		break;
	default:
		console.log("Usage: node network-example.js [-i IP address of server] " +
			"[-n netmask] [-g gateway address] " +
			"[-a DNS address 1] [-b DNS address 2] " +
			"[-e for ethernet] (wifi by default)");
		process.exit(0);
    }
});

var config = network.get_network_config(interface);

if (config) {
	console.log("IP Address: " + config.ip_addr);
	console.log("Netmask: " + config.netmask);
	console.log("Gateway: " + config.gw_addr);
	console.log("DNS Address 1: " + config.dns_addr[0]);
	console.log("DNS Address 2: " + config.dns_addr[1]);
	console.log("MAC Address: " + config.mac_addr);
}

console.log("Your IP is " + network.get_current_public_ip());

var online_status = network.get_online_status("artik.cloud", 1000);
console.log("Status : " + online_status);

var watcher = new net.network_watcher("artik.cloud", 5000, 500);
watcher.on("connectivity-change", function(status) {
    console.log("Status change : " + status);
});

network.add_watch_online_status(watcher);
process.on('SIGINT', function () {
    network.remove_watch_online_status(watcher);
    process.exit(0);
});

if (enable_set_config) {

	network.set_network_config(net_config, interface);

	config = network.get_network_config(interface);

	if (config) {
		console.log("IP Address: " + config.ip_addr);
		console.log("Netmask: " + config.netmask);
		console.log("Gateway: " + config.gw_addr);
		console.log("DNS Address 1: " + config.dns_addr[0]);
		console.log("DNS Address 2: " + config.dns_addr[1]);
		console.log("MAC Address: " + config.mac_addr);
	}
}
