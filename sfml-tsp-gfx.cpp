int coord2screenX(double x) { return x * (wWidth/2) + wWidth/2; }
int coord2screenY(double y) { return y * (wHeight/2) + wHeight/2; }
double screen2coordX(int x) { return ((double)x - (wWidth/2)) / (wWidth/2); }
double screen2coordY(int y) { return ((double)y - (wHeight/2)) / (wHeight/2); }


class TSPPainter;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// GLOBAL VARIABLES:                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

TSPPainter * painter;



/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CLASSES AND METHODS:                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TSPPainter {
    protected:
        vector<sf::CircleShape> dots;
        sf::Vertex routePoints[TSP_N+1];
    public:
        void updatePoints(vector<TSPPoint> data);
        void paintPoints(sf::RenderWindow * window, int hightlight);
        void updateRoute(TSPRoute * r);
        void paintRoute(sf::RenderWindow * window);
};

void TSPPainter::updatePoints(vector<TSPPoint> data) {
    dots.clear();
    dots.reserve(TSP_N);

    vector<TSPPoint>::iterator i;
    for(i=data.begin(); i!=data.end(); ++i) {
        // cout << (*i) << endl;
        sf::CircleShape s(5.f);
        s.setFillColor(getRandomColor());
        s.move(-5, -5); // move center to 0;0
        s.move(
            coord2screenX((*i).getX()),
            coord2screenY((*i).getY())
        );
        dots.push_back(s);
    }
}

void TSPPainter::paintPoints(sf::RenderWindow * window, int highlight) {
    for(int i=0; i<dots.size(); i++) {
        if (i==highlight) {
            dots[i].setRadius(20);
            dots[i].move(-15,-15);
        }

        window->draw(dots[i]);

        if (i==highlight) {
            dots[i].setRadius(5);
            dots[i].move(15,15);
        }
    }
}

void TSPPainter::updateRoute(TSPRoute * r) {
    for (int i=0; i<r->getSize(); i++) {
        int idx = r->getStep(i);
        int x = coord2screenX(points[idx].getX());
        int y = coord2screenY(points[idx].getY());
        this->routePoints[i] = sf::Vertex(sf::Vector2f(x, y));
    }
    // ... back to the first point (closest path):
    int idx = r->getStep(0);
    int x = coord2screenX(points[idx].getX());
    int y = coord2screenY(points[idx].getY());
    this->routePoints[r->getSize()] = sf::Vertex(sf::Vector2f(x, y));
}

void TSPPainter::paintRoute(sf::RenderWindow * window) {
    window->draw(routePoints, TSP_N+1, sf::LineStrip);
}
