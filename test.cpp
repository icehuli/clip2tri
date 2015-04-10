
#include "clipper/clipper.hpp"
#include "poly2tri/poly2tri.h"
#include "clip2tri/clip2tri.h"

#include <iostream>
#include <cstdio>

using namespace std;
using namespace c2t;


// TODO actually fill out some test data...  this code compiles, but doesn't work!
int main() {
	vector<vector<Point> > inputPolygons;
	//{ { Point(0,0), Point(10,0), Point(10,10), Point(0,10) } };
   vector<Point> outputTriangles;  // Every 3 points is a triangle

   vector<Point> boundingPolygon = { Point(-1, -1), Point(11, -1), Point(11, 11), Point(-1, 11) };

   clip2tri clip2tri;
   clip2tri.triangulate(inputPolygons, outputTriangles, boundingPolygon);
}
