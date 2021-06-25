//Programmer: Chris Tralie
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class SierpinskiLauncher extends JApplet implements ActionListener{
	public Image image;
	public Graphics OSG;
	
	public void init(){
		Container content=getContentPane();
		content.setLayout(null);
		JButton tri=new JButton("New Triangle");
		tri.addActionListener(this);tri.setBounds(0,0,150,50);
		content.add(tri);
		JButton sqr=new JButton("New Carpet");
		sqr.addActionListener(this);sqr.setBounds(150,0,150,50);
		content.add(sqr);
	}
	
	public void actionPerformed(ActionEvent evt){
		String str=evt.getActionCommand();
		int xres, yres, numPoints;
		if(str.equals("New Triangle")){
			xres=new Integer(JOptionPane.showInputDialog(this, "Set triangle width (px)")).intValue();
			yres=new Integer(JOptionPane.showInputDialog(this, "Set triangle height (px)")).intValue();	
			numPoints=new Integer(JOptionPane.showInputDialog(this, "How many points would you like to draw?")).intValue();	
			image=createImage(xres,yres);
			OSG=image.getGraphics();
			new Triangle(image, OSG, xres, yres, numPoints);
		}
		else{
			xres=new Integer(JOptionPane.showInputDialog(this, "Set carpet width (px)")).intValue();
			yres=new Integer(JOptionPane.showInputDialog(this, "Set carpet height (px)")).intValue();	
			numPoints=new Integer(JOptionPane.showInputDialog(this, "How many points would you like to draw?")).intValue();	
			image=createImage(xres,yres);
			OSG=image.getGraphics();
			new Carpet(image, OSG, xres, yres, numPoints);	
		}
	}
	
}