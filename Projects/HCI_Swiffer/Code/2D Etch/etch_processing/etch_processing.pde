//Programmers: Chris Koscielny and Chris Tralie
//Purpose: To render a virtual Etch-A-Sketch to the screen
//based on serial data from a BASIC STAMP.

import processing.serial.*;
Serial s;
int STATE = 0;
int x = 0, y = 0;
int data1, data2;
int lastx = -1, lasty = -1;
int max = 0x3ff;
int mode, strokeW, lineW;
final int CARTESIAN = 0;
final int POLAR = 1;
final int COLOR_WEIGHT = 2;

void setup() {
    size( 800, 900 );
    background( 255 );
    smooth();
    ellipseMode( CENTER );
    noStroke();
    fill( 0 );
    frameRate( 50 );
    s = new Serial( this, "COM6", 38400, 'Y', 8, 1.0 );
    frameRate( 24 );
    mode = CARTESIAN;
    strokeW = 1;
    lineW = 128;
    textFont(createFont("SansSerif", 16), 24);
}

void draw() {
    while ( s.available() > 1 ) {
        // Keep track of states, discard inputs until back
        // in sync with states
        int input = s.read();
        int inputState = input & 3;
        int inputValue = (input >> 2) & 0x1f;

        // If for some reason the states become out of sync between the board
        // and the program, then ignore data from previous byte sequence and
        // start over and wait until the board starts sending a new sequence,
        // beginning with the LSB of the X coordinate
        if (inputState != STATE) {
            STATE = 0;
            continue;
        }
      
        switch(STATE) {
            case 0:
                // set LS part of X
                data1 = inputValue;
                break;  
            case 1:
                // add MS part of X
                data1 = data1 + (inputValue << 6);
                break;
            case 2:
                // set LS part of Y
                data2 = inputValue;
                break;
            case 3:
                // Request a new byte (the "fifth byte," which has the data for
                // mode and whether or not to clear the screen)
                while ( s.available() == 0 ) {
                    // Wait until the next byte is available
                }                
                int modeInput = s.read();

                // clear the screen
                if ((modeInput & 0x20) == 0) {
                    background(255);
                    println("Trying to clear");
                    break; 
                }

                // Detect if this is the fifth byte
                // If so, then change the mode accordingly
                if ((modeInput & 0x1f) == 0x1f) {
                    mode = (modeInput >> 6) & 0x03;
                }

                // add MS part of Y
                data2 = data2 + (inputValue << 6);

                fill(255,255,255);
                rect(0, 800, 800, 900);
                fill(0,0,0);
                if (mode == CARTESIAN) {
                    text("Cartesian", 20, 850);
                    x = max - data1;
                    y = max - data2;
                    x = width * x / max;
                    y = width * y / max;
                }
                else if (mode == POLAR) {
                    text("Polar", 20, 850);
                    double r = 0.5 * (double)data1 / (double)max * (double)width;
                    double theta = (double)data2 / (double)max * (-2*Math.PI);
                    x = width / 2 + (int)(r*Math.cos(theta));
                    y = width / 2 + (int)(r*Math.sin(theta));
                }
                else if (mode == COLOR_WEIGHT) {
                    text("Color & Weight", 20, 850);
                    // If we're changing the color, then don't change
                    // x or y
                    x = lastx;
                    y = lasty;
                    strokeW = (data1 * 10) / max;
                    lineW = data2 / (max / 255);
                    stroke(lineW);
                    strokeWeight(strokeW);
                    line(850,300,850,500);
                }

                //ellipse (width * x / max, height*y / max, 10, 10);
                if (lastx == -1 || lasty == -1) {
                    lastx = x; lasty = y;
                    break;
                }

                // Draw if the mode is in polar or rectangular
                if (mode != COLOR_WEIGHT) {
                    stroke(lineW);
                    strokeWeight(strokeW);
                    line (lastx, lasty, x, y);
                    lastx = x; lasty = y;
                }
                break;
            default:
                println("INVALID STATE");
        }

        // set next expected state
        STATE++;
        if (STATE > 3)
            STATE = 0;
    }
}
