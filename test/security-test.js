/* Global Includes */
var testCase = require('mocha').describe;
var pre = require('mocha').before;
var preEach = require('mocha').beforeEach;
var post = require('mocha').after;
var postEach = require('mocha').afterEach;
var assertions = require('mocha').it;
var step = require('mocha-steps').step;
var assert = require('chai').assert;
var validator = require('validator');
var exec = require('child_process').execSync;
var artik = require('../src');
var fs = require('fs');

/* Test Specific Includes */
var security = {};
var pem_cert = "-----BEGIN CERTIFICATE-----\n" +
	"MIICBjCCAaygAwIBAgIQWLB3huXHE8gw1wJF7K6X3DAKBggqhkjOPQQDAjBjMQsw\n" +
	"CQYDVQQGEwJLUjEkMCIGA1UEChMbU2Ftc3VuZyBTZW1pY29uZHVjdG9yIEFSVElL\n" +
	"MRYwFAYDVQQLEw1BUlRJSyBSb290IENBMRYwFAYDVQQDEw1BUlRJSyBSb290IENB\n" +
	"MB4XDTE3MDIyNDE4MTIyMloXDTMyMDIyNDE4MTIyMlowYzELMAkGA1UEBhMCS1Ix\n" +
	"JDAiBgNVBAoTG1NhbXN1bmcgU2VtaWNvbmR1Y3RvciBBUlRJSzEWMBQGA1UECxMN\n" +
	"QVJUSUsgUm9vdCBDQTEWMBQGA1UEAxMNQVJUSUsgUm9vdCBDQTBZMBMGByqGSM49\n" +
	"AgEGCCqGSM49AwEHA0IABDFxz+e+hsmEuj+0ikoh01TZp6B7AUUFHvz08rStVvfe\n" +
	"9n2XoiQsJJycZ51Ex9JM1XtsjYvg5j5zBNqC9syA7v6jQjBAMA4GA1UdDwEB/wQE\n" +
	"AwIBBjAdBgNVHQ4EFgQUotPhsbXrQoIX7ngaetQqFlK+FJcwDwYDVR0TAQH/BAUw\n" +
	"AwEB/zAKBggqhkjOPQQDAgNIADBFAiEAvS7IdSkE4Flk4S0hPeTYJvPyKH96tR+v\n" +
	"DHw5gMUmRdECICcgp06TVZSeXOAILvWYqnU/y5kjEV3HViY81+4isMlv\n" +
	"-----END CERTIFICATE-----\n";

var sample_key = new Buffer([
	0x30, 0x78, 0x02, 0x01, 0x01, 0x04, 0x20, 0x53, 0x02, 0x02, 0xF3, 0x1A,
	0x24, 0x2E, 0x1B, 0xCA, 0xE8, 0x2B, 0xAB, 0xF2, 0x86, 0x25, 0x59, 0x36,
	0xE5, 0x20, 0x6C, 0x39, 0xAC, 0x1C, 0x3D, 0xBD, 0x6B, 0x59, 0x29, 0xD0,
	0xC2, 0xA2, 0xB3, 0xA0, 0x0B, 0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02,
	0x08, 0x01, 0x01, 0x07, 0xA1, 0x44, 0x03, 0x42, 0x00, 0x04, 0x2B, 0x10,
	0x61, 0x1F, 0xFC, 0xAA, 0x9C, 0xEC, 0xE3, 0xD9, 0x87, 0x05, 0x49, 0x3E,
	0x7E, 0xCD, 0xC8, 0xCC, 0xDA, 0xA2, 0x92, 0xBE, 0x28, 0x71, 0x8E, 0x81,
	0xAB, 0x8B, 0x22, 0xCB, 0xFE, 0x46, 0xA7, 0x31, 0xE7, 0x36, 0xAB, 0xD6,
	0x12, 0x9E, 0x05, 0x1C, 0x1E, 0xEB, 0xA8, 0x11, 0x56, 0x92, 0x6A, 0xC6,
	0xB1, 0x85, 0x4D, 0x41, 0x43, 0x75, 0x7B, 0xE8, 0x6E, 0x83, 0x2C, 0xF5,
	0xCC, 0x6B
]);

