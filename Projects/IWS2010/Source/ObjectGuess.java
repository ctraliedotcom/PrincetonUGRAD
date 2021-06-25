/**
 * @(#)ObjectGuess.java
 *
 *
 * @author 
 * @version 1.00 2010/5/9
 */
 
import java.util.*;

public class ObjectGuess implements Comparable {
	public static int currClass;//The class that is currently being examined
	public static int totalExamples = 0;
	
	public double[] probs;
	public int actualClass;
	public String stringClass;
	public boolean misclassified;
	public int classes = 44;
	public int exampleID;
	
	public ObjectGuess(String str) {
		totalExamples++;
		exampleID = totalExamples;
		
		double totalProb = 0.0;
		misclassified = false;
		probs = new double[classes];
		StringTokenizer tokenizer = new StringTokenizer(str, " ");
		tokenizer.nextToken();//Skip inst #
		stringClass = tokenizer.nextToken();
		StringTokenizer objTokenizer = new StringTokenizer(stringClass, ":");
		actualClass = Integer.parseInt(objTokenizer.nextToken()) - 1;
		tokenizer.nextToken();//Skip predicted class
		
		for (int i = 0; i < classes; i++) {
			if (!tokenizer.hasMoreTokens())
				break;
			String prob = tokenizer.nextToken();
			if (prob.equals("+")) {
				misclassified = true;
				prob = tokenizer.nextToken();
			}
			if (prob.charAt(0) == '*') {
				prob = prob.substring(1, prob.length());
			}
			probs[i] = Double.parseDouble(prob);
			totalProb += probs[i];
		}
		while (tokenizer.hasMoreTokens())
			System.out.println(tokenizer.nextToken());
		if (totalProb != 1) {
			//System.err.println("Warning: Probability ( " +totalProb + ") isn't 1");
		}
	}
    
    public int compareTo(Object other) {
    	double p1 = probs[currClass];
    	double p2 = ((ObjectGuess)other).probs[currClass];
    	if (p1 > p2)
    		return -1;
    	if (p1 == p2)
    		return 0;
    	return 1;
    }
    
    public String toString() {
    	String ret = actualClass + " : {";
    	for (int i = 0; i < probs.length; i++) {
    		ret += probs[i] + ",";
    	}
    	ret += "}";
    	return ret;
    }
}