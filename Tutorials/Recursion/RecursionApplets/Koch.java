//Programmer: Chris Tralie
//150 x 200
import java.awt.*;
import javax.swing.*;
import java.lang.Math;

public class Koch extends JApplet{
	public static int minPixelLength=1;
	public Pencil pencil;
	public Graphics OSG;
	public Image image;
	public Display canvas;
	
	public void init(){
		pencil=new Pencil(0,120,0);
		image=createImage(400,200);
		OSG=image.getGraphics();
		canvas=new Display();
		getContentPane().setLayout(new BorderLayout());
		getContentPane().add(canvas,BorderLayout.CENTER);
		drawCurve(400);
	}
	
	class Display extends JPanel{
		public void paintComponent(Graphics g){
			g.drawImage(image, 0,0,this);
		}
	}
	
	class Pencil{
		private double theta;
		private int xpos, ypos;
		public Pencil(int x, int y, double t){
			xpos=x;ypos=y;theta=t;
		}
		public void turn(double angle){
			theta+=(angle*Math.PI/180);
		}
		public void move(double pixels){
			int newx, newy;
			newx=xpos+(int)(Math.cos(theta)*pixels);
			newy=ypos-(int)(Math.sin(theta)*pixels);
			OSG.setColor(Color.black);
			OSG.drawLine(xpos,ypos,newx,newy);
			xpos=newx;ypos=newy;
			canvas.repaint();
		}
	}
	
	public void drawCurve(double length){
         double newLength=length/3;
           if((int)length==minPixelLength){
                  pencil.move(length);
                  return;
               }
               drawCurve(newLength);
               pencil.turn(60);
               drawCurve(newLength);
               pencil.turn(-120);
               drawCurve(newLength);
               pencil.turn(60);
               drawCurve(newLength);
    }
}