var hash = new Buffer([
	0xdc, 0x71, 0x78, 0xb3, 0xad, 0xb8, 0x90, 0xad, 0x47, 0x7e, 0x8b, 0xa0,
	0x0c, 0x70, 0x11, 0x93, 0xa7, 0x8e, 0xe7, 0xb8, 0xf6, 0xb2, 0x7e, 0xa2,
	0x67, 0x0d, 0xc5, 0x0b, 0xfe, 0xbe, 0xef, 0xc9
]);

var expected_hash = new Buffer([
	0xdc, 0x71, 0x78, 0xb3, 0xad, 0xb8, 0x90, 0xad, 0x47, 0x7e, 0x8b, 0xa0,
	0x0c, 0x70, 0x11, 0x93, 0xa7, 0x8e, 0xe7, 0xb8, 0xf6, 0xb2, 0x7e, 0xa2,
	0x67, 0x0d, 0xc5, 0x0b, 0xfe, 0xbe, 0xef, 0xc9
]);

/* Test Case Module */
testCase('Test JS security API', function() {

	pre(function(done) {
		this.timeout(30000);
		try {
			security = new artik.security();
			done();
		} catch (err) {
			console.log("[Exception] : " + err.message);
		}
	});

	testCase('#Authentification Test', function() {

		step('#get_ec_pub_key_from_cert: Get "EC pub key" of the certificate.', function(done) {

			this.timeout(10000);

			try {
				var ec_pub_key = security.get_ec_pubkey_from_cert(pem_cert);
				assert.isString(ec_pub_key, "Invalid return type of the variable : 'Key'.\n");
				console.log("EC Pub Key of certificate : ");
				console.log(ec_pub_key);
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#get_random_bytes(): Generate random bytes for an array of 32 bytes.', function(done) {

			this.timeout(10000);

			try {
				var random = security.get_random_bytes(32);
				assert.equal(random.length, 32, "Invalid buffer returned due to the invalid size wichi is not equal to 32 for : 'Random'.\n");
				console.log("Bytes : ");
				for (var i = 0; i < random.length; ++i) {
					process.stdout.write(" " + random.readUInt8(i));
				}
				process.stdout.write("\n");
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#get_certificate: Get the certificate from the Secure Element.', function(done) {

			this.timeout(10000);

			try {
				certificate = security.get_certificate('ARTIK/0', 'ARTIK_SECURITY_CERT_TYPE_PEM');
				console.log("Certificate : ");
				console.log(certificate.toString());
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}

		});

		step('#get_certificate_sn(): Get the serial number of the certificate from the Secure Element.', function(done) {

			this.timeout(10000);

			try {
				var sn = security.get_certificate_sn(pem_cert);
				assert.isNotNull(sn, "Invalid return type of the variable : 'sn'.\n");
				console.log("Serial Number : ");
				console.log(sn);
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#get_certificate_pem_chain: Get root and intermediate certificates from the Secure Element.', function(done) {

			this.timeout(10000);

			try {
				var certificates = security.get_certificate_pem_chain('ARTIK/0');

				for (var i = 0; i < certificates.length; i++) {
					assert.isArray(certificates, "Invalid return type of the variable : 'certificates'.\n");
					console.log("Cert #" + i + "\n" + certificates[i]);
				}
				done();

			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#set_certificate: Set certificate from the Secure Element.', function(done) {

			this.timeout(10000);

			try {
				var res = security.set_certificate('SSDEF/0', pem_cert);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_certificate: Remove certificate from the Secure Element.', function(done) {

			this.timeout(10000);

			try {
				var res = security.remove_certificate('SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#Hash Test', function() {
		step('#get_hash: Get the hash of the input message.', function(done) {

			this.timeout(10000);

			var hash_algorithm = 0x00230000;

			try {
				var hash = security.get_hash(hash_algorithm, sample_key);
				console.log("Hash of the input message: \n" + hash.toString('hex'));

				if (Buffer.compare(expected_hash, hash) == 0) {
					console.log("Hash and expected_hash are identical. OK");
					done();
				} else {
					console.log("Hash and expected_hash are different result  not OK");
				}
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#HMAC Test', function() {
		step('#generate_key: Generate a HMAC key.', function(done) {

			this.timeout(10000);

			var hmac_algorithm = 0x4000;

			try {
				var res = security.generate_key(hmac_algorithm, 'SSDEF/O');
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#get_hmac: Get HMAC from input data.', function(done) {

			this.timeout(10000);

			var see_hash_mode = 0x00230000;

			try {
				var hmac = security.get_hmac(see_hash_mode, 'SSDEF/O', sample_key);
				console.log("HMAC of the input data: \n" + hmac.toString('hex'));
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a HMAC key.', function(done) {

			this.timeout(10000);

			var hmac_algorithm = 0x4000;

			try {
				var res = security.remove_key(hmac_algorithm, 'SSDEF/O');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#RSA Test', function() {

		var rsa_sig;

		step('#generate_key: Generate a RSA key.', function(done) {

			this.timeout(10000);

			var rsa_key_algorithm = 0x1000;

			try {
				var res = security.generate_key(rsa_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#get_rsa_signature: Get RSA get_rsa_signature from input data.', function(done) {

			this.timeout(10000);

			var rsa_signature_algorithm = 0x00231000;

			try {
				rsa_sig = security.get_rsa_signature(rsa_signature_algorithm, 'SSDEF/0', hash);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#verify_rsa_signature: Verify RSA signature for input hash.', function(done) {

			this.timeout(10000);

			var rsa_signature_algorithm = 0x00231000;

			try {
				var res = security.verify_rsa_signature(rsa_signature_algorithm, 'SSDEF/0', hash, rsa_sig);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a RSA key.', function(done) {

			this.timeout(10000);

			var rsa_algorithm = 0x1000;

			try {
				var res = security.remove_key(rsa_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#ECDSA Test', function() {

		var ecdsa_sig;

		step('#generate_key: Generate a ECDSA key.', function(done) {

			this.timeout(10000);

			var ecdsa_key_algorithm = 0x2010;

			try {
				var res = security.generate_key(ecdsa_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#get_ecdsa_signature: Get ECDSA get_ecdsa_signature from input data.', function(done) {

			this.timeout(10000);

			var ecdsa_signature_algorithm = 0x2010;

			try {
				ecdsa_sig = security.get_ecdsa_signature(ecdsa_signature_algorithm, 'SSDEF/0', hash);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}

		});

		step('#verify_ecdsa_signature: Verify ECDSA signature for input hash.', function(done) {

			this.timeout(10000);

			var ecdsa_signature_algorithm = 0x2010;

			try {
				var res = security.verify_ecdsa_signature(ecdsa_signature_algorithm, 'SSDEF/0', hash, ecdsa_sig);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a ECDSA key.', function(done) {

			this.timeout(10000);

			var ecdsa_key_algorithm = 0x2010;

			try {
				var res = security.remove_key(ecdsa_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('# Key Manager Test', function() {

		step('#set_key: Set a "key" in secure element.', function(done) {

			this.timeout(10000);

			var sample_key_algorithm = 0x2000;

			try {
				var res = security.set_key(sample_key_algorithm, 'SSDEF/0', sample_key);
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a SAMPLE_KEY.', function(done) {

			this.timeout(10000);

			var sample_key_algorithm = 0x2000;

			try {
				var res = security.remove_key(sample_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#generate_key: Generate a key.', function(done) {

			this.timeout(10000);

			var generate_key_algorithm = 0x1000;

			try {
				var res = security.generate_key(generate_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#get_publickey: Get public key from an asymmetric key.', function(done) {

			this.timeout(10000);

			var generate_key_algorithm = 0x1000;

			try {
				var publickey = security.get_publickey(generate_key_algorithm, 'SSDEF/0');
				console.log("Publickey from asymmetric key:\n" + publickey.toString('hex'));
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a key.', function(done) {

			this.timeout(10000);

			var generate_key_algorithm = 0x1000;

			try {
				var res = security.remove_key(generate_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#Secure Storage Test: WRITE/READ/REMOVE small data into secure storage', function() {

		var buffer = Buffer.alloc(512);

		step('#get_random_bytes(): Generate random bytes for an array of 512 bytes.', function(done) {

			this.timeout(10000);

			try {
				buffer = security.get_random_bytes(512);
				assert.equal(buffer.length, 512, "Invalid buffer returned due to the invalid size wichi is not equal to 512 for : 'Random'.\n");
				console.log(buffer);
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#write_secure_storage(): Write a small data into secure storage.', function(done) {

			this.timeout(10000);

			try {
				var res = security.write_secure_storage('SSDEF/0', 0, buffer);
				console.log(res);
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#read_secure_storage(): Read a data from secure storage.', function(done) {

			this.timeout(10000);

			try {
				var read_buf = security.read_secure_storage('SSDEF/0', 0, 512);
				console.log(read_buf);
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_secure_storage(): Remove a data from secure storage.', function(done) {

			this.timeout(10000);

			try {
				var res = security.remove_secure_storage('SSDEF/0');
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#Encryption decryption Test', function() {

		var aes_enc_data;
		var rsa_enc_data;
		var aes_enc_mode = 3;
		var rsa_enc_mode = 0x1000;
		var iv = Buffer.alloc(16, 0);

		step('#generate_key: Generate a AES key.', function(done) {

			this.timeout(10000);

			var aes_key_algorithm = 0x2;

			try {
				var res = security.generate_key(aes_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#aes_encryption(): Encrypt a input message using AES.', function(done) {

			this.timeout(10000);

			try {
				aes_enc_data = security.aes_encryption(aes_enc_mode, 'SSDEF/0', iv, sample_key);
				console.log(aes_enc_data.toString('hex'));

				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}

		});

		step('#aes_decryption(): Decrypt a input message using AES.', function(done) {

			this.timeout(10000);

			try {
				var aes_dec_data = security.aes_decryption(aes_enc_mode, 'SSDEF/0', iv, aes_enc_data);
				console.log(aes_dec_data.toString('hex'));
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a AES key.', function(done) {

			this.timeout(10000);

			var aes_key_algorithm = 0x2;

			try {
				var res = security.remove_key(aes_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#generate_key: Generate a RSA key.', function(done) {
			this.timeout(10000);

			var rsa_key_algorithm = 0x1000;

			try {
				var res = security.generate_key(rsa_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#rsa_encryption(): Encrypt a input message using RSAES.', function(done) {

			this.timeout(10000);

			try {
				rsa_enc_data = security.rsa_encryption(rsa_enc_mode, 'SSDEF/0', sample_key);
				console.log(rsa_enc_data.toString('hex'));
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#rsa_decryption(): Decrypt a input message using RSAES.', function(done) {

			this.timeout(10000);

			try {
				var rsa_dec_data = security.rsa_decryption(rsa_enc_mode, 'SSDEF/0', rsa_enc_data);
				console.log(rsa_dec_data.toString('hex'));
				done();
			} catch (err) {
				console.log("[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a RSA key.', function(done) {

			this.timeout(10000);

			var rsa_key_algorithm = 0x1000;

			try {
				var res = security.remove_key(rsa_key_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#Convert pem to der Test', function() {
		step('#convert_pem_to_der: Convert a certificate or a key from PEM format to DER format.', function(done) {

			this.timeout(10000);

			try {
				var der_data = security.convert_pem_to_der(pem_cert);
				console.log(der_data);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#DHM params Test', function() {

		var dh_algorithm = 0x5000;
		var pubkey;

		step('#generate_dhm_params: Generate DH key pair and get public key.', function(done) {

			this.timeout(10000);

			try {
				var pubkey = security.generate_dhm_params(dh_algorithm, 'SSDEF/0');
				console.log(pubkey);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a dh key.', function(done) {

			this.timeout(10000);

			try {
				var res = security.remove_key(dh_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#set_dhm_params: Generate DH key pair and get public key using dh parameter.', function(done) {

			var dh_params = new Buffer([
				0x30, 0x81, 0x87, 0x02, 0x81, 0x81, 0x00, 0xcb, 0x00, 0x32, 0x75, 0x58, 0xf1, 0xd4, 0x4b, 0xe8,
				0xf6, 0x27, 0x47, 0x69, 0xdc, 0x63, 0xe6, 0x1f, 0xc9, 0xe7, 0xb6, 0xbd, 0x1e, 0xaf, 0xed, 0xfe,
				0xd8, 0x43, 0x34, 0xa8, 0x4a, 0x6a, 0xb8, 0x16, 0x1c, 0x22, 0xc4, 0x95, 0x80, 0x21, 0x63, 0x0e,
				0xac, 0xff, 0x26, 0x84, 0xd6, 0x28, 0x53, 0x2f, 0xf2, 0x2b, 0xab, 0x98, 0x3a, 0x97, 0xdc, 0xe5,
				0x0a, 0x56, 0xc0, 0x36, 0x4f, 0x67, 0xc0, 0x86, 0x6a, 0x70, 0x70, 0x21, 0x89, 0x88, 0xad, 0xef,
				0xaa, 0x00, 0x3b, 0x57, 0x52, 0x7e, 0xcf, 0x59, 0x42, 0x87, 0x04, 0x62, 0x4a, 0x33, 0xe0, 0xda,
				0xf3, 0xcc, 0x3d, 0x6b, 0xe7, 0xd4, 0xc6, 0xc3, 0xa8, 0xd8, 0xd1, 0x9b, 0x1a, 0xb7, 0xa3, 0x7f,
				0x48, 0xf3, 0x03, 0xcc, 0x7c, 0x78, 0xb0, 0x2c, 0x6a, 0xa2, 0x97, 0x7c, 0xf1, 0xee, 0x7d, 0x10,
				0x24, 0x56, 0xbd, 0x87, 0x69, 0xf5, 0x0b, 0x02, 0x01, 0x02
			]);

			this.timeout(10000);

			try {
				pubkey = security.set_dhm_params('SSDEF/0', dh_params);
				console.log(pubkey);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#compute_dhm_params: Compute secret key from DH key and public key.', function(done) {

			this.timeout(10000);

			try {
				var secret = security.compute_dhm_params('SSDEF/0', pubkey);
				console.log(secret);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#remove_key: Remove a dh key.', function(done) {

			this.timeout(10000);

			try {
				var res = security.remove_key(dh_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});

	testCase('#ECDH params Test', function() {
		var ecdh_algorithm = 0x2000;
		var pubkey;

		step('#generate_ecdh_params: Generate ECDH key.', function(done) {

			this.timeout(10000);

			try {
				pubkey = security.generate_ecdh_params(ecdh_algorithm, 'SSDEF/0');
				console.log(pubkey);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});

		step('#compute_ecdh_params: Compute secret key from ECDH key and public key..', function(done) {

			this.timeout(10000);

			try {
				var secret = security.compute_ecdh_params('SSDEF/0', pubkey);
				console.log(secret);
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});


		step('#remove_key: Remove a ecdh key.', function(done) {

			this.timeout(10000);

			try {
				var res = security.remove_key(ecdh_algorithm, 'SSDEF/0');
				done();
			} catch (err) {
				assert(!err, "[Exception] : " + err.message);
				done(err);
			}
		});
	});
});