#ifndef TSP_GFX
#define TSP_GFX 1

#define CANVAS_W 750
#define CANVAS_H 750

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// GLOBAL VARIABLES:                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CLASSES AND METHODS:                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TSPPainter {
    protected:
        vector<sf::CircleShape> dots;
        sf::Vertex routeLine[TSP_N+1];
        // canvas position and size:
        int canvasX0, canvasX1, canvasSX;
        int canvasY0, canvasY1, canvasSY;
        // logical position and size:
        double x0, x1, xSize;
        double y0, y1, ySize;
    public:
        TSPPainter(void) {
            canvasX0 = 0; canvasX1 = 750; canvasSX = canvasX1 - canvasX0;
            canvasY0 = 0; canvasY1 = 750; canvasSY = canvasY1 - canvasY0;
        }
        void setCanvas(int x0, int y0, int x1, int y1) {
            canvasX0 = x0; canvasX1 = x1; canvasSX = canvasX1 - canvasX0;
            canvasY0 = y0; canvasY1 = y1; canvasSY = canvasY1 - canvasY0;
        }
        void updatePoints(vector<TSPPoint> data);
        void paintPoints(sf::RenderWindow * window, int hightlight);
        void updateRoute(TSPRoute * r);
        void paintRoute(sf::RenderWindow * window);
        // convert between logical and screen coords:
        int x2px(double x);
        int y2py(double y);
        double px2x(int x);
        double py2y(int y);

};

int TSPPainter::x2px(double x) {
    double fraction = (x - this->x0) / this->xSize;
    return fraction * this->canvasSX + this->canvasX0;
}
int TSPPainter::y2py(double y) {
    double fraction = (y - this->y0) / this->ySize;
    return fraction * this->canvasSY + this->canvasY0;
}
double TSPPainter::px2x(int x) {
    double fraction = (double)(x - this->canvasX0) / this->canvasSX;
    return fraction * this->xSize + this->x0;
}
double TSPPainter::py2y(int y) {
    double fraction = (double)(y - this->canvasY0) / this->canvasSY;
    return fraction * this->ySize + this->y0;
}

void TSPPainter::updatePoints(vector<TSPPoint> data) {
    // TODO: re-calculate the conversion variables coords logical <-> screen
    double minX = data[0].getX();
    double maxX = data[0].getX();
    double minY = data[0].getY();
    double maxY = data[0].getY();
    for (int i=0; i<data.size(); i++) {
        TSPPoint * p = &data[i];
        if (p->getX() < minX) minX = p->getX();
        if (p->getX() > maxX) maxX = p->getX();
        if (p->getY() < minY) minY = p->getY();
        if (p->getY() > maxY) maxY = p->getY();
    }
    // allow 5% margins at min and max:
    this->xSize = (maxX - minX) * 1.1;
    this->x0 = minX - this->xSize * 0.05;
    this->x1 = maxX + this->xSize * 0.05;
    this->ySize = (maxY - minY) * 1.1;
    this->y0 = minY - this->ySize * 0.05;
    this->y1 = maxY + this->ySize * 0.05;


    dots.clear();
    dots.reserve(TSP_N);

    vector<TSPPoint>::iterator i;
    for(i=data.begin(); i!=data.end(); ++i) {
        // cout << (*i) << endl;
        sf::CircleShape s(5.f);
        s.setFillColor(getRandomColor());
        s.move(-5, -5); // move center to 0;0
        s.move(
            this->x2px((*i).getX()),
            this->y2py((*i).getY())
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
        int x = this->x2px(points[idx].getX());
        int y = this->y2py(points[idx].getY());
        this->routeLine[i] = sf::Vertex(sf::Vector2f(x, y));
    }
    // ... back to the first point (closest path):
    int idx = r->getStep(0);
    int x = this->x2px(points[idx].getX());
    int y = this->y2py(points[idx].getY());
    this->routeLine[r->getSize()] = sf::Vertex(sf::Vector2f(x, y));
}

void TSPPainter::paintRoute(sf::RenderWindow * window) {
    window->draw(routeLine, TSP_N+1, sf::LineStrip);
}

#endif
