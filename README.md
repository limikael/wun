# WUN

Wun runs JavaScript in a window. In this way it can be said to be similar to electron or tauri. However, these programs are based on the concept of a front end, which has access to the DOM, and a backend which has access to filesystem etc. As an experiment, I wanted to create something where you can mix DOM access and system access in the same code. Therefore, in wun there is just a single entrypoint to a JavaScript file, and from this file you have access to both the DOM and the raw posix api. Here is an example program that reads a file an displays it in the browser:

    let enc=new TextDecoder("utf-8");
    let fd=sys.open("README.md",sys.O_RDONLY);
    document.body.innerHTML=enc.decode(sys.read(fd,10000));
    sys.close(fd);
