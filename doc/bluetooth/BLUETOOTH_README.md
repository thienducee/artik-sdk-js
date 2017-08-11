# Bluetooth API
## class AdapterInfo
### Attributes
#### address
```javascript
String address
```

**Description**

The bluetooth mac address of the adapter.

#### name
```javascript
String name
```

**Description**

The bluetooth system name.

#### alias
```javascript
String alias
```

**Description**

The bluetooth friendly name.
In case of no alias is set it will set with the bluetooth system name.

#### cod
```javascript
{
	String major;
	String minor;
	String service_class;
}
```

**Description**
The bluetooth class of device.

#### discoverable
```javascript
Boolean discoverable;
```

**Description**
Indicate the disocverable mode.

#### pairable
```javascript
Boolean pairable;
```

**Description**
Indicate the pairable mode.

#### pair_timeout
```javascript
Number pair_timeout;
```

**Description**
The pairable timeout in seconds. A value of zero means that the timeout is disabled.

#### discover_timeout
```javascript
Number discover_timeout;
```

**Description**
The discoverable timeout in seconds. A value of zero means that the timeout is disbaled.

#### discovering
```javascript
Boolean discovering;
```

**Description**
Indicate that a device discovery procedure is active.

#### uuids
```javascript
Uuid = {
  String uuid,
  String uuid_name,
};
Array[Uuid] uuids;
```

**Description**
List of 128-bit UUIDs that represents the available local services.

## class Filter
### Scan type

|     Scan type    | Value |
|:----------------:|:-----:|
| interleaved scan | auto  |
| BR/EDR scan      | bredr |
| LE scan          |  le   |

### Constructor

```javascript
var filter = new bluetooth.Filter(Number rssi, String[] uuids, String type);
```

**Description**

Create a new instance of Filter

**Parameters**

