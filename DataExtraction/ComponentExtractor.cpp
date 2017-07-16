/**
 * Component extractor extracts from the given binarised image
 * all components having foreground colour.
 * 
 * Usage:
 * 1. initialise the instance with an image, where the background has
 *    black colour and the foreground white colour.
 * 2. calling of extractComponents() returns all connected foreground components
 *    represented by the sequences of pixels. Each sequence definines one component
 *    as a sequence of pixels along its border in counter-clockwise order.
 */

class ComponentExtractor {
	
	Image image;
	
	// determines if given point is a border pixel of the foreground component
	// e.g. coincides with at least one white pixel.
	inline bool IsShapePixel(IntegerPoint p) const {
		const PixelPacket* pixels = image.getConstPixels(p.x-1,p.y-1,2,2);
		for (int i = 0; i < 4; i++) {
			if (ColorMono(*pixels++).mono()) {
				return true;
			}
		}
		return false;
	}
	
	// returns the sequence of pixels around one component starting in the given point
	// the starting point has to be on the border of the component
	// = IsShapePixel(start) returns true
	Shape TraceComponent(const IntegerPoint start) {
		Shape shape;
		
		int dir = 2;
		IntegerPoint p = start;
		
		do {
			using Utils::Direction;
			shape.push_back(Utils::convert(p)-RealPoint(2.5,2.5));
			dir = (dir+1)%4;
			while (!IsShapePixel(p+Direction[dir]))
				dir = (dir+3)%4;
			p += Direction[dir];
		} while (p != start);
		
		return shape;
	}
	
	public:
	
	// initializes the instance with given binarised image
	ComponentExtractor(const Image &image)
		: image(image) {
	}
	
	// extract all components from the initialised image
	// note. this call is allowed only once, each following call returns empty set
	vector<Shape> extractComponents() {
		vector<Shape> components;
		
		// intend to ensure there are no shape pixels directly on the border
		image.borderColor(ColorMono(false));
		image.border("2x2");
		
		int columns  = image.columns();
		int rows = image.rows();
		
		// iterate the image and extract the components
		const PixelPacket* pixels = image.getPixels(0,0,columns,rows);
		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < columns; x++) {
				if (ColorMono(*pixels++).mono()) {
					cout << '.';
					components.push_back(TraceComponent(IntegerPoint(x,y)));
					image.floodFillColor(x,y,"black");
				}
			}
		}
		
		return components;
	}
	
};
