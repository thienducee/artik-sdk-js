# Bluetooth FTP API
## class FtpProperty
### Attributes
#### name
```javascript
String name
```

**Description**
Name of the transfered object.

#### filename
```javascript
String filename
```

**Description**
Complete name of the file being received or sent.

#### status
```javascript
String status;
```

**Description**
The current status of the transfer.

Can take one of the following value "queued", "active", "suspended", "complete" or "error"

#### transfered
```javascript
Number transfered;
```

**Description**
Number of bytes transferred.

#### size
```javascript
Number size;
```

**Description**
Size of the transferred file.

## class Ftp
### Constructor
```javascript
var ftp = new bluetooth.Ftp();
```

**Description**

Create a new instance of FTP

**Parameters**

None

**Return Value**

 New Instance

**Example**

```javascript
var bt = new bluetooth.Ftp();
```

### create_session
```javascript
create_session(String addr);
```

**Description**

Create FTP session with FTP server

**Parameters**

*String*: The bluetooth address of FTP server.

**Return value**

None

**Example**
```javascript
ftp.create_session("01:02:03:04:05:06");
```

### remove_session
```javascript
remove_session();
```

**Description**

Remove FTP session

**Parameters**

None

**Return value**

None

**Example**
```javascript
ftp.remove_session();
```

### change_folder
```javascript
change_folder(String folder)
```

**Description**

Change the working directory

**Parameters**

*String*: The new working directory

**Return value**

None

**Example**
```javascript
ftp.change_folder("/myfolder");
```

### create_folder
```javascript
create_folder(String folder)
```

**Description**

Create new sub-directory on the FTP server.

**Parameters**

*String*: The new sub-directory

**Return value**

None

**Example**
```javascript
ftp.create_folder("/mynewfolder");
```

### delete_file
```javascript
delete_file(String file)
```

**Description**

Delete the file on the FTP server.

**Parameters**

*String*: The file to be deleted.

**Return value**

None

**Example**
```javascript
ftp.delete_file("myfile.txt");
```

### list_folder
```javascript
FileItem[] list_folder();
```

**Description**

List contents of the current directory.

**Parameters**

None

**Return value**

*FileItem[]*: Array containing information about all the files and directories in the current directory.

**Example**
```javascript
var files = ftp.list_folder();
```

### get_file
```javascript
get_file(String target_file, String source_file);
```

**Description**

Download a file from the FTP server.

**Parameters**

*String*: The target file (on local filesystem)
*String*: The source file (from remote device)

**Return value**

None

**Example**
```javascript
ftp.get_file("/tmp/test.txt", "/test.txt");
```

### put_file
```javascript
put_file(String source_file, String target_file);
```

**Description**

Upload a file to the FTP server.

**Parameters**

*String*: The source file (from local filesystem)
*String*: The target file (on remote device)

**Return value**

None

**Example**
```javascript
ftp.put_file("/root/file.txt", "/file.txt");
```

### cancel_transfer
```javascript
cancel_transfer()
```

**Description**

Stop the current transference.

**Parameters**

None

**Return value**

None

**Example**
```javascript
ftp.cancel_transfer();
```

### resume_transfer
```javascript
resume_transfer();
```

**Description**

Resume the current transference.

**Parameters**

None

**Return value**

None

**Example**
```javascript
ftp.resume_transfer();
```

### suspend_transfer
```javascript
suspend_transfer();
```

**Description**

Suspend the current transference.

**Parameters**

None

**Return value**

None

**Example**
```javascript
ftp.suspend_transfer();
```

### Events
#### transfer
```javascript
ftp.on('transfer', function(FtpProperty));
```

**Description**

Called every time a transfer is updated.

**Parameters**
*FtpProperty*: The transfer property.

**Example**
```javascript
ftp.on('transfer', function(prop) { console.log("Transfer updated"); });
```

## Full example

  * See [bluetooth-ftp-example.js](/examples/bluetooth-ftp-example.js)
