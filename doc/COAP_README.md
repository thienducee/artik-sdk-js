# CoAP API

## Constructor

```javascript
var coap = new coap(Object config);
```

**Description**

Create and configure a new CoAP client or server object.

**Parameters**

- *Object*: object containing the different parameters for configuring the CoAP
client or server. The object must be structured as the following example :

```javascript
var config = {
	/*
	required for the client for establishing a session with the
	server
	*/
	uri: String (e.g: "coap://californium.eclipse.org/"),

	/*
	optional for the server for defining a port (5683 by default for coap or
	5684 for coaps)
	*/
	port: Number,

	/*
	optional if the user wants to establish a SSL connection with Elliptic
	Curve Cryptography (ECC) from server or client
	*/
	ssl: {
		/*
		optional
		Secure Element Configuration (not supported)
		*/
		se_config: {
			/*
			mandatory
			The selected certificate for SSL handshake
			Must be 'artik' or 'manufacturer'
			*/
			certificate_identifier: String
		},

		/*
		optional but could be required for verification
		CA root certificate of the server (not supported)
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
		Verification of server certificate (not supported)

		"none" for no verification,
		"optional" for optional verification,
		"required" for required verification
		*/
		verify_cert: "none"
	},

	/*
	optional if the user wants to establish a SSL connection with Pre-Shared
	Key (PSK) from server or client
	*/
	psk: {
		// Identity
		identity: Buffer (e.g: Buffer.from("Client_identity")),
		// Password, passphrase
		psk: Buffer (e.g: Buffer.from("secretPSK"))
	},

	/*
	optional for enabling the verificaton of PSK parameters from client.
	The user has to set a callback as parameter of the function start_server.
	*/
	verify_psk: Boolean
}
```
**Return value**

New instance.

**Example**

