//Programmer: Chris Tralie

import java.util.ArrayList;
import java.util.Arrays;
import java.io.*;

public class Scale {
	public Note[] notes;
	public String[] notenames;
	public String currentNote;
	
    public Scale(String filename) {
		ArrayList<Note> notesList = new ArrayList<Note>();
		ArrayList<String> notenamesList = new ArrayList<String>();
		FileInputStream fstream;
		DataInputStream in = null;
		           
		try {
			fstream = new FileInputStream(filename);
			in = new DataInputStream(fstream);
			while (in.available() != 0) {
				String str = in.readLine();
				double freq = 110.0;
				double halfStep = Math.pow(2.0, (1.0 / 12.0));
				notenamesList.add(str);
				
				if (str.equals("Ab"))
					freq *= Math.pow(halfStep, -1);
				else if (str.equals("A")) 
					;
				else if (str.equals("A#"))
					freq *= Math.pow(halfStep, 1);
					
				else if (str.equals("Bb"))
					freq *= Math.pow(halfStep, 1);
				else if (str.equals("B")) 
					freq *= Math.pow(halfStep, 2);
				else if (str.equals("B#"))
					freq *= Math.pow(halfStep, 3);
					
				else if (str.equals("Cb"))
					freq *= Math.pow(halfStep, 2);
				else if (str.equals("C")) 
					freq *= Math.pow(halfStep, 3);
				else if (str.equals("C#"))
					freq *= Math.pow(halfStep, 4);
					
				else if (str.equals("Db"))
					freq *= Math.pow(halfStep, 4);
				else if (str.equals("D")) 
					freq *= Math.pow(halfStep, 5);
				else if (str.equals("D#"))
					freq *= Math.pow(halfStep, 6);
					
				else if (str.equals("Eb"))
					freq *= Math.pow(halfStep, 6);
				else if (str.equals("E")) 
					freq *= Math.pow(halfStep, 7);
				else if (str.equals("E#"))
					freq *= Math.pow(halfStep, 8);
					
				else if (str.equals("Fb"))
					freq *= Math.pow(halfStep, 7);
				else if (str.equals("F")) 
					freq *= Math.pow(halfStep, 8);
				else if (str.equals("F#"))
					freq *= Math.pow(halfStep, 9);	
						
				else if (str.equals("Gb"))
					freq *= Math.pow(halfStep, 9);
				else if (str.equals("G")) 
					freq *= Math.pow(halfStep, 10);
				else if (str.equals("G#"))
					freq *= Math.pow(halfStep, 11);
				else {
					System.err.println("WARNING: Unknown note " + str + " encountered");
					continue;
				}
				//Add all harmonics
				while (freq < 5000.0) {
					notesList.add(new Note(str, freq));	
					freq *= 2.0;
				}	
			}
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		
		notes = new Note[notesList.size()];
		for (int i = 0; i < notes.length; i++)
			notes[i] = notesList.get(i);
		Arrays.sort(notes);//Put the notes in order by frequency so
		//that the binary search can do its job	
				
		notenames = new String[notenamesList.size()];
		for (int i = 0; i < notenames.length; i++)
			notenames[i] = notenamesList.get(i);

    }
    
    //Will return frequency less than or equal to
    public static int binarySearch(Note[] a, double target, int begin, int end) {
		int mid = (begin + end) / 2;
		if (a[mid].freq == target || begin > end)
			return mid;
		if (target > a[mid].freq) {
			return binarySearch(a, target, mid + 1, end);
		}
		else {
			return binarySearch(a, target, begin, mid - 1);
		}
	}
    
    public Note getClosestNote(double freq) {
    	int index = binarySearch(notes, freq, 0, notes.length - 1);
    	if (index == notes.length - 1)
    		return notes[notes.length - 1];
    	Note less = notes[index];
    	Note greater = notes[index + 1];
    	//System.out.println(freq / less.freq + "    " + greater.freq / freq);
    	if ((freq / less.freq) < (greater.freq / freq))
    		return less;
    	return greater;
    }
    
    public static void main(String[] args) {
    	Scale scale = new Scale("Scales/CMajor.txt");
    	for (int i = 0; i < scale.notes.length; i++) {
    		System.out.println(scale.notes[i]);
    	}
    	double check = 1000.0;
		System.out.println("\n\n" + scale.getClosestNote(check));
    }
}