// Save this as ESX8GTEncoder.sc in your Extensions folder
// (~/.local/share/SuperCollider/Extensions/ or ~/Library/Application Support/SuperCollider/Extensions/)
// Then recompile the class library (Language > Recompile Class Library)

ESX8GTEncoder {
    // gates 1-8 should each be a signal of 0.0 or 1.0
    // returns a single 0-255 signal for one ES-5 header input
    *ar { |gate1=0, gate2=0, gate3=0, gate4=0,
            gate5=0, gate6=0, gate7=0, gate8=0|
        ^(gate1 * 1)   + (gate2 * 2)   + (gate3 * 4)   + (gate4 * 8)
        + (gate5 * 16) + (gate6 * 32)  + (gate7 * 64)  + (gate8 * 128)
    }
}


// g1-g8 are gate signals (0 or 1)
var out = (g1 * 1) + (g2 * 2) + (g3 * 4) + (g4 * 8)
        + (g5 * 16) + (g6 * 32) + (g7 * 64) + (g8 * 128);
