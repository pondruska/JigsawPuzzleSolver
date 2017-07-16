/**
 * Identifies the four corners of the given piece's shape
 * See thesis for the exact algorithm
 */
namespace ShapeAnalysis {
	
	// generates all possible combination of choosing four
	// points from N
	vector<Quadruplet> quadruplets(int n) {
		vector<Quadruplet> result;
		Quadruplet q;
		for (q[0] = 0; q[0] < n; q[0]++)
			for (q[1] = q[0]+1; q[1] < n; q[1]++)
				for (q[2] = q[1]+1; q[2] < n; q[2]++)
					for (q[3] = q[2]+1; q[3] < n; q[3]++)
						result.push_back(q);
		return result;
	}
	
	// computes the first derivation of the curve
	vector<double> relativeAngles(Shape shape) {
		int length = shape.size();
		vector<double> angles(length-1);
		double last = M_PI;
		
		for (int i = 0; i < length-1; i++) {
			RealVector dir = shape[i+1]-shape[i];
			double angle = atan2(-dir.y,dir.x);
			// handle 2Pi overflow
			while (angle < last-M_PI) angle += 2*M_PI;
			while (angle > last+M_PI) angle -= 2*M_PI;
			
			angles[i] = angle;
			last = angle;
		}
		
		return angles;
	}
	
	// computes the smooth second derivation of the given curve.
	// to smooth the curve is used gaussuan blur with defined sigma = blurRadius
	vector<double> shapeSignature(const Shape &shape, double blurRadius = 10.0) {
		vector<double> angles = relativeAngles(shape);
		vector<double> diff = SignalProcessor<double>::difference(angles);
		vector<double> filtered = SignalProcessor<double>::gaussianBlur(diff,blurRadius);
		return filtered;
	}
	
	// computes the second derivative for closed curves
	vector<double> circularShapeSignature(const Shape &shape) {
		int length = shape.size();
		
		Shape extendedShape = shape;
		for (int i = 0; i < length; i++) {
			extendedShape.push_back(shape[i]);
		}
		for (int i = 0; i < length; i++) {
			extendedShape.push_back(shape[i]);
		}
		vector<double> signature = shapeSignature(extendedShape);
		
		rotate(signature.begin(),signature.begin()+length,signature.end());
		signature.resize(length);
		
		return signature;
	}
	
	// checks if the point is a local minimum in the neighbourhood of radius pixels.
	bool isLocalMinimum(int pos, const vector<double> &signature, int radius) {
		int start = max(0,pos-radius);
		int end = min(int(signature.size())-1,pos+radius);
		for (int i = start; i <= end; i++) {
			if (signature[i] < signature[pos])
				return false;
		}
		return true;
	}

	// checks if the point is a local maximum in the neighbourhood of radius pixels.	
	bool isLocalMaximum(int pos, const vector<double> &signature, int radius) {
		int start = max(0,pos-radius);
		int end = min(int(signature.size())-1,pos+radius);
		for (int i = start; i <= end; i++) {
			if (signature[i] > signature[pos])
				return false;
		}
		return true;
	}
	
	// predcessor, succesor in the cyclic interval of given length
	inline int predcessor(int i, int length) { return (i+length-1)%length; }
	inline int successor(int i, int length) { return (i+length+1)%length; }
	
	// computes the distance of each point of the curve to a virutal line
	// between the first and the last point
	vector<double> segmentDistanceSignature(const Shape &segment) {
		int length = segment.size();
		RealPoint l1 = segment[0];
		RealPoint l2 = segment.back();
		vector<double> signature(length);
		for (int i = 0; i < length; i++) {
			signature[i] = Geometry2D::distanceFromLine(l1,l2,segment[i]);
		}
		return signature;
	}
	
	vector<double> segmentAngularSignature(const Shape &segment) {
		vector<double> angles = relativeAngles(segment);
		vector<double> filtered = SignalProcessor<double>::gaussianBlur(angles,10.0);
		vector<double> diff = SignalProcessor<double>::difference(filtered);
		diff[0] = 0.0;
		return diff;
	}
	
	// chcks th similarity of the edge to an edge having no padding
	double FlatScore(const Shape &segment, bool = false) {
		int length = segment.size();
		vector<double> signature = segmentDistanceSignature(segment);
		double diff = 0.0;
		for (int i = 0; i < length; i++) {
			diff = max(diff,abs(signature[i]));
		}
		return diff;
	}
	
	// finds the first nad last point of the longest peak entire over the x-axis
	Pair longestPeak(const vector<double> &signature) {
		int length = signature.size();
		Pair longest;
		int j = 0;
		for (int i = 0; i < length; i++) {
			if (signature[i] > 0) j = i;
			if (i-j > longest.second-longest.first)
				longest = Pair(j,i);
		}
		return longest;
	}
	
