# Wi-Fi API

## scan_request

```javascript
String scan_request()
```

**Description**

Start a scan of the surrounding Wi-Fi access points. The application
should catch the **scan** event to get the results.

**Parameters**

None.

**Return value**

*String*: Error message.

**Example**

See [full example](#full-example)

## get_scan_result

```javascript
String get_scan_result()
```

**Description**

Return the list of all the access points that were previously discovered during
the scan process.

**Parameters**

None.

**Return value**

*String*: JSON formatted string containing the surrounding Wi-Fi access points.

**Example**

See [full example](#full-example)

## connect

```javascript
String connect(String ssid, String passphrase, Boolean persistent)
```

**Description**

Connect to a specific access point. Calling application should catch the 
**connected** event to be notified of the success of the connection.

**Parameters**

 - *String*: SSID of the access point to connect to.
 - *String*: secure passphrase of the access point. Ignored if no security is
requested by the acces point.
 - *Boolean*: if **true** store access point configuration to be persistent
across reboot.

**Return value**

*String*: Error message.

**Example**

See [full example](#full-example)

## disconnect

```javascript
String disconnect()
```

**Description**

Disconnect from the current access point.

**Parameters**

None.

**Return value**

*String*: Error message.

**Example**

See [full example](#full-example)

## start_ap

```javascript
Number start_ap(String SSID, String passphrase, Number channel, Number encryption)
```

**Description**

Configure and start a Wi-Fi Access Point.

**Parameters**

 - *String*: SSID of the access point to connect to.
 - *String*: secure passphrase of the access point. Ignored if
open mode is selected.
 - *Number*: channel number for the access point.
 - *Number*: encryption mode among the following ones

| Encryption mode  | Value                |
|:-----------------|:--------------------:|
| Open             | WIFI_ENCRYPTION_OPEN |
| WEP              | WIFI_ENCRYPTION_WEP  |
| WPA              | WIFI_ENCRYPTION_WPA  |
| WPA2             | WIFI_ENCRYPTION_WPA2 |

**Return value**

*String*: Error message.

**Example**

See [full example](#full-example)

#Events

## started

```javascript
wifi.on('started', function())
```
**Description**

Called after the Wi-Fi subsystem has been properly initialized.

**Parameters**

None.

**Example**

See [full example](#full-example)

## scan

```javascript
wifi.on('scan', function(String list))
```
**Description**

Return the scan results in a JSON formatted string.

**Parameters**

 - *String*: JSON formatted string containing the surrounding Wi-Fi access points.

**Example**

See [full example](#full-example)

## connected

```javascript
wifi.on('connected', function())
```
**Description**

Called after successful connection to an access point.

**Parameters**

None.

**Example**

See [full example](#full-example)

# Full example

```javascript
var artik = require('artik-sdk');
var wifi = new artik.wifi();

var ssid = '<enter a SSID here>';
var pwd = '<passphrase of the SSID>';

wifi.on('started', function() {
	wifi.scan_request();
});

wifi.on('connected', function() {
	console.log('connected');
	process.exit(0);
});

wifi.on('scan', function(list) {
	var results = JSON.parse(list);
	console.log(results);
	var ap = results.filter(function(item) {
		return item.name == ssid;
	});

	if (ap.length > 0) {
		console.log('Found SSID ' + ssid + ', connecting...');
		wifi.disconnect();
		wifi.connect(ssid, pwd, false);
	}
});

process.on('SIGINT', function () {
	process.exit(0);
});
```

   * See [wifi-example.js](/examples/wifi-example.js)
   * See [wifi-ap-example.js](/examples/wifi-ap-example.js)
