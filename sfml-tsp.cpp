/**
 * because the program did not run on my Laptop (on a GeForce Go 7300),
 * I had to switch to software rendering, using an entry in ~/.bashrc:
 *
 * export LIBGL_ALWAYS_SOFTWARE=1
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib> // for rand() and srand()
#include <cmath> // for sqrt()
#include <SFML/Graphics.hpp>

#define TSP_N 15 // Number of desired points in the TSP model

#include "sfml-tsp-class-declarations.hpp"
#include "sfml-tsp-global.hpp"
#include "sfml-tsp-model.hpp"
#include "sfml-tsp-analyses.hpp"
#include "sfml-tsp-gfx.hpp"


void init(void) {
    currentRoute = NULL;
    painter = new TSPPainter();
    routeHistory = new TSPRouteHistory();
    optimizer = new TSPRouteOptimizer();

    // create and set up the application's data model:
    createPoints();
    painter->updatePoints(points);

    createRoutingTable();
    cout << routingTable->debug();

    // setCurrentRoute(TSPRouter::naiveOrdered());
    // setCurrentRoute(TSPRouter::naiveClosest());
    setRandomRoute();
    cout << "Current Route is " << (currentRoute->isComplete()?"complete":"not complete") << "." << endl;
    cout << "Route length: " << currentRoute->getLength() << "." << endl;
}

void destroy(void) {
    // clean up after the application:

    delete optimizer; optimizer = NULL;
    delete routeHistory; routeHistory = NULL;
    delete painter; painter = NULL;

    deletePoints(); // in sfml-tsp-model.cpp
    deleteRoutingTable();
}

int main() {
    // LinearEquation::testCase2(); exit(1);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(
        sf::VideoMode(CANVAS_W, CANVAS_H),
        "Travelling Salesman Problem using SFML",
        sf::Style::Default,
        settings
    );

    // sf::CircleShape circle1(100.f);
    // circle1.setFillColor(sf::Color::Red);
    // circle1.move(-100,-100);

    // never use both of the following settings at the same time:
    window.setVerticalSyncEnabled(true); // call it once, after creating the window
    // window.setFramerateLimit(60); // call it once, after creating the window

    window.setKeyRepeatEnabled(false);

    init();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Resized:
                    std::cout << "new width: " << event.size.width << std::endl;
                    std::cout << "new height: " << event.size.height << std::endl;
                    painter->setCanvas(0,0,event.size.width, event.size.height);
                    break;

                // window closed
                case sf::Event::Closed:
                    window.close();
                    destroy();
                    break;

                // key pressed
                case sf::Event::KeyPressed:
                    std::cout << "key pressed: " << event.key.code << std::endl;
                    if (event.key.code == sf::Keyboard::Space) {
                        setRandomRoute();
                    }
                    if (event.key.code == sf::Keyboard::O) {
                        // optimize the current route:
                        TSPRoute * candidate = optimizer->switchAnyTwoPoints(currentRoute);
                        if (candidate != NULL) setCurrentRoute(candidate);
                    }
                    if (event.key.code == sf::Keyboard::B) {
                        // one step back in the route history:
						routeHistory->back();
                    }
                    break;

                // mouse events
                case sf::Event::MouseEntered:
                    // std::cout << "the mouse cursor has entered the window" << std::endl;
                	break;

                case sf::Event::MouseLeft:
                    // std::cout << "the mouse cursor has left the window" << std::endl;
                	break;

                case sf::Event::MouseMoved:
                    currentMouseX = event.mouseMove.x;
                    currentMouseY = event.mouseMove.y;
                    highlightedPoint = routingTable->findClosestPointIdx(
                        painter->px2x(currentMouseX),
                        painter->py2y(currentMouseY)
                    );
                    break;

                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Right) {
                    	int x = event.mouseButton.x;
                    	int y = event.mouseButton.y;
                    	cout << "the right button was pressed @(";
                        cout << x << ";" << y << "); closest point #";
                        int pointIdx = routingTable->findClosestPointIdx(painter->px2x(x), painter->py2y(y));
                        cout << pointIdx << " at position ";
                        cout << currentRoute->getIndexOf(pointIdx);
                        cout << " of the route." << endl;
                    }
                    break;

                // text entered
                case sf::Event::TextEntered:
                    // if (event.text.unicode < 128)
                    //    cout << "ASCII character typed: " << static_cast<char>(event.text.unicode) << std::endl;
                    break;

                // we don't process other types of events
                default: break;
            }
        }

        // clear the window with black color:
        window.clear(sf::Color::Black);

        // window.draw(circle1);

        // paint all points, highlight the point which is closest to the mouse pointer:
        painter->paintPoints(&window, highlightedPoint);

        // draw the current route:
        painter->paintRoute(&window);

        // internally swaps the front and back buffers:
        window.display();

    }

    return 0;
}
