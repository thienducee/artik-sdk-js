# Websocket API

## Constructor

```javascript
var ws = new websocket(String uri, Object ssl_config = null);
```

**Description**

Create and configure a new websocket object.

**Parameters**

 - *String*: URI of the websocket server to connect to.
 - *Object*: object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. The object must be structured as the following example :

```javascript
var ssl_config = {

	/*
	optional
	Enabling Secure Element
	*/
	use_se: false,

	/*
	optional but could be required for verification
	CA root certificate of the server
	*/
	ca_cert: Buffer.from(""),

	/*
	optional but could be required for verification
	Client certificate
	*/
	client_cert: Buffer.from(""),

	/*
	optional but could be required for verification
	Client private key
	*/
	client_key: Buffer.from(""),

	/*
	optional
	Verification of server certificate

	"none" for no verification,
	"optional" for optional verification,
	"required" for required verification
	*/
	verify_cert: "none"
};

```

**Return value**

New instance.

**Examples**

```javascript
var websocket = new websocket('ws://echo.websocket.org/');
```

```javascript
var fs = require('fs');

var data = fs.readFileSync('artik_cloud_ca.pem');

ssl_config = {
	use_se: true,
	ca_cert = Buffer.from(data),
	verify_cert: "required"
}

var cloud = new websocket('wss://api.artik.cloud/v1.1', ssl_config);
```

## open_stream

```javascript
Number open_stream()
```

**Description**

Initiate the connection to the websocket host. After calling this function,
the application should catch the [connected](#connected) event to get notified
of the connection status change.

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## close_stream

```javascript
Number close_stream()
```

**Description**

Close the current websocket connection. After calling this function,
the application should catch the [connected](#connected) event to get notified
of the connection status change.

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## write_stream

```javascript
Number write_stream(String data)
```

**Description**

Send data over the websocket.

**Parameters**

 - *String*: string containing the data to send over the websocket.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

# Events

## connected

```javascript
ws.on('connected', function(String))
```

**Description**

Called after a status change of the websocket connection.

**Parameters**

 - *String*: string containing the current state of the connection.
**CONNECTED** if the connection was successful, **CLOSED** if it was closed
manually or by the remote host or **HANDSHAKE ERROR** if the handshake has failed
when SSL/TLS is enabled.

**Example**

See [full example](#full-example)

## receive

```javascript
ws.on('receive', function(String))
```

**Description**

Called after a data was received on the websocket.

**Parameters**

 - *String*: string containing the received data.

**Example**

See [full example](#full-example)

# Full example

   * See [websocket-example.js](/examples/websocket-example.js)
