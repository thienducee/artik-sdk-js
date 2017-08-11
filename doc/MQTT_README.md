# MQTT API

## Constructor

```javascript
var mqtt_client = new mqtt(String client_id,
                           String user_name,
                           String user_pass,
                           Boolean clean_session,
                           Number keep_alive_time,
                           Boolean asynchronous,
                           Object ssl_config,
                           function callbackConnect(String result),
                           function callbackDisconnect(String result),
                           function callbackSubscribe(Integer message_id),
                           function callbackUnsubscribe(Integer message_id),
                           function callbackPublish(Integer message_id),
                           function callbackReceive(Integer message_id,
                                                    String topic,
                                                    Buffer buffer,
                                                    Integer qos,
                                                    Boolean retain));
```

**Description**

Create and configure a new mqtt client object.

**Parameters**

 - *String*: client_id of the mqtt client.
 - *String*: user_name of the mqtt client.
 - *String*: user_pass password corresponding to the user name.
 - *Boolean*: clean_session permits to delete all the message when the client is disconnected.
 - *Number*: keep_alive_time in milliseconds.
 - *Boolean*: asynchronous let the mqtt client to be non-blocking.
 - *Object*: object containing the different parameters as CA certificate, client certificate,
client key, enabling Secure Element and defining the level of verification of the server
certificate. The object must be structured as the following example :

