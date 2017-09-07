# LWM2M API

## Constructor

```javascript
var lwm2m_client = new lwm2m();
```

**Description**

Create and configure a new mqtt client object.

**Parameters**

None

**Return value**

New instance.

**Example**

See [full example](#full-example)

## client_connect
```javascript
String client_connect(Integer id, String uri, String name,
				      Number lifetime, String json_objects,
				      String psk_id, String psk_key, certificate_mode_config)
```

**Description**

Create an LWM2M client and connect it to the server.

**Parameters**

 - *Integer*: id is the server identity.
 - *String*: uri of the server.
 - *String*: name of the client.
 - *Number*: lifetime of the client.
 - *String*: json_objects LWM2M objects to expose.
 - *String*: psk_id is the public identity for PSK based DTLS/TLS connection.
 - *String*: psk_key is the secret for PSK based DTLS/TLS connection.
 - *Object*: Parameters for Certificate base DTLS/TLS connection.
This object is structured as the following:
```javascript
{
	/*
	 * Use the certificate stored in the SE as the client certificate
	 * When this field is true the field *client_cert* and *client_private_key* are ignored and optional.
	 * When this field does not exist the *client_cert* and *client_private_key* must be filled.
	 */
	'use_se': Boolean,

	/*
	 * The client certificate use for DTLS/TLS connection.
	 * This X509 certificate must be in PEM format.
	 */
	'client_cert': String,

	/*
	 * The client private key associated with the certificate in *client_cert*.
	 * This private key must be in PEM format.
	 */
	'client_private_key': String,

	/*
	 * The server certificate or root CA.
	 * For certificate based DTLS:
	 *      This attribute is the server certificate and must be present.
	 *
	 * For PSK base DTLS:
	 *      This attribute is not used.
	 *
	 *
	 * For PSK/Certificate based TLS connection:
	 *      This attribute is a root CA and is an optional attribute.
	 *
	 */
	'server_or_root_cert': String,

	/*
	 * Verify the server certificate (only in TLS).
	 *
	 * "none" if no verification,
	 * "optional" if optional verification,
	 * "required" if the verification must be done.
	 *
	 * default value: "required"
	 */
	'verify_cert': String,
}
```

**Return value**

*String*: Error message

**Example**

See [full example](#full-example)

## client_disconnect

```javascript
String client_disconnect()
```

**Description**

Disconnect from the LWM2M server

**Parameters**

None

**Return value**

*String*: Error message

**Example**

See [full example](#full-example)

## client_disconnect

```javascript
String client_write_resource(String uri, Buffer message)
```

**Description**

Write LWM2M resource value

**Parameters**

 - *String*: uri of the resource to write.
 - *String*: message wich should be written to the resource.

**Return value**

*String*: Error message

**Example**

See [full example](#full-example)

## client_read_resource

```javascript
String client_read_resource(String uri)
```

**Description**

Read LWM2M resource value.

**Parameters**

 - *String*: uri of the resource to read.

**Return value**

*String*: Error message

## serialize_tlv_int

```javascript
Buffer serialize_tlv_int(Array array_of_int)
```

**Description**

Serialize an array of integer under the TLV format.

**Parameters**

 - *Array*: array_of_int is the data to serialize.

**Return value**

*String*: Error message or the buffer filled

## serialize_tlv_string

```javascript
Buffer serialize_tlv_string(Array array_of_str)
```

**Description**

Serialize an array of string under the TLV format.

**Parameters**

 - *Array*: array_of_str is the data to serialize.

**Return value**

*String*: Error message or the buffer filled

**Example**

See [full example](#full-example)

# Events

## execute

```javascript
lwm2m_client.on('execute', function (String uri))
```

**Description**

Called every time when the lwm2m client resource has been executed.

**Parameters**

 - *String*: uri of the resource.

**Example**

See [full example](#full-example)

## changed

```javascript
lwm2m_client.on('changed', function (String uri))
```

**Description**

Called every time when the lwm2m client resource has been changed.

**Parameters**

 - *String*: uri of the resource.

**Example**

See [full example](#full-example)

## error

```javascript
lwm2m_client.on('error', function (String err))
```

**Description**

Called every time when the lwm2m client received an error.

**Parameters**

 - *String*: err is the error message.

**Example**

See [full example](#full-example)

# Full example

   * See [lwm2m-example.js](/examples/lwm2m-example.js)
