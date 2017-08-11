const artik = require('../src');

var json = JSON.parse(artik.get_device_info());
console.log('Device name: ' + json.name);
console.log('Modules: ');
for (i in json.modules)
	console.log('\t' + json.modules[i]);


