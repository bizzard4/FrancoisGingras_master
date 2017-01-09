var net = require('net');
var LinkedList = require('linkedlist');

var list = new LinkedList();

var host = '127.0.0.1';
var port = 6060;

var thread_count = process.argv[2];
var update_count = process.argv[3];
var update_size = process.argv[4];

var i = 0;

net.createServer(function(sock) {
	sock.on('data', function(data) {
		// Add to 
		//list.push(10);
		i++
		console.log(i);
		if (i==thread_count*update_count) {
			console.log("exiting server");
			process.exit();
		}
	});
}).listen(port, host);

console.log('Server started');