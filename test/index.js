let fildes;
let el=document.createElement("textarea");
el.style.width="100%";
el.style.height="24em";
el.value="Testing stufff...\n";

document.body.appendChild(el);

console.log("Starting...");

fildes=sys.open("/home/micke/Repo.lab/wun/test/fifo");

function onRead(fd, cond) {
	console.log("on read, fd="+fd+" cond="+cond);

	let data=sys.read(fildes,1024);
	console.log("read: "+String(data).trim());
	el.value+=data;
	document.write(data);
	sys.watch(fildes,sys.G_IO_IN,onRead);
}

sys.watch(fildes,sys.G_IO_IN,onRead);
