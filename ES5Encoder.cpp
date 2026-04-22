#include "SC_PlugIn.h"

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct ES5Encoder : public Unit {
    // declare state variables here.
};

//function declarations 
static void ES5Encoder_next(ES5Encoder* unit, int inNumSamples);
static void ES5Encoder_Ctor(ES5Encoder* unit);

// the constructor function is called when a Synth containing this ugen is played.
// it MUST be named "PluginName_Ctor", and the argument must be "unit."
void ES5Encoder_Ctor(ES5Encoder* unit) {
    // initialize state variables here.
    // set a calculation function. for now, we only have one calculation function.
    SETCALC(ES5Encoder_next);
    // calculate one sample of output.
    // if you don't do this, downstream ugens might access garbage memory in their Ctor functions.
    ES5Encoder_next(unit, 1);
}


// the calculation function can have any name, but this is conventional. the first argument must be "unit."
// this function is called every control period (64 samples is typical)
// Don't change the names of the arguments, or the helper macros won't work.
void ES5Encoder_next(ES5Encoder* unit, int inNumSamples) {

    // IN and OUT are helper macros that return audio-rate input and output buffers. These are known as "wire buffers."

    // scsynth saves memory by aliasing wire buffers. In this case, "out" and "left" are the same. You should either
    // be mindful of this behavior or turn it off in the PluginLoad section.
    float *in1 = IN(0); // first header (ES-5 main panel)
    float *in2 = IN(1); // second header
    float *in3 = IN(2); // third header
    float *in4 = IN(3); // fourth header
    float *in5 = IN(4); // fifth header
    float *in6 = IN(5); // sixth header

    float *out7 = OUT(0); //  "left" output, ie ch7, drives headers 1,2,3
    float *out8 = OUT(1); //  "right" output, ie ch8, drives headers 4,5,6

    // Loop through samples and do the computation.
    for (int i = 0; i < inNumSamples; i++) {
        //out[i] = (left[i] + right[i]) * 0.5;
    }
}



// the entry point is called by the host when the plug-in is loaded
PluginLoad(ESPlugins) {
    // InterfaceTable *inTable implicitly given as argument to the load function
    ft = inTable; // store pointer to InterfaceTable
    // DefineSimpleUnit is one of four macros defining different kinds of ugens
//COMMENTED TO ALLOW FURTHER ERROR CHECKING    DefineSimpleUnit(ES5Encoder);
}