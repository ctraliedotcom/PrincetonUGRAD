//Author: Chris Tralie

public class Complex {
	public double re, im;
	
	public Complex() {
		re = 1.0;
		im = 0.0;
	}
	
	public Complex(double r, double i) {
		re = r;
		im = i;
	}
	
	public double magSquared() {
		return re*re + im*im;
	}
	
	public void scale(double k) {
		re *= k;
		im *= k;
	}
	
	public Complex mult(Complex x) {
		double nre = (x.re * re - x.im * im);
		double nim = (im * x.re + re * x.im);
		return new Complex(nre, nim); 
	}
	
	public Complex divide(Complex x) {
		double a = re, b = im, c = x.re, d = x.im;
		double nre = (a*c + b*d) / (c*c + d*d);
		double nim = (b*c - a*d) / (c*c + d*d);
		return new Complex(nre, nim);
	}
	
	public Complex add(Complex x) {
		double nre = x.re + re;
		double nim = x.im + im;
		return new Complex(nre, nim);
	}
	
	public Complex subtract(Complex x) {
		double nre = re - x.re;
		double nim = im - x.im;
		return new Complex(nre, nim);
	}
	
	public Complex clone() {
		Complex toReturn = new Complex();
		toReturn.im = im;
		toReturn.re = re;
		return toReturn;
	}
	
	public Complex conjugate() {
		return new Complex(re, -im);
	}
	
	public double getAngle() {
		if (re == 0) {
			if (im > 0)
				return Math.PI / 2;
			return -Math.PI / 2;
		}
		double angle = Math.atan(im / re);
		if (re < 0)
			angle += Math.PI;
		else if (im < 0)
			angle += 2.0*Math.PI;
		return angle;
	}
	
	public String toString() {
		return re + " + " + im + "i";
	}
	
	public static void main(String[] args) {
		Complex test = new Complex(1.0, 1.0);
		System.out.println(test.getAngle() * 180 / Math.PI + "");
	}
}