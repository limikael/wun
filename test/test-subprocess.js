import {spawn} from "wun:subprocess";

//import {fopen} from "wun:stream";

let el=document.createElement("textarea");
el.style.width="100%";
el.style.height="24em";
document.body.appendChild(el);
el.value+="Testing subprocess...\n";

/*let subprocess=spawn("/bin/ls");
subprocess.on("data",(data)=>{
	el.value+=data;
});
subprocess.on("close",()=>{
	el.value+="Process closed...\n";
});*/
