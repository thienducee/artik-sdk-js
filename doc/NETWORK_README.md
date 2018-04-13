# NetworkWatcher
## constructor

```javascript
var watcher = new network.network_watcher(String addr, Integer interval, Integer timeout);
```

**Description**

Create a new network watcher object.

**Parameters**

- *addr*: The host name or IP address to ping to detect connectivity change
- *interval*: Wait interval between sending each ping (in milliseconds)
- *timeout*: Max time to wait for a ping response (in milliseconds)

# Events

## connectivity-change

```javascript
watcher.on("connectivity-change", function(Boolean))
```

**Description**

Get notified of web connectivity changes.

**Parameters**
 - *Boolean* containing the web connectivity status.

**Example**

```javascript
watcher.on("connectivity-change", function(status) {
	console.log('New connectivity status: ' + status);
});
```

# Network API
## constructor

```javascript
var network = new network();
```

**Description**

Create a new network object.

**Parameters**

None.


## set_network_config

```javascript
Number set_network_config(Object config, String interface)
```

**Description**

Set the network configuration (IP address, netmask, gateway, DNS servers)

**Parameters**
- *Object*: object containing the different parameters as IP address, netmask,
 gateway... for applying network configuration. The object must be structured
 as the following example :

```javascript
var config = {

	/*
	required
	IP Address of the server
	*/
	ip_addr: "192.168.4.4",

	/*
	required
	Netmask
	*/
	netmask: "255.255.255.0",

	/*
	required
	Gateway address
	*/
	gw_addr: "192.168.4.1",

	/*
	required
	DNS server addresses
	If one address is enough,
	just initialize like this :
	dns_addr: ["192.168.4.2"]
	*/
	dns_addr: ["192.168.4.2", "192.168.4.3"]

}
```

 - *String*: string specifying the type of network connection.
**wifi** if the type of connection is wireless or **ethernet** if
the type of connection is ethernet (cable).

**Return value**

*Number*: Error code

**Example**

```javascript
var net_config = {
	ip_addr: "1.4.2.4",

	netmask = "255.255.0.0",

	gw_addr: "1.4.0.1",

	dns_addr: ["1.4.0.2", "1.4.0.3"]
}

network.set_network_config(net_config, "wifi");
```

## get_network_config

```javascript
Object get_network_config(String interface)
```

**Description**

Get the network configuration (IP address, netmask, gateway, DNS servers and MAC address)

**Parameters**

 - *String*: string specifying the type of network connection.
**wifi** if the type of connection is wireless or **ethernet** if
the type of connection is ethernet (cable).

**Return value**

*Object* containing the different settings of the network, it is structured like this :

```javascript
var config = {

	/*
	IP Address
	*/
	String ip_addr,

	/*
	Netmask
	*/
	String netmask,

	/*
	Default Gateway
	*/
	String gw_addr,

	/*
	DNS Servers (two max)
	*/
	Array[String] dns_addr[2],

	/*
	MAC Address
	*/
	String mac_addr
};
```

**Example**

```javascript
var config = network.get_network_config("wifi");

if (config) {
	console.log("IP Address: " + config.ip_addr);
	console.log("Netmask: " + config.netmask);
	console.log("Gateway: " + config.gw_addr);
	console.log("DNS Address 1: " + config.dns_addr[0]);
	console.log("DNS Address 2: " + config.dns_addr[1]);
	console.log("MAC Address = " + config.mac_addr);
}
```

## get_current_ip

```javascript
String get_current_ip()
```

**Description**

Get the current public IP address exposed by the device on the web.

**Parameters**

None.

**Return value**

*String* containing the public IP address by which the device is known on the web.

**Example**

```javascript
console.log('Public IP: ' + network.get_current_ip());
```

## dhcp_client_start

```javascript
Number dhcp_client_start(String interface)
```

**Description**

Initiate the DHCP client in order to look for a DHCP server
and to negociate a IP configuration with the server.

**Parameters**

 - *String*: string specifying the type of network connection.
**wifi** if the type of connection is wireless or **ethernet** if
the type of connection is ethernet (cable).

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## dhcp_client_stop

```javascript
Number dhcp_client_stop()
```

**Description**

Stop the DHCP client

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## dhcp_server_start

```javascript
Number dhcp_server_start(Object dhcp_server_config)
```

**Description**

Initiate the DHCP server in order to assign IP configuration to devices
(with DHCP client) on the local network.

**Parameters**

 - *Object*: object containing the different parameters as IP address, netmask,
 gateway... for assigning IP configuration to clients on the local network.
 The object must be structured as the following example :

```javascript
var dhcp_server_config = {
	/*
	required
	Type of network interface (wifi by default)
	*/
	iface: "wifi",

	/*
	required
	IP Address of the server
	*/
	ip_addr: "192.168.4.1",

	/*
	required
	Netmask
	*/
	netmask: "255.255.255.0",

	/*
	required
	Gateway address
	*/
	gw_addr: "192.168.4.1",

	/*
	required
	DNS server addresses
	If one address is enough,
	just initialize like this :
	dns_addr: ["192.168.4.2"]
	*/
	dns_addr: ["192.168.4.2", "192.168.4.3"],

	/*
	required
	IP address from which to start to assign
	*/
	start_addr: "192.168.4.4",

	/*
	required
	Number of IP addresses to assign starting from
	'start_addr'
	*/
	num_leases: 5

}
```

**Return value**

*Number*: Error code

## dhcp_server_stop

```javascript
Number dhcp_server_stop()
```

**Description**

Stop the DHCP server

**Return value**

*Number*: Error code

## get_online_status

```javascript
Boolean get_online_status(String addr, Integer timeout)
```

**Description**

Get the web connectivity status.

**Parameters**

- *addr*: The host name or IP address to ping to detect connectivity change
- *timeout*: Max time to wait for a ping response (in milliseconds)

**Return value**

*Boolean* containing the web connectivity status.

**Example**

```javascript
console.log('Connectivity status: ' + network.get_online_status("artik.cloud", 500));
```

## add_watch_online_status

```javascript
Undefined add_watch_online_status(NetworkWatcher watcher)
```
**Description**

Register a watcher

**Parameters**

- *watcher*: Watcher to be registred

**Return value**

None

**Example**

```javascript
var watcher = new net.network_watcher("artik.cloud", 5000, 500);
network.add_watch_online_status(watcher);
```

## remove_watch_online_status

```javascript
Undefined remove_watch_online_status(NetworkWatcher watcher)
```
**Description**

Unregister a watcher

**Parameters**

- *watcher*: Watcher to be unregistred

**Return value**

None

**Example**

```javascript
var watcher = new net.network_watcher("artik.cloud", 5000, 500);
network.add_watch_online_status(watcher);
network.remove_watch_online_status(watcher);
```


# Full example

   * See [network-example.js](/examples/network-example.js)
   * See [network-dhcp-client-example.js](/examples/network-dhcp-client-example.js)
   * See [network-dhcp-server-example.js](/examples/network-dhcp-server-example.js)
