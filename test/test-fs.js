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
