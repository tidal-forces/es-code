ES5Encoder : MultiOutUGen {
    *ar { |in1=0, in2=0, in3=0, in4=0, in5=0, in6=0|
        ^this.multiNew('audio', in1, in2, in3, in4, in5, in6)
    }

    initOutputs {
        ^this.initOutputs(2, rate)
    }
}