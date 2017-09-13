# HTTP API

## Constructor

```javascript
var http = new http();
```

**Description**

Create a new http object.

# get_stream

```javascript
Stream get_stream(String uri, String[] headers, Object ssl_config)
```

**Description**

Return a readable stream from an uri.

**Parameters**

- *String*: URI to target for the request.
 - *String[]*: array of strings containing the headers to pass along the request. e.g.

```javascript
var headers = [
	"user-agent", "ARTIK browser",
	"Accept-Language", "en-US,en;q=0.8"
];
```

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

**Events**

When a stream is finished, an event 'end' will be emitted :

```javascript
stream.on('end', function() {
	console.log("GET STREAM - Stream finished");
})
```

If an error occurs, you have to catch the exception :

```javascript
var ws = fs.createWriteStream("./image.jpeg");

try {

	var stream = http.get_stream("http://httpbin.org/image/jpeg", headers, null);

	stream.on('end', function() {
		console.log("GET STREAM - Stream finished");
	})

	stream.pipe(ws);

	ws.on('end', function() {
	    ws.end();
	});

} catch(err) {
	console.log(err);
}
```

# get

```javascript
String get(String uri, String[] headers, Object ssl_config, function(String response, Number status))
```

**Description**

Perform a GET HTTP request on a remote host.

**Parameters**

 - *String*: URI to target for the request.
 - *String[]*: array of strings containing the headers to pass along the request. e.g.

```javascript
var headers = [
	"user-agent", "ARTIK browser",
	"Accept-Language", "en-US,en;q=0.8"
];
```

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

 - *function(String, Number)*: optional callback function that will be
called after performing the request asynchronously. The status and the response
from the host are passed as parameters to the callback.
If no function is provided, the request will be performed synchronously.

**Return value**

*Undefined* if the callback function is provided, a *String*
containing the response from the host otherwise (synchronous call).

**Example**

See [Full example](#full-example)

# post

```javascript
String post(String uri, String[] headers, String body, Object ssl_config, function(String response, Number status))
```

**Description**

Perform a POST HTTP request on a remote host.

**Parameters**

 - *String*: URI to target for the request.
 - *String[]*: array of strings containing the headers to pass along the request. e.g.

```javascript
var headers = [
	"user-agent", "ARTIK browser",
	"Accept-Language", "en-US,en;q=0.8"
];
```
 - *String*: body data to send along the request.

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

 - *function(String, Number)*: optional callback function that will be
called after performing the request asynchronously. The status and the response
from the host are passed as parameters to the callback.
If no function is provided, the request will be performed synchronously.

**Return value**

*Undefined* if the callback function is provided, a *String*
containing the response from the host otherwise (synchronous call).

**Example**

See [Full example](#full-example)

# put

```javascript
String put(String uri, String[] headers, String body, Object ssl_config, function(String response, Number status))
```

**Description**

Perform a PUT HTTP request on a remote host.

**Parameters**

 - *String*: URI to target for the request.
 - *String[]*: array of strings containing the headers to pass along the request. e.g.

```javascript
var headers = [
	"user-agent", "ARTIK browser",
	"Accept-Language", "en-US,en;q=0.8"
];
```
 - *String*: body data to send along the request.

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

 - *function(String, Number)*: optional callback function that will be
called after performing the request asynchronously. The status and the response
from the host are passed as parameters to the callback.
If no function is provided, the request will be performed synchronously.

**Return value**

*Undefined* if the callback function is provided, a *String*
containing the response from the host otherwise (synchronous call).

**Example**

See [Full example](#full-example)

# delete

```javascript
String delete(String uri, String[] headers, Object ssl_config, function(String response, Number status))
```

**Description**

Perform a DELETE HTTP request on a remote host.

**Parameters**

 - *String*: URI to target for the request.
 - *String[]*: array of strings containing the headers to pass along the request. e.g.

```javascript
var headers = [
	"user-agent", "ARTIK browser",
	"Accept-Language", "en-US,en;q=0.8"
];
```

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

 - *function(String, Number)*: optional callback function that will be
called after performing the request asynchronously. The status and the response
from the host are passed as parameters to the callback.
If no function is provided, the request will be performed synchronously.

**Return value**

*Undefined* if the callback function is provided, a *String*
containing the response from the host otherwise (synchronous call).

**Example**

See [Full example](#full-example)

# Full example

   * See [http-example.js](/examples/http-example.js)
