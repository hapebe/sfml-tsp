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
class TSPRouteHistory;
class TSPPainter;
class TSPRouteOptimizer;


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// CLASS DECLARATIONS:                                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

class TSPRouteHistory {
    private:
        vector<TSPRoute *> * data;
    public:
        TSPRouteHistory(void) { data = new  vector<TSPRoute *>(100); }
        ~TSPRouteHistory(void) { delete data; }
        void add(TSPRoute * r);
        void back(void);
};

class TSPPainter {
    protected:
		sf::Font font0;
        vector<sf::CircleShape> dots;
        sf::Vertex routeLine[TSP_N+1];
        // canvas position and size:
        int canvasX0, canvasX1, canvasSX;
        int canvasY0, canvasY1, canvasSY;
        // logical position and size:
        double x0, x1, xSize;
        double y0, y1, ySize;
        bool paintPointLabels;
    public:
        TSPPainter(void) {
        	string font0File = FONT0;
        	if (!font0.loadFromFile(font0File)) {
        	    cout << "Could not load font: " << font0File << endl;
        	}
            x0=0, x1=0, xSize=0;
            y0=0, y1=0, ySize=0;
            canvasX0 = 0; canvasX1 = 750; canvasSX = canvasX1 - canvasX0;
            canvasY0 = 0; canvasY1 = 750; canvasSY = canvasY1 - canvasY0;
            paintPointLabels = true;
        }
        void setCanvas(int x0, int y0, int x1, int y1) {
            canvasX0 = x0; canvasX1 = x1; canvasSX = canvasX1 - canvasX0;
            canvasY0 = y0; canvasY1 = y1; canvasSY = canvasY1 - canvasY0;
        }
        void updatePoints(vector<TSPPoint> data);
        void paintPoints(sf::RenderWindow * window, size_t hightlight);
        void updateRoute(TSPRoute * r);
        void paintRoute(sf::RenderWindow * window);
        // convert between logical and screen coords:
        int x2px(double x);
        int y2py(double y);
        double px2x(int x);
        double py2y(int y);
        ~TSPPainter() { }

};


#endif // SFML_TSP_CLASS_DECLARATIONS
