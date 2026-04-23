ESX8CVEncoder : UGen {

    *ar { |esx8cvDAC1=0, esx8cvDAC2=0, esx8cvDAC3=0, esx8cvDAC4=0, esx8cvDAC5=0, esx8cvDAC6=0, esx8cvDAC7=0, esx8cvDAC8=0|
        ^this.multiNew(
            'audio',
            esx8cvDAC1, esx8cvDAC2, esx8cvDAC3, esx8cvDAC4,
            esx8cvDAC5, esx8cvDAC6, esx8cvDAC7, esx8cvDAC8 
        )
    }

}