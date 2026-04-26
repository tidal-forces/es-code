// Define once
(
SynthDef(\esGate, { |out = 2, bpm = 120, duty = 0.1|
    var freq = bpm / 60.0;
    // LFPulse is unipolar (0 or 1) — ES-3 interprets amplitude as voltage
    var gate = LFPulse.ar(freq, 0, duty);
    Out.ar(out, gate);
}).add;
)

// Start the gate on ES-3 output 1 (audio bus 2)
~gate1 = Synth(\esGate, [\out, 2, \bpm, 120]);

// Change BPM live
~gate1.set(\bpm, 240);

// Stop
~gate1.free;






// Define once
(
SynthDef(\es5Gate, { |out = 6, bpm = 120, duty = 0.5, header = 1|
    var freq  = bpm / 60.0;
    // Scale LFPulse (0.0–1.0) up to 0–255 for the encoder
    var gate  = LFPulse.ar(freq, 0, duty) * 255;

    // Route gate to header 1 only; all others silent
    var encoded = ES5Encoder.ar(
        es5Header1: gate,
        es5Header2: 0,
        es5Header3: 0,
        es5Header4: 0,
        es5Header5: 0,
        es5Header6: 0
    );

    Out.ar(out, encoded); // 2-channel output → ES-3 ch7+ch8
}).add;
)



// Start
~es5 = Synth(\es5Gate, [\bpm, 120, \duty, 0.1]); // duty 0.1 = short trigger pulse

// Change live
~es5.set(\bpm, 140);
~es5.set(\duty, 0.5); // hold gate

// Stop
~es5.free;





