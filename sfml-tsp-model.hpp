#ifndef TSP_MODEL
#define TSP_MODEL 1

using namespace std;


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CLASSES AND METHODS:                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TSPPoint {
    protected:
        double x;
        double y;
        sf::Vector2<double> * v2d;
    public:
        TSPPoint() { x=0; y=0; this->v2d = new sf::Vector2<double>(x,y); }
        TSPPoint(double x, double y) { this->x=x; this->y=y; this->v2d = new sf::Vector2<double>(x,y); }
        double getX(void) { return x; }
        double getY(void) { return y; }
        double getDistanceTo(double x, double y) {
            double dx = this->x - x;
            double dy = this->y - y;
            return sqrt(dx*dx + dy*dy);
        }
        double getDistanceTo(TSPPoint other) { return getDistanceTo(other.getX(), other.getY()); }
        sf::Vector2<double> const * getVector2(void) { return v2d; }
        // ~TSPPoint() { delete v2d; }

    // allow the following function to access private elements:
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
            for (size_t i=0; i<points.size(); i++) {
                for (size_t j=i+1; j<points.size(); j++) {
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
            for (size_t i=0; i<points.size(); i++) {
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
        TSPRoute * clone(void);
    // get / evaluate:
        bool equals(TSPRoute * other);
        int getIndexOf(int pointID);
        int getStep(int idx) { return seq[(idx + getSize()) % getSize()]; }
        size_t getSize(void) { return seq.size(); }
        bool isComplete(void);
        bool hasDuplicatePoints(void);
        double getLength(void);
    // modify:
        void addStep(int idx) {
        	seq.push_back(idx);
        	length = -1; // length has to be recalculated
        }
        void setStep(int idx, int point) {
            seq[(idx + getSize()) % getSize()] = point;
            length = -1; // length has to be recalculated
        }
        void moveStepForward(int idx);
        void reverse(void);
        void reverseFromTo(int a, int b);
    // output tools:
        string describe(void);
        string describePoints(void);
};

TSPRoute * TSPRoute::clone(void) {
    TSPRoute * retval = new TSPRoute();
    for (size_t i=0; i<seq.size(); i++) {
        retval->addStep(seq[i]);
    }

    return retval;
}

/**
 * move the point at idx forward by exactly 1 position (wraps around, if applicable)
 */
void TSPRoute::moveStepForward(int idx) {
	int ptA = this->getStep(idx);
	int ptB = this->getStep(idx + 1);
	this->setStep(idx, ptB);
	this->setStep(idx + 1, ptA);

    length = -1; // length has to be recalculated
}


void TSPRoute::reverse(void) {
	// create a new vector containing the elements in reverse order:
	vector<int> seq1;
	for (size_t i=seq.size(); i>0; i--) seq1.push_back(seq[i-1]);

	// copy back the new vector:
	for (size_t i=0; i<seq.size(); i++)	seq[i] = seq1[i];
}


/**
 * reverses a part of the route: points from index #a to #b (inclusive)
 */
void TSPRoute::reverseFromTo(int a, int b) {
	if (b < a) b+= this->getSize();
	size_t length = b - a + 1;
	// create a new vector containing the elements in reverse order:
	vector<int> seq1;
	for (size_t i=0; i<length; i++) {
		seq1.push_back(getStep(b-i));
	}

	// copy back the new vector:
	for (size_t i=0; i<length; i++) seq[a+i] = seq1[i];
}


bool TSPRoute::equals(TSPRoute * other) {
    if (other == NULL) return false;
	if (this->getSize() != other->getSize()) return false;
    for (size_t i=0; i<this->getSize(); i++) {
        if (this->seq[i] != other->seq[i]) return false;
    }
    // same size, all points are the same:
    return true;
}


bool TSPRoute::isComplete() {
    static bool found[TSP_N];
    for (size_t i=0; i<TSP_N; i++) found[i] = false;
    for (size_t i=0; i<seq.size(); i++) {
        found[seq[i]] = true;
    }
    for (size_t i=0; i<TSP_N; i++) {
        if (!found[i]) return false;
    }
    return true;
}

bool TSPRoute::hasDuplicatePoints() {
    static int cnt[TSP_N];
    for (size_t i=0; i<TSP_N; i++) cnt[i] = 0;
    for (size_t i=0; i<seq.size(); i++) {
        cnt[seq[i]] ++;
    }
    for (size_t i=0; i<TSP_N; i++) {
        if (cnt[i] > 1) return true;
    }
    return false;
}

double TSPRoute::getLength() {
    if (length >= 0) return length;

    // okay, we have to calculate:
    for (size_t i=0; i<seq.size(); i++) {
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

int TSPRoute::getIndexOf(int pointID) {
	for (size_t i=0; i<this->getSize(); i++) {
		if (this->getStep(i) == pointID) return i;
	}
	// not found!
	return -1;
}

string TSPRoute::describe(void) {
	stringstream ss;

	ss << "Route with " << this->getSize() << " points, l=" << this->getLength() << endl;
	if (this->isComplete()) {
		ss << "It contains all points." << endl;
	} else {
		ss << "It does not contain all points!" << endl;
	}
	if (this->hasDuplicatePoints()) ss << "It contains duplicate points!" << endl;

	// Append the points themselves:
	ss << this->describePoints();

	return ss.str();
}

string TSPRoute::describePoints(void) {
	stringstream ss;

	// Append the points themselves:
	size_t i;
	for (i=0; i<this->getSize(); i++) {
		ss << "#" << this->getStep(i);
		if (i < this->getSize() - 1) ss << "; ";
		if ((i+1) % 10 == 0) ss << endl;
	}
	if (this->getSize() % 10 != 0) ss << endl;

	return ss.str();
}


class TSPSplitRoute {
	private:
		TSPRoute * ra;
		TSPRoute * rb;
	public:
		TSPSplitRoute(TSPRoute * orig, size_t splitA, size_t splitB);
		string describe(void);
		void reverseB(void);
		TSPRoute * join(void);
		~TSPSplitRoute() { delete ra; delete rb; }
};

/**
 * @param splitA the index of the point in the route AFTER which the first cut should be made
 * @param splitB the index of the point in the route AFTER which the second cut should be made
 */
TSPSplitRoute::TSPSplitRoute(TSPRoute * orig, size_t splitA, size_t splitB) {
	if (orig == NULL) { throw new runtime_error("TSPSplitRoute: original route is NULL!"); exit(1); }
	if (splitA == splitB) { throw new runtime_error("TSPSplitRoute: split points A and B are identical!"); exit(1); }

	// make sure that point A is before B:
	if (splitA > splitB) {
		int temp = splitA;
		splitA = splitB;
		splitB = temp;
	}

	this->ra = new TSPRoute();
	for (size_t i=splitA + 1 ; i <= splitB; i++) {
		ra->addStep(orig->getStep(i));
	}

	// wrap over the end of the route:
	splitA += orig->getSize();

	this->rb = new TSPRoute();
	for (size_t i=splitB + 1 ; i <= splitA; i++) {
		rb->addStep(orig->getStep(i));
	}

	if (ra->getSize() + rb->getSize() != orig->getSize()) {
		throw new runtime_error("TSPSplitRoute: something went wrong (unexpected size of split routes...)!"); exit(1);
	}
}

void TSPSplitRoute::reverseB(void) {
	rb->reverse();
}

TSPRoute * TSPSplitRoute::join(void) {
	TSPRoute * retval = new TSPRoute();
	for (size_t i=0; i<ra->getSize(); i++) retval->addStep(ra->getStep(i));
	for (size_t i=0; i<rb->getSize(); i++) retval->addStep(rb->getStep(i));
	return retval;
}

string TSPSplitRoute::describe(void) {
	stringstream ss;
	ss << "Split route:" << endl;
	ss << "Part A: " << ra->describe();
	ss << "Part B: " << rb->describe();
	return ss.str();
}


class TSPRouter {
    public:
        static TSPRoute * naiveOrdered(void) {
            TSPRoute * r = new TSPRoute();
            for (size_t i=0; i<TSP_N; i++) r->addStep(i);
            return r;
        }
        static TSPRoute * naiveRandom(void) {
            int seq[TSP_N]; for (int i=0; i<TSP_N; i++) seq[i] = i;

            // shuffle the sequence 100 times completely:
            for (int rounds = 0; rounds<100; rounds++) {
                for (size_t i=0; i<TSP_N; i++) {
                    int j=rand() % TSP_N;

                    int temp = seq[i];
                    seq[i] = seq[j];
                    seq[j] = temp;
                }
            }

            TSPRoute * r = new TSPRoute();
            for (size_t i=0; i<TSP_N; i++) r->addStep(seq[i]);
            return r;
        }
        static TSPRoute * naiveClosest(void) {
            bool free[TSP_N];
            for (size_t i=0; i<TSP_N; i++) free[i] = true;

            TSPRoute * r = new TSPRoute();

            // add the origin:
            size_t currentIdx = 0;
            r->addStep(currentIdx);
            free[currentIdx] = false;

            // find TSP-N - 1 connections:
            for (size_t i=1; i<TSP_N; i++) {
                // cout << "Searching for the best destination from pt #" << currentIdx << ": " << endl;
                int closestIdx = -1;
                double closestDistance = 1e100;
                for (size_t j=0; j<TSP_N; j++) {
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
	protected:
    	int verbosity;
    	int successCount;
    	string lastMessage;
	public:
		TSPRouteOptimizer() { successCount=0; verbosity=0; }
        TSPRoute * optimizeStep(TSPRoute * r);
		TSPRoute * switchAnyTwoPoints(TSPRoute * r);
        TSPRoute * moveSinglePoint(TSPRoute * r);
        TSPRoute * moveSinglePointOLD(TSPRoute * r); // the original implementation
        TSPRoute * untangleIntersection(TSPRoute * r);
        void setVerbosity(int v) { if (v>=0 && v<=2) this->verbosity=v; }
        int getSuccessCount(void) { return successCount; }
        string getLastMessage(void) { return lastMessage; }
        ~TSPRouteOptimizer() {}
};

TSPRoute * TSPRouteOptimizer::optimizeStep(TSPRoute * r) {
	TSPRoute * candidate = NULL;

    if (candidate == NULL) {
		// try to simply switch two connected points:
		candidate = optimizer->switchAnyTwoPoints(currentRoute);
    }

    if (candidate == NULL) {
    	// try to eliminate an intersection:
    	candidate = optimizer->untangleIntersection(currentRoute);
    }

    if (candidate == NULL) {
    	// try to move any single point anywhere:
    	candidate = optimizer->moveSinglePoint(currentRoute);
    }

    return candidate;
}

TSPRoute * TSPRouteOptimizer::switchAnyTwoPoints(TSPRoute * original) {
    TSPRoute * r = original->clone();

    double benchmark = r->getLength();
    int actualSwitchIdx = -1;
    double switchLength = benchmark;

    // cout << "Trying to find a shorter route (<" << switchLength << ") by switching any two points:" << endl;

    for (size_t i=0; i<r->getSize(); i++) {
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
		successCount ++;

        // actually do:
        int idxA = r->getStep(actualSwitchIdx);
        int idxB = r->getStep(actualSwitchIdx+1);

        r->setStep(actualSwitchIdx, idxB);
        r->setStep(actualSwitchIdx+1, idxA);

		stringstream ss;
		ss << "Found a shorter (" << r->getLength() << ") route in switchAnyTwoPoints: " << idxA << "<->" << idxB << endl;
		if (verbosity > 0) cout << ss.str();
		lastMessage = ss.str();

        if (!r->isComplete()) {
            throw new runtime_error("switchAnyTwoPoints() produced an incomplete route!"); exit(1);
        }

        return r;
    } else {
        // free the unsuccessful temporary route:
        delete r;
    }

    return NULL;
}

TSPRoute * TSPRouteOptimizer::moveSinglePoint(TSPRoute * original) {
    double benchmark = original->getLength();
    size_t N = original->getSize();

    double bestLength = benchmark;
    int actualSwitchIdx = -1; // point at this index in the original route should be moved
    int actualSwitchShift = -1; // ... by this many positions (forward)
    TSPRoute * bestRoute = NULL;

    if (verbosity >= 1) {
		cout << "Current route: " << original->describe();
		cout << "Trying to find a shorter route (<" << benchmark << ") by moving any single point anywhere:" << endl;
    }

    for (size_t i=0; i<N; i++) {
        int idxA = original->getStep(i);
        TSPRoute * r = original->clone();

        if (verbosity >= 2) { cout << "  Considering point at #" << i << " which is p" << idxA << endl; }
        for (size_t j=0; j < N-2; j++) {
        	r->moveStepForward(i+j);
        	if (verbosity >= 3) { cout << "Step: The route is now " << r->describePoints(); }

			if (r->getLength() < bestLength) {
				if (verbosity >= 1) { cout << "    Found a better route! (l=" << r->getLength() << ")" << endl; }
				bestLength = r->getLength();

				actualSwitchIdx = i;
				actualSwitchShift = j+1;

				if (bestRoute != NULL) delete bestRoute;
				bestRoute = r->clone();
			} else {
				if (verbosity >= 2) { cout << "    ... not shorter: " << r->getLength() << endl; }
			}
        } // next shift amount
		delete r;
    } // next focus point

    if (bestRoute != NULL) {
    	this->successCount ++;
		stringstream ss;
		ss << "Found a shorter route in TSPRouteOptimizer::moveSinglePoint()" << endl;
		ss << "Moving point at " << actualSwitchIdx << " by " << actualSwitchShift << " positions." << endl;
		if (verbosity >= 2) ss << bestRoute->describe();
		this->lastMessage = ss.str();
		if (verbosity >= 1) cout << ss.str();

        if (!bestRoute->isComplete()) {
            throw new runtime_error("TSPRouteOptimizer::moveSinglePoint() produced an incomplete route!"); exit(1);
        }
    }

    return bestRoute;
}

TSPRoute * TSPRouteOptimizer::moveSinglePointOLD(TSPRoute * original) {
    double benchmark = original->getLength();
    size_t N = original->getSize();

    double bestLength = benchmark;
    int actualSwitchIdx = -1; // point at this index in the original route should be moved
    int actualSwitchShift = -1; // ... by this many positions (forward)
    TSPRoute * bestRoute = NULL;

    if (verbosity >= 1) {
		cout << "Current route: " << original->describe();
		cout << "Trying to find a shorter route (<" << benchmark << ") by moving any single point anywhere:" << endl;
    }

    for (size_t i=0; i<N; i++) {
        int idxA = original->getStep(i);

        if (verbosity >= 2) { cout << "  Considering point at #" << i << " which is p" << idxA << endl; }
        for (size_t j=1; j<N-1; j++) {
            TSPRoute * r = original->clone();

            if (verbosity >= 2) { cout << "    Considering moving the next " << j << " points over..." << endl; }

            for (size_t k=0; k<j; k++) {

    			// pull the elements in-between forward:
                // cout << "      Pulling forward point at #" << (i+k+1) << "..." << endl;
                r->setStep(i+k, r->getStep(i+k+1));
            }
            // and place the original element:
            // cout << "      Placing original point " << idxA << " at #" << (i+j) << "..." << endl;
            r->setStep(i+j, idxA);

            // cout << "    Resulting route: " << r->describePoints();

			if (r->getLength() < bestLength) {
				if (verbosity >= 1) { cout << "    Found a better route! (l=" << r->getLength() << ")" << endl; }
				bestLength = r->getLength();

				actualSwitchIdx = i;
				actualSwitchShift = j;

				if (bestRoute != NULL) delete bestRoute;
				bestRoute = r;
			} else {
				if (verbosity >= 2) { cout << "    ... not shorter: " << r->getLength() << endl; }
				delete r;
			}
        } // next shift amount
    } // next focus point

    if (bestRoute != NULL) {
    	this->successCount ++;
		stringstream ss;
		ss << "Found a shorter route in TSPRouteOptimizer::moveSinglePoint()" << endl;
		ss << "Moving point at " << actualSwitchIdx << " by " << actualSwitchShift << " positions." << endl;
		if (verbosity >= 2) ss << bestRoute->describe();
		this->lastMessage = ss.str();
		if (verbosity >= 1) cout << ss.str();

        if (!bestRoute->isComplete()) {
            throw new runtime_error("TSPRouteOptimizer::moveSinglePoint() produced an incomplete route!"); exit(1);
        }
    }

    return bestRoute;
}

TSPRoute * TSPRouteOptimizer::untangleIntersection(TSPRoute * r) {
	TSPSplitRoute * split = TSPRouteAnalyzer::findIntersections(r);

	// do we even have intersections?
	if (split == NULL) return NULL;

	this->successCount ++;

	// part B of the split routes has already been reversed
	TSPRoute * retval = split->join();

	stringstream ss;
	ss << "Found a shorter route in TSPRouteOptimizer::untangleIntersection()" << endl;
	ss << split->describe();
	this->lastMessage = ss.str();
	if (verbosity >= 1) cout << ss.str();

    if (!retval->isComplete()) {
        throw new runtime_error("TSPRouteOptimizer::untangleIntersection() produced an incomplete route!"); exit(1);
    }

	return retval;
}



void TSPRouteHistory::add(TSPRoute* r) {
	// do we already have this one?
	for (size_t i=0; i<this->data->size(); i++) {
		TSPRoute * other = this->data->at(i);
		if (r->equals(other)) return; // ignore...
	}
	data->push_back(r);
}

void TSPRouteHistory::back(void) {
	if (this->data->size() == 0) return;

	TSPRoute * r = data->back();
	if (r == NULL) return; // never set the current route to NULL

	currentRoute = r;
    painter->updateRoute(currentRoute);

	data->pop_back();
}





/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// STAND-ALONE FUNCTIONS                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

void createPoints(void) {
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
		} catch (const out_of_range &ex) {
            cout << ex.what() << endl;
        }
    }
}

void deletePoints(void) {
//    for (int i=0; i<points.size(); i++) {
//        delete(points[i]);
//    }
}

void deleteRoutingTable() { delete(routingTable); routingTable = NULL; }

/**
 * sets a new currentRoute and appends the old one (if exists!) to the route history.
 */
void setCurrentRoute(TSPRoute * r) {
    if (r == NULL) {
        throw runtime_error("Refusing to set currentRoute to NULL!"); exit(1);
    }
    if (currentRoute != NULL) {
        routeHistory->add(currentRoute);
    }
    currentRoute = r;

    painter->updateRoute(currentRoute);
}

void setRandomRoute(void) {
    TSPRoute * r = TSPRouter::naiveRandom();

    if (currentRoute == NULL) {
        setCurrentRoute(r);
        return;
    }

    // only if this one is better:
    if (r->getLength() < currentRoute->getLength()) {
        cout << "Accepting new rnd. route bec. length " << r->getLength() << "." << endl;
        setCurrentRoute(r);
    } else {
        cout << "Not accepting new route because of length " << r->getLength() << "." << endl;
    }
}

#endif
