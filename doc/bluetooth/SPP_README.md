# Bluetooth SPP API

## class SppProfile
### Constructor
```javascript
var profile = new bluetooth.SppProfile(String name, String service, String role, Number channel, Number PSM, Boolean require_authentication, Boolean require_authorization, Boolean auto_connect, Number version, Number features);
```

**Description**

Create a new instance of SppProfile

**Parameters**

*String*: Human readable name
*String*: The primary service class UUID
*String*: This field is used by asymetric profile that do not have UUIDs available
to uniquely identify each side. The possible value for this field is "client" or "server"
*Number*: RFCOMM channel number
*Number*: Protocol and Service multiplexer id
*Boolean*: True if pairing is needed before any connection
*Boolean*: True if authorization is needed before any connection
*Boolean*: Enable auto-connect
*Number*: Profile version
*Number*: Profile features

**Return Value**

 New Instance

**Example**

```javascript
var profile = new bluetooth.SppProfile("ARTIK SPP", "spp char loopback", "server", 22, 3, true, false, true, 10, 20);
```

## class SppSocket

### write
```javascript
write(Buffer data)
```

**Description**

Send data to the remote device

**Parameters**

*Buffer*: The data to send.

**Return Value**

None

**Example**

```javascript
socket.write(new Buffer([0x62, 0x75]));
```

### Attributes
#### version
```javascript
Number version;
```

**Description**

Profile version

#### features
```javascript
Number features;
```

**Description**

Profile features

### Events
#### data
```javascript
socket.on('data', function(String error, Buffer buffer));
```

**Description**

Called every time data is received on the socket.

**Parameters**

*String*: This string is not null when an error occurs.
*Buffer*: Buffer containing the data that was received on the socket.

**Example**
```javascript
socket.on('data', function(error, buffer) { console.log("data = " + buffer); });
```

## class Spp

### Constructor
```javascript
var spp = new bluetooth.Spp();
```

**Description**

Create a new instance of Spp

**Parameters**

None

**Return Value**

 New Instance

**Example**

```javascript
var spp = new bluetooth.Spp();
```


### register_profile
```javascript
register_profile(SppProfile profile)
```

**Description**

Register spp profile

**Parameters**

*SppProfile*: The option of the spp profile.

**Return value**

None

**Example**
```javascript
spp.register_profile(profile);
```

### unregister_profile
```javascript
unregister_profile();
```

**Description**

Unregister spp profile.

**Parameters**

None

**Return value**

None

**Example**
```javascript
spp.unregister_profile();
```

### Events

#### release

```javascript
spp.on('release', function());
```

**Description**

Called when unregister_profile is called.

**Parameters**

None

**Example**
```javascript
spp.on('release', function() { console.log("Spp profile release"); });
```

#### new_connection

```javascript
spp.on('new_connection', function(SppSocket));
```

**Description**

Called every time new connection has been made and authorized.

**Parameters**

 - *SppSocket*: This object represents an open connection with a target device.

**Example**
```javascript
spp.on('new_connection', function(socket) { console.log("new connection"); });
```

#### disconnect

```javascript
spp.on('disconnect', function(SppSocket));
```
**Description**

Called every time a disconnection occurs.

**Parameters**

 - *SppSocket*: This object represents an open connection with a target device.

**Example**
```javascript
spp.on('disconnect', function(socket) { console.log("disconnection"); });
```

#Full example

  * See [bluetooth-spp-example.js](/example/bluetooth-spp-examples.js)
