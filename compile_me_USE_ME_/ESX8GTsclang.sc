//a "pseudo-UGen"
ESX8GTEncoder : UGen {
    // We expect 8 gate inputs (0 for off, anything > 0 for on)
    *ar { arg g1=0, g2=0, g3=0, g4=0, g5=0, g6=0, g7=0, g8=0;
        
        var sum = 0;
        
        // We use (g > 0) to ensure any incoming audio signal 
        // is strictly clamped to a binary 0 or 1 before math.
        sum = sum + ((g1 > 0).asInteger * 1);
        sum = sum + ((g2 > 0).asInteger * 2);
        sum = sum + ((g3 > 0).asInteger * 4);
        sum = sum + ((g4 > 0).asInteger * 8);
        sum = sum + ((g5 > 0).asInteger * 16);
        sum = sum + ((g6 > 0).asInteger * 32);
        sum = sum + ((g7 > 0).asInteger * 64);
        sum = sum + ((g8 > 0).asInteger * 128);
        
        ^sum; // Returns a single audio stream bouncing between 0 and 255
    }
}
