class BinaryObjectExtractor {
	
	Image image;
	
	// computes the weighted sum S[i] = sum{j * W[j] | j < i}
	static vector<int> partialWeightedSums(vector<int> histogram) {
		int length = histogram.size();
		vector<int> result(length);
		int sum = 0;
		for (int i = 0; i < length; i++) {
			sum += i * histogram[i];
			result[i] = sum;
		}
		return result;
	}
	
	// creates a histogram of the image
	vector<int> grayscaleHistogram(int numLevels = 256) {
		vector<int> histogram(numLevels);
		int rows = image.rows();
		int columns = image.columns();
		const PixelPacket* pixels = image.getConstPixels(0,0,columns,rows);
		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < columns; x++) {
				double value = ColorGray(*pixels++).shade();
				histogram[int(value * (numLevels-1))]++;
			}
		}
		return histogram;
	}
	
	// finds the optimal threshold for the segmentation of pieces
	double findThreshold() {
		vector<int> histogram = grayscaleHistogram(256);
		vector<int> weightedSums = partialWeightedSums(histogram);
		vector<int> sums = SignalProcessor<int>::partialSum(histogram);
		double threshold = double(weightedSums.back())/sums.back();
		for (int i = 0; i < 5; i++) {
			int t = threshold;
			double mean1 = double(weightedSums[t])/sums[t];
			double mean2 = double(weightedSums.back()-weightedSums[t])/(sums.back()-sums[t]);
			threshold = (mean1+mean2)/2;
		}
		return threshold;
	}
	
	// binarizing the image into bright foreground (puzzle pieces) and dark background
	void binarize() {
		// transform to grayscale
		image = ImageValue(image);
		// binarize
		image.threshold(findThreshold());
		// suppress noise
		MorphologicProcessor processor(image);
		image = processor.smooth(2);
	}
	
	// filter out the components having the area size less than the given ratio of the greatest component size
	Shapes deleteSmallComponents(const Shapes &shapes, double minMaxRatio) {
		double maxSize = 0;
		
		for (unsigned int i = 0; i < shapes.size(); i++) {
			maxSize = max(maxSize,Geometry2D::areaOfPolygon(shapes[i]));
		}
		
		Shapes filtered;
		for (unsigned int i = 0; i < shapes.size(); i++) {
			if (Geometry2D::areaOfPolygon(shapes[i]) >= minMaxRatio * maxSize)
				filtered.push_back(shapes[i]);
		}
		return filtered;
	}
	
	public:
	
	// transforms the image into grayscale replacing the value of each pixels with
	// maximum of its red, green and blue component
	static Image ImageValue(Image image) {
		int rows = image.rows();
		int columns = image.columns();
		PixelPacket* pixels = image.getPixels(0,0,columns,rows);
		for (int i = 0; i < rows*columns; i++) {
			ColorRGB c(*pixels);
			*pixels++ = ColorGray(max(c.red(),max(c.green(),c.blue())));
		}
		image.syncPixels();
		return image;
	}
	
	// initializes the extractor with given image of the pices scanned from the back side
	BinaryObjectExtractor(string fileName) 
		: image(fileName) {
		binarize();
	}
	
	// returns all bright components defined by their shape in the counterclockwise order
	Shapes extractShapes() {
		ComponentExtractor extractor(image);
		Shapes shapes = extractor.extractComponents();
		
		return deleteSmallComponents(shapes,MIN_MAX_PIECE_SIZE_RATIO);
	}
	
};
