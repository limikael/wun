import fs from "wun:fs";
console.log("i'm running...");

/*let el=document.createElement("textarea");
el.style.width="100%";
el.style.height="24em";
document.body.appendChild(el);

el.value="Testing stufff...\n";

console.log("textarea created...");*/

let v=fs.readFileSync("test/installout.txt");
console.log(v);
//el.value+=

console.log("done...")
