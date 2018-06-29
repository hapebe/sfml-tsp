#ifndef TSP_GLOBAL
#define TSP_GLOBAL 1

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// GLOBAL VARIABLES:                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

vector<TSPPoint> points(TSP_N);
TSPRoutingTable * routingTable;
TSPRoute * currentRoute;
vector<TSPRoute *> routeHistory;
TSPPainter * painter;




int currentMouseX = -1;
int currentMouseY = -1;

int highlightedPoint = -1;

const sf::Color getRandomColor(void) {
    int rnd = rand() % 8;
    switch(rnd) {
        case 0: return sf::Color::Blue;
        case 1: return sf::Color::Green;
        case 2: return sf::Color::Red;
        case 3: return sf::Color::Magenta;
        case 4: return sf::Color::White;
        case 5: return sf::Color::Cyan;
        case 6: return sf::Color::Yellow;
        case 7: return sf::Color(127,127,127);
    }
    return sf::Color::White;
}

double randomDouble(void) {
    long max = RAND_MAX;
    return (double)rand() / max;
}

#endif