*Number*: the RSSI threshold
*String[]*: filtered service UUIDs (can be empty)
*String*: Scan type among the ones defined under [Scan type](#scan-type)

**Return Value**

New Instance

**Example**

```javascript
var filter = new Filter(-70, [], 'LE');
```

## class Bluetooth

### Remote device property

|                property              |  value |
|:------------------------------------:|:------:|
| The bluetooth device address         | Adress |
| The bluetooth device name            | Name   |
| The name alias for the remote device | Alias  |

### Constructor
```javascript
var bt = new bluetooth();
```

**Description**

Create a new instance of Bluetooth

**Parameters**

None

**Return Value**

 New Instance

**Example**

```javascript
var bt = new bluetooth();
```

### start_scan

```javascript
start_scan()
```

**Description**

Start scanning for surrounding Bluetooth devices.

**Parameters**

None

**Return value**

None

**Example**

```javascript
bt.start_scan();
```

### stop_scan

```javascript
stop_scan()
```

**Description**

Stop scanning for Bluetooth devices.

**Parameters**

None

**Return value**

None

**Example**

```javascript
bluetooth.stop_scan();
```

### get_devices

```javascript
String get_devices()
```

**Description**

Return the list of Bluetooth devices that were previously discovered during
scan.

**Parameters**

None

**Return value**

*String*: JSON formatted string containing the list of discovered devices and their
properties.

**Example**

```javascript
console.log('Discovered devices: ' + bt.get_devices());
```

### get_connected_devices

```javascript
String get_connected_devices()
```

**Description**

Return the list of Bluetooth devices to which the device is currently connected to.

**Parameters**

None

**Return value**

*String*: JSON formatted string containing the list of connected devices and their
properties.

**Example**

```javascript
console.log('Connected devices: ' + bt.get_connected_devices());
```

### get_paired_devices

```javascript
String get_paired_devices()
```

**Description**

Return the list of Bluetooth devices with which the device is currently paired.

**Parameters**

None

**Return value**

*String*: JSON formatted string containing the list of paired devices and their
properties.

**Example**

```javascript
console.log('Paired devices: ' + bt.get_paired_devices());
```

### start_bond

```javascript
start_bond(String addr)
```

**Description**

Start pairing process with a remote Bluetooth device.

**Parameters**

 - *String*: Bluetooth address of the remote device to pair with.

**Return value**

None

**Example**

```javascript
bt.start_bond('01:02:03:04:05:06');
```

### stop_bond

```javascript
stop_bond(String bdaddr)
```

**Description**

Unpair with a previously paired Bluetooth remote device.

**Parameters**

 - *String*: Bluetooth address of the remote device to unpair with.

**Return value**

None

**Example**

```javascript
bt.stop_bond('01:02:03:04:05:06');
```

### connect

```javascript
None connect(String bdaddr)
```

**Description**

Connect to a remote Bluetooth device.

**Parameters**

 - *String*: Bluetooth address of the remote device to connect to.

**Return value**

None

**Example**

```javascript
bt.connect('01:02:03:04:05:06');
```

### disconnect

```javascript
disconnect(String bdaddr)
```

**Description**

Disconnect from a previously connected Bluetooth remote device.

**Parameters**

 - *String*: Bluetooth address of the remote device to disconnect from.

**Return value**

None

**Example**

```javascript
bt.disconnect('01:02:03:04:05:06');
```

### remove_unpaired_devices

```javascript
remove_unpaired_devices()
```

**Description**

Remove all devices to which the host is not paired from the discovered
devices list.

**Parameters**

None

**Return value**

None

**Example**

```javascript
bt.remove_unpaired_devices();
```

### remove_device

```javascript
remove_device(String bdaddr)
```

**Description**

Remove specific device from the discovered devices list.

**Parameters**

 - *String*: Bluetooth address of the remote device to remove.

**Return value**

None

**Example**

```javascript
bt.remove_device('01:02:03:04:05:06');
```

### set_scan_filter

```javascript
set_scan_filter(Filter filter)
```

**Description**

Set the device discovery filter

**Parameters**

*Filter*: The filter to apply

**Return value**

None

**Example**
```javascript
var filter = new Filter(-70, [], 'LE');
bt.set_scan_filter(filter);
```

### get_adapter_info
```javascript
AdapterInfo get_adapter_info()
```

**Description**

Return informations about the bluetooth adapter.

**Parameters**

None

**Return value**

*AdapterInfo*: Javascript object containing infomations about the bluetooth adapter.

**Example**
```javascript
bt.get_adapter_info();
```

### set_alias
```javascript
set_alias(String alias);
```

**Description**

Set the bluetooth friendly name.
By default the bluetooth friendly name is the systeme provided name.

**Parameters**

*String*: The bluetooth friendly name. If alias is empty, the system provided name is used.

**Return value**

None

**Example**
```javascript
bt.set_alias("My bluetooth device");
```

### set_discoverable
```javascript
set_discoverable(Boolean discoverable);
```

**Description**

Switch the bluetooth adapter to discoverable or non discoverable mode.

**Parameters**

*Boolean*: If discoverable is true, set the bluetooth adapter to discoverable mode; otherwise
the non pairable mode is used.

**Return value**

None

**Example**
```javascript
bt.set_discoverable(true);
```

### set_discoverableTimeout
```javascript
set_discoverableTimeout(Number timeout);
```

**Description**

Set the discoverable timeout.
When the discoverable timeout expired the bluetooth adapter switchs to non discoverable mode.
If the disocverable timeout is set to zero the bluetooth adapter does not swhitch to discoverable mode.

**Parameters**

*Number*: The discoverable timeout in seconds

**Return value**

None

**Example**
```javascript
bt.set_discoverableTimeout(10);
```

### set_pairable
```javascript
set_pairable(Boolean pairable);
```

**Description**

Switch the bluetooth adapter to pairable or non pairable mode.

**Parameters**

*Boolean*: If pairable is true, set the bluetoot adapter to pairable mode; otherwise the non pairable mode is used.

**Return value**

None

**Example**

```javascript
bt.set_pairable(false)
```

### set_pairableTimeout
```javascript
set_pairableTimeout(Number timeout);
```

**Description**

Set the pairable timeout.
When the pairable timeout expired the bluetooth adapter switchs to non pairable mode.
If the pairable timeout is set to zero the bluetooth adapter does not swhitch to discoverable mode.

**Parameters**

*Number*: Pairable timeout in seconds.

**Return value**

None

**Example**
```javascript
bt.set_pairableTimeout(0);
```

### get_device_property
```javascript
String get_device_property(String addr, String property);
```

**Description**

Get a property of a remote device.

**Parameters**

*String*: Bluetooth address of the remote device.
*String*: The property type among the ones defined under [Property type](#remote-device-property)

**Return value**

*String*: The value of the property.

**Example**
```javascript
var name = bt.get_device_property("01:02:03:04:05:06", "Name");
console.log("Name of 01:02:03:04:05:06 is " + name);
```

### connect_profile
```javascript
connect_profile(String add, String uuid);
```

**Description**

Connect a specific profile of a remote device

**Parameters**

*String*: Bluetooth address of the remote device.
*String*: The UUID of the specific profile.

**Return value**

None

**Example**
```javascript
bt.connect_profile("01:02:03:04:05:06", "0000180f-0000-1000-8000-00805f9b34fb");
```

### is_scanning
```javascript
Boolean is_scanning()
```

**Description**

Indicate if the device disocvery proecdure is active.

**Parameters**

None

**Return value**

*Boolean*: True if the device discovery procedure is active, otherwise false.

**Example**

```javascript
var is_scanning = bt.is_scanning()
console.log("Bluetooth adapter is scanning: " + is_scanning);
```

### is_paired
```javascript
Boolean is_paired(String addr);
```

**Description**

Indicate if the remote device is paired.

**Parameters**

*String*: Bluetooth address of the remote device.

**Return value**

*Boolean*: True if the remote device is paired, otherwise false.

**Example**
```javascript
var is_paired = bt.is_paired("01:02:03:04:05:06");
console.log("The bluetooth device 01:02:03:04:05:06 is paired: " + is_paired);
```

### is_connected
```javascript
Boolean is_connected(String addr);
```

**Description**

Indicate if the remote device is connected.

**Parameters**

*String*: Bluetooth address of the remote device.

**Return value**

*Boolean*: True if the remote device is connected, otherwise false.

**Example**
```javascript
var is_connected = bt.is_connected("01:02:03:04:05:06");
console.log("The bluetooth device 01:02:03:04:05:06 is connected: " + is_connected);
```

### is_trusted
```javascript
Boolean is_trusted(String addr);
```

**Description**

Indicate if the remote device is trusted.

**Parameters**

*String*: Bluetooth address of the remote device.

**Return value**

*Boolean*: True if the remote device is trusted, otherwise false;

**Example**
```javascript
var is_trusted = bt.is_trusted("01:02:03:04:05:06");
console.log("The bluetooth device 01:02:03:04:05:06 is trusted: " + is_trusted);
```

### is_blocked
```javascript
Boolean is_blocked(String addr);
```

**Description**

Indicate if the remote device is blocked

**Parameters**

*String*: Bluetooth address of the remote device.

**Return value**

*Boolean*: True if the remote device is blocked, otherwise false.

**Example**
```javascript
var is_blocked = bt.is_blocked("01:02:03:04:05:06");
console.log("The bluetooth device 01:02:03:04:05:06 is blocked:" + is_blocked);
```

### set_trust
```javascript
set_trust(String addr);
```

**Description**

Seen the remote device as trusted.

**Parameters**

*String*: Bluetooth address of the remote device.

**Return value**

None

**Example**
```javascript
bt.set_trust("01:02:03:04:05:06");
```

### unset_trust
```javascript
unset_trust(String addr);
```

**Description**

Seen the remote device as untrusted.

**Parameters**

*String*: Bluetooth address of the remote device.

**Return value**

None

**Example**
```javascript
bt.unset_trust("01:02:03:04:05:06");
```

### set_block
```javascript
set_block(String addr);
```

**Description**

Reject any incomming connections from the remote device.

**Parameters**

*String*: Bluetooth address of the remote device.

**Return value**

None

**Example**
```javascript
bt.set_block("01:02:03:04:05:06");
```

### unset_block
```javascript
unset_block(String addr);
```

**Description**

Accept all incoming connections from the remote device.

**Parameters**

*String*: Bluetooth address of the remote device.

**Return value**

None

**Example**
```javascript
bt.set_unblock("01:02:03:04:05:06");
```

### Events
#### started
```javascript
bt.on('started', function());
```

**Description**

Called when the bluetooth module is ready.

**Parameters**

None

**Example**
```javascript
bt.on('started', function() { console.log('onstarted'); });
```

#### scan
```javascript
bt.on('scan', function(String err, String device));
```

**Description**

Called every time devices is discovered.

**Parameters**

*String*: Not null when an error occurs.
*String*: The discovered devices as a JSON formatted string

**Example**
```javascript
bt.on('scan', function(err, device) { console.log('onscan (err=' + err + '): ' + device); });
```

#### bond
```javascript
bt.on('bond', function(String err, Boolean paired));
```

**Description**

Called every time a device is paired or unpaired.

**Parameters**

*String*: Not null when an error occurs.
*Boolean*: True if the device is paired, otherwise false

**Example**
```javascript
bt.on('bond', function(err, paired) { console.log('bonded (err=' + err +'): ' + paired); });
```

#### connect
```javascript
bt.on('connect', function(String err, Boolean connected));
```

**Description**

Called every time a device is connected or disconnected.

**Parameters**

*String*: Not null when an error occurs.
*Boolean*: True if the device is connected, otherwise false

**Example**
```javascript
bt.on('connect', function(err, connected) { console.log('connected (err=' + err + '): ' + connected); });
```

## Full example

   * See [bluetooth-example.js](/examples/bluetooth-example.js)
