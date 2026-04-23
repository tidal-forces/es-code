#include "SC_PlugIn.h"
#include <cstdint>

//=============UGEN=BOILERPLATE=================================================================================

// InterfaceTable contains pointers to functions in the host (server)
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct ESX8CVEncoder : public Unit {
    // declare state variables here.
    int mPhase;
    uint32_t mValue;
};

// function declarations
static void ESX8CVEncoder_next(ESX8CVEncoder *unit, int inNumSamples);
static void ESX8CVEncoder_Ctor(ESX8CVEncoder *unit);

// the constructor function is called when a Synth containing this ugen is played
static void ESX8CVEncoder_Ctor(ESX8CVEncoder *unit) {
    // initialize state variables here.
    unit->mPhase = 0;
    unit->mValue = 0;
    // set a calculation function
    SETCALC(ESX8CVEncoder_next);
    // calculate one sample of output
    ESX8CVEncoder_next(unit, 1);
}

//=============HELPER=FXNS=================================================================================

// Clamp helper. ensures CV data is 12-bit (3 bytes per DAC = 24 transmitted bits - overhead)
static inline uint32_t clampValueTo12BitCV(double s) {
    if (s < -2048.0) {
        s = -2048.0;
    } else if (s > 2047.0) {
        s = 2047.0; 
    }

    return 2048 + (int32_t)s;
}

// Bit slicer helper
static inline uint32_t encodeOutputBits(int state, uint32_t value, int dac) {
    if (state == 0) {                   // first packet marker
        uint32_t low5bits = value & 0x1f;
        return 0x80 | low5bits;         // take the lowest 5 bits of value,OR with 0x80 - CV data
    } else if (state == 1) {            // second 5 bits - CV data
        uint32_t mid5bits = (value >> 5) & 0x1f;
        return mid5bits;                // shift value to right by 5, take next 5 bits
    } else {
        uint32_t topRemainingBits;
        uint32_t dacIDbits;
        uint32_t dacGroupBit;           // sets one identifying bit depending on whether dac is
        if (dac > 3) {                  // 4,5,6,7  → use 0x40
            dacGroupBit = 0x40;         // i.e. 0100 0000
        } else {                        // 0,1,2,3  → use 0x20
            dacGroupBit = 0x20;         // i.e. 0010 0000
        }
        dacIDbits = (dac & 3) << 2;     // ID for 0/1/2/3 or 4/5/6/7 depending on group
        topRemainingBits = value >> 10; // shift value to right by 10, take next 5 bits - CV data
        return topRemainingBits | dacGroupBit | dacIDbits;
    }
}

// helper to tick through phases (N.B.: no phase 4)
static inline int returnNextPhase(int phase, int phaseInc) {
    const uint32_t low3Mask = 7;
    const uint32_t skippedResidue = 6;
    const uint32_t skipAmount = 2;
    const uint32_t phaseWrapMask = 63;

    uint32_t nextPhase = phase + phaseInc;
    if ((nextPhase & low3Mask) == skippedResidue) {
        nextPhase += skipAmount;
    }
    return nextPhase & phaseWrapMask;
}

//=============CALCULATION=FXN=================================================================================

// this function is called every control period 
static void ESX8CVEncoder_next(ESX8CVEncoder *unit, int inNumSamples) {

    int phase = unit->mPhase;
    uint32_t value = unit->mValue;

    // IN and OUT are helper macros that return audio-rate input and output buffers
    const float *ESX8CVdac1 = IN(0); // first dac channel (ESX-8CV jack labeled "1")
    const float *ESX8CVdac2 = IN(1); // second dac ch
    const float *ESX8CVdac3 = IN(2); // third dac ch
    const float *ESX8CVdac4 = IN(3); // fourth dac ch
    const float *ESX8CVdac5 = IN(4); // fifth dac ch
    const float *ESX8CVdac6 = IN(5); // sixth dac ch
    const float *ESX8CVdac7 = IN(6); // seventh dac ch
    const float *ESX8CVdac8 = IN(7); // eighth dac ch

    float *ES5headerX = OUT(0); //  send to ES5

    double values[8];

    // set phase to even number
    phase = phase & ~1;
    const int phaseInc = 2;

    // loop through samples and do the computation for out
    for (int i = 0; i < inNumSamples; i++) {

        values[0] = ESX8CVdac1[i];
        values[1] = ESX8CVdac2[i];
        values[2] = ESX8CVdac3[i];
        values[3] = ESX8CVdac4[i];
        values[4] = ESX8CVdac5[i];
        values[5] = ESX8CVdac6[i];
        values[6] = ESX8CVdac7[i];
        values[7] = ESX8CVdac8[i];

        // Phase tracker, use bitwise math to extract two counters from single phase number
        int state = (phase >> 1) & 3;
        int dac = (phase >> 3) & 7;

        // Sample input (State 0)
        if (state == 0) {
            value = clampValueTo12BitCV(values[dac]);
        }

        // Bit slicer to output
        uint32_t outBits = encodeOutputBits(state, value, dac);
        ES5headerX[i] = (float)outBits;

        // Tick to next phase
        phase = returnNextPhase(phase, phaseInc);

    }
    // save the new ugen state re: phase and value
    unit->mPhase = phase;
    unit->mValue = value;
}

//=============UGEN=BOILERPLATE=================================================================================

// the entry point is called by the host when the plug-in is loaded
PluginLoad(ESPlugins) {
    ft = inTable; // store pointer to InterfaceTable
    DefineSimpleUnit(ESX8CVEncoder);  //comment out if interfering w/ VS code error highlighting
}