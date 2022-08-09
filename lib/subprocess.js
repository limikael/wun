import EventEmitter from "wun:events";
import Stream from "wun:stream";

export class Subprocess extends EventEmitter {
	constructor(cmd) {
		super();
		this.cmd=cmd;
		this.run();
	}

	run() {
		let stdout=sys.pipe();
		this.pid=sys.fork();

		// Child
		if (!this.pid) {
			sys.dup2(stdout[1],sys.STDOUT_FILENO);
			sys.exec(this.cmd);
			sys.exit(1);
		}

		// Parent
		else {
			sys.close(stdout[1]);
			this.stdout=new Stream(stdout[0]);
			this.stdout.on("close",this.onClose);
			this.stdout.on("data",(data)=>{
				this.emit("stdout",data);
				this.emit("data",data);
			});
		}
	}

	onClose=()=>{
		this.status=sys.waitpid(this.pid);
		this.emit("close",this.status);
	}
}

export function spawn(cmd) {
	return new Subprocess(cmd);
}

export default {
	spawn
}