
WebAssembly demo for 80GFX library    - Notes Oct 1 2018
==================================

In this folder
--------------
This folder contains a compiled-to-WASM version of my 80GFX library, and within a small demo program.



How to just run this dammit  - Updated 20 July 2019
---------------------------
You "just run" the program by checking out this repo.

Recent browser security improvements mean you cannot just double-click the "index.html" 
and see the program running in your browser, so you will need to install a small local 
web server to serve this, for example:

- sudo npm install http-server -g

Then "cd" to the WebAssemblyDemo folder and start the HTTP server:

- cd WHEREVER/80GFX/WebAssemblyDemo
- http-server ./

The default location is "http://127.0.0.1:8080/" which you can view in your browser.



To build
--------
I did this on my Linux box, using the Emscripten emcc compiler, so you too will need Emscripten!


- ./setup.sh  Sets up the emcripten commands in the bash session.  Note this won't work on
              your system unless you install your Emscripten "emsdk" folder in the same
              location relative to this folder, like I have.

- ./build.sh  This runs the emcc compiler and, in lieu of doing anything more sophisticated,
              for this tiny project I have stuffed all the dependencies onto the command line.



References
==========

You will need to install the Emscripten compiler to generate a WASM executable.
Official Emscripten WIKI at time of writing:

http://kripken.github.io/emscripten-site/index.html

WebAssembly technical specification, in case you haven't seen this yet:

https://webassembly.org/

If you are after a minimal get-you-into-it-quick guide on using a C++ / SDL / WASM solution 
in the browser, check out Tim Hutton's github:

https://github.com/timhutton/sdl-canvas-wasm

Thanks Tim, saved me a ton of Googling!

