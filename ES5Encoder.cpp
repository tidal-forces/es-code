#include "SC_PlugIn.h"
#include <cstdint>

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
static void ES5Encoder_Ctor(ES5Encoder* unit) {
    // initialize state variables here.
    // set a calculation function. for now, we only have one calculation function.
    SETCALC(ES5Encoder_next);
    // calculate one sample of output.
    // if you don't do this, downstream ugens might access garbage memory in their Ctor functions.
    ES5Encoder_next(unit, 1);
}
//**********_HELPERS_*************************************************************************************

//clampByte helper: ensure the incoming signals are kept strictly between 0 and 255 (NB uint32 stil preferred
//due to subsequent bitwise ops) and convert them to integers (uint32). 
static inline uint32_t clampByte(float x) {
    if (x < 0.f) {
        return 0;
    }
    if (x > 255.f) {
        return 255;
    }
    return static_cast<uint32_t>(x);
}

//bit-to-float conversion helper: trick audio interface DAC into outputting the exact 24-bit digital word
//instead of normal 32-bit floating-point numbers (-1.0 to 1.0)
// ***seems the pd version used preprocessor macros for this!!!
/*  #define ES_BITSTOFLOAT_SETUP()								\
 *  	float btf_n_factor, btf_p_factor;						\
 *  	btf_n_factor = -(float)0x800000;			btf_p_factor = (float)0x800000;

 *  #define ES_BITSTOFLOAT( bits )	\
 *  	( ( bits & 0x800000 ) ? ( (((float)(0xffffff&(-(SInt32)(bits)))) / btf_n_factor ) ) : ( ((float)(bits)) / btf_p_factor ) ) */
static inline float bitsToFloat24(uint32_t bits) {
    const float btf_n_factor = -(float)0x800000;			
    const float btf_p_factor = (float)0x800000;
    if ( bits & 0x800000u ) {
        return ((float)(0x00ffffffu&(-(int32_t)(bits)))) / btf_n_factor;
    } else {
        return ((float)(bits)) / btf_p_factor;
    } 
}

//**********_CALCULATION_FXN_*************************************************************************************

// the calculation function can have any name, but this is conventional. the first argument must be "unit."
// this function is called every control period (64 samples is typical)
// Don't change the names of the arguments, or the helper macros won't work.
static void ES5Encoder_next(ES5Encoder* unit, int inNumSamples) {

    // IN and OUT are helper macros that return audio-rate input and output buffers. These are known as "wire buffers."

    // scsynth saves memory by aliasing wire buffers. In this case, "out" and "left" are the same. You should either
    // be mindful of this behavior or turn it off in the PluginLoad section.
    const float *in1 = IN(0); // first header (ES-5 main panel)
    const float *in2 = IN(1); // second header
    const float *in3 = IN(2); // third header
    const float *in4 = IN(3); // fourth header
    const float *in5 = IN(4); // fifth header
    const float *in6 = IN(5); // sixth header

    float *outES3_7 = OUT(0); //  "left" output, ie ch7, drives headers 1,2,3
    float *outES3_8 = OUT(1); //  "right" output, ie ch8, drives headers 4,5,6

    // Loop through samples and do the computation. *for* loop given in template
    // ***pd version uses a *while* loop
    //      while (n--) & uses pointer arithmetic (*in1++ and *dstpL++)
    for (int i = 0; i < inNumSamples; i++) {

        //Clamp: ensure the incoming signals are kept strictly between 0 and 255 (an 8-bit byte) 
        //and convert them to integers. ***pd:
        /*  UInt32 out1 = (UInt32)( std::max( 0.0f, std::min( 255.0f, *in1++ ) ) );
		 *  UInt32 out2 = (UInt32)( std::max( 0.0f, std::min( 255.0f, *in2++ ) ) );
		 *  UInt32 out3 = (UInt32)( std::max( 0.0f, std::min( 255.0f, *in3++ ) ) );
		 *  UInt32 out4 = (UInt32)( std::max( 0.0f, std::min( 255.0f, *in4++ ) ) );
		 *  UInt32 out5 = (UInt32)( std::max( 0.0f, std::min( 255.0f, *in5++ ) ) );
		 *  UInt32 out6 = (UInt32)( std::max( 0.0f, std::min( 255.0f, *in6++ ) ) ); */
        //this ugen will use a clampByte helper
        uint32_t h1 = clampByte(in1[i]);
        uint32_t h2 = clampByte(in2[i]);
        uint32_t h3 = clampByte(in3[i]);
        uint32_t h4 = clampByte(in4[i]);
        uint32_t h5 = clampByte(in5[i]);
        uint32_t h6 = clampByte(in6[i]);

        //Bit Packing: create the 24-bit word (N.B. 3 for each of ch7/left and ch8/right)
        //Input 1 is shifted left by 16 bits (filling the top 8 bits).
        //Input 2 is shifted left by 8 bits (filling the middle 8 bits).
        //Input 3 stays where it is (filling the bottom 8 bits).
        //***pd:
		/*  SInt32 bitsL = ( out1 << 16 ) | ( out2 << 8 ) | out3;
		 *  SInt32 bitsR = ( out4 << 16 ) | ( out5 << 8 ) | out6; */
        uint32_t bitsL = ( h1 << 16 ) | ( h2 << 8 ) | h3;
		uint32_t bitsR = ( h4 << 16 ) | ( h5 << 8 ) | h6; 

        //DSP Loop: process audio in "blocks" (arrays of samples) rather than one sample at a time
        // ***pd uses pointer arithmetic (*in1++ / *dstpL++) moving pointer forward in memory w/ while loop
        /*  float floatL = ES_BITSTOFLOAT( bitsL );
		 *  float floatR = ES_BITSTOFLOAT( bitsR );
		 *  *dstpL++ = floatL;
		 *  *dstpR++ = floatR; */    
        outES3_7[i] = bitsToFloat24( bitsL );
		outES3_8[i] = bitsToFloat24( bitsR );

        
    }
}

//**********_FOR_UGEN_*************************************************************************************

// the entry point is called by the host when the plug-in is loaded
PluginLoad(ESPlugins) {
    // InterfaceTable *inTable implicitly given as argument to the load function
    ft = inTable; // store pointer to InterfaceTable
    // DefineSimpleUnit is one of four macros defining different kinds of ugens
//***********COMMENTED OUT TO ALLOW IDE ERROR CHECKING    DefineSimpleUnit(ES5Encoder);
}