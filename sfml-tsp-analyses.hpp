#ifndef TSP_ANALYSES
#define TSP_ANALYSES 1


#include<iostream>
using namespace std;

// 2-line intersection analysis built upon work at
// https://github.com/PaulMacDonald/Find_Intersection
class LinearEquation {
private:
	double a, b, c, d, e, f; //six variables for linear equation

public:
	//constructor for the six variables
	LinearEquation(double a, double b, double c, double d, double e, double f) {
		this->a = a;
		this->b = b;
		this->c = c;
		this->d = d;
		this->e = e;
		this->f = f;
	}

	//constructor for intersection AB x CD
	LinearEquation(sf::Vector2<double> a, sf::Vector2<double> b, sf::Vector2<double> c, sf::Vector2<double> d)
        : LinearEquation(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y) { }


	//constructor for intersection
	LinearEquation(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
		//For first line
		a = y2 - y1;
		b = x1 - x2;
		e = (a * x1) + (b * y1);
		//for second line
		c = y4 - y3;
		d = x3 - x4;
		f = (c * x3) + (d * y3);
	}

	//six "getter" functions, one for each variable
	double getA() { return a; }
	double getB() { return b; }
	double getC() { return c; }
	double getD() { return d; }
	double getE() { return e; }
	double getF() { return f; }

	//tells whether or not the equation is solvable
	bool isSolvable() {
		if ((a * d) == (b * c))	{
			return false;
		} else {
			return true;
		}
	};

	//"getter" function that returns x
	double getX() {
		double first = e * d;
		double second = b * f;
		double sub = first - second;
		double third = a * d;
		double fourth = b * c;
		double subTwo = third - fourth;
		double x = sub / subTwo;
		//double x = ((e * d) - (b * f)) / ((a * d) - (b * c));

		return x;
	}

	//"getter" function that returns y
	double getY() {
		double y = ((a * f) - (e * c)) / ((a * d) - (b * c));

		return y;
	}

	sf::Vector2<double> getIntersection() {
        return sf::Vector2<double>(getX(), getY());
	}

	static void testCase1(void) {
        sf::Vector2<double> a(1.0, 3.0);
        sf::Vector2<double> b(5.0, 3.0);
        sf::Vector2<double> c(2.0, 1.0);
        sf::Vector2<double> d(2.0, 4.0);
        LinearEquation le(a, b, c, d);
        cout << boolalpha << le.isSolvable() << endl;
        if (le.isSolvable()) {
            cout << le.getX() << ";" << le.getY() << endl;
            // cout << le.getIntersection().x << ";" << le.getIntersection().y << endl;
        }
	}
};

#endif
