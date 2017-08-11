# Bluetooth AVRCP API
## class AvrcpItem
### Attributes
#### property
```javascript
AvrcpProperty property;
```

**Description**

The property of the item.

#### path
```javascript
String path;
```

**Description**

The path of the item

## class AvrcpProperty
### Attributes
#### name
```javascript
String name;
```

**Description**
Displayable name

#### type
```javascript
String type;
```

**Description**
Type of the item.

Can take one of the following values "video", "audio" or "folder"

#### folder
```javascript
String folder;
```

**Description**
Folder type.

Can take one of the following values "mixed, "titles", "albums" or "artists"

This field is available if the property *type* is "folder".

#### playable
```javascript
Boolean playable;
```

**Description**

Indicate if the media can be played.

**Description**
#### title
```javascript
String title;
```

**Description**
Title of the media.

This field is available if the property *type* is set to "audio" or "video".

#### artist
```javascript
String artist;
```

**Description**
Artist of the media.

This field is available if the property *type* is set to "audio" or "video".

#### album
```javascript
String album;
```

**Description**
The album of the media.

This field is available if the property *type* is set to "audio" or "video".

#### genre
```javascript
String genre;
```

**Description**
The genre of the media.

This field is available if the property *type* is set to "audio" or "video".

#### number_of_tracks
```javascript
Number number_of_tracks;
```

**Description**
Number of the tracks in the album.

This field is available if the property *type* is set to "audio" or "video".

#### number
```javascript
Number number;
```

**Description**
Track number of the media.

This field is available if the property *type* is set to "audio" or "video".

#### duration
```javascript
Number duration;
```

**Description**
Playing time in milliseconds

This field is available if the property *type* is set to "audio" or "video".
## class Avrcp
### Repeat modes
|        Repeat mode       |    value    |
|:------------------------:|:-----------:|
| Repeat a single track    | singletrack |
| Repeat all tracks        | alltracks   |
| Repeat a group of tracks | group       |
| Turn off repeat mode     | off         |

### constructor
```javascript
var avrcp = new bluetooth.Avrcp();
```

**Description**

Create a new instance of Avrcp

**Parameters**

None

**Return Value**

New Instance

**Example**

```javascript
var avrcp = new Avrcp();
```

### controller_change_folder
```javascript
controller_change_folder(String folder)
```

**Description**

Change to the specified folder

**Parameters**

*String*: The path of the folder.

**Return value**

None

**Example**
```javascript
avrcp.controller_change_folder("/Filesystem/Album1");
```

### controller_list_item
```javascript
AvrcpItem[] controller_list_item(Number start, Number end)
```

**Description**

List the items of the current folder

**Parameters**

*Number*: The index of the start item
*Number*: The index of the end item, -1 means index of the last item.

**Return value**

*AvrcpItem[]*: The list of items in the folder.

**Example**
```javascript
var items = avrcp.controller_list_item(-1, -1);
```

### controller_get_repeat_mode
```javascript
String controller_get_repeat_mode()
```

**Description**

Get the repeat mode of the player

**Parameters**

None

**Return value**

