//import EventEmitter from "wun:events";

export default class Stream {
	constructor(fd) {
		//super();

		this.fd=fd;
		sys.watch(this.fd,sys.G_IO_IN,this.onFdData);
	}

	onFdData=()=>{
		let data=sys.read(this.fd,4096);
		this.emit("data",data);
	}

	close=()=>{
		sys.close(this.fd);
	}
}

/*export function fopen(fn, mode) {
	let fd=sys.open(fn,sys.O_RDONLY|sys.O_NONBLOCK);

	return new Stream(fd);
}

export default {Stream, fopen};*/
