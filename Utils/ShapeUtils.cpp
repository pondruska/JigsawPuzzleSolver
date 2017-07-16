/**
 * Shape utils provide a collection of fucntions for manipultaion of curves
 * represented by the sequence of points along the curve.
 *
 */
namespace ShapeUtils {
	
	// smallest box containing all points of the given shape
	Geometry boundingBox(const Shape &shape) {
		int minX = +Utils::INT_INF, maxX = -Utils::INT_INF;
		int minY = +Utils::INT_INF, maxY = -Utils::INT_INF;
		for (unsigned int i = 0; i < shape.size(); i++) {
			IntegerPoint p = Utils::convert(shape[i]);
			minX = min(minX,p.x), maxX = max(maxX,p.x);
			minY = min(minY,p.y), maxY = max(maxY,p.y);
		}
		return Geometry(maxX-minX+1,maxY-minY+1,minX,minY);
	}
	
	// smallest rectangle having th center in the given point
	// containing all points of the given shape
	Geometry boundingBox(const Shape &shape, IntegerPoint center) {
		int dimX = 0, dimY = 0;
		for (unsigned int i = 0; i < shape.size(); i++) {
			dimX = max(dimX,abs(Utils::Convert(shape[i].x)-center.x));
			dimY = max(dimY,abs(Utils::Convert(shape[i].y)-center.y));
		}
		return Geometry(dimX+1+dimX,dimY+1+dimY,center.x-dimX,center.y-dimY);
	}
	
	// image of the component defined by the closed curve
	// the size of the image is determined by passed geometry
	Image shapeMask(Geometry geometry, const Shape &shape) {
		Image image(geometry,ColorMono(true));
		for (unsigned int i = 0; i < shape.size(); i++) {
			IntegerPoint p = Utils::convert(shape[i]);
			image.pixelColor(p.x,p.y,ColorMono(false));
		}
		image.borderColor(ColorMono(true));
		image.border("1x1");
		image.floodFillColor(0,0,ColorMono(false));
		geometry.xOff(1);
		geometry.yOff(1);
		image.crop(geometry);
		return image;
	}
	
	// mirro the shape
	Shape flipShape(Shape shape) {
		for (unsigned int i = 0; i < shape.size(); i++)
			shape[i] *= RealPoint(-1,1);
		return shape;
	}
	
}
