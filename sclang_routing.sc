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