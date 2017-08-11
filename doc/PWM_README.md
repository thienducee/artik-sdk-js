# PWM API

## Constructor

```javascript
var signal = new pwm(Number pin, String name, Number period, Number polarity, Number duty_cycle);
```

**Description**

Create and configure a new instance of a PWM signal.

**Parameters**

 - *Number*: PWM pin number. This value is defined in the board-specific
definition files.
 - *String*: friendly name to store for the PWM signal.
 - *Number*: period of the PWM signal in nanoseconds.
 - *Number*: polarity of the signal. Must be **0** (normal) or **1** (inverted).
 - *Number*: active time of the signal in nanoseconds over the full period.
**Return value**

New instance.

**Example**

```javascript
var signal = new pwm(1, "square", 10000, 0, 5000);
```

## request

```javascript
Number request()
```

**Description**

Request the PWM pin tied to the PWM instance. If this one has
already been requested by some other program, the function will return
an error. When the calling program is no longer using the PWM signal,
it should release it by calling the *release* function.

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

Release a PWM signal after being previously reserved by the *request*
function.

**Parameters**

None

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## enable

```javascript
Number enable()
```

**Description**

Enable the PWM output.

**Parameters**

None

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## disable

```javascript
Number disable()
```

**Description**

Disable the PWM output.

**Parameters**

None

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## set_pin_num

```javascript
Number set_pin_num(Number pin)
```

**Description**

Change the pin number of the PWM signal

**Parameters**

 - *Number*: pin number of the PWM signal.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## set_name

```javascript
Number set_name(String name)
```

**Description**

Change the friendly name of the PWM signal.

**Parameters**

 - *String*: friendly name of the PWM signal.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## set_period

```javascript
Number set_period(Number period)
```

**Description**

Change the period of the PWM signal

**Parameters**

 - *Number*: period of the PWM signal in nanoseconds.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## set_duty_cycle

```javascript
Number set_duty_cycle(Number duty_cycle)
```

**Description**

Change the duty cycle of the PWM signal

**Parameters**

 - *Number*: active time of the signal in nanoseconds over the full period.

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## set_polarity

```javascript
Number set_polarity(Number polarity)
```

**Description**

Change the polarity of the PWM signal

**Parameters**

 - *Number*: polarity of the signal. Must be **0** (normal) or **1** (inverted).

**Return value**

*Number*: Error code

**Example**

See [full example](#full-example)

## get_pin_num

```javascript
Number get_pin_num()
```

**Description**

Get the pin number of the PWM signal.

**Parameters**

None.

**Return value**

 - *Number*: PWM pin number.

**Example**

See [full example](#full-example)

## get_name

```javascript
String get_name()
```

**Description**

Get the friendly name of the PWM signal.

**Parameters**

None.

**Return value**

 - *String*: friendly name of the PWM signal.

**Example**

See [full example](#full-example)

## get_period

```javascript
Number get_period()
```

**Description**

Get the period of the PWM signal.

**Parameters**

None.

**Return value**

 - *Number*: period in nanoseconds of the PWM signal.

**Example**

See [full example](#full-example)

## get_duty_cycle

```javascript
Number get_duty_cycle()
```

**Description**

Get the duty cycle of the PWM signal.

**Parameters**

None.

**Return value**

 - *Number*: active time in nanosecond of the PWM signal over the full period.

**Example**

See [full example](#full-example)

## get_polarity

```javascript
Number get_polarity()
```

**Description**

Get the polarity of the PWM signal.

**Parameters**

None.

**Return value**

 - *Number*: polarity of the PWM signal. Can be **0** (normal) or **1** (inverted).

**Example**

See [full example](#full-example)

# Full example

   * See [pwm-example.js](/examples/pwm-example.js)
