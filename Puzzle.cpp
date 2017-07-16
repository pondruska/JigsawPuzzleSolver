/**
 * Automatic assembly of jigsaw puzzles from digital images
 * Charles University in Prague
 * Bachelor thesis
 * Peter Ondruska
 * 2011
 */

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <climits>
#include <cstring>
#include <cassert>

#include <complex>
#include <iostream>
#include <sstream>
#include <complex>
#include <string>
#include <vector>
#include <set>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <list>
#include <stack>
#include <bitset>
#include <utility>
#include <numeric>
#include <functional>
#include <algorithm>
#include <tr1/memory>
#include <tr1/array>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace tr1;

#include <armadillo>
#include <Magick++.h>
using namespace Magick;

#include "Utils/Vector2D.cpp"
#include "Utils/Array2D.cpp"

#include "Constants.cpp"
#include "Settings.cpp"
#include "Types.cpp"

#include "Utils/Utility.cpp"
#include "Utils/Parallel.cpp"
#include "Utils/SignalProcessor.cpp"
#include "Utils/Geometry2D.cpp"
#include "Utils/ShapeUtils.cpp"
#include "Utils/MorphologicProcessor.cpp"
#include "PuzzleSolving/MinCostMatching.cpp"
#include "PuzzleSolving/SuccessiveMinCostMatching.cpp"

#include "DataExtraction/ComponentExtractor.cpp"
#include "DataExtraction/BinaryObjectExtractor.cpp"
#include "DataExtraction/ObjectDetector.cpp"
#include "DataExtraction/ShapeAlignOptimizer.cpp"
#include "DataExtraction/ShapeClassificator.cpp"
#include "DataExtraction/PieceExtractor.cpp"
#include "DataExtraction/ExtractionPipeline.cpp"
#include "DataExtraction/ShapeAligner.cpp"

#include "PuzzleSolving/CompatibilityClassificator.cpp"
#include "PuzzleSolving/EdgeScores.cpp"
#include "PuzzleSolving/CompatibilityTable.cpp"

#include "Visualization/LinearSystemSolver.cpp"
#include "Visualization/GeometricLayoutComputer.cpp"
#include "Visualization/Visualizer.cpp"

#include "PuzzleSolving/FrameSolver.cpp"
#include "PuzzleSolving/InteriorSolver.cpp"
#include "PuzzleSolving/Solver.cpp"

// loads all the pieces from given images of front and backs sides of the pieces
Pieces loadPieces(vector<string> frontImages, vector<string> backImages) {
	int numImages = frontImages.size();
	if (numImages != int(backImages.size()))
		throw "bad number of input files";
	vector<ExtractionPipeline> pipelines;
	// create pipeline for each edge pair
	for (int i = 0; i < numImages; i++) {
		pipelines.push_back(ExtractionPipeline(frontImages[i],backImages[i]));
	}
	cout << "data extraction" << endl;
	// run parallel extraction
	Parallel::ForEach(pipelines,&ExtractionPipeline::extractPieces);
	vector<Pieces> pieces;
	// gather results
	for (unsigned int i = 0; i < pipelines.size(); i++) {
		pieces.push_back(pipelines[i].getPieces());
	}
	return Utils::Join(pieces);
}

int main(int argc, char** argv) {
	Settings settings(argc,argv);
	Pieces pieces = loadPieces(settings.getFrontFileNames(),settings.getBackFileNames());
	
	Solver solver;
	PuzzleLayout layout = solver.assemblePuzzle(pieces);
	
	Visualizer visualizer;
	Image result = visualizer.visualize(layout);
	result.write(settings.getOutputFileName());
	
	return 0;
}