See [full example](#full-example)

## create_client

```javascript
Number create_client()
```

**Description**

Create client CoAP context.

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## destroy_client

```javascript
Number destroy_client()
```

**Description**

Destroy client CoAP context.

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## connect

```javascript
Number connect()
```

**Description**

Establish the communication with the server

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## disconnect

```javascript
Number disconnect()
```

**Description**

Close the communication with the server

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## create_server

```javascript
Number create_server()
```

**Description**

Create server CoAP context

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## destroy_server

```javascript
Number destroy_server()
```

**Description**

Destroy server CoAP context

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## start_server

```javascript
Number start_server(function(Buffer, Number))
```

**Description**

Launch the server

**Parameters**

- *Function*: optional callback for verifying PSK parameters from peer with
two parameters:
	- *Buffer*: identity
	- *Number*: Max length of the expected key
It must return a buffer with the key corresponding to the identity.
The parameter *verify_psk* of CoAP configuration must be set to **true** for
calling the callback.

Example :

```javascript
coap.start_server(function(identity, key_len) {
	var tab = [
		{
			identity: Buffer.from("Clident_identity"),
			psk: Buffer.from("secretPSK")
		},
		{
			identity: Buffer.from("password"),
			psk: Buffer.from("sesame")
		}
	]

	for (var i = 0; i < tab.length; i++) {
		if (identity.toString() == tab[i].identity.toString()) {
			if (key_len < tab[i].psk.length) {
				/*
				If the length of the key is too high, return
				an empty buffer.
				*/
				return Buffer.from("");
			}
			/*
			Return the corresponding key
			*/
			return tab[i].psk;
		}
	}

	/*
	If no match, return an empty buffer
	*/
	return Buffer.from("");
});
```

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## stop_server

```javascript
Number stop_server()
```

**Description**

Stop the server

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## send_message

```javascript
Number send_message(String path, Object msg, function(Object, String))
```

**Description**

Send a request for a resource

**Parameters**

- *String*: path of the resource
- *Object*: request message to send
The object must be structured as the following example :

```javascript
var msg = {
	/*
	optional, define the message type. Expect "CON", "NON", "ACK" or "RST".
	"CON" by default.
	*/
	msg_type: String,

	/*
	optional, actual token in hexadecimal format
	*/
	token: Buffer (hex),

	/*
	optional, message ID
	*/
	msg_id: Number,

	/*
	optional, request method (value 1-10) or respone code (value 40-255).
	0 by default (EMPTY).
	*/
	code: Number,

	/*
	optional, data to send as payload.
	*/
	data: Buffer,

	/*
	optional, list of options.
	It must be structured as following:

	options: [
		{
			key: Number,
			data: String, Array or Number
		},
		...
	]

	The format of the data of an option depends on the key.

	The following keys expect the data as a string:
		* URI_HOST
		* LOCATION_PATH
		* URI_PATH
		* URI_QUERY
		* LOCATION_QUERY
		* PROXY_URI
		* PROXY_SCHEME

	The key IF_MATCH expect the data as a buffer:

	The following keys expect the data as a number:
		* CONTENT_FORMAT
		* URI_PORT
		* MAXAGE
		* ACCEPT
		* SIZE1
		* OBSERVE
		* BLOCK2
	*/
	options: Array[Object]
}
```

- *Function*: Response callback after sending request with two parameters:
	- *Object*: response message with the same structure of
	            the request message (see above)
	- *String*: error returned by the CoAP process, **"NONE"** on success,
		    **"TOO MANY RETRIES"**, **"NOT DELIVERABLE"**, **"RST"** or
		    **"TLS FAILED"** on failure.

**Return value**

*Number*: Error code

**Example**

```javascript
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
```
See [full example](#full-example)

## observe

```javascript
Number observe(String path, String msg_type, function(Object, String),
		Object options, Buffer token)
```

**Description**

Observe a resource

**Parameters**

- *String*: path of the resource
- *String*: define the message type. Expect "CON", "NON", "ACK" or "RST"
- *Function*: response callback after observe request with two parameters:
	- *Object*: response message with the same structure of
	            the request message (see in [send_message](##send_message)
	            function)
	- *String*: Error returned by the CoAP process, **"NONE"** on success,
		    **"TOO MANY RETRIES"**, **"NOT DELIVERABLE"**, **"RST"** or
		    **"TLS FAILED"** on failure.
- *Object*: optional, list of options (see in [send_message](##send_message)
	    function)
- *Buffer*: optional, actual token in hexadecimal format

**Return value**

*Number*: Error code

**Example**

```javascript
coap.observe(path_cloud, "CON", function(resp, error) {
	if (error != "NONE" || resp == undefined) {
		console.log("Fail to observe");
		process.exit(-1);
	}

	print_response(resp);
});
```

See [full example](#full-example)

## cancel_observe

```javascript
Number cancel_observe(String path, Buffer token, function(Object, String))
```

**Description**

Cancel observation of a resource

**Parameters**

- *String*: path of the resource
- *Buffer*: optional, actual token in hexadecimal format
- *Function*: response callback after observe request with two parameters:
	- *Object*: response message with the same structure of
	            the request message (see in [send_message](##send_message)
	            function)
	- *String*: Error returned by the CoAP process, **"NONE"** on success,
		    **"TOO MANY RETRIES"**, **"NOT DELIVERABLE"**, **"RST"** or
		    **"TLS FAILED"** on failure.

**Return value**

*Number*: Error code

**Example**

```javascript
coap.cancel_observe("obs", token, function(resp, error) {
	if (error != "NONE" || resp == undefined) {
		console.log("Fail to cancel observe");
		process.exit(-1);
	}

	print_response(resp);
});
```

See [full example](#full-example)

## init_resources

```javascript
Number init_resources(Object resources)
```

**Description**

Initialize resources

**Parameters**

- *Object*: list of resources, must be structured as the following example:

```javascript
var resources = [
	{
		/*
		required, path of the resource
		*/
		path: String,

		/*
		optional, type of the notification
		NON (0x0) or CON (0x2)
		NON by default
		*/
		notify_type: Number,

		/*
		optional, if the resource can be observed
		*/
		observable: Boolean,

		/*
		optional, list of attributes for defining the resource
		It must be structured as following:

		attributes: [
			{
				name: String,
				val: Buffer
			},
			...
		]
		*/
		attributes: Array[Object],

		/*
		optional, callback in case of GET request
		An *Object* parameter is the message sent in the request.
		For its structure see in [send_message](##send_message) function.
		An *Object message response must be returned (same structure as the
		request message).
		*/
		get_resource: function(Object),

		/*
		optional, callback in case of POST request
		An *Object* parameter is the message sent in the request.
		For its structure see in [send_message](##send_message) function.
		An *Object message response must be returned (same structure as the
		request message).
		*/
		post_resource: function(Object),

		/*
		optional, callback in case of PUT request
		An *Object* parameter is the message sent in the request.
		For its structure see in [send_message](##send_message) function.
		An *Object message response must be returned (same structure as the
		request message).
		*/
		put_resource: function(Object),

		/*
		optional, callback in case of DELETE request
		An *Object* parameter is the message sent in the request.
		For its structure see in [send_message](##send_message) function.
		An *Object message response must be returned (same structure as the
		request message).
		*/
		delete_resource: function(Object)
	}
]
```

**Return value**

*Number*: Error code

**Example**

```javascript
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
				coap.notify_resource_changed("test");
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
```
See [full example](#full-example)

## notify_resource_changed

```javascript
Number notify_resource_changed(String path)
```

**Description**

Notify possible observers that the resource has changed

**Parameters**

- *String*: Path of the resource

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

# Full example

   * See [coap-client-example.js](/examples/coap-client-example.js)
   * See [coap-cloud-example.js](/examples/coap-cloud-example.js)
   * See [coap-server-example.js](/examples/coap-server-example.js)