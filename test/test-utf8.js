class StringDecoder {
	constructor() {
		this.tmpBuffer=[];
		this.buffer="";
		this.decoder=new TextDecoder("utf-8");
	}

	writeCharCode(i) {
		this.tmpBuffer.push(i);
		this.flush();
	}

	write(s) {
		for (let i=0; i<s.length; i++)
			this.writeCharCode(s.charCodeAt(i));
	}

	flush() {
		for (let i=0; i<this.tmpBuffer.length; i++) {
			let s=this.decoder.decode(new Uint8Array(this.tmpBuffer.slice(0,i+1)));
			if (s.charCodeAt(0) && s.length==1 && s.charCodeAt(0)!=65533) {
				this.buffer+=s[0];
				this.tmpBuffer.splice(0,i+1);
				return;
			}
			if (s.length==2) {
				this.buffer+=s[0];
				this.tmpBuffer.splice(0,i);
				this.flush();
				return;
			}
		}
	}

	forceFlush() {
		this.buffer+=this.decoder.decode(new Uint8Array(this.tmpBuffer));
		this.tmpBuffer=[];
	}

	read() {
		let b=this.buffer;
		this.buffer="";
		return b;
	}

	getLength() {
		return this.buffer.length;
	}
}

let dec=new StringDecoder();
dec.write("\xe2\x96\x88e\xe2\x96\x88\xe2\x96\x88\xe2\x96\x88");
console.log(dec.read());
