//Programmer: Chris Tralie
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.applet.*;
import java.util.ArrayList;

public class Hanoi extends JFrame{
	public ArrayList leftPeg, middlePeg, rightPeg;
	public static int steps=0;
	boolean finished;
	public int sleepTime, numDisks;
	
	public Hanoi(int pegs, int st){
		super("Towers of Hanoi");
		sleepTime=st; numDisks=pegs;
		leftPeg=new ArrayList();middlePeg=new ArrayList(); rightPeg=new ArrayList();
		setSize(400,200);
		show();
		finished=false;
		new Game();
	}
	
		public void paint(Graphics g){
			int s, p;
			g.setColor(Color.white);
			g.fillRect(0,0,400,200);
			g.setColor(Color.red);
			for(int i=0; i<leftPeg.size(); i++){
				s=((Integer)leftPeg.get(i)).intValue()+1;
				p=leftPeg.size()-1;
				g.fillRect(100-s*5 ,150-((p-i)*5),s*10, 5);
			}
			for(int i=0; i<middlePeg.size(); i++){
				s=((Integer)middlePeg.get(i)).intValue()+1;
				p=middlePeg.size()-1;
				g.fillRect(200-s*5 ,150-((p-i)*5),s*10, 5);
			}
			for(int i=0; i<rightPeg.size(); i++){
				s=((Integer)rightPeg.get(i)).intValue()+1;
				p=rightPeg.size()-1;
				g.fillRect(300-s*5 ,150-((p-i)*5),s*10, 5);
			}
			g.setColor(Color.black);		
			g.drawString((steps+1)+" steps",5,50);	
			}
	
	//The ArrayLists are pointers to the pegs that are being used
	public void move(int n, ArrayList from, ArrayList to, ArrayList temporary){
		if(finished) return;
		//Stopping state, or base case, which only involves one move
		if(n==0){
			if(from.get(0).equals(new Integer(0))){
				to.add(0,from.remove(0));
				update();//This repaints everything and adds to a counter
				//keeping track of the number of steps taken so far
			}	
		}
		
		else{
			//First, move all of the n-1 smaller discs out of the way
			//onto a peg that is free
			move(n-1,from,temporary,to);
			//Now move largest one to  its destination
			if(!finished)
			to.add(0,from.remove(0));
			update();
			//Move the n-1 pegs to their destination on top of the largest one
			move(n-1,temporary,to,from);
		}
	}
	
	public void update(){
			StopWatch.sleep(sleepTime);
			repaint();
			steps++;
	}
	
	//This thread is needed so that repainting can go on simultaneously 
	//with recursive calls
	class Game implements Runnable{
		public Game(){
			Thread t=new Thread(this);
			t.start();
		}
		
	public void run(){
		finished=true;
		steps=0;
		leftPeg=new ArrayList(); middlePeg=new ArrayList(); rightPeg=new ArrayList();
		for(int i=0; i<numDisks; i++){
            leftPeg.add(new Integer(i));
        }
        finished=false;
        move(numDisks-1,leftPeg,rightPeg,middlePeg);
	}
	
    }
	
}
	
