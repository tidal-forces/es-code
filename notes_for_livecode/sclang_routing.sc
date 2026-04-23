(
{
    var myCVs, esxStream, es5Left, es5Right;

    // -----------------------------------------------------------
    // STEP 1: Create your 8 CV signals 
    // (We'll use 8 sine waves running at different LFO speeds)
    // -----------------------------------------------------------
    myCVs = [
        SinOsc.ar(0.1),  // CV 1
        SinOsc.ar(0.2),  // CV 2
        SinOsc.ar(0.3),  // CV 3
        SinOsc.ar(0.4),  // CV 4
        SinOsc.ar(0.5),  // CV 5
        SinOsc.ar(0.6),  // CV 6
        SinOsc.ar(0.7),  // CV 7
        SinOsc.ar(0.8)   // CV 8
    ];

    // -----------------------------------------------------------
    // STEP 2: Encode the 8 CVs into a single multiplexed data stream
    // -----------------------------------------------------------
    // The *myCVs syntax simply unpacks the array of 8 signals 
    // into the 8 arguments of the UGen.
    esxStream = ESX8CVEncoder.ar(*myCVs);

    // -----------------------------------------------------------
    // STEP 3: Plug the ESX-8CV stream into the ES-5 Encoder
    // -----------------------------------------------------------
    // The ES5Encoder expects 6 inputs (the 6 hardware headers).
    // Let's pretend your physical ESX-8CV is plugged into Header 1.
    // We send esxStream to input 1, and send 0 to the empty headers.
    
    // (Assuming your ES5Encoder outputs an array of [left, right])
    # es5Left, es5Right = ES5Encoder.ar(
        esxStream, // Header 1 (Our ESX-8CV)
        0,         // Header 2 (Empty)
        0,         // Header 3 (Empty)
        0,         // Header 4 (Empty)
        0,         // Header 5 (Empty)
        0          // Header 6 (Empty)
    );

    // -----------------------------------------------------------
    // STEP 4: Send to the Audio Interface
    // -----------------------------------------------------------
    // Send the fake, bit-packed "audio" out via ADAT/SPDIF
    // (Assuming your ES-5 is on hardware outputs 6 & 7)
    Out.ar(6, [es5Left, es5Right]); 

}.play;
)

# es5Left, es5Right = ES5Encoder.ar(
    ESX8CVEncoder.ar(*drumCVs),     // Header 1
    ESX8CVEncoder.ar(*synthCVs),    // Header 2
    ESX8CVEncoder.ar(*seqCVs),      // Header 3
    0, 0, 0
);



(
{
    var myGates, esxGateStream, es5Left, es5Right;

    // 1. Create 8 rhythmic triggers/gates
    myGates = [
        Impulse.ar(4),   // Gate 1: 16th notes
        Impulse.ar(2),   // Gate 2: 8th notes
        Impulse.ar(1),   // Gate 3: Quarter notes
        0, 0, 0, 0, 0    // Gates 4-8: Off
    ];

    // 2. Encode the 8 gates into a single 0-255 stream
    esxGateStream = ESX8GTEncoder.ar(*myGates);

    // 3. Plug it into the ES-5! 
    // Let's pretend the 8GT is on Header 2 this time.
    # es5Left, es5Right = ES5Encoder.ar(
        0,              // Header 1 (Empty)
        esxGateStream,  // Header 2 (Our ESX-8GT!)
        0, 0, 0, 0      // Headers 3-6
    );

    // 4. Send to Audio Interface (ADAT/SPDIF)
    Out.ar(6, [es5Left, es5Right]); 

}.play;
)



// 1. Allocate 8 invisible audio buses in SC's memory
~cvBuses = Array.fill(8, { Bus.audio(s, 1) });

// 2. Your continuous hardware router
SynthDef(\es5_router, {
    var cv1 = In.ar(~cvBuses[0].index, 1);
    var cv2 = In.ar(~cvBuses[1].index, 1);
    // ... etc ...
    
    var esxStream = ESX8CVEncoder.ar(cv1, cv2, /* ... */);
    var es5L, es5R;
    # es5L, es5R = ES5Encoder.ar(esxStream, 0, 0, 0, 0, 0);
    
    Out.ar(6, [es5L, es5R]); // Send to ADAT continuously
}).play;


SynthDef(\tidal_cv_ping, { |out, sustain, cv_val, port=0|
    // Write the voltage to the correct bus
    // using ReplaceOut to overwrite whatever is currently there
    ReplaceOut.ar(~cvBuses[port].index, K2A.ar(cv_val));
}).add;



//or
SynthDef(\es5gate, { |out=6, g1=0, g2=0, g3=0, g4=0, g5=0, g6=0, g7=0, g8=0|
    var encoded = (g1*1)+(g2*2)+(g3*4)+(g4*8)+(g5*16)+(g6*32)+(g7*64)+(g8*128);
    var signal = ES5Encoder.ar(encoded, 0, 0, 0, 0, 0);
    Out.ar(out, signal);
}).add;






// ---------------------------------------------------------
// 1. Allocate 8 contiguous Control Buses (Our S&H memory)
// ---------------------------------------------------------
~cvBusBase = Bus.control(s, 8);
// Initialize them all to 0 volts so your modular stays quiet on boot
~cvBusBase.setAll(0.0);

// ---------------------------------------------------------
// 2. The Persistent Hardware Router (Runs 24/7)
// ---------------------------------------------------------
SynthDef(\es5_router, {
    var cvAudio, esxStream, es5L, es5R;

    // Read the 8 control buses, and convert them to audio rate (K2A)
    // This gives us 8 continuous, held audio signals.
    cvAudio = K2A.ar(In.kr(~cvBusBase.index, 8)); 
    
    // Encode the 8 continuous signals through your custom UGens
    esxStream = ESX8CVEncoder.ar(*cvAudio);
    
    // Plug the ESX-8CV into Header 1 of the ES-5 Encoder
    # es5L, es5R = ES5Encoder.ar(esxStream, 0, 0, 0, 0, 0);
    
    // Send out of the audio interface to the ES-5 hardware!
    Out.ar(6, [es5L, es5R]);
}).play; // .play starts it immediately and permanently

// ---------------------------------------------------------
// 3. The SuperDirt "Ping" Synth
// ---------------------------------------------------------
SynthDef(\tidal_cv_ping, { |out, cv_val=0, port=0|
    // Write the new value to the specific control bus (0-7)
    ReplaceOut.kr(~cvBusBase.index + port, cv_val);
    
    // Destroy this temporary synth almost instantly. 
    // The Control Bus will hold the cv_val forever anyway!
    Line.kr(0, 1, 0.001, doneAction: 2);
}).add;