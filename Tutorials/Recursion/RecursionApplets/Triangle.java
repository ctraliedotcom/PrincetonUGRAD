//Programmer: Chris Tralie
import java.awt.*;
import javax.swing.*;
import java.util.Random;
import java.lang.Math;

//Sierpinski Triangle

public class Triangle extends JFrame{
public int xres, yres, numPoints;
public Image triangleImage;
public Graphics OSG;
public Display canvas;
public Thread drawThread;

public Triangle(Image i, Graphics g, int x, int y, int n){
	super("Sierpinski Triangle");
	Container c=getContentPane();
	c.setLayout(new BorderLayout());
	canvas=new Display();
	c.add(canvas,BorderLayout.CENTER);
	
	triangleImage=i;OSG=g;
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
	g.drawImage(triangleImage,0,0,null);
}

}

class Draw implements Runnable{
	public Draw(){
		drawThread=new Thread(this);
		drawThread.start();
	}
	public void run(){
		int[][] verticies={{xres/2,0,xres},{0,yres,yres}};
		Random rand=new Random();
		int xpos=xres/2, ypos=0;
		int point;
		OSG.setColor(Color.black);
		for(int i=0; i<numPoints; i++){
			point=rand.nextInt(3);
			OSG.fillRect(xpos,ypos,1,1);
			xpos+=(verticies[0][point]-xpos)/2;
			ypos+=(verticies[1][point]-ypos)/2;
			canvas.repaint();
		}
	}
}

}