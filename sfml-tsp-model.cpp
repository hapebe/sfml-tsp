#define TSP_N 101 // Number of desired points in the TSP model
#include <sstream>
#include <vector>
using namespace std;

class TSPPoint;
class TSPRoutingTable;
class TSPRoute;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// GLOBAL VARIABLES:                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

vector<TSPPoint> points(TSP_N);
TSPRoutingTable * routingTable;
TSPRoute * currentRoute;



/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CLASSES AND METHODS:                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TSPPoint {
    protected:
        double x;
        double y;
    public:
        TSPPoint() { this->x=0; this->y=0; }
        TSPPoint(double x, double y) { this->x=x; this->y=y; }
        double getX(void) { return x; }
        double getY(void) { return y; }
        double getDistanceTo(double x, double y) {
            double dx = this->x - x;
            double dy = this->y - y;
            return sqrt(dx*dx + dy*dy);
        }
        double getDistanceTo(TSPPoint other) { return getDistanceTo(other.getX(), other.getY()); }

    // allow the following funcation to access private elements:
    friend ostream& operator<<(ostream& os, TSPPoint const& p);
};

ostream& operator<<(ostream& os, TSPPoint const& p) {
    os << "TSPPoint(" << p.x << ";" << p.y << ")";
    return os;
}

class TSPRoutingTable {
    private:
        int n;
        double distances[TSP_N][TSP_N];
    public:
        TSPRoutingTable(vector<TSPPoint> points) {
            if (points.size() > TSP_N) {
                cout << "Too many points in constructor of TSPRoutingTable!" << endl;
                exit(1);
            }
            n = points.size();
            for (int i=0; i<points.size(); i++) {
                for (int j=i+1; j<points.size(); j++) {
                    distances[i][j] = points[i].getDistanceTo(points[j]);
                }
            }
        }
        double getDistance(int i, int j) {
            if (i<j) return distances[i][j];
            if (i==j) return 0;
            if (i>j) return distances[j][i];
            return 0;
        }
        string debug(void) {
            stringstream s("");
            s << "TSPRoutingTable for " << n << " points, i.e. " << (n*(n-1)/2) << " relations." << endl;
            return s.str();
        }
        int findClosestPointIdx(double x, double y) {
            double minD = 1e100;
            double minIdx = -1;

            // iterate all points, remember the one with the least distance to x;y
            for (int i=0; i<points.size(); i++) {
                double d = points[i].getDistanceTo(x,y);
                if (d < minD) {
                    minD = d;
                    minIdx = i;
                }
            }

            return minIdx;
        }
};

class TSPRoute {
    protected:
        double length;
        vector<int> seq;
    public:
        TSPRoute() { this->length = -1; }
        void addStep(int idx) { seq.push_back(idx); }
        int getStep(int idx) { return seq[(idx + getSize()) % getSize()]; }
        void setStep(int idx, int point) {
            seq[(idx + getSize()) % getSize()] = point;
            length = -1; // length has to be recalculated
        }
        bool isComplete(void);
        int getSize(void) { return seq.size(); }
        double getLength(void);
};

bool TSPRoute::isComplete() {
    static bool found[TSP_N];
    for (int i=0; i<TSP_N; i++) found[i] = false;
    for (int i=0; i<seq.size(); i++) {
        found[seq[i]] = true;
    }
    for (int i=0; i<TSP_N; i++) {
        if (!found[i]) return false;
    }
    return true;
}

double TSPRoute::getLength() {
    if (length >= 0) return length;

    // okay, we have to calculate:
    for (int i=0; i<seq.size(); i++) {
        int from = seq[i];
        int to = -1;
        if (i<seq.size()-1) {
            to = seq[i+1]; // either the next point,
        } else {
            to = seq[0]; // or back to the first one.
        }

        length += points[from].getDistanceTo(points[to]);
    }

    return length;
}

