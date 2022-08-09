export function readFileSync(fn) {
	let fd=sys.open(fn,sys.O_RDONLY);
	let data,res="";

	do {
		data=sys.read(fd,4096);
		if (data!==null)
			res+=data;

	} while (data!==null);

	sys.close(fd);
	return res;
}

export default {
	readFileSync
}