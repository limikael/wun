import EventEmitter from "wun:events";
import StringDecoder from "wun:decoder";

export default class Stream extends EventEmitter {
	constructor(fd) {
		super();

		this.decoder=new StringDecoder();
		this.fd=fd;
		sys.watch(this.fd,sys.G_IO_IN,this.onFdData);
		sys.watch(this.fd,sys.G_IO_HUP,this.onFdHup);
		sys.watch(this.fd,sys.G_IO_ERR,this.onFdErr);
	}

	onFdData=()=>{
		let data=sys.readCharCodeArray(this.fd,4066);
		if (data===null) {
			let s=this.decoder.end();
			if (s)
				this.emit("data",s);

			sys.close(this.fd);
			this.emit("close");
			return;
		}

		let s=this.decoder.write(data);
		if (s)
			this.emit("data",s);

		sys.watch(this.fd,sys.G_IO_IN,this.onFdData);
	}

	onFdHup=()=>{
		//console.log("HUP, will read some more...");
		try {
			let data=true;
			do {
				data=sys.readCharCodeArray(this.fd,4096);
				let s=this.decoder.write(data);
				if (s)
					this.emit("data",s);
			} while (data!==null)

			let s=this.decoder.end();
			if (s)
				this.emit("data",s);

			sys.close(this.fd);
			this.emit("close");
		}

		catch (e) {
			console.log("hup: err: "+e.message);
		}
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
//	console.log(sys.O_RDONLY|sys.O_NONBLOCK);

	let fd=sys.open(fn,sys.O_RDONLY|sys.O_NONBLOCK);

	return new Stream(fd);
}
