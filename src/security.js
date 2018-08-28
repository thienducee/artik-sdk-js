var events = require('events');
var util = require('util');
const fs = require('fs');
var security = require('../build/Release/artik-sdk.node').security;

var Security = function() {
	this.security = new security();
}

module.exports.Security = Security;

const begin_cert = "-----BEGIN CERTIFICATE-----\n";
const end_cert = "-----END CERTIFICATE-----\n";

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

jsonReturn = function(err, reason, pkcs7_signing_time) {
	var json = {
		error: (err != 0),
		reason: reason,
		error_code: error_code[String(err)],
		signing_time: pkcs7_signing_time,
	}
	return json;
};

Security.prototype.get_certificate = function(cert_name, cert_type) {
	return this.security.get_certificate(cert_name, cert_type);
}

Security.prototype.get_certificate_sn = function(pem_cert) {
	return this.security.get_certificate_sn(pem_cert);
}

Security.prototype.get_certificate_pem_chain = function(cert_name) {
	return this.security.get_certificate_pem_chain(cert_name);
}

Security.prototype.set_certificate = function (cert_name, sample_cert) {
    return this.security.set_certificate(cert_name, sample_cert);
}

Security.prototype.remove_certificate = function (cert_name) {
    return this.security.remove_certificate(cert_name);
}

Security.prototype.get_hash = function(see_hash_mode, sample_key) {
	return this.security.get_hash(see_hash_mode, sample_key);
}

Security.prototype.generate_key = function(see_algorithm, const_key_name) {
	return this.security.generate_key(see_algorithm, const_key_name);
}

Security.prototype.get_hmac = function(see_hash_mode, hmac_key_name, sample_key) {
	return this.security.get_hmac(see_hash_mode, hmac_key_name, sample_key);
}

Security.prototype.remove_key = function (see_algorithm, const_key_name) {
    return this.security.remove_key(see_algorithm, const_key_name);
}

Security.prototype.get_rsa_signature = function (see_rsa_mode, rsa_key_name, hash) {
    return this.security.get_rsa_signature(see_rsa_mode, rsa_key_name, hash);
}

Security.prototype.verify_rsa_signature = function (see_rsa_mode, rsa_key_name, hash, rsa_sig) {
    return this.security.verify_rsa_signature(see_rsa_mode, rsa_key_name, hash, rsa_sig);
}

Security.prototype.get_ecdsa_signature = function (see_algorithm, ecc_key_name, hash) {
    return this.security.get_ecdsa_signature(see_algorithm, ecc_key_name, hash);
}

Security.prototype.verify_ecdsa_signature = function (see_algorithm, ecc_key_name, hash, ecdsa_sig) {
    return this.security.verify_ecdsa_signature(see_algorithm, ecc_key_name, hash, ecdsa_sig);
}

Security.prototype.set_key = function (see_algorithm, sample_key_name, sample_key) {
    return this.security.set_key(see_algorithm, sample_key_name, sample_key);
}

Security.prototype.get_publickey = function (see_algorithm, sample_key_name) {
    return this.security.get_publickey(see_algorithm, sample_key_name);
}

Security.prototype.write_secure_storage = function (name_storage, offset_storage, data_write_storage) {
    return this.security.write_secure_storage(name_storage, offset_storage, data_write_storage);
}

Security.prototype.read_secure_storage = function (name_storage, offset_storage, data_read_size) {
    return this.security.read_secure_storage(name_storage, offset_storage, data_read_size);
}

Security.prototype.remove_secure_storage = function (name_storage) {
    return this.security.remove_secure_storage(name_storage);
}

Security.prototype.aes_encryption = function (see_aes_mode, aes_key_name, iv, sample_key) {
    return this.security.aes_encryption(see_aes_mode, aes_key_name, iv, sample_key);
}

Security.prototype.aes_decryption = function (see_aes_mode, aes_key_name, iv, aes_enc_data) {
    return this.security.aes_decryption(see_aes_mode, aes_key_name, iv, aes_enc_data);
}

Security.prototype.rsa_encryption = function (see_rsa_mode, rsa_key_name, sample_key) {
    return this.security.rsa_encryption(see_rsa_mode, rsa_key_name, sample_key);
}