*String*: The repeat mode of the player among the ones defined under [Repeat modes](#repeat-modes)

**Example**
```javascript
var repeat_mode = avrcp.controller_get_repeat_mode();
console.log("Repeat mode is " + repeat_mode);
```

### controller_set_repeat_mode
```javascript
controller_set_repeat_mode(String repeat_mode)
```

**Description**

Change the repeat mode of the player

**Parameters**

*String*: The repeat mode among the ones defined under [Repeat modes](#repeat-modes)

**Return value**

None

**Example**
```javascript
avrcp.repeat_mode("singletrack");
```

### controller_is_connected
```javascript
Boolean controller_is_connected();
```

**Description**

Indicate if an AVRCP target is connected with the bluetooth adapter.

**Parameters**

None

**Return value**

*Boolean* True if an AVRCP target is connected, otherwise false

**Example**
```javascript
var is_connected = avrcp.controller_is_connected();
console.log("Is connected ? " + is_connected);
```

### controller_resume_play
```javascript
controller_resume_play();
```

**Description**

Control remote AVRCP target to resume play.

**Parameters**

None

**Return value**

None

**Example**
```javascript
avrcp.controller_resume_play();
```

### controller_pause
```javascript
controller_pause();
```

**Description**

Control remote AVRCP target to pause playing.

**Parameters**

None

**Return value**

None

**Example**
```javascript
avrcp.controller_pause();
```


### controller_stop
```javascript
controller_stop();
```

**Description**

Control remote AVRCP target to stop playing.

**Parameters**

None

**Return value**

None

**Example**
```javascript
avrcp.controller_stop();
```


### controller_next
```javascript
controller_next();
```

**Description**

Control remote AVRCP target to play the next item.

**Parameters**

None
**Return value**

None

**Example**
```javascript
avrcp.controller_next();
```

### controller_previous
```javascript
controller_previous();
```

**Description**

Control remote AVRCP target to play the previous item.

**Parameters**

None

**Return value**

None

**Example**
```javascript
avrcp.controller_previous();
```

### controller_fast_forward
```javascript
controller_fast_forward();
```

**Description**

Control remote AVRCP target to fast forward current item.

**Parameters**

None

**Return value**

None

**Example**
```javascript
avrcp.controller_fast_forward();
```

### controller_rewind
```javascript
controller_rewind();
```

**Description**

Control remote AVRCP target to rewind current item.

**Parameters**

None

**Return value**

None

**Example**
```javascript
avrcp.controller_rewind();
```

### controller_get_property
```javascript
AvrcpProperty controller_get_property(String item);
```

**Description**

Get property of the item.

**Parameters**

*String*: The path of AVRCP item

**Return value**

*AvrcpItem*: The item's property

**Example**
```javascript
var item = avrcp.controller_get_property("/Filesystem/song1");
```

### controller_play_item
```javascript
controller_play_item(String item);
```

**Description**

Play the item.

**Parameters**

*String*: The path of AVRCP item

**Return value**

None

**Example**
```javascript
avrcp.controller_play_item("/Filesystem/song1");
```

### controller_add_to_playing
```javascript
controller_add_to_playing(String item);
```

**Description**

Add the item to the playing list.

**Parameters**

*String*: The path of AVRCP item

**Return value**

None

**Example**
```javascript
avrcp.controller_add_to_playing("/Filesystem/song1");
```

### controller_get_name
```javascript
String controller_get_name();
```

**Description**

Get the player name from the AVRCP target.

**Parameters**

None

**Return value**

*String*: The player name.

**Example**
```javascript
var name = avrcp.controller_get_name();
console.log("Player name is " + name);
```

### controller_get_status
```javascript
String controller_get_status();
```

**Description**

Get the player status from the AVRCP target

**Parameters**

None

**Return value**

*String*: The player status.

**Example**
```javascript
var status = avrcp.controller_get_status();
console.log("Player status is " + status);
```

### controller_get_subtype
```javascript
String controller_get_subtype()
```

**Description**

Get the player subtype from the AVRCP target.

**Parameters**

None

**Return value**

*String*: The player subtype.

**Example**
```javascript
var subtype = avrcp.controller_get_subtype();
console.log("Player subtype is " + subtype);
```

### controller_get_type
```javascript
String controller_get_type()
```

**Description**

Get the player type from the AVRCP target.

**Parameters**

None

**Return value**

*String*: The player type.

**Example**
```javascript
var type = avrcp.controller_get_type();
console.log("Player type is " + type);
```

### controller_get_browsable
```javascript
Boolean controller_get_browsable()
```

**Description**

Indicate if the player is browsable.

**Parameters**

None

**Return value**

*Boolean*: True if the player is browsable, otherwise false.

**Example**
```javascript
var browsable = avrcp.controller_get_browsable();
console.log("Browsable: " + browsable);
```

### controller_get_position
```javascript
Number controller_get_position()
```

**Description**

Get the playback position.

**Parameters**

None
**Return value**

*Number*: The playback position in milliseconds.

**Example**
```javascript
var position = avrcp.controller_get_position();
console.log("Position: " + position + " ms");
```

## Full example

  * See [bluetooth-avrcp-example.js](/examples/bluetooth-avrcp-example.js)
