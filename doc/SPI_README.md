# SPI API

## Constructor

```javascript
var chip = new spi(Number bus, Number chipselect, Number mode, Number bpw, Number speed);
```

**Description**

Create and configure a new instance of an SPI chip.

**Parameters**

 - *Number*: controller ID of the SPI bus to use. This value is defined in the board-specific definition files.
 - *Number*: chipselect pin connected to the SPI chip. This value is defined in the board-specific definition files.
 - *Number*: SPI mode. Must be **0**, **1**, **2**, or **3**.
 - *Number*: number of bits per word.
 - *Number*: maximum speed in Hz to use on the SPI bus.

**Return value**

New instance.

**Example**

See [Full example](#full-example)

## request

```javascript
Number request()
```

**Description**

Request the SPI bus/chipselect pair tied to the SPI instance. If this one has
already been requested by some other program, the function will return
an error. When the calling program is no longer using the SPI chip, it should
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

Release a SPI bus/chipselect pair after being previously reserved by the *request* function.

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

Read data from the SPI bus.

**Parameters**

 - *Number*: number of words to read from the bus.

**Return value**

*Buffer*: buffer containing the read data.

**Example**

See [Full example](#full-example)

## write

```javascript
Number write(Buffer data)
```

**Description**

Write data to the SPI bus.

**Parameters**

 - *Buffer*: buffer containing the data to send.

**Return value**

*Number*: Error code.

**Example**

See [Full example](#full-example)

## read_write

```javascript
Buffer read_write(Buffer data)
```

**Description**

Read and write data simultaneously on the SPI bus.

**Parameters**

 - *Buffer*: buffer containing the data to send.

**Return value**

*Buffer*: buffer containing the read data.

**Example**

See [Full example](#full-example)

# Full example

   * See [spi-example.js](/examples/spi-example.js)
