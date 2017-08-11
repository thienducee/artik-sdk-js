# GPIO API 

## Constructor

```javascript
var io = new gpio(Number gpio_id, String name, String direction, String edge, Number init_value);
```

**Description**

Create and configure a new instance of the GPIO tied to a specific hardware pin.

**Parameters**

 - *Number*: ID of the GPIO to configure. This value is defined in the board-specific
definition files.
 - *String*: friendly name to attach to the GPIO.
 - *String*: direction to set for the GPIO. Must be **out** or **in**.
 - *String*: edge on which interrupts should be triggered. Must be **rising**, **falling**, **both**, or
**none**. This parameter is ignored when the GPIO is configured as an output.
 - *Number*: initial value to set after configuration when the GPIO is set as an output. Must be **0** or **1**. This parameter is ignored when the GPIO is configured as an input.

**Return value**

New instance.

**Example**

```javascript
var led = new gpio(1, 'Green LED', 'out', 'none', 0);
var button = new gpio(2, 'User Button', 'in', 'rising', 0);
```

## request

```javascript
Number request()
```

**Description**

Request the GPIO pin tied to the GPIO instance. If this one has
already been requested by some other program, the function will return
an error. When the calling program is no longer using the GPIO, it should
release it by calling the *release* function.

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## release

```javascript
Number release()
```

**Description**

Release a GPIO pin after being previously reserved by the *request*
function.

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## read

```javascript
Number read()
```

**Description**

Read the current state of the GPIO when this one is configured as an input.

**Parameters**

None.

**Return value**

*Number*: current state of the pin. Can be **0** (inactive) or **1** (active).

**Example**

See [full example](#full-example)

## write

```javascript
Number write(Number state)
```

**Description**

Set the state of the GPIO when this one is configured as an output.

**Parameters**

 - *Number*: state to set to the output. Must be **0** (inactive) or **1** (active).

**Return value**

*Number*: Error code.

**Example**

See [full example](#full-example)

## get_name

```javascript
String get_name()
```

**Description**

Get the friendly name of the GPIO instance.

**Parameters**

None.

**Return value**

*String*: friendly name of the GPIO.

**Example**

```javascript
console.log('Name: ' + led.get_name());
```

## get_direction

```javascript
String get_direction()
```

**Description**

Get the direction of the GPIO instance.

**Parameters**

None.

**Return value**

*String*: direction of the GPIO, can be **out** or **in**.

**Example**

```javascript
console.log('Direction: ' + led.get_direction());
```

## get_id

```javascript
String get_id()
```

**Description**

Get the ID of the GPIO instance.

**Parameters**

None.

**Return value**

*Number*: ID of the GPIO pin.

**Example**

```javascript
console.log('ID: ' + led.get_id());
```

# Full example

   * See [gpio-example.js](/examples/gpio-example.js)
