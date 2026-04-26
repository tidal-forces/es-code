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
        es5Header1: 0,
        es5Header2: 0,
        es5Header3: 0,
        es5Header4: 0,
        es5Header5: 0,
        es5Header6: gate
    );

    Out.ar(out, encoded); // 2-channel output → ES-3 ch7+ch8
}).add;
)



// Start
~es5 = Synth(\es5Gate, [\bpm, 120, \duty, 0.1]); // duty 0.1 = short trigger pulse

// Change live
~es5.set(\bpm, 80);
~es5.set(\duty, 0.1); // hold gate

// Stop
~es5.free;






(
SynthDef(\esGates, {
    // Individual gate signals — LFPulse outputs 0.0 or 1.0
    var g1 = LFPulse.ar(120/60);       // 120 bpm on ESX-8GT output 1
    var g2 = LFPulse.ar(60/60);        // 60 bpm on output 2
    var g3 = LFPulse.ar(240/60, 0, 0.1); // 240 bpm short trigger on output 3

    // Pack bits into a byte for ES-5 header 1
    var header1 = ESX8GTEncoder.ar(
        g1: g1,
        g2: g2,
        g3: g3
        // gates 4-8 default to 0 (off)
    );

    // Feed into ES5Encoder — header1 carries the ESX-8GT data
    Out.ar(6, ES5Encoder.ar(
        es5Header1: header1,
        es5Header2: 0,
        es5Header3: 0,
        es5Header4: 0,
        es5Header5: 0,
        es5Header6: 0
    ));
}).add;
)

~esGates = Synth(\esGates);
~esGates.free;





(
SynthDef(\es5_h6_ch1_test, { |out = 6, bpm = 120, duty = 0.1|
    var pulse = LFPulse.ar(bpm / 60, 0, duty);

    // Header 6, gate output 1 only.
    // Use bit value 1, not 255.
    var h6 = pulse * 1;

    var encoded = ES5Encoder.ar(
        DC.ar(0), DC.ar(0), DC.ar(0),
        DC.ar(0), DC.ar(0), h6
    );

    Out.ar(out, encoded);
}).add;
)

~es5 = Synth(\es5_h6_ch1_test);
~es5.free;