class TSPRouter {
    public:
        static TSPRoute * naiveOrdered(void) {
            TSPRoute * r = new TSPRoute();
            for (int i=0; i<TSP_N; i++) r->addStep(i);
            return r;
        }
        static TSPRoute * naiveRandom(void) {
            int seq[TSP_N]; for (int i=0; i<TSP_N; i++) seq[i] = i;

            // shuffle the sequence 100 times completely:
            for (int rounds = 0; rounds<100; rounds++) {
                for (int i=0; i<TSP_N; i++) {
                    int j=rand() % TSP_N;

                    int temp = seq[i];
                    seq[i] = seq[j];
                    seq[j] = temp;
                }
            }

            TSPRoute * r = new TSPRoute();
            for (int i=0; i<TSP_N; i++) r->addStep(seq[i]);
            return r;
        }
        static TSPRoute * naiveClosest(void) {
            bool free[TSP_N];
            for (int i=0; i<TSP_N; i++) free[i] = true;

            TSPRoute * r = new TSPRoute();

            // add the origin:
            int currentIdx = 0;
            r->addStep(currentIdx);
            free[currentIdx] = false;

            // find TSP-N - 1 connections:
            for (int i=1; i<TSP_N; i++) {
                // cout << "Searching for the best destination from pt #" << currentIdx << ": " << endl;
                int closestIdx = -1;
                double closestDistance = 1e100;
                for (int j=0; j<TSP_N; j++) {
                    if (j==currentIdx) continue;
                    if (!free[j]) continue;
                    double d = routingTable->getDistance(currentIdx, j);
                    if (d < closestDistance) {
                        // cout << "    " << j << " is closer to " << currentIdx << ": " << d << endl;
                        closestIdx = j;
                        closestDistance = d;
                    }
                }
                // add the closest point:
                r->addStep(closestIdx);
                free[closestIdx] = false;

                // cout << "Travelling to pt #" << closestIdx << "..." << endl;
                // move forward:
                currentIdx = closestIdx;
            }
            return r;
        }
};

class TSPRouteOptimizer {
public:
static bool switchAnyTwoPoints(TSPRoute * r) {
    double benchmark = r->getLength();
    int actualSwitchIdx = -1;
    double switchLength = benchmark;

    // cout << "Trying to find a shorter route (<" << switchLength << ") by switching any two points:" << endl;

    for (int i=0; i<r->getSize(); i++) {
        int idxA = r->getStep(i);
        int idxB = r->getStep(i+1);

        // try:
        r->setStep(i, idxB);
        r->setStep(i+1, idxA);

        if (r->getLength() < switchLength) {
            actualSwitchIdx = i;
            switchLength = r->getLength();
        }

        // restore:
        r->setStep(i, idxA);
        r->setStep(i+1, idxB);
    }

    if (actualSwitchIdx >= 0) {

        // actually do:
        int idxA = r->getStep(actualSwitchIdx);
        int idxB = r->getStep(actualSwitchIdx+1);

        r->setStep(actualSwitchIdx, idxB);
        r->setStep(actualSwitchIdx+1, idxA);

        cout << "Found a shorter (" << r->getLength() << ") route in switchAnyTwoPoints: " << idxA << "<->" << idxB << endl;

        if (!r->isComplete()) {
            throw new runtime_error("switchAnyTwoPoints() produced an incomplete route!");
        }

        return true;
    }

    return false;
}
};




/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// STAND-ALONE FUNCTIONS                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void createPoints(void) {
    srand(0); // use a fixed random seed, so the point configuration becomes predictable

    for (int i=0; i<TSP_N; i++) {
        double x = 0;
        for (int j=0; j<12; j++) x += randomDouble();
        x = (x / 3) - 2; // -2..+2;

        double y = 0;
        for (int j=0; j<12; j++) y += randomDouble();
        y = (y / 3) - 2; // -2..+2;

        TSPPoint p(x,y);
        try{
            points.at(i) = p;
        } catch(out_of_range ex) {
            cout << ex.what() << endl;
        }
    }
}

void deletePoints(void) {
//    for (int i=0; i<points.size(); i++) {
//        delete(points[i]);
//    }
}

void createRoutingTable() {
    routingTable = new TSPRoutingTable(points);
}

void deleteRoutingTable() { delete(routingTable); routingTable = NULL; }
