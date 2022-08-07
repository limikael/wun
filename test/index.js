let fildes;
let el=document.createElement("textarea");

window.addEventListener("watch",()=>{
	let s=sys.read(fildes,1024);
	console.log("watch="+fildes+": "+s.trim());

	el.value+=s;
	el.scrollTop=el.scrollHeight;

	sys.watch(fildes);
});

async function main(fn) {
	fildes=sys.open("/home/micke/Repo.lab/wun/test/fifo");

	sys.watch(fildes);
}

main();

el.style.width="100%";
el.style.height="24em";
el.value="Testing stufff...\n";

document.body.appendChild(el);
console.log("Starting...");