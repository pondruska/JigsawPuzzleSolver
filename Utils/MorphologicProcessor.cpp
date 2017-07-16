/**
 * Morphologic processor providing the basic morphologic operations
 * dilate, erode, open and close on given binarized image
 * using the circle kernel of defined radius
 *
 * Given images have to be binarized into black and white color,
 * - white is the color of foreground
 * - black is the color of background
 */
class MorphologicProcessor {
	
	Image image;
	
	public:
	
	MorphologicProcessor(const Image &image)
		: image(image) {
	}
	
	Image& dilate(double radius) {
		int half = radius+0.5;
		int size = 2*half+1;
		
		double kernel[size*size];
		// create kernel
		for (int y = -half; y <= +half; y++) {
			for (int x = -half; x <= +half; x++) {
				if (x*x+y*y <= radius*radius)
					kernel[(y+half)*size+x+half] = 1.0;
				else
					kernel[(y+half)*size+x+half] = 0.0;
			}
		}
		image.convolve(size,kernel);
		
		image.threshold(0.5); 
		
		return image;
	}
	
	Image& erode(double radius) {
		image.negate();
		dilate(radius);
		image.negate();
		return image;
	}
	
	Image& open(double radius) {
		erode(radius);
		return dilate(radius);
	}
	
	Image& close(double radius) {
		dilate(radius);
		return erode(radius);
	}
	
	Image& smooth(double radius) {
		open(radius);
		return close(radius);
	}
	
};
