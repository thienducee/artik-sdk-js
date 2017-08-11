# I2C API

## Constructor

```javascript
var chip = new i2c(Number id, Number frequency, String word_size, Number address);
```

**Description**

Create and configure a new instance of an I2C chip.

**Parameters**

 - *Number*: ID of the I2C bus to target. This value is defined in the board-specific
definition files.
 - *Number*: maximum frequency to which the bus should operate when performing a transaction.
 - *String*: size of a word (in bits) to be transmitted over the I2C bus. Must be **8** or **16**.
 - *Number*: address of the I2C chip to target.

**Return value**

New instance.

**Example**

See [Full example](#full-example)

## request

```javascript
Number request()
```

**Description**

Request the I2C bus/address pair tied to the I2C instance. If this one has
already been requested by some other program, the function will return
an error. When the calling program is no longer using the I2C chip, it should
release it by calling the *release* function.

**Parameters**

None.

**Return value**

*Number*: Error code.

**Example**

See [Full example](#full-example)

## release

```javascript
Number release()
```

**Description**

Release a I2C bus/address pair after being previously reserved by the *request*
function.

**Parameters**

None.

**Return value**

*Number*: Error code

**Example**

See [Full example](#full-example)

## read

```javascript
Buffer read(Number length)
```

**Description**

Perform a read transaction over the I2C bus.

**Parameters**

 - *Number*: length in bytes of the data to read from the bus.

**Return value**

*Buffer*: buffer containing the data read from the bus.

**Example**

See [Full example](#full-example)

## write

```javascript
Number write(Buffer data)
```

**Description**

Perform a write transaction over the I2C bus.

**Parameters**

 - *Buffer*: buffer containing the data to write to the I2C bus.

**Return value**

*Number*: Error code.

**Example**

See [Full example](#full-example)

## read_register

```javascript
Buffer read_register(Number address, Number length)
```

**Description**

Read a register from a remote I2C chip.

**Parameters**

 - *Number*: subaddress of the register to read from the remote chip.
 - *Number*: length in bytes of the data to read from the register.

**Return value**

*Buffer*: buffer containing the data read from the register.

**Example**

See [Full example](#full-example)

## write_register

```javascript
Number write_register(Number address, Buffer data)
```

**Description**

Write to a register on a remote I2C chip.

**Parameters**

 - *Number*: subaddress of the register to write on the remote chip.
 - *Buffer*: buffer containing the date to write to the register.

**Return value**

*Number*: Error code.

**Example**

See [Full example](#full-example)

# Full example

   * See [i2c-example.js](/examples/i2c-example.js)
