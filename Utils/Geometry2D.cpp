/**
 * utility functions to perform basic geometry procedures
 */
namespace Geometry2D {
	
	typedef Vector2D<double> Vector, Point;
	
	// angle of vector v against x-axis
	double angle(Vector v) {
		return atan2(v.y,v.x);
	}
	
	Vector translate(Vector v, Vector offset) {
		return v + offset;
	}
	
	Vector rotate(Vector v, double angle) {
		double new_x =  cos(angle) * v.x + sin(angle) * v.y;
		double new_y = -sin(angle) * v.x + cos(angle) * v.y;
		return Vector(new_x, new_y);
	}
	
	// apply rigid transformation to given point 
	Vector transform(Vector v, RigidTransformation t) {
		return translate(rotate(v,t.rotationAngle), t.translation);
	}
	
	// translate the set of points
	vector<Vector> translate(vector<Vector> v, Vector offset) {
		int length = v.size();
		for (int i = 0; i < length; i++) {
			v[i] = translate(v[i],offset);
		}
		return v;
	}
	
	// rotate the set of vectors
	vector<Vector> rotate(vector<Vector> v, double angle) {
		int length = v.size();
		for (int i = 0; i < length; i++) {
			v[i] = rotate(v[i],angle);
		}
		return v;
	}
	
	// apply rigid transformation to given set of points
	vector<Vector> transform(vector<Vector> v, RigidTransformation t) {
		return translate(rotate(v,t.rotationAngle), t.translation);
	}
	
	double crossProduct(Vector A, Vector B) {
		return A.x * B.y - A.y * B.x;
	}
	
	// returns <0, >0 or 0 based on the relative position
	// of the point p3 against line defined by points p1, p2
	double direction(Point p1, Point p2, Point p3) {
		return crossProduct(p2-p1,p3-p1);
	}
	
	// signed distance of point p3 from the line defined by points p1 and p2
	double signedDistanceFromLine(Point l1, Point l2, Point p) {
		return crossProduct(l1-l2,p-l2) / (l1-l2).length();
	}
	
	// distance of point p3 from the line defined by points p1 and p2
	double distanceFromLine(Point l1, Point l2, Point p) {
		return abs(signedDistanceFromLine(l1,l2,p));
	}
	
	// cewnter of mass of the given polygon
	Point centerOfPolygon(vector<Vector> polygon) {
		int length = polygon.size();
		double sumArea = 0;
		Point center;
		for (int i = 0; i < length; i++) {
			Point a = polygon[(i+0)%length];
			Point b = polygon[(i+1)%length];
			double area = crossProduct(b,a);
			center += Point(area * (a.x+b.x), area * (a.y+b.y));
			sumArea += area;
		}
		return center / (3 * sumArea);
	}
	
	double areaOfPolygon(const vector<Point> &polygon) {
		int length = polygon.size();
		double sum = 0;
		for (int i = 0; i < length; i++) {
			sum += crossProduct(polygon[i],polygon[(i+1)%length]);
		}
		return abs(sum) / 2;
	}
	
	// closest point from the set to the given point
	int closestPoint(Point point, const vector<Point> &set) {
		int best = 0;
		for (unsigned int i = 0; i < set.size(); i++) {
			if ((set[i]-point).squareLength() < (set[best]-point).squareLength())
				best = i;
		}
		return best;
	}
	
	// determines for each point from the first set the closest point from the second set
	Permutation closestPoints(const Shape &shape, const Shape &shapeTo) {
		Permutation perm;
		for (unsigned int i = 0; i < shape.size(); i++) {
			perm.push_back( closestPoint(shape[i],shapeTo) );
		}
		return perm;
	}
	
	// inverse rigid transformation to the given transformation
	RigidTransformation inverseTransformation(RigidTransformation t) {
		RealPoint invShift = rotate(-t.translation,-t.rotationAngle);
		return RigidTransformation(-t.rotationAngle,invShift);
	}
	
	// composite transformation of two transformations (t2 is executed after t1)
	RigidTransformation compositeTransformation(RigidTransformation t1, RigidTransformation t2) {
		RealPoint compShift = transform(t1.translation,t2);
		return RigidTransformation(t1.rotationAngle+t2.rotationAngle,compShift);
	}
	
	// schwartz-sharir algorithm dtermines the optimal rigid transformation of the shape
	// to match fiven pattern
	RigidTransformation optimalAlign(const Shape &pattern, const Shape &shape) {
		assert(shape.size() == pattern.size());
		
		RealPoint mean = SignalProcessor<RealPoint>::mean(shape);
		RealPoint center = SignalProcessor<RealPoint>::mean(pattern);
		
		Shape norm = Geometry2D::translate(shape,-mean);
		
		typedef complex<double> ComplexNumber;
		
		int length = shape.size();
		ComplexNumber sum;
		for (int i = 0; i < length; i++) {
			ComplexNumber u(norm[i].x,norm[i].y);
			ComplexNumber v(pattern[i].x,pattern[i].y);
			sum += u * conj(v);
		}
		
		double angle = arg(sum);
		
		mean = Geometry2D::rotate(mean,angle);
		
		return RigidTransformation(angle,center-mean);
	}
	
}
