/**
 * Piece Extractor is the factory for creating puzzle pieces after we have
 * defined their border on the image of their front scan.
 * usage:
 * 1. Initialize the PieceExtractor with an image of the pieces scanned from the front side 
 * 2. Each call of extractPiece() with specified location of the piece position on the inmage
 *    by defining its shape will create an instance of one Piece.
 */

int edgeCounter;
int pieceCounter;

class PieceExtractor {
	
	Image image;
	
	RealPoint ImageCenter(const Image &image) {
		return RealPoint(0.5*(image.columns()-1),0.5*(image.rows()-1));
	}
	
	// returns a sequence of points in a constant given distace from the given closed curve
	// - the points are inside this closed regipn
	Shape erodeShape(const Shape &shape, double radius) const {
		// find smallest box the curve fits into
		Geometry geometry = ShapeUtils::boundingBox(shape);
		RealPoint offset(geometry.xOff(), geometry.yOff());
		Shape shiftedShape = Geometry2D::translate(shape,-offset);
		// fill the component defined by the given closed curve into image
		Image shapeMask = ShapeUtils::shapeMask(geometry,shiftedShape);
		MorphologicProcessor processor(shapeMask);
		// erode the component by given number of pixels
		// may split the component into several parts
		Image erodedMask = processor.erode(radius);
		ComponentExtractor extractor(erodedMask);
		// extract the shape of eroded component
		Shape erodedShape = Utils::Join(extractor.extractComponents());
		return Geometry2D::translate(erodedShape,+offset);
	}
	
	// extracts for each point on the edge the average colour in a constant distance
	// from this point
	ColorSignature createColorSignature(Shape shape) const {
		Shape colorPoints = erodeShape(shape,EDGE_TO_COLOR_DISTANCE);
		Permutation pairs = Geometry2D::closestPoints(shape,colorPoints);
		
		ColorSignature colorSignature;
		for (unsigned int i = 0; i < shape.size(); i++) {
			IntegerPoint p = Utils::convert(colorPoints[pairs[i]]);
			colorSignature.push_back(image.pixelColor(p.x,p.y));
		}
		
		return colorSignature;
	}
	
	// returns center of piece defined as the mean of its four corner points
	RealPoint centerOfPiece(const Shape &shape, const Quadruplet &corners) const {
		RealPoint center;
		for (int i = 0; i < 4; i++) {
			center += shape[ corners[i] ];
		}
		return center/4;
	}
	
	static int getNewPieceID() {
		return pieceCounter++;
	}
	
	static int getNewEdgeID() {
		return edgeCounter++;
	}
	
	public:
	
	// initializes an instance with given image of the pieces scanned from the front side
	PieceExtractor(string fileName)
		: image(fileName) {
		image.blur(COLOR_BLUR_RADIUS);
	}
	
	// eextract one piece from the image specified by the given shape
	PieceRef extractPiece(Shape shape) const {
		ColorSignature color = createColorSignature(shape);
		Quadruplet corners = ShapeAnalysis::IdentifyCorners(ShapeUtils::flipShape(shape));
		
		Piece *piece = new Piece;
		piece->id = PieceExtractor::getNewPieceID();
		piece->imageName = image.fileName();
		piece->center = Utils::convert(centerOfPiece(shape,corners));
		
		Edge *edges = new Edge[4];
		for (int i = 0; i < 4; i++) {
			int begin = corners[i], end = corners[(i+1)%4];
			
			edges[i].id    = PieceExtractor::getNewEdgeID();
			edges[i].prev  = &edges[(i+3)%4];
			edges[i].next  = &edges[(i+1)%4];
			edges[i].piece = piece;
			edges[i].shape = Geometry2D::translate(
				SignalProcessor<RealPoint>::interval(shape,begin,end),
				-Utils::convert(piece->center)
			);
			edges[i].color = SignalProcessor<Color>::interval(color,begin,end);
			edges[i].type  = ShapeAnalysis::shapeScore(edges[i].shape).second;
			
			piece->edges[i] = &edges[i];
		}
		
		return PieceRef(piece);
	}
	
};

