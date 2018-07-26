#ifndef TSP_GFX
#define TSP_GFX 1

#include <stdio.h>

#define CANVAS_W 750
#define CANVAS_H 750


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CLASSES AND METHODS:                                                    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// class TSPPainter declared in sfml-tsp-class-declarations.hpp

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
    double minX = data[0].getX();
    double maxX = data[0].getX();
    double minY = data[0].getY();
    double maxY = data[0].getY();
    for (size_t i=0; i<data.size(); i++) {
        TSPPoint * p = &data[i];
        if (p->getX() < minX) minX = p->getX();
        if (p->getX() > maxX) maxX = p->getX();
        if (p->getY() < minY) minY = p->getY();
        if (p->getY() > maxY) maxY = p->getY();
    }
    this->xSize = (maxX - minX);
    this->ySize = (maxY - minY);
    double maxSize = (this->xSize > this->ySize) ? this->xSize : this->ySize;
    double cx = (minX + maxX) / 2.0; // x center
    double cy = (minY + maxY) / 2.0; // y center

    // allow 5% margins at min and max:
    this->x0 = cx - maxSize * 0.55;
    this->x1 = cx + maxSize * 0.55;
    this->y0 = cy - maxSize * 0.55;
    this->y1 = cy + maxSize * 0.55;

    this->xSize = maxSize * 1.1;
    this->ySize = maxSize * 1.1;

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

void TSPPainter::paintPoints(sf::RenderWindow * window, size_t highlight) {
    for(size_t i=0; i<dots.size(); i++) {
        if (i==highlight) {
            dots[i].setRadius(20);
            dots[i].move(-15,-15);
        }

        window->draw(dots[i]);

        if (i==highlight) {
            dots[i].setRadius(5);
            dots[i].move(15,15);
        }

        if (this->paintPointLabels) {
        	sf::Text text;
        	text.setFont(font0);
        	text.setString(to_string(i));
        	text.setCharacterSize(14); // in pixels, not points!
        	text.setFillColor(sf::Color::Red);
        	// text.setStyle(sf::Text::Bold | sf::Text::Underlined);

        	text.move(dots[i].getPosition().x -16, dots[i].getPosition().y - 6);
        	window->draw(text);
        }
    }
}

void TSPPainter::updateRoute(TSPRoute * r) {
    for (size_t i=0; i<r->getSize(); i++) {
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

    if (this->paintPointLabels) {
        for(size_t i=0; i<currentRoute->getSize(); i++) {
        	// which original point is at this position?
        	int pointIdx = currentRoute->getStep(i);

			sf::Text text;
			text.setFont(font0);
			text.setString(to_string(i)); // print the position within the route!
			text.setCharacterSize(14); // in pixels, not points!
			text.setFillColor(sf::Color::Green);
			// text.setStyle(sf::Text::Bold | sf::Text::Underlined);

			text.move(dots[pointIdx].getPosition().x +12, dots[pointIdx].getPosition().y - 6);
			window->draw(text);
        }
    }

    // display current route length:
    if (currentRoute != NULL) {
    	char buffer[32];
    	char* pBuffer = &buffer[0];
    	sprintf(pBuffer, "%02.3lf", currentRoute->getLength());
    	string s("l=");
    	s += pBuffer;

		sf::Text text;
		text.setFont(font0);
		text.setString(s); // print the position within the route!
		text.setCharacterSize(14); // in pixels, not points!
		text.setFillColor(sf::Color::White);
		// text.setStyle(sf::Text::Bold | sf::Text::Underlined);

		text.move(10, 10);
		window->draw(text);
    }

    // display count of successful optimizations:
    if (currentRoute != NULL) {
    	string s("N(opt)=");
    	s += to_string(optimizer->getSuccessCount());

		sf::Text text;
		text.setFont(font0);
		text.setString(s); // print the position within the route!
		text.setCharacterSize(14); // in pixels, not points!
		text.setFillColor(sf::Color::White);
		// text.setStyle(sf::Text::Bold | sf::Text::Underlined);

		text.move(this->canvasX1 - 100, 10);
		window->draw(text);
    }

}

#endif
