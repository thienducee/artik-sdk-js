/* Global Includes */
var testCase   = require('mocha').describe;
var pre        = require('mocha').before;
var preEach    = require('mocha').beforeEach;
var post       = require('mocha').after;
var postEach   = require('mocha').afterEach;
var assertions = require('mocha').it;
var assert     = require('chai').assert;
var validator  = require('validator');
var exec       = require('child_process').execSync;
var artik      = require('../src');

var fs = require('fs');

/* Test Specific Includes */
var auth_token = process.env.CLOUD_TOKEN;
var device_id  = process.env.CLOUD_DEVICE_ID;
var message    = process.env.CLOUD_MESSAGE;
var action     = process.env.CLOUD_ACTION;
var cloud      = new artik.cloud(auth_token);
var user_id;

var data = "";

if (process.env.CLOUD_CA_ROOT_FILE != "")
	data = fs.readFileSync(process.env.CLOUD_CA_ROOT_FILE);

var ssl_config = {
	ca_cert: Buffer.from(data),
	verify_cert: process.env.CLOUD_VERIFY_CERT == 1 ? "required" : "none",
	use_se: (process.env.WEBSOCKET_ENABLE_SDR == 1 ? true : false)
}

/* Test Case Module */
testCase('Cloud', function() {

	this.timeout(5000);

	pre(function() {
	});

	testCase('#get_current_user_profile()', function() {

		assertions('Get User Profile - Should return ARTIK Cloud User profile', function(done) {
			if (!auth_token || !auth_token.length)
				this.skip();

			var response = cloud.get_current_user_profile(ssl_config);
			assert.notInclude(response, "error");
			user_id = JSON.parse(response).data.id;
			done();
		});

		assertions('Get User Profile Callback - Returns user profile response in the callback', function(done) {

			if (!auth_token || !auth_token.length)
				this.skip();

			cloud.get_current_user_profile(ssl_config, function(response) {
				console.log(response);
				assert.notInclude( response, "error");
				done();
			});
		});
	});

	testCase('#get_device_token()', function() {

		assertions('Get Device Token - Should return the ARTIK cloud device token ', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			assert.notInclude(cloud.get_device_token(device_id, ssl_config), "error");
			done();
		});

		assertions('Get Device Token Callback - Should return the callback with ARTIK cloud device token', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			cloud.get_device_token(device_id, ssl_config, function(response) {
				console.log(response);
				assert.notInclude(response, "error");
				done();
			});
		});

	});

	testCase('#get_device()', function() {

		assertions('Get Device without properties ', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();
			assert.notInclude(cloud.get_device(device_id, false, ssl_config), "error");
			done();
		});

		assertions('Get Device with properties ', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			assert.notInclude(cloud.get_device(device_id, true, ssl_config), "error");
			done();
		});

		assertions('Get Device without properties - Calllback', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			cloud.get_device(device_id, false, ssl_config, function(response) {
				console.log(response);
				assert.notInclude(response, "error");
				done();
			});
		});

		assertions('Get Device with properties - Calllback', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			cloud.get_device(device_id, true, ssl_config, function(response) {
				console.log(response);
				assert.notInclude(response, "error");
				done();
			});
		});

	});

	testCase('#get_user_device_types()', function() {

		assertions('Get User Device Types ', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			assert.notInclude(cloud.get_user_device_types(100, false, 0, user_id, ssl_config), "error");
			done();
		});

		assertions('Get User Device Types - Calllback', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			cloud.get_user_device_types(100, false, 0, user_id, ssl_config, function(response) {
				console.log(response);
				assert.notInclude(response, "error");
				done();
			});
		});

	});

	testCase('#send_action()', function() {

		assertions('Send Actions to a device via Cloud', function(done) {

			if (!auth_token || !device_id  || !action || !auth_token.length || !device_id.length || !action.length)
				this.skip();

			assert.notInclude(cloud.send_action(device_id, action, ssl_config), "error");
			done();
		});

	});


	testCase('#send_message()', function() {

		assertions('Send Messages to Cloud from Device ', function(done) {

			if (!auth_token || !device_id  || !message || !auth_token.length || !device_id.length || !message.length)
				this.skip();

			assert.notInclude(cloud.send_message(device_id, message, ssl_config), "error");
			done();

		});
	});

	testCase('#delete_device_token()', function() {

		assertions('Delete Device Token', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			assert.notInclude(cloud.delete_device_token(device_id, ssl_config), "error");
			done();

		});

		assertions('Delete Device Token - Calllback', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			cloud.delete_device_token(device_id, ssl_config, function(response) {
				console.log(response);
				assert.notInclude(response, "error");
				done();
			});
		});

	});

	testCase('#update_device_token()', function() {

		assertions('Update Device Token', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			assert.notInclude(cloud.update_device_token(device_id, ssl_config), "error");
			done();

		});

		assertions('Update Device Token - Calllback', function(done) {

			if (!auth_token || !device_id || !auth_token.length || !device_id.length)
				this.skip();

			cloud.update_device_token(device_id, ssl_config, function(response) {
				console.log(response);
				assert.notInclude(response, "error");
				done();
			});
		});

	});



});
