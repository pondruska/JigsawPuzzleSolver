/**
 * ShapeAligner computes an optimal geometric align of the two edges
 * or the align of the edge along the given line.
 * This align contains the rigid transformation of the second edge itself
 * and also the matching points for this transformation.
 */
class ShapeAligner {
	// creates the uniform sampling of the interval of the length M
	// with N points
	Permutation uniformSample(int N, int M) {
		Permutation p(N);
		for (int i = 0; i < N; i++) {
			p[i] = Utils::Convert(double(i)*(M-1)/(N-1));
		}
		return p;
	}
	
	// find matching points on the second for all points located at the first edge
	// if we have approximate position for each this point on the second edge.
	void findPairs(const Shape &shape1, const Shape &shape2, Permutation &p) {
		int length1 = shape1.size();
		int length2 = shape2.size();
		
		for (int i = 0; i < length1; i++) {
			int &j = p[i];
			j = max(j,0);
			j = min(j,length2-1);
			int a = j-1, b = j+1;
			double dist = (shape1[i]-shape2[j]).squareLength();
			while (a >= 0 && a > j-5) {
				double d = (shape1[i]-shape2[a]).squareLength();
				if (d < dist) dist = d, j = a;
				a--;
			}
			while (b < length2 && b < j+5) {
				double d = (shape1[i]-shape2[b]).squareLength();
				if (d < dist) dist = d, j = b;
				b++;
			}
		}
		
	}
	
	// finds the optimal rigid transformation to place the edge along the x-axis
	RigidTransformation optimizeLineAlign(const Shape &shape) {
		Shape line;
		for (unsigned int i = 0; i < shape.size(); i++) {
			line.push_back(RealPoint(shape[i].x,0));
		}
		return Geometry2D::optimalAlign(line,shape);
	}
	
	public:
	
	// computes optimal geometric layout if we have the information about optimal geometric
	// layou for this two edges in a lower resolution (runs faster ).
	ShapeAlign shapeAlign(const Shape &shape1, const Shape &shape2, ShapeAlign align) {
		RigidTransformation t;
		// reoprimize the transformation until the layout is not changing
		do {
			Shape shapeT = Geometry2D::transform(shape2,align.t);
			// find the matching points on the curves
			findPairs(shape1,shapeT,align.pairs12);
			findPairs(shapeT,shape1,align.pairs21);
			
			Shape s1, s2;
			for (unsigned int i = 0; i < align.pairs12.size(); i++) {
				s1.push_back(shape1[i]);
				s2.push_back(shapeT[ align.pairs12[i] ]);
			}
			for (unsigned int i = 0; i < align.pairs21.size(); i++) {
				s2.push_back(shapeT[i]);
				s1.push_back(shape1[ align.pairs21[i] ]);
			}
			// find the optimal transformation under the given matching points
			t = Geometry2D::optimalAlign(s1,s2);
			align.t = Geometry2D::compositeTransformation(align.t,t);
		} while (!Utils::identity(t));
		
		return align;
	}
	
	// Computes the optimal geometric layout of two edges
	ShapeAlign shapeAlign(const Shape &shape1, const Shape &shape2) {
		ShapeAlign align;
		
		align.pairs12 = uniformSample(shape1.size(),shape2.size());
		align.pairs21 = uniformSample(shape2.size(),shape1.size());
		reverse(align.pairs12.begin(),align.pairs12.end());
		reverse(align.pairs21.begin(),align.pairs21.end());
		// find the rought placemnet of one edge along the other based on the end points
		Shape s1, s2;
		s1.push_back(shape1[0]);
		s1.push_back(shape1.back());
		s2.push_back(shape2.back());
		s2.push_back(shape2[0]);
		
		align.t = Geometry2D::optimalAlign(s1,s2);
		// find the exact placement based on the rought placement
		return shapeAlign(shape1, shape2, align);
	}
	
	// Computes the optimal rigid transformation to align the shape along the line
	// with defined angle to the x-axis
	RigidTransformation lineAlign(const Shape &shape, double angle = 0.0) {
		RigidTransformation t, transform(Geometry2D::angle(shape.back()-shape[0]));
		// reoptimize the transformation until the layout is not changing
		do {
			Shape s = Geometry2D::transform(shape,transform);
			t = optimizeLineAlign(s);
			transform = Geometry2D::compositeTransformation(transform,t);
		} while (!Utils::identity(t));
		
		return Geometry2D::compositeTransformation(transform,RigidTransformation(angle));
	}
	
};
