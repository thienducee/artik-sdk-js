# Sensor API
## Sensor types
| Type           | Value |
|:---------------|:-----:|
| Accelerometer  | 1     |
| Humidity       | 2     |
| Light          | 3     |
| Temperature    | 4     |
| Proximity      | 5     |
| Flame          | 6     |

## list

```javascript
SensorDevice[] list()
```

**Description**

Return an array containing all the sensor devices supported on the board.

**Parameters**

None.

**Return value**

Array of *SensorDevice* objects.

**Example**

```javascript
var list = sensors.list();
list.forEach(function(sensor){
	console.log('Name: ' + sensor.get_name());
});
```

## get_sensor

```javascript
SensorDevice get_sensor(Number index, Number type)
```

**Description**

Return a specific sensor based on its type and index.

**Parameters**

 - *Number*: index of the sensor to get among one given type of sensors. e.g. If only one humidity sensor is available on the board, index should be 0.
 - *Number*: type of the sensor to get, among the ones defined under [sensor types](#sensor-types).

**Return value**

*SensorDevice* object.

**Example**

```javascript
var temperature = sensors.get_sensor(0, 4);
```

## get_accelerometer_sensor

```javascript
AccelerometerSensor get_accelerometer_sensor(Number index)
```

**Description**

Return an accelerometer sensor based on its index.

**Parameters**

 - *Number*: index of the sensor to get among all the available accelerometer sensors. If only one accelerometer sensor is available on the board, index should be 0.

**Return value**

*AccelerometerSensor* object.

**Example**

```javascript
var accelerometer = sensors.get_accelerometer_sensor(0);
```

## get_humidity_sensor

```javascript
HumiditySensor get_humidity_sensor(Number index)
```

**Description**

Return a humidity sensor based on its index.

**Parameters**

 - *Number*: index of the sensor to get among all the available humidity sensors. If only one humidity sensor is available on the board, index should be 0.

**Return value**

*HumiditySensor* object.

**Example**

```javascript
var humidity = sensors.get_humidity_sensor(0);
```

## get_light_sensor

```javascript
LightSensor get_light_sensor(Number index)
```

**Description**

Return a light sensor based on its index.

**Parameters**

 - *Number*: index of the sensor to get among all the available light sensors. If only one light sensor is available on the board, index should be 0.

**Return value**

*LightSensor* object.

**Example**

```javascript
var light = sensors.get_light_sensor(0);
```

## get_temperature_sensor

```javascript
TemperatureSensor get_temperature_sensor(Number index)
```

**Description**

Return a temperature sensor based on its index.

**Parameters**

 - *Number*: index of the sensor to get among all the available temperature sensors. If only one temperature sensor is available on the board, index should be 0.

**Return value**

*TemperatureSensor* object.

**Example**

```javascript
var temperature = sensors.get_temperature_sensor(0);
```

## get_proximity_sensor

```javascript
ProximitySensor get_proximity_sensor(Number index)
```

**Description**

Return a proximity sensor based on its index.

**Parameters**

 - *Number*: index of the sensor to get among all the available proximity sensors. If only one proximity sensor is available on the board, index should be 0.

**Return value**

*ProximitySensor* object.

**Example**

```javascript
var proximity = sensors.get_proximity_sensor(0);
```

## get_flame_sensor

```javascript
FlameSensor get_flame_sensor(Number index)
```

**Description**

Return a flame sensor based on its index.

**Parameters**

 - *Number*: index of the sensor to get among all the available flame sensors. If only one flame sensor is available on the board, index should be 0.

**Return value**

*FlameSensor* object.

**Example**

```javascript
var flame = sensors.get_flame_sensor(0);
```

# SensorDevice API
## get_type

```javascript
Number get_type()
```

**Description**

Return the type of the current sensor object.

**Parameters**

None.

**Return value**

*Number*: type of the sensor, among the ones defined under [sensor types](#sensor-types).

**Example**

See [full example](#full-example).

## get_index

```javascript
Number get_index()
```

**Description**

Return the index of the current sensor object.

**Parameters**

None.

**Return value**

*Number*: index of the sensor among the list of sensors of the same type.

**Example**

See [full example](#full-example).

## get_name

```javascript
String get_name()
```

**Description**

Return the name of the current sensor object.

**Parameters**

None.

**Return value**

*String*: name of the sensor.

**Example**

See [full example](#full-example).

# AccelerometerSensor API
## get_speed_x

```javascript
Number get_speed_x()
```

**Description**

Return the acceleration along the X axis.

**Parameters**

None.

**Return value**

*Number*: acceleration along the X axis.

**Example**

See [full example](#full-example).

## get_speed_y

```javascript
Number get_speed_y()
```

**Description**

Return the acceleration along the Y axis.

**Parameters**

None.

**Return value**

*Number*: acceleration along the Y axis.

**Example**

See [full example](#full-example).

## get_speed_z

```javascript
Number get_speed_z()
```

**Description**

Return the acceleration along the Z axis.

**Parameters**

None.

**Return value**

*Number*: acceleration along the Z axis.

**Example**

See [full example](#full-example).

# HumiditySensor API
## get_humidity

```javascript
Number get_humidity()
```

**Description**

Return the measured humidity level.

**Parameters**

None.

**Return value**

*Number*: measured humidity level.

**Example**

See [full example](#full-example).

# LightSensor API
## get_intensity

```javascript
Number get_intensity()
```

**Description**

Return the measured light intensity.

**Parameters**

None.

**Return value**

*Number*: measured light intensity.

**Example**

See [full example](#full-example).

# TemperatureSensor API
## get_celsius

```javascript
Number get_celsius()
```

**Description**

Return the measured temperature in celsius degrees.

**Parameters**

None.

**Return value**

*Number*: measured temperature in celsius degrees.

**Example**

See [full example](#full-example).

## get_fahrenheit

```javascript
Number get_fahrenheit()
```

**Description**

Return the measured temperature in Fahrenheit degrees.

**Parameters**

None.

**Return value**

*Number*: measured temperature in Fahrenheit degrees.

**Example**

See [full example](#full-example).

# ProximitySensor API
## get_presence

```javascript
Number get_presence()
```

**Description**

Return the presence status of a nearby obstacle.

**Parameters**

None.

**Return value**

*Number*: **1** if an obstacle is detected nearby the sensor, **0** otherwise.

**Example**

See [full example](#full-example).

# FlameSensor API
## get_signals

```javascript
Number get_signals()
```

**Description**

Return the status of flame detection.

**Parameters**

None.

**Return value**

*Number*: **1** if a flame is detected, **0** otherwise.

**Example**

See [full example](#full-example).

# Full example

   * See [sensor-example.js](/examples/sensor-example.js)
