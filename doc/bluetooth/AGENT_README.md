# Bluetooth Agent API
## class AgentRequest
### send_pincode
```javascript
send_pincode(String pincode);
```

**Description**

Send the pincode.

**Parameters**

*String*: The pincode.

**Return value**

None

**Example**
```javascript
request.send_pincode("012345");
```

### send_passkey
```javascript
send_passkey(Number passkey);
```

**Description**

Send the passkey.

**Parameters**

*Number*: The passkey

**Return value**

None

**Example**
```javascript
request.send_passkey(12345);
```

### send_error
```javascript
send_error(String error_type, String msg);
```

**Description**

Send an error

**Parameters**

*String*: Error type ('rejected' or 'canceled')
*String*: Error message.

**Return value**

None

**Example**
```javascript
request.send_error("", "Error: Wrong pincode");
```

### send_empty_response
```javascript
send_empty_response();
```

**Description**

Send an empty response.

**Parameters**

None

**Return value**

None

**Example**
```javascript
request.send_empty_response();
```

## class Agent
### Capability type

|           Capability            |        Value       |
|:-------------------------------:|:------------------:|
| Display and write text       | keyboard_display   |
| Only display text               | display_only       |
| Display and confirm with yes/no | display_yesno      |
| Only write text                 | keyboard_only      |
| No input and output capability  | no_input_no_output |

### Constructor
```javascript
var agent = new bluetooth.Agent();
```

**Description**

Create a new instance of Agent

**Parameters**

None

**Return Value**

 New Instance

**Example**

```javascript
var agent = new bluetooth.Agent();
```

### register_capability
```javascript
register_capability(String capability)
```

**Description**

Register an agent.

**Parameters**

*String*: The capability among the ones defined under [Capability type](#capability-type)

**Return Value**

None

**Example**
```javascript
agent.register_capability("display_yesno");
```

### set_default
```javascript
set_default()
```

**Description**

Set the registered agent as the default agent.

**Parameters**

None

**Return Value**

None

**Example**
```javascript
agent.set_default()
```

### unregister
```javascript
unregister()
```

**Description**

Unregister the agent.

**Parameters**

None

**Return Value**

None

**Example**
```javascript
agent.unregister()
```

### Events
#### release
```javascript
agent.on('release', function());
```

**Description**

Called when the agent is unregistred.

**Parameters**

None

**Example**
```javascript
agent.on('release), function() { console.log("Agent is unregistered"); });
```

#### request_pincode
```javascript
agent.on('request_pincode', function(AgentRequest request, String device));
```

**Description**

Called every time the pincode is needed for an authentication.

The pincode or an error should be returned with *request.send_pincode* or *request.send_error*

**Parameters**

*AgentRequest*: The agent request.
*String*: The address of the remote device.

**Example**
```javascript
agent.on('request_pincode', function(request, device) {
  console.log("Request pincode from " + device);
  request.send_pincode("12345");
});
```

#### display_pincode
```javascript
agent.on('display_pincode', function(String device, String pincode));
```

**Description**

Called every time the agent needs to display the pincode.

**Parameters**

*String*: The address of the remote device.
*String*: The pincode to display.

**Example**
```javascript
agent.on('display_pincode', function(device, pincode) {
  console.log("Display pincode "+ pincode + " for device " + device);
});
```

#### request_passkey
```javascript
agent.on('request_passkey', function(AgentRequest request, String device));
```

**Description**

Called every time the passkey is needed for an authentication.

The passkey or an error should be returned with *request.send_passkey* or *request.send_error*.

**Parameters**

*AgentRequest*: The agent request.
*String*: The address of the remote device.

**Example**
```javascript
agent.on('request_passkey', function(request, device) {
  console.log("Request passkey from " + device);
  request.send_passkey(12345);
});
```

#### display_passkey
```javascript
agent.on('display_passkey', function(String device, Number passkey, Number entered));
```

**Description**

Called every time the agent needs to display the passkey.

**Parameters**

*String*: The address of the remote device.
*Number*: The passkey to display.
*Number*: Indicate the number of already typed keys on the remote side.

**Example**
```javascript
agent.on('display_passkey', function(device, passkey, entered) {
  console.log("Display passkey "+ passkey + " for device " + device + "(entered " + entered + )");
});
```

#### confirmation
```javascript
agent.on('confirmation', function(AgentRequest request, String device, Number passkey));
```

**Description**

Called every time the agent needs to confirm a passkey for an authentication.

To confirm the passkey use *request.send_empty_response* or returns an error with *request.send_error*.

**Parameters**

*AgentRequest*: The agent request.
*String*: The address of the remote device.
*Number*: The passkey to confirm.

**Example**
```javascript
agent.on('confirmation', function(request, device, passkey) {
  console.log('Confirm passkey ' + passkey);
  request.send_empty_response();
});
```

#### authorization
```javascript
agent.on('authorization', function(AgentRequest request, String device));
```

**Description**

Called every time the agent needs to authorize an incoming pairing.

To authorize the request use *request.send_empty_response* or returns an error with *request.send_error*

**Parameters**

*AgentRequest*: The agent request.
*String*: The address of the remote device.

**Example**
```javascript
agent.on('authorization', function(request, device) {
  console.log("Request pairing from " + device);
  request.send_empty_response();
});
```

#### authorize_service
```javascript
agent.on('authorize_service', function(AgentRequest request, String device, String uuid));
```

**Description**

Called every time the agent needs to authorize a connection to a service.

To authorize the request use *request.send_empty_response* or returns an error with *request.send_error*

**Parameters**

*AgentRequest*: The agent request.
*String*: The address of the remote device.
*String*: The uuid of the service.

**Example**
```javascript
agent.on('authorize_service', function(request, device, uuid) {
  console.log("Request connection from " + device + " to service " + uuid);
  request.send_empty_response();
});
```

#### cancel
```javascript
agent.on('cancel', function());
```

**Description**

Called every time the agent needs to indicate that a request failed before the agent returned a reply.

**Parameters**

None

**Example**
```javascript
agent.on('cancel', function(){ console.log("Request canceled."); });
```

## Full example

  * See [bluetooth-agent-example.js](/examples/bluetooth-agent-example.js)
