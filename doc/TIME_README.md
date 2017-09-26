# Time API

## set_time

```javascript
	Number set_time(Date date)
```

**Description**

Set the current system time

**Parameters**

 - *Date*: object containing the date to set

**Return value**

*Number*: Error code

**Example**

```javascript
	var today = new Date(2016, 10, 19); 
	time.set_time(today);
```

## get_time

```javascript
	Date get_time()
```

**Description**

Get the current system time

**Parameters**

None

**Return value**

*Date*: object containing the current date

**Example**

```javascript
	var now = time.get_time();
```

## get_time_str
```javascript
	String get_time_str(String format, Number timezone)
```

**Description**

Get the current system time formatted as a string

**Parameters**

- *String*: format of the string to return. e.g. ```%I:%M:%S-%w-%d/%m/%Y```

Where:

```
	%I: hours for 12h format or %H: hours for 24h format
	%M: minutes
	%S: seconds
	%w: day of the week
	%d: day of the month
	%m: month
	%Y: year
```
 - *Number*: timezone in GMT format

```
	UTC: 0
	GMT1: 1
	GMT2: 2
	GMT3: 3
	GMT4: 4
	GMT5: 5
	GMT6: 6
	GMT7: 7
	GMT8: 8
	GMT9: 9
	GMT10: 10
	GMT11: 11
	GMT12: 12
	
	Negative GMT zones can be used as well. e.g. -1 for -GMT1
```

**Return value**

*String* containing the formatted current time

**Example**

```javascript
	console.log('UTC current time is ' + time.get_time_str('%I:%M:%S-%w-%d/%m/%Y', 0);
```

## get_tick

```javascript
	Number get_tick()
```

**Description**

Get the current system tick

**Parameters**

None

**Return value**

*Number*: current system tick in milliseconds

**Example**

```javascript
	console.log('System tick: ' + get_tick());
```

## create_alarm_second

```javascript
	create_alarm_second(Number timezone, Number seconds, function())
```

**Description**

Set an alarm to set off after a defined number of seconds. When the alarm
sets off, the provided callback function is called.

**Parameters**

 - *Number*: timezone in GMT format

 ```
	UTC: 0
	GMT1: 1
	GMT2: 2
	GMT3: 3
	GMT4: 4
	GMT5: 5
	GMT6: 6
	GMT7: 7
	GMT8: 8
	GMT9: 9
	GMT10: 10
	GMT11: 11
	GMT12: 12
	
	Negative GMT zones can be used as well. e.g. -1 for -GMT1
```

- *Number*: number of seconds starting from current time after which the alarm sets off.
- *function()*: callback function to be called when the alarm sets off. 

**Return value**

None

**Example**

```javascript
	time.create_alarm_second(O, 42, function() {
	    console.log("42 seconds have elapsed since the alarm was set");
	});
```

## create_alarm_date

```javascript
	create_alarm_date(Number timezone, Date date, function())
```

**Description**

Set an alarm to set off at a specific date and time. When the alarm
sets off, the provided callback function is called.

**Parameters**

 - *Number*: timezone in GMT format

 ```
	UTC: 0
	GMT1: 1
	GMT2: 2
	GMT3: 3
	GMT4: 4
	GMT5: 5
	GMT6: 6
	GMT7: 7
	GMT8: 8
	GMT9: 9
	GMT10: 10
	GMT11: 11
	GMT12: 12
	
	Negative GMT zones can be used as well. e.g. -1 for -GMT1
```

- *Date*: date and time at which the alarm sets off.
- *function()*: callback function to be called when the alarm sets off. 

**Return value**

None

**Example**

```javascript
	var alarm_date = new Date("2017-07-04T05:00:00-07:00");
	time.create_alarm_date(-7, alarm_date, function() {
	    console.log("Time to light up the barbecue!");
	});
```

## get_delay

```javascript
	Number get_delay()
```

**Description**

Retrieves the remaining time in seconds to from an alarm object.

**Parameters**

None

**Return value**

*Number*: Number of seconds from now after which the alarm is triggered.


**Example**

```javascript
	var delay = alarm_date.get_delay();
	console.log("It remains " + delay + " before the alarm triggers.");
```

## sync_ntp

```javascript
	Number sync_ntp(String hostname)
```

**Description**

Synchronize current system time with NTP server.

**Parameters**

 - *String*: hostname of the NTP server to synchronize with.

**Return value**

*Number*: Error code

**Example**

```javascript
	time.sync_ntp('0.pool.ntp.org');
```
## convert_timestamp_to_time

```javascript
	Date convert_timestamp_to_time(Number timestamp)
```

**Description**

Convert UNIX timestamp (seconds elapsed since Jan 01 1970 UTC) into time.

**Parameters**

 - *Number*: Timestamp to convert from.

**Return value**

*Date*: Result of the conversion into date.

**Example**

```javascript
	var timestamp = 1500472980;
	var date = time.convert_timestamp_to_time(timestamp);
	console.log("date: " + date); // "date: Wed, 19 Jul 2017 14:03:00 GMT"
```
## convert_time_to_timestamp

```javascript
	Number convert_time_to_timestamp(Date date)
```

**Description**

Convert time into UNIX timestamp (seconds elapsed since Jan 01 1970 UTC).

**Parameters**

 - *Date*: Date to convert from.

**Return value**

*Number*: Result of the conversion into timestamp.

**Example**

```javascript
	var date = new Date(Date.UTC(2017, 6, 19, 14, 03, 00))
	var timestamp = time.convert_time_to_timestamp(date);
	console.log("timestamp: " + timestamp + " s"); // "timestamp: 1500472980 s"
```

# Full example

   * See [time-example.js](/examples/time-example.js)
