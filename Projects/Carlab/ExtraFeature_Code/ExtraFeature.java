//Programmer: Chris Tralie

// derived from SUN's examples in the javax.comm package
import java.io.*;
import java.util.*;
import javax.comm.*; 
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

//Import classes for recording and moving sound around
import javax.sound.sampled.*;
import java.nio.channels.*;
import java.nio.*;

import java.util.LinkedList;
 
public class ExtraFeature extends JFrame implements WindowListener {
	//Sound variables
	double sampleRate = 8000.0f;
	public int fftsize = 256;
	public Scale scale;
	    	
	public final double CUTOFF = 1400.0;
	public final double centerFreq = 523;//C5
	
	//GUI variables
	public boolean recording = true;
    public WaveForm wavein;
    public FFTPlot fftplot;
    public JLabel peakFreq;
    
    public ExtraFeature() {
    	super("Chow and Tralie Carlab Extra Feature");
    	addWindowListener(this);
    	Container content = getContentPane();
    	content.setLayout(null);
    	
    	wavein = new WaveForm(Color.GREEN);
    	wavein.setBounds(0, 0, 500, 200);
    	content.add(wavein);
    	fftplot = new FFTPlot(Color.BLACK);
    	fftplot.setBounds(0, 200, 500, 200);
    	content.add(fftplot);
    	peakFreq = new JLabel("Waiting for frequency...");
    	peakFreq.setBounds(0, 400, 500, 100);
    	content.add(peakFreq);
    	
    	scale = new Scale("Scales/CMajor.txt");
    	
    	try{
    		new CaptureAudio();
    	}
    	catch (Exception e) {
    		e.printStackTrace();
    	}
    	
    	setSize(500, 550);
    	setVisible(true);
    }
    
    class WaveForm extends JPanel {
    	int width, height;
    	short[] samples;//NOTE: I have to use a variable bigger
    	//than a byte to hold the unsigned bytes that are coming back
    	Color waveColor;
    	public WaveForm(Color c) {
    		waveColor = c;
    		samples = new short[1];
    	}
    	public void paintComponent(Graphics g) {
    		width = getWidth();
    		height = getHeight();
    		g.setColor(Color.black);
    		g.fillRect(0, 0, width, height);
    		g.setColor(waveColor);
    		double xratio = (double)width / (double)samples.length;
    		double yratio = (double)height / (double)(1 << 8);
    		for (int i = 1; i < samples.length; i++) {
    			int x1 = (int)(xratio * (double)(i - 1));
    			int x2 = (int)(xratio * (double)i);
    			int y1 = (int)(yratio * (double)samples[i - 1]);
    			int y2 = (int)(yratio * (double)samples[i]);
    			g.drawLine(x1, height - y1, x2, height - y2);
    		}
    	}
    }
    
    
    class FFTPlot extends JPanel {
    	int width, height;
		Complex[] fft;
		int[] bins;
    	Color waveColor;
    	public FFTPlot(Color c) {
    		waveColor = c;
    		fft = new Complex[1];
    		fft[0] = new Complex(0, 0);
    		bins = new int[0];
    	}
    	public void paintComponent(Graphics g) {
    		width = getWidth();
    		height = getHeight();
    		g.setColor(Color.white);
    		g.fillRect(0, 0, width, height);
    		g.setColor(waveColor);
    		
    		//Find max
    		double[] toDraw = new double[fft.length/2];
    		double max = 0.0;
    		for (int i = 2; i < fft.length/2; i++) {
    			toDraw[i] = Math.sqrt(fft[i].magSquared());
    			if (toDraw[i] > max)
    				max = toDraw[i];
    		}
    		
    		double xratio = (double)width / (double)toDraw.length;
    		double yratio = (double)height / max;
    		for (int i = 1; i < toDraw.length; i++) {
    			int x1 = (int)(xratio * (double)(i - 1));
    			int x2 = (int)(xratio * (double)i);
    			int y1 = (int)(yratio * (double)toDraw[i - 1]);
    			int y2 = (int)(yratio * (double)toDraw[i]);
    			g.drawLine(x1, height - y1, x2, height - y2);
    		}
    		g.setColor(Color.RED);
    		for (int i = 0; i < bins.length; i++) {
    			int x = (int)(xratio*(double)bins[i]);
    			g.drawLine(x, 0, x, height);
    		}
    	}
    }
    
