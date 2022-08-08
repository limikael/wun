import fs from "wun:fs";
console.log("i'm running...");

let el=document.createElement("textarea");
el.style.width="100%";
el.style.height="24em";
document.body.appendChild(el);

el.value="Testing stufff...\n";

console.log("textarea created...");

el.value+=fs.readFileSync("test/myfile.txt");

console.log("done...")
/*console.log("read file: "+fs.readFileSync("/home/micke/Repo.lab/wun/test/myfile.txt"));

let fildes;


console.log("Starting...");

fildes=sys.open("/home/micke/Repo.lab/wun/test/fifo");

function onRead(fd, cond) {
	let data=sys.read(fildes,1024);
	console.log("read, fd="+fd+" cond="+cond+" data="+String(data).trim());
	el.value+=data;
	//document.write(data);
	sys.watch(fildes,sys.G_IO_IN,onRead);
}

sys.watch(fildes,sys.G_IO_IN,onRead);*/
