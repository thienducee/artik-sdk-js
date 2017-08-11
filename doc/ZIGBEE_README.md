# ZigBee API

## Device types

| Type                 | Value |
|:---------------------|:-----:|
| On-Off Switch        | 0     |
| Level Control Switch | 1     |
| On-Off Light         | 2     |
| Dimmable Light       | 3     |

## initialize

```javascript
initialize(Number[] devices)
```

**Description**

Initialize the ZigBee module with the list of device types it should expose.

**Parameters**

 - *Number[]*: array of device types among the ones defined under the 
[Device types](#device-types) section.

**Return value**

None.

**Example**

See [full example](#full-example)

## network_start

```javascript
String network_start()
```

**Description**

Start the ZigBee network module.

**Parameters**

None.

**Return value**

*String*: Error code.

**Example**

See [full example](#full-example)

## network_form

```javascript
network_form()
```

**Description**

Form a new ZigBee network for other devices to join using default 
parameters.

**Parameters**

None.

**Return value**

None.

**Example**

See [full example](#full-example)

## network_form_manually

```javascript
network_form_manually(Number[] parameters)
```

**Description**

Form a new ZigBee network for other devices to join using specific 
parameters.

**Parameters**

 - *Number[]*: array of parameters, which must be passed in the order detailed
below:

| Index | Parameter |
|:-----:|:----------|
| 0     | Channel   |
| 1     | TX Power  |
| 2     | PAN ID    |

**Return value**

*String*: Error code.

**Example**

```javascript
var params = [ 14, 2, 1200 ];
zigbee.network_form_manually(params);
```

## network_permitjoin

```javascript
String network_permitjoin(Number duration)
```

**Description**

Allow other ZigBee devices to join the formed network for a limited duration.

**Parameters**

 - *Number*: duration in seconds for which remote ZigBee devices are allowed
to join the network.

**Return value**

*String*: Error code.

**Example**

See [full example](#full-example)

## network_leave

```javascript
String network_leave()
```

**Description**

Leave from the currently connected network.

**Parameters**

None.

**Return value**

*String*: Error code.

**Example**

See [full example](#full-example)

## network_join

```javascript
network_join()
```

**Description**

Join a network formed by another ZigBee device.

**Parameters**

None.

**Return value**

None.

**Example**

See [full example](#full-example)

## network_find

```javascript
network_find()
```

**Description**

Find networks formed by other ZigBee device.

**Parameters**

None.

**Return value**

None.

**Example**

See [full example](#full-example)

## network_request_my_network_status

```javascript
String network_request_my_network_status()
```

**Description**

Request current status of the network.

**Parameters**

None.

**Return value**

*String*: Status or error code.

**Example**

See [full example](#full-example)

## device_request_my_node_type

```javascript
String device_request_my_node_type()
```

**Description**

Request current node type of the device.

**Parameters**

None.

**Return value**

*String*: Status or error code.

**Example**

See [full example](#full-example)

## device_find_by_cluster

```javascript
String device_find_by_cluster(Number cluster_id)
```

**Description**

Returns the devices on the network that correspond to a cluster ID.

**Parameters**

 - *Number*: cluster ID to look up.

**Return value**

*String*: JSON formatted string containing the array of found devices.

**Example**

See [full example](#full-example)

## onoff_command

```javascript
String onoff_command(Number[] endpoint, Number onoff)
```

**Description**

Send an On/Off command to a remote device.

**Parameters**

 - *Number[]*: array of integers containing the IDs of the endpoint to target
following the order detailed below:

| Index | Parameter          |
|:-----:|:-------------------|
| 0     | Endpoint ID        |
| 1     | Device ID          |
| 2     | Server Cluster 0   |
| 3     | Server Cluster 1   |
| 4     | Server Cluster 2   |
| 5     | Server Cluster 3   |
| 6     | Server Cluster 4   |
| 7     | Server Cluster 5   |
| 8     | Client Cluster 0   |
| 9     | Client Cluster 1   |
| 10    | Client Cluster 2   |
| 11    | Client Cluster 3   |
| 12    | Client Cluster 4   |
| 13    | Client Cluster 5   | 

- *Number*: On/Off command to send, among the ones defined below:

| Command | Value |
|:--------|:-----:|
| OFF     | 3220  |
| ON      | 3221  |
| TOGGLE  | 3222  |

**Return value**

*String*: Error code.

**Example**

See [full example](#full-example)

## onoff_get_value

```javascript
Number onoff_get_value(Number endpoint_id)
```

**Description**

Get the current On/Off state of a remote endpoint.

**Parameters**

 - *Number*: endpoint ID of the remote device to query On/Off state from.

**Return value**

*Number*: current On/Off state of the endpoint, among the ones defined below:

| State   | Value |
|:--------|:-----:|
| OFF     | 3220  |
| ON      | 3221  |


**Example**

See [full example](#full-example)

# Events

## started

```javascript
zigbee.on('started', function())
```

**Description**

Called after a the ZigBee module was properly loaded. No other functions
from the ZigBee module should be called until this event has been fired.

**Parameters**

None.

**Example**

See [full example](#full-example)

## event

```javascript
zigbee.on('event', function(String))
```

**Description**

Called by the ZigBee module every time an event occurs. It passes a JSON formatted string containing information related to the event that occurred.

**Parameters**

 - *String*: JSON formatted string containing information about the event.

**Example**

See [full example](#full-example)

# Full example

   * See [zigbee-example.js](/examples/zigbee-example.js)
