//Programmers: Chris Koscielny and Chris Tralie
//Purpose: To provide access to a midi synthesizer that
//is built into the computer to be used in conjunction
//with serial messages from the basic stamp

import processing.serial.*;
import javax.sound.midi.*;

Serial serial;
Synthesizer synth;
Receiver synthRcvr;
MidiDevice device;

int stat;
int b1;
int b2;
int read;
int s = -1;
final int PRGCHNG = 192;

void setup() {
    serial = new Serial( this, "COM6", 38400, 'Y', 8, 1.0 );
    
    // The default midi synth on our testing machine didn't work, so
    // we needed to write extra code to use one that worked
    try {
        MidiDevice.Info[] devices = MidiSystem.getMidiDeviceInfo();
        for (int i = 0; i < devices.length; i++) {
            println(i + ": " + devices[i].toString()); 
        }
        device = MidiSystem.getMidiDevice(devices[1]);
    } catch (MidiUnavailableException e) {
        // Handle or throw exception...
        System.out.println("can't load midi device?");
    }

    if (!(device.isOpen())) {
      try {
          device.open();
      } catch (MidiUnavailableException e) {
          // Handle or throw exception...
          System.out.println("can't open midi device?");
      }
    }

    // old code to use default synth:
    //synth = MidiSystem.getSynthesizer();
    //synth.open();
    //synthRcvr = synth.getReceiver();

    // get receiver from selected device
    try {
      synthRcvr = device.getReceiver();
    }
    catch (Exception e) {
      e.printStackTrace();  
    }

    if (synthRcvr == null)
        println("synthRcvr == null ?");
    else
        println("OK");
}


void draw() {
    while ( serial.available() > 1 ) {
         read = serial.read();

         // only status bytes have bit 7 set to 1
         // since s=-1 bny default, don't start using data until
         // the first status byte is read
         if ( ((read >> 7)& 0x1) == 1) {
            s = 0;
         }

        switch(s) {
            // reading in status byte
            case 0:
                // program change only uses 1 databyte, so send it
                if ( (read&0xF0) == PRGCHNG) {
                    stat = read;
                    s = 3;
                }
                // Many midi messages require a status byte along with 2 databytes.
                // Program change was the only 2-byte message we used, so assume everything
                // else is 3 byte
                else if ( ((read >> 7)& 0x1) == 1) {
                    stat = read;
                    s = 1;
                }
                break;
            // read the first data byte
            case 1:
                b1 = read;
                s = 2;
                break;
            // read the second data byte and send the 3=byte message to the synth 
            case 2:
                b2 = read;
                s = 0;
                ShortMessage myMsg = new ShortMessage();
                try {
                    myMsg.setMessage(stat, b1, b2);
                    synthRcvr.send(myMsg, -1); // -1 means no time stamp
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            // read the first and only data byte and send the message
            case 3:
                b1 = read;
                s = 0;
                ShortMessage myMsg2 = new ShortMessage();
                try {
                    myMsg2.setMessage(stat, b1, 0);
                    synthRcvr.send(myMsg2, -1); // -1 means no time stamp
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
        }
    } 
}
