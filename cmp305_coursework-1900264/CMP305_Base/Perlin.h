#include <vector>
#include <iostream>
// this class is a 2D perlin noise implementation based on the improved perlin 
// function java implementation, copyright 2002 Ken Perlin. http://mrl.nyu.edu/~perlin/noise/

class Perlin {
	std::vector<int> p; // The permutation vector

public:
	// Initialize with the reference values for the permutation vector
	Perlin();

	double noise(double x, double y);

private:
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double distanceVecX, double distanceVecY);
};


