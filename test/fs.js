export function readFileSync(fn) {
	let fd=sys.open(fn);
	let data=sys.read(fd,10);
	sys.close(fd);

	return data;
}

export default {
	readFileSync
}