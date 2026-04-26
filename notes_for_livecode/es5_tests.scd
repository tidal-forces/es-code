//impulse test
(
SynthDef(\es5_gt_ch1_test, {
    var bpm = 120;
    var trig;
    var header1;
    var es3_7_8;

    trig = Trig1.ar(Impulse.ar(bpm / 60), 0.01);

    header1 = ESX8GTEncoder.ar(
        trig, 0, 0, 0,
        0,    0, 0, 0
    );

    es3_7_8 = ES5Encoder.ar(
        header1, 0, 0,
        0,       0, 0
    );

    Out.ar(6, es3_7_8);
}).add;
)


x = Synth(\es5_gt_ch1_test);

//raw header test
(
SynthDef(\es5_raw_header1_test, {
    var zero = DC.ar(0);
    var gate = Trig1.ar(Impulse.ar(2), 0.01) * 1;
    var encoded;

    encoded = ES5Encoder.ar(
        gate, zero, zero,
        zero, zero, zero
    );

    Out.ar(6, encoded);
}).add;
)

x = Synth(\es5_raw_header1_test);


s.reboot;
