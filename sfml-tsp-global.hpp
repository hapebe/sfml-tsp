#ifndef TSP_GLOBAL
#define TSP_GLOBAL 1

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

/**
 * sets a new currentRoute and appends the old one (if exists!) to the route history.
 */
void setCurrentRoute(TSPRoute * r) {
    if (r == NULL) {
        throw runtime_error("Refusing to set currentRoute to NULL!"); exit(1);
    }
    if (currentRoute != NULL) {
        routeHistory.push_bash(currentRoute);
    }
    currentRoute = r;

    painter->updateRoute(currentRoute);
}

#endif
