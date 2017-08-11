# Media API

## play_sound_file

```javascript
play_sound_file(String file, function())
```

**Description**

Play a sound file through the default sound output.

**Parameters**

 - *String*: absolute path and filename of the sound file to play.
 - *function()*: optional callback function that will be called after
the sound file is done playing. If no function is provided
the sound will be played but the calling application has no way of
knowing when playback is over.

**Return value**

None.

**Example**

```javascript
media.play_sound_file('/opt/music.mp3', function() {
	console.log('Finished playing sound');
});
```

# Full example

   * See [media-example.js](/examples/media-example.js)
