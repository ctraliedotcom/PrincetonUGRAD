//Programmer: Chris Tralie
import java.awt.*;
import javax.swing.*;
import java.applet.*;
import java.awt.event.*;

public class JuliaLauncher extends JApplet implements ActionListener{
	public Image juliaImage;
	public Graphics OSG;
	public JLabel c, i1, i2, i3, plus1, plus2, plus3, upperLeft, lowerRight, widthL, heightL;
	public JTextField creal, cimag, leftreal, leftimag, rightreal, rightimag, width, height; 
	public JButton Go;
	
	public void init(){
		Container content=getContentPane();
		content.setLayout(null);
		c=new JLabel("c = ");c.setBounds(5,5,20,20);content.add(c);
		creal=new JTextField("-1");creal.setBounds(25,5,60,20);content.add(creal);
		plus1=new JLabel(" + ");plus1.setBounds(95,5,20,20);content.add(plus1);
		cimag=new JTextField("0");cimag.setBounds(115,5,60,20);content.add(cimag);
		i1=new JLabel("    i ");i1.setBounds(170,5,30,25);content.add(i1);
		
		upperLeft=new JLabel(" t l=");upperLeft.setBounds(0,35,20,20);content.add(upperLeft);
		leftreal=new JTextField("-2");leftreal.setBounds(25,35,60,20);content.add(leftreal);
		plus2=new JLabel(" + ");plus2.setBounds(95,35,20,20);content.add(plus2);
		leftimag=new JTextField("2");leftimag.setBounds(115,35,60,20);content.add(leftimag);
		i2=new JLabel("    i ");i2.setBounds(170,35,30,25);content.add(i2);
		
		lowerRight=new JLabel("l r=");lowerRight.setBounds(0,70,20,20);content.add(lowerRight);
		rightreal=new JTextField("2");rightreal.setBounds(25,70,60,20);content.add(rightreal);
		plus3=new JLabel(" + ");plus3.setBounds(95,70,20,20);content.add(plus3);
		rightimag=new JTextField("-2");rightimag.setBounds(115,70,60,20);content.add(rightimag);
		i3=new JLabel("    i ");i3.setBounds(170,70,30,25);content.add(i3);

		widthL=new JLabel("width (px) =");widthL.setBounds(5,100,80,20);content.add(widthL);
		width=new JTextField("400");width.setBounds(100,100,90,20);content.add(width);
		
		heightL=new JLabel("height (px) =");heightL.setBounds(5,125,80,20);content.add(heightL);
		height=new JTextField("400");height.setBounds(100,125,90,20);content.add(height);
		
		Go=new JButton("Generate");
		Go.addActionListener(this);
		Go.setBounds(20,150,160,20);
		content.add(Go);
	}
	
	public void actionPerformed(ActionEvent evt){
		int w=Integer.parseInt(width.getText()), h=Integer.parseInt(height.getText());
		juliaImage=createImage(w,h);
		OSG=juliaImage.getGraphics();
		new JuliaSet(juliaImage, OSG, Double.parseDouble(creal.getText()), Double.parseDouble(cimag.getText()),
		Double.parseDouble(leftreal.getText()), Double.parseDouble(leftimag.getText()),
		Double.parseDouble(rightreal.getText()), Double.parseDouble(rightimag.getText()),w,h);
	}
}