/**
 * Settings of the method described in thesis
 */

// num of parallel execution threads
const int NUM_THREADS = 1;

// the threshold distance from the average coloour of the background to be considered foreground
const double COLOR_FUZZ = 20.0;

// the minimal average change of the cluster means until they are considered converged
const double AVG_RECLUSTER_CHANGE = 1.0;

// number of versions in lower resolutions for every edge
const int RESOLUTION_DEPTH = 3;

// number of best edges for which is computed the optimal geometric layout in the full resolution
const int BASE_SIZE = 50;

// the frame arounf the solved puzzle in pixels
const double VISUALIZATION_FRAME = 30;

// erosion of the pieces in the visualized solution
// - makes an aisle between the pieces
const double VISUALIZATION_ERODE = 2.0;

// blur of images used during extraction of the colour
const double COLOR_BLUR_RADIUS = 2.0;

// the distance from the edge in which is extracted color for every point
const double EDGE_TO_COLOR_DISTANCE = 6.0;

// the minimal length of th edge in pixels
const int MIN_EDGE_SIZE = 30;

const double MIN_MAX_PIECE_SIZE_RATIO = 0.25;

// weights of elementary scores in the final compatibility score
// configured for shape-only information, for using the colour define the parameters as mentioned int hesis
const double SHAPE_WEIGHT = 1.0;
const double HUE_WEIGHT = 0.0;
const double SATURATION_WEIGHT = 0.0;
const double LUMINOSITY_WEIGHT = 0.0;

