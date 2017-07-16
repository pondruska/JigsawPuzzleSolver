/**
 * Object detector computes the positions of pieces on the given front scan image
 * based on their expected positions.
 * 
 * Usage:
 * 1. Initialize the instance with name of image of front scan of pieces together
 * with their expected positions on image.
 * 2. run detectObjectPositions with itinial means to obtain the centers of detected objects.
 * 
 * The process of positions computation is sequence of several steps:
 * 1. Binarize the given image to background and foreground (pieces)
 * 2. Run K-means clustering to determine for reach pixel of foreground
 *    his incidence to one puzzle piece.
 * 3. Compute center (of mass) for each group of pixels forming one piece
 */
class ObjectDetector {
	
	Image image;
	
	// run one iteration of the K-means clustering on the binarised image
	// e.g. determine the centers of clusters given by given points
	RealPoints recluster(RealPoints means) const {
		int numClusters = means.size();
		int rows    = image.rows();
		int columns = image.columns();
		
		vector<RealPoint> sumPoints(numClusters);
		vector<int> numPoints(numClusters);
		
		const PixelPacket* pixels = image.getConstPixels(0,0,columns,rows);
		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < columns; x++) {
				if (ColorMono(*pixels++).mono()) {
					RealPoint p(x,y);
					int nearest = Geometry2D::closestPoint(p,means);
					sumPoints[nearest] += p;
					numPoints[nearest] += 1;
				}
			}
		}
		
		for (int i = 0; i < numClusters; i++) {
			means[i] = sumPoints[i] / numPoints[i];
		}
		return means;
	}
	
	// binarizes front scan into foreground (pixels of pieces) and background
	// using the realtive colour distance from the background colour.
	void binarize() {
		image.colorFuzz(COLOR_FUZZ);
		image.floodFillColor(20,20,"black");
		image.threshold(1.0); // monochrome
	}
	
	// computes the squared distances of two sets of points
	double squareDifference(RealPoints &v1, RealPoints &v2) const {
		double sum = 0.0;
		int length = v1.size();
		for (int i = 0; i < length; i++) {
			sum += (v1[i]-v2[i]).squareLength();
		}
		return sum / length;
	}
	
	public:
	
	// initializes an instance with the name of processed image
	ObjectDetector(string fileName)
		: image(fileName) {
		binarize();
	}
	
	// runs the clusterization of the image with given set of initial means
	// returns the final means
	RealPoints detectObjectPositions(RealPoints positions) const {
		RealPoints oldPos;
		// iteratively reclusterize binarized image
		// until the found clusters are changing
		do {
			oldPos = positions;
			positions = recluster(positions);
		} while (squareDifference(oldPos,positions) < AVG_RECLUSTER_CHANGE);
		return positions;
	}
	
};
