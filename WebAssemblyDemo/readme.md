
WebAssembly demo for 80GFX library    - Notes Oct 1 2018
==================================

In this folder
--------------
This folder contains a compiled-to-WASM version of my 80GFX library, and within a small demo program.


How to just run this dammit
---------------------------
You run the program by checking out this repo, then double-click the index.html.  The demo
program is C++ compiled to WebAssembly, and runs sandboxed in your browser.  In case you are
old, this does NOT mean "ActiveX controls" are used  ;)


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

