#include "SC_PlugIn.h"
#include <cstdint>

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct ESX8CVEncoder : public Unit {
    // declare state variables here.
    int phase = x->m_phase;
    uint32_t value = x->m_value;
};

//function declarations 
static void ESX8CVEncoder_next(ESX8CVEncoder* unit, int inNumSamples);
static void ESX8CVEncoder_Ctor(ESX8CVEncoder* unit);

// the constructor function is called when a Synth containing this ugen is played.
// it MUST be named "PluginName_Ctor", and the argument must be "unit."
void ESX8CVEncoder_Ctor(ESX8CVEncoder* unit) {
    // initialize state variables here.
    // set a calculation function. for now, we only have one calculation function.
    SETCALC(ESX8CVEncoder_next);
    // calculate one sample of output.
    // if you don't do this, downstream ugens might access garbage memory in their Ctor functions.
    ESX8CVEncoder_next(unit, 1);
}

// the calculation function can have any name, but this is conventional. the first argument must be "unit."
// this function is called every control period (64 samples is typical)
// Don't change the names of the arguments, or the helper macros won't work.
void ESX8CVEncoder_next(ESX8CVEncoder *unit, int inNumSamples)
{

    // Phase Tracker
    //  use bitwise math to extract two different counters from a single phase number
    int state = (phase >> 1) & 3;
    int dac = (phase >> 3) & 7;

    // scsynth saves memory by aliasing wire buffers. In this case, "out" and "left" are the same. You should either
    // be mindful of this behavior or turn it off in the PluginLoad section.
    const float *ESX8CVdac1 = IN(0); // first header (ES-5 main panel)
    const float *ESX8CVdac2 = IN(1); // second header
    const float *ESX8CVdac3 = IN(2); // third header
    const float *ESX8CVdac4 = IN(3); // fourth header
    const float *ESX8CVdac5 = IN(4); // fifth header
    const float *ESX8CVdac6 = IN(5); // sixth header

    float *ES5headerX = OUT(0); //  send to ES5

    // Loop through samples and do the computation.
    for (int i = 0; i < inNumSamples; i++)
    {
        // Sampling the Input (State 0)
        if (state == 0)
        {
            double s = values[dac];
            value = 2048 + (SInt32)(std::max(-2048.0, std::min(2047.0, s)));
        }


        //Tick through phases (no phase 4)
        /*  ***pd version makes heavy use of side effcts:
         *  phase += phaseInc;
		 *  if ( ( phase & 7 ) == 6 )
		 *  	phase += 2;
		 *  phase = phase & 63; */

        const uint32_t low3Mask = 7;
        const uint32_t skippedResidue = 6;
        const uint32_t skipAmount = 2;
        const uint32_t phaseWrapMask = 63;

        uint32_t nextPhase = phase + phaseInc;
        if ((nextPhase & low3Mask) == skippedResidue) {
            nextPhase += skipAmount;
        }
        phase = nextPhase & phaseWrapMask;




        //Bit Slicer for Output
        /*  *** from pd (obfuscated code? or just really compact?):
         *  uint32_t out = ( state == 0 ) ? ( 0x80 | ( value & 0x1f ) )
         *  	: ( ( state == 1 ) ? ( ( value >> 5 ) & 0x1f )
         *  	   : ( ( ( dac > 3 ) ? 0x40 : 0x20 ) | ( value >> 10 ) | ( ( dac & 3 ) << 2 ) ) ); */

        uint32_t out;
        if (state == 0)
        { // first packet marker
            uint32_t low5bits = value & 0x1f;
            out = 0x80 | low5bits; // take the lowest 5 bits of value,OR with 0x80 - CV data
        }
        else if (state == 1)
        { // second 5 bits - CV data
            uint32_t mid5bits = (value >> 5) & 0x1f;
            out = mid5bits; // shift value to right by 5, take next 5 bits
        }
        else
        {
            uint32_t topRemainingBits;
            uint32_t dacIDbits;
            uint32_t dacGroupBit; // sets one identifying bit depending on whether dac is
            if (dac > 3)
            {                       // 4,5,6,7  → use 0x40
                dacGroupBit = 0x40; // i.e. 0100 0000
            }
            else
            {                       // 0,1,2,3  → use 0x20
                dacGroupBit = 0x20; // i.e. 0010 0000
            }

            dacIDbits = (dac & 3) << 2;     // ID for 0/1/2/3 or 4/5/6/7 depending on group
            topRemainingBits = value >> 10; // shift value to right by 10, take next 5 bits - CV data

            out = topRemainingBits | dacGroupBit | dacIDbits;
        }

        ES5headerX[i] = (float)out;

    }
    //somehow save the new state? re phase and value
}

// the entry point is called by the host when the plug-in is loaded
PluginLoad(ESPlugins) {
    // InterfaceTable *inTable implicitly given as argument to the load function
    ft = inTable; // store pointer to InterfaceTable
    // DefineSimpleUnit is one of four macros defining different kinds of ugens
//******DON"T FORGET!!!!  DefineSimpleUnit(ESX8CVEncoder);  //comment out if need VS code to check errors
}