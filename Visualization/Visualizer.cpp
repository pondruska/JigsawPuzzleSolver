/**
 * Visualizer creates the final image of solved puzzle from the combinatoric solution 
 */
class Visualizer {
	
	Image image;
	
	// geometrical center of given image
	RealPoint ImageCenter(const Image &image) {
		return RealPoint(0.5*(image.columns()-1),0.5*(image.rows()-1));
	}
	
	// returns one enclosed curve defining the shape of the entire piece
	Shape pieceShape(PieceRef piece) {
		Shape shape;
		for (int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < piece->edges[i]->shape.size(); j++) {
				shape.push_back(piece->edges[i]->shape[j]);
			}
		}
		return shape;
	}
	
	// cuts one piece from its source image
	Image piecePixels(PieceRef piece) {
		Image pixels(piece->imageName);
		pixels.border("50x50");
		IntegerPoint center = piece->center+IntegerPoint(50,50);
		Shape shape = Geometry2D::translate(pieceShape(piece), Utils::convert(center));
		pixels.crop(ShapeUtils::boundingBox(shape, center));
		return pixels;
	}
	
	// paints one single piece at the defined place of the resulting image
	void drawPiece(PieceRef piece, RigidTransformation position) {
		Image pixels = piecePixels(piece);
		Shape shape = Geometry2D::translate(pieceShape(piece), ImageCenter(pixels));
		
		MorphologicProcessor processor(
			ShapeUtils::shapeMask(pixels.size(),shape)
		);
		
		Image mask = processor.erode(VISUALIZATION_ERODE);
		pixels.composite(mask,0,0,MultiplyCompositeOp);
		
		pixels.backgroundColor(ColorMono(false));
		pixels.rotate(Utils::radiansToDegrees(-position.rotationAngle));
		
		IntegerPoint p = Utils::convert(position.translation - ImageCenter(pixels));
		image.composite(pixels,p.x,p.y,AddCompositeOp);
	}
	
	// add frame of given size to a geometric layout
	GeometricLayout addFrame(GeometricLayout layout, double frameSize) {
		FOREACH(it,layout.positions)
			it->second.translation += RealPoint(frameSize,frameSize);
		layout.width  += frameSize + frameSize;
		layout.height += frameSize + frameSize;
		return layout;
	}
	
	public:
	// returns the visualized solution of the given combinatoric solution
	Image visualize(const PuzzleLayout &puzzleLayout) {
		GeometricLayoutComputer computer;
		// compute the geometric layout
		GeometricLayout layout = computer.computeLayout(puzzleLayout);
		
		layout = addFrame(layout,VISUALIZATION_FRAME);
		image = Image(Geometry(layout.width,layout.height),"black");
		// draw all pieces
		FOREACH(it,layout.positions) {
			drawPiece(it->first, it->second);
		}
		
		return image;
	}
	
};
