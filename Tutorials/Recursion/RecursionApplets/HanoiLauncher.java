//Programmer: Chris Tralie
//150 x 200
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;

public class HanoiLauncher extends JApplet implements ActionListener{
	
	public void init(){
		getContentPane().setLayout(new BorderLayout());
		JButton start=new JButton("Solve a Towers of Hanoi puzzle");
		start.addActionListener(this);
		getContentPane().add(start, BorderLayout.CENTER);
	}
	
	public void actionPerformed(ActionEvent evt){
		new Hanoi(Integer.parseInt(JOptionPane.showInputDialog
			(this, "How many discs would you like to use?")),
			Integer.parseInt(JOptionPane.showInputDialog
			(this, "How many milliseconds should be between each move?")));
	}
}