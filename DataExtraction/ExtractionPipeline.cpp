/**
 * ExtractionPipeline represents sequence of steps which have
 * to be executed on particular pair of images (back+front scan)
 * to obtain the contained pieces.
 *
 * Usage:
 * 1. initialize instance with filename of front and connecred back scan image.
 * 2. call extractPieces() to run the extraction process.
 * 3. call getPieces() to get the extracted pieces
 *
 * For the best performance on paralell systems it is desired to create a vector
 * of instances - for each pair of images one. Next call of Parallel::Map() on this
 * vector will paralelize the the entire process among diffeent instances.
 */
class ExtractionPipeline {
	// filenames of the front and the connected back scan
	string frontImage, backImage;
	Pieces pieces;
	
	Shapes pieceBackShapes() {
		BinaryObjectExtractor extractor(backImage);
		return extractor.extractShapes();
	}
	
	RealPoints expectedFrontPositions(const Shapes &backShapes) {
		return MAP1(RealPoint,Geometry2D::centerOfPolygon,backShapes);
	}
	
	RealPoints piecePositions(const RealPoints &expPositions) {
		ObjectDetector detector(frontImage);
		return detector.detectObjectPositions(expPositions);
	}
	
	Shapes flipShapes(const Shapes &shapes) {
		return MAP1(Shape,ShapeUtils::flipShape,shapes);
	}
	
	Shapes pieceShapes(const RealPoints &positions, const Shapes &frontShapes) {
		PatternAlignOptimizer optimizer(frontImage);
		return MAP2(Shape,optimizer.optimizeAlign,frontShapes,positions);
	}
	
	Pieces extractPieces(const Shapes &shapes) {
		PieceExtractor extractor(frontImage);
		return MAP1(PieceRef,extractor.extractPiece,shapes);
	}
	
	public:
	
	ExtractionPipeline(string frontImage, string backImage)
		: frontImage(frontImage), backImage(backImage) {
	}
	
	void extractPieces() {
		// extract shapes of pieces from the back scan
		Shapes backShapes       = pieceBackShapes();
		// mirror flip the shapes to obtain shapes as viewed from front side
		Shapes frontShapes      = flipShapes(backShapes);
		// compute for each shape its center, it is the expected position
		// of piece on the front scan
		RealPoints expPositions = expectedFrontPositions(backShapes);
		// find the real positions of pieces on the front scan based on
		// expected positions
		RealPoints positions    = piecePositions(expPositions);
		// for each shape compute the transformation to exactly match the
		// desired piece
		Shapes shapes           = pieceShapes(positions,frontShapes);
		// segment pieces from the front scan based on their shape
		pieces = extractPieces(shapes);
	}
	
	// return extracted pieces
	Pieces getPieces() {
		return pieces;
	} 
	
};
