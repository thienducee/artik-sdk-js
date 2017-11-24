# Bluetooth A2DP API

## class A2dp
### Constructor
```javascript
var pan = new bluetooth.A2dp();
```

**Description**

Create a new instance of A2dp

**Parameters**

None

**Return Value**

 New Instance

**Example**

```javascript
var a2dp = new bluetooth.A2dp();
```
### source_get_state
```javascript
String source_get_state()
```

**Description**

Get the state of A2DP media streaming transport.

**Parameters**

None

**Return value**

*String*: state of A2DP media transport, possible values :
		 "idle": not streaming
		 "pending": streaming but not acquired
		 "active": streaming and acquired

**Example**
```javascript
var state = a2dp.source_get_state();
console.log("Streaming state is " + state);
```