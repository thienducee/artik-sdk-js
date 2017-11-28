# Cloud API

## Constructor

```javascript
var cl = new cloud(String token);
```

**Description**

Create a new instance of the cloud API with a specific authorization token
needed to make requests to the ARTIK Cloud API.

**Parameters**

 - *String*: authorization token

**Return value**

New instance.

**Example**

```javascript
var cl = new cloud('<authorization token>');
```

## send_message

```javascript
String send_message(String device_id, String message, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Send a message to ARTIK cloud from a specific device ID.

**Parameters**

 - *String*: ID of the device sending the message.
 - *String*: message to send in JSON formatted string.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
- *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. The object must be structured as the following example :

```javascript
var ssl_config = {

	/*
	optional
	Secure Element Configuration
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

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
var msg = '{ "data": { "state": true } }';
send_message('<device ID>', msg, function(response){
	console.log('Response: ' + response);
});
```

## send_action

```javascript
String send_action(String device_id, String action, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Send an action to ARTIK cloud targeted to a specific device ID.

**Parameters**

 - *String*: ID of the destination device to send the action to.
 - *String*: action to send in JSON formatted string.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
var seton = '{ "actions": [{ "name": "setOn", "parameters": {} }]}';
send_action('<device ID>', seton, function(response){
	console.log('Response: ' + response);
});
```

## get_current_user_profile

```javascript
String get_current_user_profile(function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Get the current user profile, based on the authorization token.

**Parameters**

 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
get_current_user_profile(function(response){
	console.log('Response: ' + response);
});
```

## get_user_devices

```javascript
String get_user_devices(Number count, Boolean properties, Number offset, String user_id, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Get the list of devices belonging to a specific user.

**Parameters**

 - *Number*: maximum count of devices to return.
 - *Boolean*: if *true*, return device properties as well.
 - *Number*: offset in the devices list from which to start, used for pagination.
 - *String*: user ID corresponding to the owner of the devices to list.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
get_user_devices(10, false, 0, '<user ID>', function(response){
	console.log('Response: ' + response);
});
```

## get_user_device_types

```javascript
String get_user_device_types(Number count, Boolean shared, Number offset, String user_id, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Get the list of device types available to a specific user.

**Parameters**

 - *Number*: maximum count of device types to return.
 - *Boolean*: if *true*, return device types shared by all users as well.
 - *Number*: offset in the device types list from which to start, used for pagination.
 - *String*: user ID corresponding to the owner of the device types to list.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
get_user_device_types(10, false, 0, '<user ID>', function(response){
	console.log('Response: ' + response);
});
```

## get_user_application_properties

```javascript
String get_user_application_properties(String user_id, String app_id, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Get properties stored by a user for a specific application.

**Parameters**

 - *String*: user ID whose application properties are to be returned.
 - *String*: application ID whose properties are to be returned.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
get_user_application_properties('<user ID>', '<app ID>', function(response){
	console.log('Response: ' + response);
});
```

## get_device

```javascript
String get_device(String device_id, Boolean properties, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Get information related to a specific device.

**Parameters**

 - *String*: device ID whose information is to be returned.
 - *Boolean*: if *true*, return the device properties as well.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
get_device('<device ID>', true, function(response){
	console.log('Response: ' + response);
});
```

## get_device_token

```javascript
String get_device_token(String device_id, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Get authorization token associated to a specific device.

**Parameters**

 - *String*: device ID whose token is to be returned.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
get_device_token('<device ID>', function(response){
	console.log('Response: ' + response);
});
```

## add_device

```javascript
String add_device(String user_id, String device_type_id, String name, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Add a new device to a specific user.

**Parameters**

 - *String*: user ID to associate the new device to.
 - *String*: device type ID of the new device to create.
 - *String*: friendly name to give to the new device.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
add_device('<user ID>', '<device type ID>', 'Heartrate sensor', function(response){
	console.log('Response: ' + response);
});
```

## delete_device

```javascript
String delete_device(String device_id, function(String err, Object response) = undefined, , Object ssl_config = undefined)
```

**Description**

Delete a specific device from the authorized user account.

**Parameters**

 - *String*: device ID of the device to delete.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
delete_device('<device ID>', function(response){
	console.log('Response: ' + response);
});
```

## update_device_token

```javascript
String update_device_token(String device_id, function(String err, Object response) = undefined, , Object ssl_config = undefined)
```

**Description**

Update the authorization token of a specific device.

**Parameters**

 - *String*: device ID whose token is to be refreshed.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
update_device_token('<device ID>', function(response){
	console.log('Response: ' + response);
});
```

## delete_device_token

```javascript
String delete_device_token(String device_id, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Delete the authorization token of a specific device.

**Parameters**

 - *String*: device ID whose token is to be deleted.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
delete_device_token('<device ID>', function(response){
	console.log('Response: ' + response);
});
```

## get_device_properties

```javascript
String get_device_properties(String device_id, Boolean timestamp, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Get a device's properties (server/system/device properties)

**Parameters**

 - *String*: device ID whose properties will be read.
 - *Boolean*: include timestamp.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
get_device_properties('<device ID>', true, function(response){
	console.log('Response: ' + response);
});
```

## set_device_server_properties

```javascript
String set_device_server_properties(String device_id, String data, function(String err, Object response) = undefined, Object ssl_config = undefined)
```

**Description**

Set a device's server properties

**Parameters**

 - *String*: device ID whose properties will be read.
 - *String*: JSON data for setting a device's server properties.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.
 - *Object*: optional object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. See the method send_message for ssl_config structure.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

```javascript
set_device_server_properties('<device ID>', <data>, function(response){
	console.log('Response: ' + response);
});
```

## sdr_start_registration

```javascript
String sdr_start_registration(String cert_id, String device_type_id, String vendor_id, function(String err, Object response))
```

**Description**

Start Secure Device Registration process.

**Parameters**

 - *String*: Certificate identifier. Must be **artik** or **manufacturer**.
 - *String*: device type ID of the device to register.
 - *String*: vendor specific ID of the device to register.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

See [Secure Device Registration example](#secure-device-registration-example)

## sdr_registration_status

```javascript
String sdr_registration_status(String cert_id, String registration_id, function(String err, Object response))
```

**Description**

Get current status of the Secure Device Registration process.

**Parameters**

 - *String*: Certificate identifier. Must be **artik** or **manufacturer**.
 - *String*: registration ID returned by the *sdr_start_registration* function.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

See [Secure Device Registration example](#secure-device-registration-example)

## sdr_complete_registration

```javascript
String sdr_complete_registration(String cert_id, String registration_id, String nonce, function(String err, Object response))
```

**Description**

Complete the Secure Device Registration process after the user has entered
the PIN into ARTIK Cloud portal. Should be called only after registration status
is set to **PENDING_DEVICE_COMPLETION**.

**Parameters**

 - *String*: Certificate identifier. Must be **artik** or **manufacturer**.
 - *String*: registration ID returned by the *sdr_start_registration* function.
 - *String*: registration nonce returned by the *sdr_start_registration* function.
 - *function(String, Object)*: optional callback function that will be called after
performing the request asynchronously. Response from the cloud is passed as a
parameter to the callback in a JavaScript object representing the JSON response.
If *err* is not null an error occurs during the request. If no function is provided
the request will be performed synchronously.

**Return value**

*Undefined* if the callback function is provided, a JSON formatted *String*
containing the response from the cloud otherwise (synchronous call).

**Example**

See [Secure Device Registration example](#secure-device-registration-example)

# Full example

## Secure Device Registration example

Full SDR procedure is documented [here](https://developer.artik.cloud/documentation/advanced-features/secure-your-devices.html)

See [sdr-example.js](/examples/sdr-example.js) for client side implementation.

## Websocket example
   * See [cloud-websocket-example.js](/examples/cloud-websocket-example.js)

## Other example
   * See [cloud-example.js](/examples/cloud-example.js)
