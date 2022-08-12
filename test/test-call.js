import {call} from "wun:subprocess";

let el=document.createElement("textarea");
el.style.width="100%";
el.style.height="24em";
document.body.appendChild(el);
el.value+="Testing subprocess...\n";

async function main() {
	el.value+=await call("/bin/ls",["/"]);
}

main();
