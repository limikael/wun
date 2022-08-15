import StringDecoder from "wun:decoder";

export function readFileSync(fn) {
	let fd=sys.open(fn,sys.O_RDONLY);
	let decoder=new StringDecoder();
	let data,res="";

	do {
		data=sys.readCharCodeArray(fd,4096);
		res+=decoder.write(data);
	} while (data!==null);

	sys.close(fd);
	res+=decoder.end();

	return res;
}

export default {
	readFileSync
}