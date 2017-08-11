# ADC API

## Constructor

```javascript
var analog = new adc(Number pin, String name);
```

**Description**

Create a new instance of the ADC tied to a specific analog pin with
custom friendly name.

**Parameters**

 - *Number*: number of the analog input pin. This value is defined in the board-specific
definition files.
 - *String*: friendly name to attach to the analog input.

**Return value**

New instance.

**Example**

```javascript
var temperature = new adc(1, 'Temperature Sensor');
```

## request

```javascript
Number request()
```

**Description**

Request the ADC analog pin tied to the ADC instance. If this one has
already been requested by some other program, the function will return
an error. When the calling program is no longer reading from the analog
input, it should release it by calling the *release* function.

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

Release an ADC analog pin after being previously reserved by the *request*
function.

**Parameters**

None

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## get_value

```javascript
Number get_value()
```

**Description**

Read the analog value converted by the ADC on the analog pin.

**Parameters**

None

**Return value**

*Number*: analog value read by the ADC. If an error occurs during the
conversion process, **-1** is returned instead.

**Example**

See [full example](#full-example)

## get_pin_num

```javascript
Number get_pin_num();
```

**Description**

Return the analog pin number of the ADC instance.

**Parameters**

None

**Return value**

*Number*: analog pin number

**Example**

```javascript
console.log('Analog pin number: ' + temperature.get_pin_num());
```

## get_name

```javascript
String get_name()
```

**Description**

Return the friendly name of the ADC instance.

**Parameters**

None

**Return value**

*String*: friendly name

**Example**

```javascript
console.log('Analog pin name: ' + temperature.get_name());
```

## set_pin_num

```javascript
set_pin_num(Number pin)
```

**Description**

Change the analog pin number of the ADC instance.

**Parameters**

 - *Number*: analog pin number to set

**Return value**

None

**Example**

```javascript
temperature.set_pin_num(3);
```

## set_name

```javascript
set_name(String name)
```

**Description**

Change the friendly name of the ADC instance.

**Parameters**

 - *String*: friendly name to set

**Return value**

None

**Example**

```javascript
temperature.set_name('Humidity sensor');
```

# Full Example

```javascript
var temperature = new adc(1, 'Temperature Sensor');

if (temperature.request()) {
	console.log('Failed to request temperature sensor analog pin');
} else {
	console.log('Temperature: ' + temperature.get_value());
	temperature.release();
}
```

   * See [adc-example.js](/examples/adc-example.js)

