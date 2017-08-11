# Bluetooth PAN API

## class Pan
### Constructor
```javascript
var pan = new bluetooth.Pan();
```

**Description**

Create a new instance of Pan

**Parameters**

None

**Return Value**

 New Instance

**Example**

```javascript
var pan = new bluetooth.pan();
```

### register
```javascript
register(String uuid, String bridge)
```

**Description**

Register server for the provided UUIDs.

**Parameters**

*String*: The UUID (valid UUIDs are "gn", "panu" or "nap")
*String*: The bridge interface

**Return value**

None

**Example**
```javascript
pan.register("nap", "bnep0");
```

### unregister
```javascript
unregister(String uuid)
```

**Description**

Unregister server for the provided UUIDs.

**Parameters**

*String*: The UUID (valid UUIDs are "gn", "panu" or "nap")

**Return value**

None

**Example**
```javascript
pan.unregister("nap");
```

### connect
```javascript
String connect(String addr, String uuid);
```

**Description**

Connect the PAN service to another.

**Parameters**

*String*: Bluetooth address of the remote device.
*String*: The UUID (valid UUIDs are "gn", "panu" or "nap")

**Return value**

*String*: The network interface name.

**Example**
```javascript
pan.connect("01:02:03:04:05:06", "nap");
```

### disconnect
```javascript
disconnect();
```

**Description**

Disconnect from the pan service.

**Parameters**

None

**Return value**

None

**Example**
```javascript
pan.disconnect();
```

### get_connected
```javascript
Boolean get_connected()
```

**Description**

Get the status of the connection.

**Parameters**

None

**Return value**

*Boolean*: True if the device is connected, otherwise false.

**Example**
```javascript
var connected = pan.get_connected();
console.log("Connected: " + connected);
```

### get_interface
```javascript
String get_interface();
```

**Description**

Get the network interface of the connection.

**Parameters**

None

**Return value**

*String*: The network interface.

**Example**
```javascript
var if = pan.get_interface();
console.log("Network interface: " + if);
```

### get_UUID
```javascript
String get_UUID();
```

**Description**

Get the device UUID.

**Parameters**

None

**Return value**

*String*: The device UUID

**Example**
```javascript
var uuid = pan.get_UUID();
console.log("UUID: " + uuid);
```

## Full example

  * See [bluetooth-panu-example.js](/examples/bluetooth-panu-example.js)
  * See [bluetooth-nap-example.js](/examples/bluetooth-nap-example.js)