```javascript
var ssl_config = {

	/*
	optional
	Enabling Secure Element
	*/
	use_se: false,

	/*
	optional but could be required for verification
	CA root certificate of the server
	*/
	ca_cert: Buffer.from(""),

	/*
	optional but could be required for verification
	Client certificate
	*/
	client_cert: Buffer.from(""),

	/*
	optional but could be required for verification
	Client private key
	*/
	client_key: Buffer.from(""),

	/*
	optional
	Verification of server certificate

	"none" for no verification,
	"optional" for optional verification,
	"required" for required verification
	*/
	verify_cert: "none"
};
```
 - *function*: callbackConnect call every time the client succeed to connect to the broker.
   For more details see [on_connect](#on_connect).
 - *function*: callbackDisconnect call every time the client succeed to disconnect from the broker.
   For more details see [on_disconnect](#on_disconnect).
 - *function*: callbackSubscribe call every time the client succeed to subscribe to a topic.
   For more details see [on_subscribe](#on_subcribe).
 - *function*: callbackSubscribe call every time the client succeed to unsubscribe from a topic.
   For more details see [on_unsubscribe](#on_unsubcribe).
 - *function*: callbackPublish call every time the client succeed to publish a message.
   For more details see [on_publish](#on_publish).
 - *function*: callbackReceive call every time the client receives a message.
   For more details see [on_receive](#on_receive).

**Return value**

New instance.

**Example**

See [full example](#full-example)

## set_willmsg

```javascript
Number set_willmsg(String willtopic, Buffer willmessage, Number qos, Boolean retain)
```

**Description**

Set the WILLTOPIC and WILLMSG for connection.

**Parameters**

 - *String*: willtopic of the mqtt client to target.
 - *Buffer*: willmessage to publish on the topic.
 - *Number*: qos is the quality of service (0, 1 or 2 are the indicators of quality).
 - *Boolean*: retain permits to tell to the broker to keep the last message.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## free_willmsg

```javascript
Number free_willmsg()
```

**Description**

Free the will message memory which is allocated by [set_willmsg](#set_willmsg).

**Parameters**

None

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## clear_willmsg

```javascript
Number clear_willmsg()
```

**Description**

Remove a previously configured will.
This must be called before calling [connect](#connect).

**Parameters**

None

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## connect

```javascript
Number connect(String host, String port, function callback(String result))
```

**Description**

Connect to an MQTT broker. This is a non-blocking call.

**Parameters**

 - *String*: host of the remote broker.
 - *Integer*: port of the remote broker.
 - *function*: callback is call every time the client succeed to connect to the remote broker.
   For more details see [on_connect](#on_connect).

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## disconnect

```javascript
Number disconnect(function callback(String result))
```

**Description**

Disconnect from the broker.

**Parameters**

 - *function*: callback is call every time the client is disconnecting from the broker.
   For more details see [on_disconnect](#on_disconnect).

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## subscribe

```javascript
Number subscribe(Integer qos, String topic,
       			 function callbackSubscribe(Integer mid),
			     function callbackReceive(Integer mid, String topic, Buffer message,
			    	     			      Integer qos, Boolean retain))
```

**Description**

Subscribe to a topic.

**Parameters**

 - *Integer*: qos is the quality of service (0, 1 or 2 are the indicators of quality).
 - *String*: topic to wich we will publish the message.
 - *function*: callbackSubscribe is call every time the client succeed to register to a topic.
   For more details see [on_subscribe](#on_subscribe).
 - *function*: callbackReceive is call every time the client received a message from the broker.
   For more details see [on_receive](#on_receive).

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## unsubscribe

```javascript
Number unsubscribe(String topic, function callback(Integer mid))
```

**Description**

Unsubscribe from a topic.

**Parameters**

 - *String*: topic to wich we will publish the message.
 - *function*: callback is call every time the client succeed to unsubscribe from a topic.
   For more details see [on_unsubscribe](#on_unsubscribe).

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## publish

```javascript
Number publish(Integer qos, Boolean retain, String topic, Buffer message,
       		   function callback(Integer mid))
```

**Description**

Publish a message on a given topic.

**Parameters**

 - *Integer*: qos is the quality of service (0, 1 or 2 are the indicators of quality).
 - *Boolean*: retain permits to tell to the broker to keep the last message.
 - *String*: topic to wich we will publish the message.
 - *Buffer*: message to publish on the topic.
 - *function*: callback is call every time the client succeed to send a message.
   For more details see [on_publish](#on_publish).

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

# Events

## connected

```javascript
mqtt_client.on('connected', function (String result))
```

**Description**

Called every time when the mqtt client succeed to connect.

**Parameters**

 - *String*: result is the state of the connection

**Example**

See [full example](#full-example)

## disconnected

```javascript
mqtt_client.on('disconnected', function (String result))
```

**Description**

Called every time when the mqtt client is disconnected.

**Parameters**

 - *String*: result is the state of the disconnection

**Example**

See [full example](#full-example)

## subscribed

```javascript
mqtt_client.on('subscribed', function (Integer message_id))
```

**Description**

Called every time when the mqtt client succeed to subscribe to a topic.

**Parameters**

 - *Integer*: message_id of the topic from which the client will subcribe.

**Example**

See [full example](#full-example)

## unsubscribed

```javascript
mqtt_client.on('unsubscribed', function (Integer message_id))
```

**Description**

Called every time when the mqtt client is unsubscribed from a topic.

**Parameters**

 - *Integer*: message_id of the topic from which the client was subcribed.

**Example**

See [full example](#full-example)

## published

```javascript
mqtt_client.on('published', function (Integer message_id))
```

**Description**

Called every time when the mqtt client succeed to publish a message on a topic.

**Parameters**

 - *Integer*: message_id of the message published.

**Example**

See [full example](#full-example)

## received

```javascript
mqtt_client.on('received', function (Integer message_id, String topic, Buffer buffer,
									 Integer qos, Boolean retain))
```

**Description**

Called every time when the mqtt client receives a message.

**Parameters**

 - *Integer*: message_id of the message to send.
 - *String*: topic to wich we will publish the message.
 - *Buffer*: message to publish on the topic.
 - *Integer*: qos is the quality of service (0, 1 or 2 are the indicators of quality).
 - *Boolean*: retain permits to tell to the broker to keep the last message.

**Example**

See [full example](#full-example)

# Full example

   * See [mqtt-example.js](/examples/mqtt-example.js)
