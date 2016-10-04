var net = require('net');

var host = '127.0.0.1';
var port = 6060;

var thread_count = process.argv[2];
var update_count = process.argv[3];
var update_size = process.argv[4];

var client = new net.Socket();
client.connect(port, host, function() {
	for (var i = 0; i < update_count; i++) {
		console.log("sending");
		client.write('data\n');
	}
	console.log("All done sending");
});

console.log('Client started');