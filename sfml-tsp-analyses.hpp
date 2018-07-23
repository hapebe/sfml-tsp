#ifndef TSP_ANALYSES
#define TSP_ANALYSES 1


#include<iostream>
using namespace std;

// 2-line intersection analysis built upon work at
// https://github.com/PaulMacDonald/Find_Intersection
class LinearEquation {
private:
	sf::Vector2<double> pa, pb, pc, pd; // four "corner" points, defining lines AB and CD.
	double a, b, c, d, e, f; //six variables for linear equation

public:
	//constructor for intersection AB x CD
	LinearEquation(sf::Vector2<double> ppa, sf::Vector2<double> ppb, sf::Vector2<double> ppc, sf::Vector2<double> ppd) {
		pa = ppa;
		pb = ppb;
		pc = ppc;
		pd = ppd;

		//For first line
		a = pb.y - pa.y;
		b = pa.x - pb.x;
		e = (a * pa.x) + (b * pa.y);

		//for second line
		c = pd.y - pc.y;
		d = pc.x - pd.x;
		f = (c * pc.x) + (d * pc.y);
	}

	//six "getter" functions, one for each variable
	double getA() { return a; }
	double getB() { return b; }
	double getC() { return c; }
	double getD() { return d; }
	double getE() { return e; }
	double getF() { return f; }

	//tells whether or not the equation is solvable
	bool hasIntersection() {
		if ((a * d) == (b * c))	{
			return false;
		} else {
			return true;
		}
	};

	bool isPointOnLineAB(sf::Vector2<double> p) {
		return isPointOnLine(p, pa, pb);
	}

	bool isPointOnLineCD(sf::Vector2<double> p) {
		return isPointOnLine(p, pc, pd);
	}

	bool isPointOnLine(sf::Vector2<double> p, sf::Vector2<double> a, sf::Vector2<double> b) {
		double dx = b.x - a.x;
		double dy = b.y - a.y;

		double qx = p.x - a.x;
		double qy = p.y - a.y;

		// fraction (or multiple...) of the original line length
		double fx = 0, fy = 0;
		if (dx != 0) fx = qx / dx;
		if (dy != 0) fy = qy / dy;

		if (fx < 0 || fx > 1) return false;
		if (fy < 0 || fy > 1) return false;

		// okay, both are between 0..1, they *could* be on the line:
		if (dx == 0 || dy == 0) {
			return true; // in this case, any of the other fraction is okay!
		} else {
			if (abs(fx-fy) < 1e-12) return true;
		}

		return false;
	}

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

	string describe(void) {
		 std::stringstream ss;

		 ss << "LinearEquation, " << endl;
		 ss << "line AB: (" << pa.x <<  ";" << pa.y << ") - (" << pb.x << ";" << pb.y << ")" << endl;
		 ss << "line CD: (" << pc.x <<  ";" << pc.y << ") - (" << pd.x << ";" << pd.y << ")" << endl;
		 if (hasIntersection()) {
			 sf::Vector2<double> p = getIntersection();
			 ss << "Intersection at (" << p.x << ";" << p.y << ")" << endl;
			 if (isPointOnLineAB(p)) {
				 ss << "The intersection is within line AB." << endl;
			 } else {
				 ss << "The intersection is not within line AB." << endl;
			 }
			 if (isPointOnLineCD(p)) {
				 ss << "The intersection is within line CD." << endl;
			 } else {
				 ss << "The intersection is not within line CD." << endl;
			 }
		 } else {
			 ss << "The lines do not meet." << endl;
		 }
		 return ss.str();
	}


	static void testCase1(void) {
        sf::Vector2<double> a(1.0, 0.0);
        sf::Vector2<double> b(2.0, 15.0);
        sf::Vector2<double> c(5.0, 0.0);
        sf::Vector2<double> d(4.0, 3.0);
        LinearEquation le(a, b, c, d);
        cout << "Do the lines intersect somewhere? "<< (le.hasIntersection()?"Yes.":"No.") << endl;
        cout << le.describe() << endl;
	}

	static void testCase2(void) {
        sf::Vector2<double> a(1.0, 1.0);
        sf::Vector2<double> b(5.0, 5.0);
        sf::Vector2<double> c(3.0, 0.0);
        sf::Vector2<double> d(4.0, 6.0);
        LinearEquation le(a, b, c, d);
        cout << le.describe() << endl;
	}

};


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CLASS DECLARATIONS:                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TSPRouteAnalyzer {
    public:
		static bool findIntersections(TSPRoute * r) {
			int n=0;

			// i -> all segments
			for (size_t i=0; i<r->getSize(); i++) {
				int idxA = r->getStep(i);
				int idxB = r->getStep(i+1); // this wraps around at the end
				// j -> all segments
				for (size_t j=i+2; j<r->getSize(); j++) {
					if (i == 0 && j == r->getSize()-1) continue;
					if (i == 0 && j == r->getSize()-2) continue;
					if (i == 1 && j == r->getSize()-1) continue;
					// if (i+1==j || i==j || i==j+1) continue; // ignore the very same segment
					// also, adjoining segments should never overlap.
					// For routes between 2 points things look grim,
					// but let's ignore that for now.
					int idxC = r->getStep(j);
					int idxD = r->getStep(j+1); // this wraps around at the end

					sf::Vector2<double> a(points[idxA].getX(), points[idxA].getY());
					sf::Vector2<double> b(points[idxB].getX(), points[idxB].getY());
					sf::Vector2<double> c(points[idxC].getX(), points[idxC].getY());
					sf::Vector2<double> d(points[idxD].getX(), points[idxD].getY());

					LinearEquation le(a, b, c, d);

					if (!le.hasIntersection()) continue;

					sf::Vector2<double> intersection = le.getIntersection();
					if (le.isPointOnLineAB(intersection) && le.isPointOnLineCD(intersection)) {
						n++;

						// compare length of AB + CD to AD + BC:
						double ab = routingTable->getDistance(idxA, idxB);
						double cd = routingTable->getDistance(idxC, idxD);

						double ad = routingTable->getDistance(idxA, idxD);
						double bc = routingTable->getDistance(idxB, idxC);

						double reduction = (ab+cd) - (ad+bc);

						cout << "Segment " << i << " intersects with seg. " << j << ": ";
						cout << "AB (" << idxA << "-" << idxB << ") and ";
						cout << "CD (" << idxC << "-" << idxD << "), ";
						cout << "l=" << (ab+cd) << " vs. " << (ad+bc) << " (minus " << reduction <<  ")" << endl;
					}
				}
			}

			return (n>0);
		}
};


#endif
