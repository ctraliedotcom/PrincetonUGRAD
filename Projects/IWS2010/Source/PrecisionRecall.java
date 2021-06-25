import java.io.*;
import java.util.*;
import java.util.Arrays;

public class PrecisionRecall {
	public ArrayList<ObjectGuess> guessesList;
	public ObjectGuess[] guesses;
	public int[] counts;
	public int classes = 44;

	public void loadFile(String filename) {
		guessesList = new ArrayList<ObjectGuess>();
		try {
		    FileInputStream fstream = new FileInputStream(filename);
		    DataInputStream in = new DataInputStream(fstream);
		    while (in.available() != 0) {
	            String str = in.readLine();
	            ObjectGuess guess = new ObjectGuess(str);
				guessesList.add(guess);
		    }
		}
		catch (Exception e) {
		    e.printStackTrace();
		}
		guesses = new ObjectGuess[guessesList.size()];
		counts = new int[classes];
		for (int i = 0; i < guessesList.size(); i++) {
			guesses[i] = guessesList.get(i);
			counts[guesses[i].actualClass]++;
		}
		System.out.println("guesses = " + guesses.length);
	}

    public PrecisionRecall(String filename) {
    	loadFile(filename);
    	int maxCount = 0;
    	double[][] PR = new double[classes][];
    	for (int i = 0; i < classes; i++) {
    		if (counts[i] > maxCount)
    			maxCount = counts[i];
    		
    		PR[i] = new double[counts[i]];
    		//Now do precision recall on the current class
    		//Recall is on x-axis, precision is on y-axis
    		ObjectGuess.currClass = i;
    		Arrays.sort(guesses);
    		int recall = 0;
    		for (int k = 0; k < guesses.length; k++) {
    			if (guesses[k].actualClass == i) {
    				//System.out.print((recall + 1) + "/" + (k+1) + "  ");
    				if (recall == 0) {
    					System.out.println("\n\nClass " + guesses[k].stringClass + ": ");
    				}
    				System.out.print(guesses[k].exampleID + ", ");
    				PR[i][recall] = ((double)(recall+1)) / ((double)(k+1));
    				recall++;
    				if (recall == counts[i])
    					break;//Everything has been recalled
    			}
    		}
    	}
    	System.out.println("\n");
    	
    	/*for (int i = 0; i < counts.length; i++) {
    		for (int k = 0; k < PR[i].length; k++) {
    			System.out.print(PR[i][k] + " ");
    		}
    		System.out.println("");
    	}*/
    	
    	//Average all of the precision-recall counts over all classes
    	int nbins = maxCount;
    	double[] avgPR = new double[nbins];
    	//System.out.println("maxCount = " + maxCount);
    	for (int i = 0; i < nbins; i++) {
    		avgPR[i] = 0.0;
    		for (int k = 0; k < classes; k++) {
    			if (PR[k].length == 0)
    				continue;
    			//Do linear interpolation for the classes that have
    			//fewer than maxcount members
    			double binHit = ((double)counts[k]) / ((double)nbins) * (double)i;
    			int left = (int)Math.floor(binHit);
    			int right = (int)Math.ceil(binHit);
    			if (right >= counts[k])
    				right = left;
    			
    			double precision = (binHit - left)*PR[k][right];
    			precision += (right - binHit)*PR[k][left];
    			avgPR[i] += precision;
    		}
    		avgPR[i] /= (double)classes;
    		System.out.print((int)(avgPR[i]*1000) + " ");
    	}
    }
    
    public static void main(String[] args) {
    	new PrecisionRecall(args[0]);
    }
    
    
}