# ARTIK SDK Automated Test Framework

The artik-sdk uses the mochajs test framework for testing. The unit test are present in the test/ directory.

## One time Setup

1) npm install mocha validator chai mochawesome


2) Edit artik_test.conf and replace the variables with your test configuration



## Run

1) Export configuration variables by running the following command,

	source artik_test.conf


2) To run a single test like http,

	./node_modules/.bin/mocha -R mochawesome test/http-test.js


3) To run all the tests contained in the test directory

	./node_modules/.bin/mocha -R mochawesome


## Note

	Some of the test will do negative test case scenarios and hence will disable the Wifi. Hence,
	if you have the variable set "ALLOW_DISABLE_WIFI" to 1, you should NOT run these tests on ssh
	connection. In this case, you must run these tests on the serial port.

