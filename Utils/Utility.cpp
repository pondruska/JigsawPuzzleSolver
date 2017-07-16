/**
 * Collection of general purpose functions
 */
namespace Utils {
	
	const double DOUBLE_INF = 1e20;
	const int INT_INF = 1000000000;
	
	const IntegerPoint Direction[4] = {
		IntegerVector(+1,0),
		IntegerVector(0,+1),
		IntegerVector(-1,0),
		IntegerVector(0,-1)
	};
	
	// safe comparation of ral numbers
	int doubleCompare(double x, double y, double eps = 1e-9) {
		if (x < y-eps) return -1;
		if (x > y+eps) return +1;
		return 0;
	}
	
	bool doubleEqual(double x, double y, double eps = 1e-9) {
		return abs(x-y) <= eps;
	}
	
	inline double radiansToDegrees(double angle) {
		return 180 * angle / M_PI;
	}
	
	inline double DegreesToRadians(double angle) {
		return angle * M_PI / 180;
	}
	
	// finds the equvalent angle in the inteval [-Pi,Pi)
	inline double normAngle(double angle, double orig = 0.0) {
		while (angle >= orig+M_PI) angle -= 2*M_PI;
		while (angle <  orig-M_PI) angle += 2*M_PI;
		return angle;
	}
	
	// difference of two angles
	inline double angleDiff(double orig, double angle) {
		return normAngle(angle,orig)-orig;
	}
	
	// checks if the rigid transformation does nothing
	bool identity(RigidTransformation t) {
		return doubleEqual(t.rotationAngle,0,0.001) &&
			doubleEqual(t.translation.x,0,0.01) &&
			doubleEqual(t.translation.y,0,0.01);
	}
	
	int Convert(double d) {
		return d+0.5;
	}
	
	IntegerPoint convert(RealPoint p) {
		return IntegerPoint(p.x+0.5,p.y+0.5);
	}
	
	RealPoint convert(IntegerPoint p) {
		return RealPoint(p.x,p.y);
	}
	
	RealPoint round(RealPoint p) {
		return convert(convert(p));
	}
	
	// concatenate several vectors of the same type to one
	template<class T>
	vector<T> Join(const vector< vector<T> > &data) {
		vector<T> result;
		for (unsigned int i = 0; i < data.size(); i++) {
			result.insert(result.end(),data[i].begin(),data[i].end());
		}
		return result;
	}
	
};

#define FOREACH(it,t) for(typeof(t.begin()) it=t.begin(); it!=t.end(); ++it)

#define MAP1(type,func,p) ({\
  vector<type> r; \
  for(unsigned int i=0;i<p.size();i++)\
    r.push_back(func(p[i])); r;\
})

#define MAP2(type,func,p1,p2) ({\
  vector<type> r; \
  for(unsigned int i=0;i<p1.size();i++)\
    r.push_back(func(p1[i],p2[i])); r;\
})

