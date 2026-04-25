ES5Encoder : MultiOutUGen {

    *ar { |es5Header1=0, es5Header2=0, es5Header3=0, es5Header4=0, es5Header5=0, es5Header6=0|
        ^this.multiNew(
            'audio',
            es5Header1, es5Header2, es5Header3,
            es5Header4, es5Header5, es5Header6
        )
    }

    init {
        ^this.initOutputs(2, rate)
    }

}