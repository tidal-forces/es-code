# TODO: *nice intro*

# intro
we've all dreamed of controlling our modular rigs with tidal cycles at least once.

here is an intermediate step that allows you to transform your CV and gate output into actual voltages, via Expert Sleepers' <link, description...>

overall picture...
```
Tidal Cycles
  │
  ▼
SuperDirt (SC)
  │
  ├── channels 1-6 ──► ES-3 direct
  │                      └── CV/gate (superdirt-voltage)
  │                      └── audio out (superdirt samples)
  │
  ├── channel 7 ──► ES5Encoder UGen ──► ES-5 headers 1/2/3
  │                      │              └── ESX-8GT (pseudo UGen)
  │                      │              └── ESX8CVEncoder (UGen)
  └── channel 8 ──► ES5Encoder UGen ──► ES-5 headers 4/5/6
  │                      │              └── ESX-8GT (pseudo UGen)
  │                      │              └── ESX8CVEncoder (UGen)
```

overall setup process...

technical description...

# directory listing

/compile_me_USE_ME_:
* cpp and sclang source for ES-5 encoder, ESX-8CV encoder and ESX-8GT encoder
* **use these** if you wish to compile your own encoder!

/encoders_commented_reference:
* cpp encoder source code with backgound comments including ported pd code and sc ugen boilerplate
* best reference to understand function of the encoders

/my_machines_compiled_binaries: 
* scx files for ugen compiled for my machine

/notes_for_livecode:
* various code snippets for routing the ugens/ ugens from other repos relevant to tidal cycles rig setup
* sclang and haskell

/cat.gif:
* me most of the time but somehow the above worked

/README.md
* the file you are looking at


# compiling locally
to allow sc to use the ugen (my case: mac, older version gui sc) there are a few more steps to take:

## compile the cpp files to supercollider libraries

MAC (and windows?)

1. confirm the supercollider version installed on your computer!! (could be gui, whatever)
2. git clone a supercollider source 
3. git checkout the version you use (and thusly wish to compile your ugen for)
4. go to wherever you've put the encoders' cpp and sc source files
5. compile as below (e.g. for ES5Encoder, and given I had cloned sc source into my home directory)

```
clang++ -O2 -shared -fPIC \
                          -o ES5Encoder.scx \
                          ES5Encoder.cpp \
                          -I$HOME/supercollider/include/plugin_interface \
                          -I$HOME/supercollider/include/common \
                          -std=c++17
```

LINUX

1. confirm the supercollider version installed on your computer!! (could be gui, whatever)
2. git clone a supercollider source 
3. git checkout the version you use (and thusly wish to compile your ugen for)
4. go to wherever you've put the encoders' cpp and sc source files
5. TODO_ cmake makefile something somthing _WIP