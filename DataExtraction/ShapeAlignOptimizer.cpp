/**
 * PatternAlignOptimizer finds the RigidTransformation for the known shape of the piece
 * to exactly match this piece on the image.
 * 
 * First initialize an instance with the image of pieces scanned from the front side.
 *
 * Later call of optimizeAlign() with known shape of the piece together with its expected position
 * will find the best transformation of this shape to match the piece on the image.
 * 
 * Use ObjectDetector, to find the expected positions of pieces, .
 */
class PatternAlignOptimizer {
	
	// table of prefix sums
	Array2D<WeightedPoint> table;
	
	// Computes the average edge position and its intensity
	// in a square area of the image defined by the central point and a given radius.
	// The average edge position is defined as a center of mass of this area
	// when each pixel has the weight equal to its value defined by an edge detection.
	WeightedPoint AverageEdgePoint(RealPoint p, int radius) const {
		IntegerPoint g = Utils::convert(p);
		
		g.x = max(g.x,radius);
		g.y = max(g.y,radius);
		g.x = min(g.x,table.columns()-radius-2);
		g.y = min(g.y,table.rows()-radius-2);
		
		WeightedPoint aa = table.at(g.x+radius+1,g.y+radius+1);
		WeightedPoint ab = table.at(g.x-radius-0,g.y+radius+1);
		WeightedPoint ba = table.at(g.x+radius+1,g.y-radius-0);
		WeightedPoint bb = table.at(g.x-radius-0,g.y-radius-0);
		
		double sum = aa.first - ab.first - ba.first + bb.first;
		RealPoint point = aa.second - ab.second - ba.second + bb.second;
		// no edge in the neighbourhood case
		if (Utils::doubleEqual(sum,0.0))
			point = p;
		else
			point /= sum;
		
		return make_pair( sum, point );
	}
	
	// performs an edge detection on the given image assigning each pixel
	// its edge intensity.
	Image EdgeImage(Image image) {
		image.reduceNoise();
		image.reduceNoise();
		image.edge();
		return BinaryObjectExtractor::ImageValue(image);
	}
	
	// precomputes the table of prefix sums to allow running of the AverageEdgePoint in O(1)
	void PrecomputeLookupTable(const Image &image) {
		int rows = image.rows();
		int columns = image.columns();
		
		table.resize(columns+1,rows+1);
		
		const PixelPacket *pixel = image.getConstPixels(0,0,columns,rows);
		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < columns; x++) {
				double weight = ColorGray(*pixel++).shade();
				WeightedPoint ba = table.at(x,y+1), ab = table.at(x+1,y), bb = table.at(x,y);
				table.at(x+1,y+1) = make_pair(
					weight                       + ab.first  + ba.first  - bb.first,
					RealPoint(weight*x,weight*y) + ab.second + ba.second - bb.second
				);
			}
		}
	}
	
	// Finds the average edge position in the neighbourhood of each point of the given shape.
	pair<Shape,Weight> EdgePoints(const Shape &shape, int radius = 10) const {
		Shape edge;
		Weight weight;
		for (int i = 0; i < int(shape.size()); i++) {
			WeightedPoint wp = AverageEdgePoint(shape[i],radius);
			weight.push_back(wp.first);
			edge.push_back(wp.second);
		}
		return make_pair(edge,weight);
	}
	
	// Iteratively reoptimalizes the rigid transformation of the shape until it
	// finds a local optimum when the shape matches with edges on the image.
	ScoredAlign optimizeAlign(Shape pattern) const {
		double score = 0.0;
		for (int i = 0; i < 10; i++) {
			// in each iteration search in smaller area
			pair<Shape,Weight> edge = EdgePoints(pattern,15-i);
			// find the best matching of the shape to the edges in its neighbourhood
			RigidTransformation t = Geometry2D::optimalAlign(edge.first,pattern);
			pattern = Geometry2D::transform(pattern,t);
			score = SignalProcessor<double>::sum(edge.second);
		}
		return ScoredAlign(score,pattern);
	}
	
	public:
	
	// initialize with the image of the front sides of pieces
	PatternAlignOptimizer(const Image &image) {
		Image edges = EdgeImage(image);
		PrecomputeLookupTable(edges);
	}
	
	// Find the best transformation of the shape to match the
	// piece having its expected center on the given position
	Shape optimizeAlign(Shape pattern, RealPoint position) const {
		pattern = Geometry2D::translate(pattern,-Geometry2D::centerOfPolygon(pattern));
		
		ScoredAlign bestAlign;
		bestAlign.first = -Utils::DOUBLE_INF;
		
		// try 72 starting transformations
		for (int angle = 0; angle < 360; angle += 5) {
			Shape baseAlign = Geometry2D::transform(
				pattern,RigidTransformation(Utils::DegreesToRadians(angle),position)
			);
			// find the best matching
			ScoredAlign align = optimizeAlign(baseAlign);
			if (align.first > bestAlign.first)
				bestAlign = align;
		}
		
		// try to further optimize the best transformation by moving it a few pixels
		Shape shape = bestAlign.second;
		for (int x = -6; x <= +6; x += 3) {
			for (int y = -6; y <= +6; y+= 3) {
				Shape s = Geometry2D::translate(shape,RealPoint(x,y));
				ScoredAlign align = optimizeAlign(s);
				if (align.first > bestAlign.first)
					bestAlign = align;
			}
		}
		
		return optimizeAlign(bestAlign.second).second;
	}
};
