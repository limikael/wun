console.log("testing...");

import {fopen} from "wun:stream";

/*let el=document.createElement("textarea");
el.style.width="100%";
el.style.height="24em";
document.body.appendChild(el);

el.value="Testing stufff...\n";*/

let stream=fopen("test/installout.txt");
let tot=0;
stream.on("data",(data)=>{
	tot+=data.length;
//	el.value+=data;
});

