#ifndef SFML_TSP_CLASS_DECLARATIONS
#define SFML_TSP_CLASS_DECLARATIONS 1

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// FORWARD DECLARATIONS:                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TSPRoute;
class TSPPoint;
class TSPRoutingTable;
class TSPPainter;


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CLASS DECLARATIONS:                                                     //
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


#endif // SFML_TSP_CLASS_DECLARATIONS
