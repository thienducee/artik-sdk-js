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

## client_request
```javascript
String client_request(Integer id, String uri, String name,
				      Number lifetime, Number connect_timeout, String json_objects,
				      String psk_id, String psk_key, certificate_mode_config)
```

**Description**

Create an LWM2M client.

**Parameters**

 - *Integer*: id is the server identity.
 - *String*: uri of the server.
 - *String*: name of the client.
 - *Number*: lifetime of the client.
 - *Number*: connect_timeout Connection timeout of the client in milliseconds.
 - *String*: json_objects LWM2M objects to expose.
 - *String*: psk_id is the public identity for PSK based DTLS/TLS connection.
 - *String*: psk_key is the secret for PSK based DTLS/TLS connection.
 - *Object*: Parameters for Certificate base DTLS/TLS connection.
This object is structured as the following:
```javascript
{
	/*
	 * Use a certificate stored in the SE as the client certificate
	 * When this field is defined the field *client_cert* and *client_private_key* are ignored and optional.
	 * When this field is undefined the *client_cert* and *client_private_key* must be filled.
	 * Must be 'artik' or 'manufacturer'
	 */
	se_config : {
		/* Key identifier used by the SE */
		key_id: String,

		/* Type of Key used by the SE */ 
		key_algo: String
	}

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

## client_release

```javascript
String client_release()
```

**Description**

Clean up the client object.

**Parameters**

None

**Return value**

*String*: Error message

**Example**

See [full example](#full-example)

## client_connect

```javascript
String client_connect()
```

**Description**

Connect to the LWM2M server.

**Parameters**

None

**Return value**

*String*: Error message

**Example**

See [full example](#full-example)

## client_disconnect

```javascript
String client_disconnect()
```

**Description**

Disconnect from the LWM2M server.

**Parameters**

None

**Return value**

*String*: Error message

**Example**

See [full example](#full-example)

## client_write_resource

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

# Constants

## LWM2M Object Constants

### Device Object

#### URIs

| Constant                          | Value     |
|:----------------------------------|:---------:|
| LWM2M_URI_DEVICE_MANUFACTURER     | /3/0/0    |
| LWM2M_URI_DEVICE_MODEL_NUM        | /3/0/1    |
| LWM2M_URI_DEVICE_SERIAL_NUM       | /3/0/2    |
| LWM2M_URI_DEVICE_FW_VERSION       | /3/0/3    |
| LWM2M_URI_DEVICE_REBOOT           | /3/0/4    |
| LWM2M_URI_DEVICE_FACTORY_RESET    | /3/0/5    |
| LWM2M_URI_DEVICE_POWER_SOURCES    | /3/0/6    |
| LWM2M_URI_DEVICE_POWER_VOLTAGE    | /3/0/7    |
| LWM2M_URI_DEVICE_POWER_CURRENT    | /3/0/8    |
| LWM2M_URI_DEVICE_BATT_LEVEL       | /3/0/9    |
| LWM2M_URI_DEVICE_MEMORY_FREE      | /3/0/10   |
| LWM2M_URI_DEVICE_ERROR_CODE       | /3/0/11   |
| LWM2M_URI_DEVICE_RESET_ERR_CODE   | /3/0/12   |
| LWM2M_URI_DEVICE_CURRENT_TIME     | /3/0/13   |
| LWM2M_URI_DEVICE_UTC_OFFSET       | /3/0/14   |
| LWM2M_URI_DEVICE_TIMEZONE         | /3/0/15   |
| LWM2M_URI_DEVICE_SUPP_BIND_MODES  | /3/0/16   |
| LWM2M_URI_DEVICE_DEVICE_TYPE      | /3/0/17   |
| LWM2M_URI_DEVICE_HW_VERSION       | /3/0/18   |
| LWM2M_URI_DEVICE_SW_VERSION       | /3/0/19   |
| LWM2M_URI_DEVICE_BATT_STATUS      | /3/0/20   |
| LWM2M_URI_DEVICE_MEMORY_TOTAL     | /3/0/21   |

### Connectivity Monitoring Object

#### URIs

| Constant                          | Value     |
|:----------------------------------|:---------:|
| LWM2M_URI_CONNMON_BEARER          | /4/0/0    |
| LWM2M_URI_CONNMON_AVAIL_BEARERS   | /4/0/1    |
| LWM2M_URI_CONNMON_SIGNAL          | /4/0/2    |
| LWM2M_URI_CONNMON_LINK_QUALITY    | /4/0/3    |
| LWM2M_URI_CONNMON_IP_ADDR         | /4/0/4    |
| LWM2M_URI_CONNMON_ROUTER_IP_ADDR  | /4/0/5    |
| LWM2M_URI_CONNMON_LINK_UTIL       | /4/0/6    |
| LWM2M_URI_CONNMON_APN             | /4/0/7    |
| LWM2M_URI_CONNMON_CELL_ID         | /4/0/8    |
| LWM2M_URI_CONNMON_SMNC            | /4/0/9    |
| LWM2M_URI_CONNMON_SMCC            | /4/0/10   |

### Firmware Object

#### URIs

| Constant                          | Value     |
|:----------------------------------|:---------:|
| LWM2M_URI_FIRMWARE_PACKAGE        | /5/0/0    |
| LWM2M_URI_FIRMWARE_PACKAGE_URI    | /5/0/1    |
| LWM2M_URI_FIRMWARE_UPDATE         | /5/0/2    |
| LWM2M_URI_FIRMWARE_STATE          | /5/0/3    |
| LWM2M_URI_FIRMWARE_UPD_SUPP_OBJ   | /5/0/4    |
| LWM2M_URI_FIRMWARE_UPDATE_RES     | /5/0/5    |
| LWM2M_URI_FIRMWARE_PKG_NAME       | /5/0/6    |
| LWM2M_URI_FIRMWARE_PKG_URI        | /5/0/7    |

#### State values

| Constant                          | Value     |
|:----------------------------------|:---------:|
| LWM2M_FIRMWARE_STATE_IDLE         | 0         |
| LWM2M_FIRMWARE_STATE_DOWNLOADING  | 1         |
| LWM2M_FIRMWARE_STATE_DOWNLOADED   | 2         |
| LWM2M_FIRMWARE_STATE_UPDATING     | 3         |

#### Update Result values

| Constant                          | Value     |
|:----------------------------------|:---------:|
| LWM2M_FIRMWARE_UPD_RES_DEFAULT    | 0         |
| LWM2M_FIRMWARE_UPD_RES_SUCCESS    | 1         |
| LWM2M_FIRMWARE_UPD_RES_SPACE_ERR  | 2         |
| LWM2M_FIRMWARE_UPD_RES_OOM        | 3         |
| LWM2M_FIRMWARE_UPD_RES_CONNE_ERR  | 4         |
| LWM2M_FIRMWARE_UPD_RES_CRC_ERR    | 5         |
| LWM2M_FIRMWARE_UPD_RES_PKG_ERR    | 6         |
| LWM2M_FIRMWARE_UPD_RES_URI_ERR    | 7         |

### Location Object

#### URIs

| Constant                          | Value     |
|:----------------------------------|:---------:|
| LWM2M_URI_LOCATION_LATITUDE       | /6/0/0    |
| LWM2M_URI_LOCATION_LONGITUDE      | /6/0/1    |
| LWM2M_URI_LOCATION_ALTITUDE       | /6/0/2    |
| LWM2M_URI_LOCATION_UNCERTAINTY    | /6/0/3    |
| LWM2M_URI_LOCATION_VELOCITY       | /6/0/4    |
| LWM2M_URI_LOCATION_TIMESTAMP      | /6/0/5    |

# Full example

   * See [lwm2m-example.js](/examples/lwm2m-example.js)