	// measures the similarity of the curve to an edge having indent padding
	double IndentScore(const Shape &segment) {
		int length = segment.size();
		if (length < MIN_EDGE_SIZE) return 1e20;
		
		vector<double> signature = segmentAngularSignature(segment);
		Pair peak = longestPeak(signature);
		
		while (peak.first > 0 && !isLocalMaximum(peak.first,signature,10))
			peak.first--;
		while (peak.second < length-1 && !isLocalMaximum(peak.second,signature,10))
			peak.second++;
		
		Shape shape;
		for (int i = 0; i <= peak.first; i++) {
			shape.push_back(segment[i]);
		}
		for (int i = peak.second; i < length; i++) {
			shape.push_back(segment[i]);
		}
		return FlatScore(shape);
	}
	
	// measures the similarity of the curve to an edge having outdent padding
	double OutdentScore(const Shape &segment) {
		int length = segment.size();
		Shape shape(length);
		for (int i = 0; i < length; i++) {
			shape[i] = RealPoint(1,-1) * segment[i];
		}
		return IndentScore(shape);
	}
	
	// returns the overall score of the given combination of the points defining
	// four subcurves
	double combinationScore(Quadruplet c, const Array2D<double> &scoreTable) {
		double worstScore = 0;
		double sumScore = 0;
		for (int i = 0; i < 4; i++) {
			sumScore += scoreTable.at(c[i],c[successor(i,4)]);
			worstScore = max(worstScore,scoreTable.at(c[i],c[successor(i,4)]));
		}
		return worstScore+sumScore;
	}
	
	template<class T>
	vector<T> filterSmallElements(vector< pair<T,double> > objects, double ratio = 0.5) {
		double maximum = 0.0;
		int length = objects.size();
		for (int i = 0; i < length; i++) {
			maximum = max(maximum,objects[i].second);
		}
		
		vector<T> result;
		
		for (int i = 0; i < length; i++) {
			if (objects[i].second >= ratio * maximum) {
				result.push_back(objects[i].first);
			}
		}
		
		return result;
	}
	
	// returns possible corners of the piece's shape
	vector<int> getPossibleCorners(const Shape &shape) {
		vector<double> signature = circularShapeSignature(shape);
		vector<int> maximas = SignalProcessor<double>::findLocalMaximas(signature,10);
		sort(maximas.begin(),maximas.end());
		vector< pair<int,double> > packedMaximas;
		for (unsigned int i = 0; i < maximas.size(); i++) {
			packedMaximas.push_back( make_pair(maximas[i],signature[ maximas[i] ]) );
		}
		return filterSmallElements(packedMaximas,0.25);
	}
	
	// cuts the given interval from the given curve
	Shape subSegment(const Shape &shape, int start, int end) {
		Shape result;
		do {
			result.push_back(shape[start]);
			start = successor(start,shape.size());
		} while (start != end);
		result.push_back(shape[end]);
		return result;
	}
	
	// returns best dcore for given curve measuring the similarity to one of classcal types of the curves.
	pair<double,EdgeType> shapeScore(const Shape &shape) {
		int length = shape.size();
		assert(length >= 2);
		
		double score = FlatScore(shape);
		EdgeType type = FRAME;
		
		if (score > 10) {
			
			double indentScore = IndentScore(shape);
			if (score > indentScore)
				score = indentScore, type = INDENT;
			
			double outdentScore = OutdentScore(shape);
			if (score > outdentScore)
				score = outdentScore, type = OUTDENT;
			
		}
		
		return make_pair(score,type);
	}
	
	// returns four corner of given shape of a piece
	Quadruplet IdentifyCorners(const Shape &shape) {
		// find all candidates for corner points
		vector<int> candidates = getPossibleCorners(shape);
		int numCandidates = candidates.size();
		// precompute the score for each possible subcurve defined by points
		Array2D<double> scoreTable(numCandidates,numCandidates);
		for (int i = 0; i < numCandidates; i++) {
			for (int j = 0; j < numCandidates; j++) {
				if (i != j) {
					Shape segment = subSegment(shape,candidates[i],candidates[j]);
					scoreTable.at(i,j) = shapeScore(segment).first;
				}
			}
		}
		// generate all possible combinations of four points
		vector<Quadruplet> combinations = quadruplets(numCandidates);
		
		typedef pair<double,Quadruplet> WeightedQuadruplet;
		// judge each combination and select the best one
		WeightedQuadruplet best(Utils::DOUBLE_INF,Quadruplet());
		for (unsigned int i = 0; i < combinations.size(); i++) {
			array<RealPoint,4> cornerPoints;
			for (int j = 0; j < 4; j++) {
				cornerPoints[j] = shape[ candidates[ combinations[i][j] ] ];
			}
			double score = combinationScore(combinations[i],scoreTable);
			best = min(best,WeightedQuadruplet(score,combinations[i]));
		}
		
		Quadruplet corners;
		for (int i = 0; i < 4; i++) {
			corners[i] = candidates[ best.second[i] ];
		}
		return corners;
	}
	
};
