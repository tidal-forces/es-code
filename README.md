#TODO: nice intro

#directory listing

compiled_binaries_my_machine: 
-scx files for ugen compiled for my machine

encoder_files:
-cpp and sclang source for ES-5 encoder, ESX-8CV encoder and ESX-8GT encoder
-use these!

encoders_commented_reference:
-cpp encoder source code with backgound comments including ported pd code and sc ugen boilerplate
-best reference to understand function of the encoders

notes_for_livecode:
-various code snippets for routing the ugens/ ugens from other repos relevant to tidal cycles rig setup
-sclang and haskell

cat.gif:
-me most of rhe time but somehow the above worked

README.md
-the file you are looking at


#to compile the ugen (mac, sc older version 3.13.0):

1. confirm the supercollider version installed on your computer!! (could be gui, whatever)
2. git clone the supercollider source 
3. git checkout the version you use (and thusly wish to compile you ugen for)
4. go to whichever directory you've put the encoders' the cpp and sc 
5. given I cloned sc source into my home directory, compile as below (e.g. for ES5Encoder)

clang++ -O2 -shared -fPIC \
                          -o ES5Encoder.scx \
                          ES5Encoder.cpp \
                          -I$HOME/supercollider/include/plugin_interface \
                          -I$HOME/supercollider/include/common \
                          -std=c++17
