//Programmer: Chris Tralie
import java.awt.*;
import javax.swing.*;
import java.util.Random;
import java.lang.Math;

//Sierpinski Carpet

public class Carpet extends JFrame{
public int xres, yres, numPoints;
public Image carpetImage;
public Graphics OSG;
public Display canvas;
public Thread drawThread;

public Carpet(Image i, Graphics g, int x, int y, int n){
	super("Sierpinski Carpet");
	Container c=getContentPane();
	c.setLayout(new BorderLayout());
	canvas=new Display();
	c.add(canvas,BorderLayout.CENTER);
	
	carpetImage=i;OSG=g;
	xres=x;yres=y;numPoints=n;
		
	setSize(xres,yres+30);
	show();
	try{
		new Draw();
	}
	catch(Exception e){
		System.out.println(e.toString());
	}
}

class Display extends JPanel{
public void paintComponent(Graphics g){
	g.drawImage(carpetImage,0,0,null);
}

}

class Draw implements Runnable{
	public Draw(){
		drawThread=new Thread(this);
		drawThread.start();
	}
	public void run(){
		int[][] verticies={{0,xres/2,xres,0,xres,0,xres/2,xres},{0,0,0,yres/2,yres/2,yres,yres,yres}};
		Random rand=new Random();
		int xpos=0, ypos=yres;
		int point;
		OSG.setColor(Color.black);
		for(int i=0; i<numPoints; i++){
			point=rand.nextInt(verticies[0].length);
			OSG.fillRect(xpos,ypos,1,1);
			xpos+=(verticies[0][point]-xpos)*2/3;
			ypos+=(verticies[1][point]-ypos)*2/3;
			canvas.repaint();
		}
	}
}

}