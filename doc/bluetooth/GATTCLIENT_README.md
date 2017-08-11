# Bluetooth GATT Client API
## class RemoteService
### discover_characteristics
```javascript
RemoteCharacteristic[] discover_characteristics();
```

**Description**

Discover the characteristics of this service.

**Parameters**

None

**Return value**

*RemoteCharacteristic[]*: The discovered characteristics.

**Example**
```javascript
var characteristics = service.discover_characteristics();
```

### Attributes
#### uuid
```javascript
String uuid;
```

**Description**

The UUID of the service.

**Example**
```javascript
console.log("Service uuid is " + service.uuid);
```

## class RemoteCharacteristic
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
### write
```javascript
write(Buffer data);
```

**Description**

Write data to the remote characteristic.

**Parameters**

*Buffer*: The data to be written.

**Return value**

None

**Example**
```javascript
chr.write(new node.Buffer([0x12]));
```

### read
```javascript
Buffer read();
```

**Description**

Read the data of the remote characteristic.

**Parameters**

None

**Return value**

*Buffer*: The value of the characteristic.

**Example**
```javascript
var data = chr.read();
console.log("  - data " + data.toString("hex"));
```

### subscribe
```javascript
subscribe();
```

**Description**

Start a notification session from this characteristic.

When the characteristic sends an notification the 'data' event is triggered.

**Parameters**

None

**Return value**

None

**Example**
```javascript
chr.subscribe();
```

### unsubscribe
```javascript
unsubscribe();
```

**Description**

Stop a notification session.

**Parameters**

None

**Return value**

None

**Example**
```javascript
chr.unsubscribe();
```

### discover_descriptors
```javascript
RemoteDescriptor[] discover_descriptors();
```

**Description**

Discover the descriptors of this characteristic.

**Parameters**

None

**Return value**

*RemoteDescriptor[]*: The discovered descriptors.

**Example**
```javascript
var descriptors = chr.discover_descriptors();
```

### Attributes
#### properties
```javascript
String[] properties;
```

**Description**

The list of GATT characteristic properties see [Characteristic properties](#characteristic-properties)

**Example**
```javascript
console.log("properties: " + chr.properties);
```


#### uuid
```javascript
String uuid;
```

**Description**

The UUID of this characteristic.

**Example**
```javascript
console.log("uuid: " + chr.uuid);
```


### Events
#### data
```javascript
chr.on('data', function(Buffer data));
```

**Description**

Called every time a notification is received.

**Parameters**

*Buffer*: The new value of this characteristic.

**Example**
```javascript
chr.on('data), function(data) {
  console.log("data: " + data.toString('hex'));
});
```

## class RemoteDescriptor
### write
```javascript
write(Buffer data);
```

**Description**

Write data to the remote descriptor.

**Parameters**

*Buffer*: The data to be written.

**Return value**

None

**Example**
```javascrip
desc.write(new node.Buffer([0x12]));
```

### read
```javascript
Buffer read();
```

**Description**

Read data from the remote descriptor.

**Parameters**

None

**Return value**

*Buffer*: The value of the descriptor.

**Example**
```javascript
var data = desc.read();
console.log("  - data " + data.toString("hex"));
```

### Attributes
#### uuid
```javascript
String uuid;
```

**Description**

The UUID of the descriptor.

**Example**
```javascript
console.log("Descriptor uuid is " + desc.uuid);
```

## class GattClient
### Constructor
```javascript
var gatt_client = new bluetooth.GattClient();
```

**Description**

Create a new instance of GattClient

**Parameters**

None

**Return Value**

 New Instance

**Example**

```javascript
var gatt_client = new bluetooth.GattClient();
```

### discover_services
```javascript
RemoteService[] discover_services(String addr);
```

**Description**

Get all GATT services of a remote device.

Note: Wait for the event 'servicesDiscover' before calling this method.

**Parameters**

*String*: The address of the remote device.

**Return value**

*RemoteService[]*: The services of the remote device.

**Example**
```javascript
var services = gatt_client.discover_services("01:02:03:04:05:06");
```

### Events
#### servicesDiscover
```javascript
gatt_client.on("servicesDiscover", function());
```

**Description**

Called when new services is available.

**Parameters**

None

**Example**
```javascript
gatt_client.on('servicesDiscover', function() { console.log('New services available); });
```

## Full example

  * See [bluetooth-gattclient-example.js](/examples/bluetooth-gattclient-example.js)
