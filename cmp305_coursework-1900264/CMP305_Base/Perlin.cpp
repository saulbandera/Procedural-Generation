#include "Perlin.h"
#include <cmath>

// initialize with the reference values for the permutation vector
Perlin::Perlin() {

	// initialize the permutation vector with the reference values (random arranged array of numbers 0-256)
	p = {
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
		8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
		134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
		55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
		18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
		250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
		189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
		43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
		97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
		107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
	// duplicate the permutation vector to avoid buffer overflow
	p.insert(p.end(), p.begin(), p.end());
}


double Perlin::noise(double x, double y) {

	// find the unit rectangle that contains the point x, y
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;

	// find relative x, y location of the point in the unit rectangle (from 0.0 to 1.0)
	x -= floor(x); 
	y -= floor(y);

	// compute fade curves for each of x, y, will ease coordinates towards intergal values (0.0 or 1.0)
	// giving a smoother result
	double u = fade(x);
	double v = fade(y);

	// hash the coordinates of the 4 rectangle corners surrounding point x, y
	int g1 = p[p[X] + Y];
	int g2 = p[p[X + 1] + Y];
	int g3 = p[p[X] + Y + 1];
	int g4 = p[p[X + 1] + Y + 1];



	// calculate the dot product of the random gradient and the 4 location 
	// vectors from the corners of the unit rectangle to the point (x, y). 
	// LOCATION VECTOR EXAMPLE: the location vector is (0,0) to (x,y) so x, y is used in d1,
	// the following would be from (1, 0) to (x,y) so final - initial gives: (x-1, y) and 
	// the same is done for (0, 1) and (1, 1) coordinates of the unit rectangle.
	double d1 = grad(g1, x, y);
	double d2 = grad(g2, x - 1, y);
	double d3 = grad(g3, x, y - 1);
	double d4 = grad(g4, x - 1, y - 1);


	// interpolate the different dot products based on the faded x, y positions
	// to create a weighted average 
	double x1Inter = lerp(u, d1, d2);
	double x2Inter = lerp(u, d3, d4);
	double yInter = lerp(v, lerp(u, d1, d2), lerp(u, d3, d4));

	return lerp(v, lerp(u, d1, d2), lerp(u, d3, d4));

}

double Perlin::fade(double t) {

	// fade function, eases the coordinate t towards integral values (0.0 or 1.0)
	// is used to produce a smoother result as coordinates change more gradually
	// when closer to either 0.0 or 1.0.
	return t * t * t * (t * (t * 6 - 15) + 10); // 6t^5 - 15t^4 + 10t^3
}

double Perlin::lerp(double t, double a, double b) {
	// linear interpolation function
	return a + t * (b - a);
}

double Perlin::grad(int hash, double distanceVecX, double distanceVecY)
{

	// returns the dot product of a randomly selected gradient vector out of:
	// (1,1), (-1,1), (1,-1), (-1,-1) and the input distance vector from a corner
	// of the rectangle to the position x, y
	switch (hash & 3) {
	case 0: return distanceVecX + distanceVecY;  // dot product of ( 1, 1) and (disVecX, disVecY)
	case 1: return -distanceVecX + distanceVecY; // dot product of (-1, 1) and (disVecX, disVecY)
	case 2: return distanceVecX - distanceVecY;  // dot product of ( 1,-1) and (disVecX, disVecY)
	case 3: return -distanceVecX - distanceVecY; // dot product of (-1,-1) and (disVecX, disVecY)
	default: return 0; // this never occurs 
	}
}
