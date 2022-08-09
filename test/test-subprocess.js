import {spawn} from "wun:subprocess";

let el=document.createElement("textarea");
el.style.width="100%";
el.style.height="24em";
document.body.appendChild(el);
el.value+="Testing subprocess...\n";

let subprocess=spawn("./test/date100");
subprocess.on("data",(data)=>{
	//console.log(data);
	el.value+=data;
});
subprocess.on("close",(status)=>{
	el.value+="Process closed: "+status+"\n";
});
