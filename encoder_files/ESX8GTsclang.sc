// g1-g8 are gate signals (0 or 1)
var out = (g1 * 1) + (g2 * 2) + (g3 * 4) + (g4 * 8)
        + (g5 * 16) + (g6 * 32) + (g7 * 64) + (g8 * 128);

//or else a "pseudo-UGen"
ESX8GTEncoder : UGen {
    // We expect 8 gate inputs (0 for off, anything > 0 for on)
    *ar { arg g1=0, g2=0, g3=0, g4=0, g5=0, g6=0, g7=0, g8=0;
        
        var sum = 0;
        
        // We use (g > 0) to ensure any incoming audio signal 
        // is strictly clamped to a binary 0 or 1 before math.
        sum = sum + ((g1 > 0) * 1);
        sum = sum + ((g2 > 0) * 2);
        sum = sum + ((g3 > 0) * 4);
        sum = sum + ((g4 > 0) * 8);
        sum = sum + ((g5 > 0) * 16);
        sum = sum + ((g6 > 0) * 32);
        sum = sum + ((g7 > 0) * 64);
        sum = sum + ((g8 > 0) * 128);
        
        ^sum; // Returns a single audio stream bouncing between 0 and 255
    }
}