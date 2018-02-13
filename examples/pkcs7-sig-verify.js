var artik = require('../src');
const fs = require('fs');
var security = artik.security();

/* Get path */
var path_signature_pem = process.argv[2];
var path_root_ca = process.argv[3];
var path_signed_data = process.argv[4];
var date = null;

const error_code = {
	"0": 0,
	"-1": -1,
	"-7001": -2,
	"-7002": -3,
	"-7003": -4,
	"-7004": -5,
	"-7005": -6,
	"-7006": -7,
}

if(process.argv.length <= 4)
{
	process.stdout.write("Usage: pkcs7-sig-verify <signature> <root CA> <signed data> [signing date]\n\n");
	process.stdout.write("signature - PKCS7 signature in PEM format\n");
	process.stdout.write("root CA - X509 root CA certificate in PEM format\n");
	process.stdout.write("signed data - file containing the signed data\n");
	process.stdout.write("signing date (optional) - current signing date for rollback detection\n");
	process.stdout.write("\tIf not provided, rollback detection is not performed\n");
	process.stdout.write("\nA JSON formatted string with verification result and error information is output on stdout\n");
	process.stdout.write("Return value contains an error code among the following ones\n");
	process.stdout.write("\t0: success\n");
	process.stdout.write("\t-1: invalid parameters\n");
	process.stdout.write("\t-2: invalid X509 certificate\n");
	process.stdout.write("\t-3: invalid PKCS7 signature\n");
	process.stdout.write("\t-4: CA verification failed\n");
	process.stdout.write("\t-5: computed digest mismatch\n");
	process.stdout.write("\t-6: signature verification failed\n");
	process.stdout.write("\t-7: signing time rollback detected\n");
	process.exit();
}

Date.prototype.isValid = function () {
    /* An invalid date object returns NaN for getTime() and NaN is the only */
    /* object not strictly equal to itself. */
    return 	this.getTime() === this.getTime();
};

jsonReturn = function (err, reason, pkcs7_signing_time) {
	var json = {
		error: (err != 0),
		reason: reason,
		error_code: error_code[String(err)],
		signing_time: pkcs7_signing_time,
	}
	process.stdout.write(JSON.stringify(json) + "\n");
	process.exit((err != 0 ) ? -1 : 0);
}

if(process.argv[5] != undefined)
{
	date = new Date(process.argv[5]);
	if(date.isValid() == false)
		jsonReturn(-1, "Invalid signing time", undefined);
}

/* Get content of the pem signature file */
try {
	/* Read file signature_pem */
	var signature_pem = String(fs.readFileSync(path_signature_pem));
} catch (err) {
	jsonReturn(-1, "Cannot read PKCS7 signature file", undefined);
}

/* Get content of the root_ca file */
try{
	/* Read file root_ca */
	var root_ca = String(fs.readFileSync(path_root_ca));
} catch (err) {
	jsonReturn(err, "Cannot read root CA file", undefined);
}

/* Get pkcs7_signing_time with verify_signature_init */
security.verify_signature_init(signature_pem, root_ca, date, function(err, reason, pkcs7_signing_time) {
	if (err != 0)
		jsonReturn(err, reason, pkcs7_signing_time);

	var readStream = fs.createReadStream(path_signed_data);

	readStream.on('data', function (buffer) {
		security.verify_signature_update(buffer, function(err, reason) {
			if (err)
				jsonReturn(err, reason, pkcs7_signing_time);
		});
	});

	readStream.on('end', function () {
		/* Use verify_signature_final to check package signature */
		security.verify_signature_final(function(err, reason) {
			if (err)
				jsonReturn(err, reason, pkcs7_signing_time);

			jsonReturn(err, "Verification successful", pkcs7_signing_time);
		});
	});

	readStream.on('error', function (err) {
		jsonReturn(err, "readstream failed", pkcs7_signing_time);
	});
});
