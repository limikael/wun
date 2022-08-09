import EventEmitter from "wun:events";

export default class Stream extends EventEmitter {
	constructor(fd) {
		super();

		this.fd=fd;
		sys.watch(this.fd,sys.G_IO_IN,this.onFdData);
		sys.watch(this.fd,sys.G_IO_HUP,this.onFdHup);
		sys.watch(this.fd,sys.G_IO_ERR,this.onFdErr);
	}

	onFdData=()=>{
		let data=sys.read(this.fd,4096);
		this.emit("data",data);

		sys.watch(this.fd,sys.G_IO_IN,this.onFdData);
	}

	onFdHup=()=>{
		sys.close(this.fd);
		this.emit("close");
	}

	onFdErr=()=>{
		this.emit("error");
		sys.close(this.fd);
		this.emit("close");
	}

	close=()=>{
		sys.close(this.fd);
	}
}

export function fopen(fn, mode) {
	let fd=sys.open(fn,sys.O_RDONLY|sys.O_NONBLOCK);

	return new Stream(fd);
}
