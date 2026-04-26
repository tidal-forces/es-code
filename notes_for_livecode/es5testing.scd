(
SynthDef(\es5_silence_crash_test, {
    var zero = DC.ar(0);
    var encoded = ES5Encoder.ar(zero, zero, zero, zero, zero, zero);
    Out.ar(0, encoded);
}).add;
)

x = Synth(\es5_silence_crash_test);

(

var zero = DC.ar(0);
var u = ES5Encoder.ar(zero, zero, zero, zero, zero, zero);
u.dump;

)