#pragma once
#include "voro/voro++.hh"

typedef std::vector<std::vector<int>> Faces;
typedef std::vector<std::vector<double>> Points;
typedef std::pair<Points, Faces> Geometry;

class Voronoi {
	
public:
	static void createVoronoiFile(int numSeeds, std::vector<float> offset, float scale, std::string outputFileName);

	static std::vector<Geometry> parseVoronoi(std::string voronoiFile, bool flip);
	static void triangulateFaces(Faces &trifaceData, const Faces &faceData, bool flip);
};