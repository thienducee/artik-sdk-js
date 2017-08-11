# Bluetooth GATT Server API
## class GattServer.Service
### Constructor
```javascript
var serv = new bluetooth.GattServer.Service({
  uuid: String,
  characteristics: GattServer.Characteristic[]
});
```

**Description**

Create a new instance of class *GattServer.Service*.

**Parameters**

*uuid*: The UUID of the GATT Service
*characteristic*: The characteristics of this service.

**Return Value**

The new instance.

**Example**
```javascript
var serv = new bluetooth.GattServer.Service({
  uuid:  "0000180f-0000-1000-8000-00805f9b34fb",
  characteristics: []
});
```

## class GattServer.Characteristic
### Characteristic properties
|                     Property                        |           Value        |
|:---------------------------------------------------:|:----------------------:|
| The characteristic is broadcastable                 | broadcast              |
| The characteristic is readable                      | read                   |
| The characteristic can be writtent without response | write-without-response |
| The characteristic is writable                      | write                  |
| The characteristic supports notification            | notify                 |
| The characteristic supports indication              | indicate               |
| The characteristic supports write with signature    | signed-write           |
### Constructor
```javascript
var char = new bluetooth.GattServer.Characteristic({
  uuid: String,
  properties: String[],
  value: node.Buffer,
  descriptors: GattServer.Descriptor[],
  onReadRequest: function(function(String error_type, node.Buffer value) callback),
  onWriteRequest: function(node.Buffer data, function(String error_type) callback),
  onSubscribe: function(function(node.Buffer data) callback),
  onUnsubscribe:function()
});
```

**Description**

Create a new instance of class *GattServer.Characteristic*.

**Parameters**

*uuid*: The UUID of the characteristic.
*properties*: The list of GATT characteristic property see [Characteristic Properties](#characteristic-properties).
*value*: (optional) The initial value of the characteristic.
*descriptors*: (optional) The descriptor of this characteristic.
*onReadRequest*: (optional) Read request handler.
*onWriteRequest*: (optional) Write request handler.
*onSubscribe*: (optional) Subscribe request handler.
*onUnsubscribe*: (optional) Unsubscribe request handler.

**Return Value**

The new instance.

**Example**
```javascript
var characteristic = new GattServer.Characteristic({
	uuid: "00002a19-0000-1000-8000-00805f9b34fb",
	properties: ["read", "notify", "write"],
	value: new Buffer([0x66]),
	descriptors: [],
});
```

## class GattServer.Descriptor

### Descriptor properties
|               Descriptor property                        |       Value    |
|:--------------------------------------------------------:|:--------------:|
| The descriptor is readable                               | read           |
| The descriptor is writable                               | write          |
| The descriptor supports encrypted read                   | enc-read       |
| The descriptor supports encrypted write                  | enc-write      |
| The descriptor supports encrypted and authenticated read | enc-auth-read  |
| The descriptor supports encrypted and authenticated write| enc-auth-write |
| The descriptor supports secure read                      | sec-read       |
| The descriptor supports secure wirte                     | sec-write      |
### Constructor
```javascript
var desc = new bluetoot.GattServer.Descriptor({
  uuid: String,
  properties: String[],
  value: node.Buffer,
  onReadRequest: function(function(String error_type, node.Buffer value) callback),
  onWriteRequest: function(node.Buffer data, function(String error_type) callback),
});
```

**Description**

Create a new instance of class *GattServer.Descriptor*.

**Parameters**

*uuid*: The UUID of the descriptor
*properties*: The list of GATT descriptor property see [Descriptor Properties](#descriptor-properties)
*value*: (optional) The initial value of the descriptor
*onReadRequest*: (optional) Read request handler.
*onWriteRequest*: (optional) Write request handler.

**Return Value**

The new instance.

**Example**
## class GattServer
### Constructor
```javascript
var gatt_server = new bluetooth.GattServer();
```

**Description**

Create a new instance of GattServer

**Parameters**

None

**Return Value**

 New Instance

**Example**

```javascript
var gatt_server = new bluetooth.GattServer();
```


### start_advertising
```javascript
Number start_advertising({
  type: String,
  serviceUuids: String[],
  solicitUuids: String[],
  includeTxPower: Boolean,
  manufacturerData: { id: Number, data: Number[] },
  serviceData: { id: String, data: Number[] }
})
```

**Description**

Start advertising over the LE chanel.

**Parameters**

*type*: Type of advertising ("broadcast" or "peripheral")
*serviceUuids*: List of UUIDs to include in advertising data
*solicitUuids*: List of UUIDs to include in "Service Solicitation" field
*includeTxPower*: If true, the TX poxer is included in the advertisement packet
*manufacturerData*: "data" is the manufacturer data to include in the advertisement packet and "id" the manufacturer ID
*serviceData*: "data" is the service data to include in the advertisement packet and "id" the service id associate with the data.

**Return value**

*Number*: Advertising id.

**Example**
```javascript

```

### stop_advertising
```javascript
stop_adertising(Number id)
```

**Description**

Stop advertising.

**Parameters**

*Number*: Advertising id.

**Return value**

None

**Example**
```javascript
gatt_server.stop_advertising(0);
```

## Full example

  * See [bluetooth-gattserver-example.js](/examples/bluetooth-gattserver-example.js)
