//Programmer: Chris Tralie
import java.awt.*;
import javax.swing.*;
import java.applet.*;

public class JuliaSet extends JFrame{
	public Display canvas;
	public Coord c, upperLeft, lowerRight;
	public int width, height;
	public double xincrement, yincrement;//These variables store the magnitude
	//of the difference between adjacent pixels on the complex plane
	public Graphics OSG;
	public Image juliaImage;
	public Color[][] pixels;
	public Thread painter;
	
	public JuliaSet(Image i, Graphics g, double cr, double ci, double lr, double li, double rr, double ri, int w, int h){
		super("c="+cr+"+"+ci+"i");
		Container content=getContentPane();
		content.setLayout(new BorderLayout());
		canvas=new Display();
		content.add(canvas,BorderLayout.CENTER);
		juliaImage=i;
		OSG=g;
		c=new Coord(cr, ci);
		upperLeft=new Coord(lr, li);
		lowerRight=new Coord(rr, ri);
		width=w;
		height=h;
		pixels=new Color[w][h];
		xincrement=(rr-lr)/(double)(width);
		yincrement=(ri-li)/(double)(height);
		setSize(width,height+50);
		show();
		try{
		new Painter();
		}
		catch(Exception e){
			System.out.println(e.toString());
		}
	}
	
	class Display extends JPanel{
		
	public void paintComponent(Graphics g){
		g.drawImage(juliaImage,0,0,this);
		}
	}
	
	class Coord{
		private double x, y;
		public Coord(double xv, double yv){
			x=xv; y=yv;
		}
		public double getX(){
			return x;
		}
		public double getY(){
			return y;
		}
		public Coord add(Coord c){
			double newx=x+c.getX();
			double newy=y+c.getY();
			return new Coord(newx,newy);
		}
		public Coord getSquared(){
			double newx=x*x-y*y;
			double newy=2*x*y;
			return new Coord(newx,newy);
		}
		public String toString(){
			return "("+x+" + "+y+"i)";
		}
		public double getDistance(Coord c){
			return Math.sqrt((x-c.getX())*(x-c.getX()) + (y-c.getY())*(y-c.getY()));
		}
	}
	
	public void finish(){
		painter.stop();
		painter=null;
	}
	
	//z=z^2 + c
	class Painter implements Runnable{
	
	public Painter(){
		painter=new Thread(this);
		painter.start();
	}
	
	public void run(){
		Coord z, origin=new Coord(0,0);
		int counts;
		for(int i=0; i<width; i++){
			for(int j=0; j<height; j++){
				z=new Coord(upperLeft.getX()+i*xincrement, upperLeft.getY()+j*yincrement);
				counts=0;
				while(z.getDistance(origin)<=2 && counts<=101){
					z=c.add(z.getSquared());
					counts++;
				}
				if(counts<=1){
					OSG.setColor(Color.white);
				}
				else if(counts<4){
					OSG.setColor(Color.getHSBColor( 0.85F, (float)(counts)/3.0F, 1.0F ));
				}
				else if(counts<10){
					OSG.setColor(Color.getHSBColor( 0.1647059F, (float)(counts-3)/12.0F+0.4F, 1.0F ));
				}
				else if(counts<20){
					OSG.setColor(Color.getHSBColor( 0.33333333F, 1.0F, 1.0F-0.333333333F*(float)(counts-9)/10.0F ));
				}
				else if(counts<100){
					OSG.setColor(Color.blue);
				}
				else{
					OSG.setColor(Color.black);
				}
				OSG.drawRect(i,j,1,1);
			}
			canvas.repaint();
		}
		finish();
	}
	}
	
}