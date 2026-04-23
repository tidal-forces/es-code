As I just got the ES3 and ES5. I post here some code to use the ES5 from
expert sleepers with sc maybe its interesting for somebody.

(
~es5_control={|channel,device,gate|
     var shift=(2-device)*8;//to which device should the gate be shifted
     var val=1<<channel<<shift;//to which channel should the gate be shifted
     var on=if((0x800000&val)>0)//calculate the right float value
     {Float.from32Bits(val) / (Float.from32Bits(0x800000) * -1)}
     {Float.from32Bits(0xffffff&val) / Float.from32Bits(0x800000)};
     if(gate>0,on,0);//output either the precalulated float or a 0
};
)

//simple function to switch on the gates Silence is necessary to create
a Audio-Rate signal
{Out.ar(0,Silence.ar+~es5_control.(0,0,1))}.play;//switch on first channel
{Out.ar(0,Silence.ar+~es5_control.(3,0,1))}.play;//switch on fourth channel

//contol gate 6 with a pulse oscilator
{Out.ar(0,~es5_control.(5,0,LFPulse.ar(1,0)))}.play;

(
//test function for first adat channel
{Out.ar(0,
     (0..2).collect{|dev| //generate gates for all 3 devices per channel
         (1/Array.geom(8,1,2)).collect{|item,chan|  //generate gates for
all channels
             ~es5_control.(chan,dev,LFPulse.ar(item,0)) //use a LFPulse
to control the gates
     }.sum}.sum) //summarise the different channels to send them to the
first channel
}.play;
)