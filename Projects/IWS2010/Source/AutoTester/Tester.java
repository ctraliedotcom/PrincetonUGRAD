import java.awt.Robot;
import java.awt.image.*;
import java.awt.event.*;
import java.io.*;
import java.net.URL;
import javax.imageio.ImageIO;
import java.awt.Rectangle;
import java.util.ArrayList;
import java.util.Random;
import java.util.TreeMap;

public class Tester {
	static int resx = 600;
	static int resy = 500;
	static TreeMap<String, String> numObjMap = new TreeMap<String, String>();
	
	public static void Sleep(int millis) {
    	try {
    		Thread.currentThread().sleep(millis);
    	}
    	catch (Exception e) {
    		e.printStackTrace();
    	}
    }
    
    public static PrintWriter startHTML(String filename, String title) {
    	PrintWriter ret = null;
    	try {
	    	FileWriter outFile = new FileWriter(filename);
	    	ret = new PrintWriter(outFile);
	    	ret.println("<html>");
	    	ret.println("<title>" + title + "</title>");
	    	ret.println("<body>");
	    	ret.println("<center><h1>" + title + "</h1><BR><BR>");
    	}
    	catch (Exception e) {
    		e.printStackTrace();
    	}
    	return ret;
    }
    
    public static void endHTML(PrintWriter out) {
    	out.println("</body></html>");
    }
    
    public static void makeHTMLEntry(PrintWriter out, String blob, String model, int num) {
    	//Figure out what type of object the blob is by looking in the hash table
    	int endtype = blob.indexOf('_');
    	String blobtype = "Unknown Class";
    	if (endtype > -1)
    		blobtype = numObjMap.get(blob.substring(0, endtype));
    	
    	out.println("<table border = \"1\"><tr><td><h1>Random Test #" + (num+1) + "</h1></td></tr>");
    	out.println("<tr>");
    	out.println("<td><img src = \"" + model + ".png\"></td>");
    	out.println("<td><img src = \"" + blob + ".png\"></td>");
    	out.println("<td><img src = \"" + blob + "_" + model + ".xyz.png\"></td>");
    	out.println("</tr>");
    	out.println("<td>" + model + "</td><td>" + blob + "<BR>(Type = " + blobtype + ")</td><td><a href = \"" +  blob + "_" + model + ".xyz" + "\">Alignment</a></td></tr></table>");
    	out.println("<BR><BR>");
    }
    
    //Double click the center mouse button in the window to get the GL programs
    //to center on the objects properly
    public static void CenterView(Robot robot) {
		robot.mouseMove(400, 300);
		robot.mousePress(InputEvent.BUTTON2_MASK);
		robot.mouseRelease(InputEvent.BUTTON2_MASK);
		robot.mousePress(InputEvent.BUTTON2_MASK);
		robot.mouseRelease(InputEvent.BUTTON2_MASK);
		Sleep(400);	
    }
	
    public static void main(String[] args) {
    	String meshdir = "..\\manual_obj_models";
    	String blobdir = "..\\manual_blobs";
    	String outdir = "..\\results";
    	String[] command;
    	int testnum = 10;
    	
		Random rand = new Random();
		Rectangle captureArea = new Rectangle();
		captureArea.x = 94;
		captureArea.y = 72;
		captureArea.width = resx;
		captureArea.height = resy;
		
		BufferedImage image;//Image to store screen captures
		PrintWriter HTMLWriter = startHTML(outdir + "\\index.html", "6D Alignment Results ("+testnum+")");
		Process ps;
		File outfile;
    	
    	try {
    		Robot robot = new Robot();
	    	ArrayList<String> meshModels = new ArrayList<String>();
	    	ArrayList<String> blobs = new ArrayList<String>();
    		File dir = new File(meshdir);
    		String[] mshcontents = dir.list();
    		for (int i = 0; i < mshcontents.length; i++) {
    			char[] arr = mshcontents[i].toCharArray();
    			if (arr[arr.length - 3] == 'o' && arr[arr.length - 2] == 'b'
    				&& arr[arr.length - 1] == 'j') {
    					meshModels.add(mshcontents[i]);
    					String str = mshcontents[i];
    					int end = str.indexOf('_');
    					if (end == -1)
    						continue;
    					numObjMap.put(str.substring(0, end), str);
    				}
    		}
    		File dir2 = new File(blobdir);
    		String[] blobcontents = dir2.list();
    		for (int i = 0; i < blobcontents.length; i++) {
    			char[] arr = blobcontents[i].toCharArray();
    			if (arr[arr.length - 3] == 'x' && arr[arr.length - 2] == 'y'
    				&& arr[arr.length - 1] == 'z') {
    					blobs.add(blobcontents[i]);
    				}
    		}		

			//Randomly choose a model and a point cloud
			for (int i = 0; i < testnum; i++) {
				//Do fitting
				String model = meshModels.get(rand.nextInt(meshModels.size()));
				String blob = blobs.get(rand.nextInt(blobs.size()));
				String outname = blob + "_" + model + ".xyz";
			    command = new String[]{"..\\BlobMeshAlign.exe", 
			    	meshdir + "\\" + model, 
			    	blobdir + "\\" + blob,
			    	outdir + "\\" + outname, "0", "0.1"};
				ps = Runtime.getRuntime().exec(command);
				ps.waitFor();
				
				//View the mesh by itself, take a screenshot, and save
				command = new String[]{"..\\mshview.exe", meshdir + "\\" + model};
				ps = Runtime.getRuntime().exec(command);
				Sleep(1000);
				image = robot.createScreenCapture(captureArea);
				outfile = new File(outdir + "\\" + model + ".png");
				ImageIO.write(image, "png", outfile);
				ps.destroy();
				
				//View the point cloud by itself, take a screenshot, and save
				System.out.println(blobdir + "\\" + blob);
				command = new String[]{"..\\simplePointsViewer.exe", blobdir + "\\" + blob};
				ps = Runtime.getRuntime().exec(command);
				Sleep(1000);
				CenterView(robot);
				image = robot.createScreenCapture(captureArea);
				outfile = new File(outdir + "\\" + blob + ".png");
				ImageIO.write(image, "png", outfile);
				ps.destroy();
				
				//View the alignment, take a screenshot, and save
				command = new String[]{"..\\simpleModelFittingViewer.exe", meshdir + "\\" + model, outdir + "\\" + outname};
				ps = Runtime.getRuntime().exec(command);
				Sleep(1000);
				CenterView(robot);
				image = robot.createScreenCapture(captureArea);
				outfile = new File(outdir + "\\" + outname + ".png");
				ImageIO.write(image, "png", outfile);
				ps.destroy();
				
				makeHTMLEntry(HTMLWriter, blob, model, i);
				
			}
			endHTML(HTMLWriter);
			HTMLWriter.close();
		}
		catch (Exception e) {
			e.printStackTrace();
		}
    }
}