    class CaptureAudio implements Runnable {
	    //Serial variables
	   	CommPortIdentifier portID;
		Enumeration portList;
		InputStream in;
		OutputStream out;
		SerialPort port;
		
	    
		public void setupPort() throws Exception {
			boolean portFound = false;
			String defaultPort = "COM5";
			
			// parse ports and if the default port is found, initialized the reader
			portList = CommPortIdentifier.getPortIdentifiers();
			while (portList.hasMoreElements()) {
				portID = (CommPortIdentifier) portList.nextElement();
				if (portID.getPortType() == CommPortIdentifier.PORT_SERIAL) {
					if (portID.getName().equals(defaultPort)) {
						System.out.println("Found port: "+defaultPort);
						portFound = true;
						port = (SerialPort) portID.open("Camera Image Viewer", 3000);
						port.setSerialPortParams(38400,
												SerialPort.DATABITS_8,
												SerialPort.STOPBITS_1,
												SerialPort.PARITY_NONE);
						port.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
						port.disableReceiveThreshold();
						port.disableReceiveTimeout();
						in = port.getInputStream();
						out = port.getOutputStream();
					} 
				} 
			}
			if (!portFound) {
				System.out.println("port " + defaultPort + " not found.");
				System.exit(0);
			} 
		}
	
    	public CaptureAudio() throws Exception {
    		DataOutputStream fileOutStream;
    		try {
    			setupPort();
    			fileOutStream = new DataOutputStream(new FileOutputStream("out.raw"));
    		}
    		catch (Exception e) { e.printStackTrace();	}
    		
    		new Thread(this).start();
    	}
    	
    	public boolean done = false;
    	
    	short[] getShortBuf(byte[] buf) {
    		short[] ret = new short[buf.length];
    		for (int i = 0; i < ret.length; i++) 
    			ret[i] = (short)buf[i];
    		return ret;
    	}
    	
    	public void run() {			
			DSP dsp = new DSP(fftsize);
			byte[] buffer = new byte[fftsize];
			try{
				while (recording) {
					int bytesRead = 0;
					while (bytesRead < fftsize) {
						bytesRead += in.read(buffer, bytesRead, fftsize - bytesRead);
					}
					wavein.samples = getShortBuf(buffer);
					wavein.repaint();
					double power = dsp.getVariance(buffer);
					int bin = dsp.getMaxBin(buffer, sampleRate, centerFreq);
					fftplot.fft = dsp.fftres;
					fftplot.bins = dsp.bins;
					fftplot.repaint();
					if (power > CUTOFF) {
						peakFreq.setText("Max Bin = " + bin);
						String command = "44444444444444444444444444444444444444444444";
						switch(bin) {
							case -2:
								command = "00000000000000000000000000000000000000000000000000000";
								break;
							case -1:
								command = "11111111111111111111111111111111111111111111111111111111";
								break;
							case 0:
								command = "2222222222222222222222222222222222222222222222222222222222";
								break;
							case 1:
								command = "3333333333333333333333333333333333333333333333333333333333333";
								break;
							case 2:
								command = "4444444444444444444444444444444444444444444444444444444444444";
								break;
							default:
								System.out.println("Invalid turn state requested");
						}
						System.out.println(command);
						for (int k = 0; k < command.length(); k++) {	
							out.write(command.charAt(k));
							for (int a = 0; a < 100000; a++) {
								//Kill some time between characters
							}
							out.flush();
						}
					}
					else {
						peakFreq.setText("Waiting for note (power only " + (int)power + ") ...");
					}
				}
			}
			catch(Exception e){ e.printStackTrace(); }
    	}
    }
    
    
	public void windowOpened(java.awt.event.WindowEvent evt){}
	public void windowClosing(java.awt.event.WindowEvent evt){
	    System.exit(0);
	}
	public void windowClosed(java.awt.event.WindowEvent evt){}
	public void windowActivated(java.awt.event.WindowEvent evt){}
	public void windowDeactivated(java.awt.event.WindowEvent evt){}
	public void windowIconified(java.awt.event.WindowEvent evt){}
	public void windowDeiconified(java.awt.event.WindowEvent evt){}

  	
  	public static void main(String[] args) {
  		new ExtraFeature();
  	}
}
