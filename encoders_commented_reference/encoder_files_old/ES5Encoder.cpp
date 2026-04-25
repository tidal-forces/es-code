#include "SC_PlugIn.h"
#include <cstdint>

// InterfaceTable contains pointers to functions in the host (server)
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct es5Encoder : public Unit {
};

//function declarations 
static void es5Encoder_next(es5Encoder* unit, int inNumSamples);
static void es5Encoder_Ctor(es5Encoder* unit);

// the constructor function is called when a Synth containing this ugen is played
static void es5Encoder_Ctor(es5Encoder* unit) {
    // set a calculation function
    SETCALC(es5Encoder_next);
    // calculate one sample of output
    es5Encoder_next(unit, 1);
}
//**********_HELPERS_********************************************************************************

//clampByte helper: ensure the incoming signals are kept within 0 and 255 
static inline uint32_t clampByte(float x) {
    if (x < 0.f) {
        return 0;
    }
    if (x > 255.f) {
        return 255;
    }
    return (uint32_t)x;
}

//bit-to-float conversion helper: trick audio interface DAC into outputting exact 24-bit digital word
static inline float bitsToFloat24(uint32_t bits) {
    const float btf_neg_factor = -(float)0x800000;			
    const float btf_pos_factor = (float)0x800000;
    if ( bits & 0x800000u ) {            //check if top bit set
        return ((float)(0x00ffffffu&(-(int32_t)(bits)))) / btf_neg_factor;
    } else {
        return ((float)(bits)) / btf_pos_factor;
    } 
}

//**********_CALCULATION_FXN_************************************************************************

// this function is called every control period 
static void es5Encoder_next(es5Encoder* unit, int inNumSamples) {

    // IN and OUT are helper macros that return audio-rate input and output buffers
    const float *es5header1 = IN(0); // first header (es-5 main panel)
    const float *es5header2 = IN(1); // second header
    const float *es5header3 = IN(2); // third header
    const float *es5header4 = IN(3); // fourth header
    const float *es5header5 = IN(4); // fifth header
    const float *es5header6 = IN(5); // sixth header

    float *es3ch7 = OUT(0); //  "left" output, ie ch7, drives headers 1,2,3
    float *es3ch8 = OUT(1); //  "right" output, ie ch8, drives headers 4,5,6

    // Loop through samples and do the computation. *for* loop given in template
    for (int i = 0; i < inNumSamples; i++) {

        //Clamp: ensure the incoming signals are kept within 0 and 255 (an 8-bit byte) 
        uint32_t h1 = clampByte(es5header1[i]);
        uint32_t h2 = clampByte(es5header2[i]);
        uint32_t h3 = clampByte(es5header3[i]);
        uint32_t h4 = clampByte(es5header4[i]);
        uint32_t h5 = clampByte(es5header5[i]);
        uint32_t h6 = clampByte(es5header6[i]);

        //Bit Packing: create the 24-bit word (N.B. 3 for each of ch7/left and ch8/right)
        uint32_t bitsL = ( h1 << 16 ) | ( h2 << 8 ) | h3;
		uint32_t bitsR = ( h4 << 16 ) | ( h5 << 8 ) | h6; 

        //DSP Loop: process audio in "blocks" (arrays of samples) rather than one sample at a time
        //Bits to float: Interpret the 24-bit word as signed 24-bit PCM and normalize to [-1, 1).
        es3ch7[i] = bitsToFloat24( bitsL );
		es3ch8[i] = bitsToFloat24( bitsR );
    }
}

//**********_FOR_UGEN_********************************************************************************

// the entry point is called by the host when the plug-in is loaded
PluginLoad(esPlugins) {
    ft = inTable; // store pointer to InterfaceTable
    DefineSimpleUnit(es5Encoder);
}