Security.prototype.rsa_decryption = function (see_rsa_mode, rsa_key_name, rsa_enc_data) {
    return this.security.rsa_decryption(see_rsa_mode, rsa_key_name, rsa_enc_data);
}

Security.prototype.get_ec_pubkey_from_cert = function(certificate) {
	return this.security.get_ec_pubkey_from_cert(certificate);
}

Security.prototype.get_random_bytes = function(number) {
	return this.security.get_random_bytes(number);
}

Security.prototype.verify_signature_init = function(signature_pem, root_ca, date, func) {
	return this.security.verify_signature_init(signature_pem, root_ca, date, func);
}

Security.prototype.verify_signature_update = function(buffer, func) {
	return this.security.verify_signature_update(buffer, func);
}

Security.prototype.verify_signature_final = function(func) {
	return this.security.verify_signature_final(func);
}

Security.prototype.convert_pem_to_der = function(pem_data) {
	return this.security.convert_pem_to_der(pem_data);
}

Security.prototype.generate_dhm_params = function(dh_algorithm, key_name) {
	return this.security.generate_dhm_params(dh_algorithm, key_name);
}

Security.prototype.set_dhm_params = function(key_name, dh_params) {
	return this.security.set_dhm_params(key_name, dh_params);
}

Security.prototype.compute_dhm_params = function(key_name, pubkey) {
	return this.security.compute_dhm_params(key_name, pubkey);
}

Security.prototype.generate_ecdh_params = function(ecdh_algoritm, key_name) {
	return this.security.generate_ecdh_params(ecdh_algoritm, key_name);
}

Security.prototype.compute_ecdh_params = function(key_name, pubkey) {
	return this.security.compute_ecdh_params(key_name, pubkey);
}

Security.prototype.pkcs7_sig_verify = function(path_signature_pem,
	path_root_ca, path_signed_data, se_id, date, return_cb) {
	var _ = this;
	/* Get content of the pem signature file */
	try {
		/* Read file signature_pem */
		var signature_pem = String(fs.readFileSync(path_signature_pem));
	} catch (err) {
		return return_cb(jsonReturn(-1, "Cannot read PKCS7 signature file", undefined));
	}
	if (se_id == "") {
		/* Get content of the root_ca file */
		try {
			/* Read file root_ca */
			var root_ca = String(fs.readFileSync(path_root_ca));
		} catch (err) {
			return return_cb(jsonReturn(err, "Cannot read root CA file", undefined));
		}
	} else {
		/* Get content of the root_ca file */
		try {
			/* Read file root_ca */
			var ca_pem = this.security.get_ca_chain(se_id);

			var len_start_cert = ca_pem.search(begin_cert);
			if (len_start_cert == -1)
				return return_cb(jsonReturn(err, "Malformed PEM certificate", undefined));

			var len_end_cert = ca_pem.search(end_cert);
			if (len_end_cert == -1)
				return return_cb(jsonReturn(err, "Malformed PEM certificate", undefined));

			var root_ca = ca_pem.substr(len_start_cert, len_end_cert + end_cert.length);

		} catch (err) {
			return return_cb(jsonReturn(err, "Failed to get ca chain in secure element", undefined));
		}
	}

	/* Get pkcs7_signing_time with verify_signature_init */
	this.security.verify_signature_init(signature_pem, root_ca, date,
		function(err, reason, pkcs7_signing_time) {

			if (err != 0)
				return return_cb(jsonReturn(err, reason, pkcs7_signing_time));

			var readStream = fs.createReadStream(path_signed_data);

			readStream.on('data', function(buffer) {
				_.security.verify_signature_update(buffer, function(err, reason) {
					if (err)
						return return_cb(jsonReturn(err, reason, pkcs7_signing_time));
				});
			});

			readStream.on('end', function() {
				/* Use verify_signature_final to check package signature */
				_.security.verify_signature_final(function(err, reason) {
					if (err)
						return return_cb(jsonReturn(err, reason, pkcs7_signing_time));

					return return_cb(jsonReturn(err, "Verification successful", pkcs7_signing_time));
				});
			});

			readStream.on('error', function(err) {
				return return_cb(jsonReturn(err, "readstream failed", pkcs7_signing_time));
			});
		});
};