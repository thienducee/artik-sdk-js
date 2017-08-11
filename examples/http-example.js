const artik_http = require("../src/http");
const fs = require('fs');
var http = new artik_http();


var headers = [
	"user-agent", "ARTIK browser",
	"Accept-Language", "en-US,en;q=0.8"
];

var httpbin_ca_root =
	"-----BEGIN CERTIFICATE-----\n" +
	"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\r\n" +
	"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\r\n" +
	"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\r\n" +
	"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\r\n" +
	"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\r\n" +
	"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\r\n" +
	"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\r\n" +
	"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\r\n" +
	"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\r\n" +
	"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\r\n" +
	"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\r\n" +
	"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\r\n" +
	"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\r\n" +
	"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\r\n" +
	"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\r\n" +
	"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\r\n" +
	"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\r\n" +
	"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\r\n" +
	"-----END CERTIFICATE-----";

var ssl_config = {
	ca_cert: Buffer.from(httpbin_ca_root), // CA root certificate of httpbin.org
	verify_cert: "required"
};

var ws = fs.createWriteStream("./image.jpeg");

try {

	var stream = http.get_stream("https://httpbin.org/image/jpeg", headers, ssl_config);

	stream.on('end', function() {
		console.log("GET STREAM - Stream finished");
	})

	stream.pipe(ws);

	ws.on('end', function() {
	    ws.end();
	});

} catch(err) {
	console.log(err);
}


http.get("https://httpbin.org/get", headers, ssl_config, function(response, status) {
	console.log("GET - status " + status + " - response: " + response);
});

var body = "name=samsung&project=artik";

http.post("https://httpbin.org/post", headers, body, ssl_config, function(response, status) {
	console.log("POST - status " + status + " - response: " + response);
});

http.put("https://httpbin.org/put", headers, body, ssl_config, function(response, status) {
	console.log("PUT - status " + status + " - response: " + response);
});

http.del("https://httpbin.org/delete", headers, ssl_config, function(response, status) {
	console.log("DELETE - status " + status + " - response: " + response);
});
