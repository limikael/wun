import EventEmitter from "wun:events";
import Stream from "wun:stream";

export class Subprocess extends EventEmitter {
	constructor(cmd, params=[]) {
		super();
		this.cmd=cmd;
		this.params=params;
		this.run();
	}

	run() {
		let stdout=sys.pipe();
		this.pid=sys.fork();

		// Child
		if (!this.pid) {
			sys.dup2(stdout[1],sys.STDOUT_FILENO);
			sys.exec(this.cmd,this.params);
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

export function spawn(cmd, params=[]) {
	return new Subprocess(cmd,params);
}

export function call(cmd, params=[], options={}) {
	if (!options.expect)
		options.expect=0;

	let subprocess=spawn(cmd,params,options);
	return new Promise((resolve, reject)=>{
		let result="";

		subprocess.on("data",(data)=>{
			result+=data;
		});
		subprocess.on("close",(status)=>{
			if (status==options.expect)
				resolve(result);

			else
				reject(status);
		});
	});
}

export default {
	spawn,
	call
}