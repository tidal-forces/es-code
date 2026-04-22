TODO: intro, use


compiling ugen (mac, sc older version 3.13.0):

clang++ -O2 -shared -fPIC \
                          -o ES5Encoder.scx \
                          ES5Encoder.cpp \
                          -I$HOME/supercollider/include/plugin_interface \
                          -I$HOME/supercollider/include/common \
                          -std=c++17
