/**
 * Basic class for work with 2D vectors and points
 */
template<class T> class Vector2D {
	
	typedef Vector2D<T> Vector;
	
	public:
	T x, y;
	
	Vector2D(T _x = 0, T _y = 0) {
		x = _x, y = _y;
	}
	
	Vector operator + (const Vector v) const {
		return Vector2D(x+v.x,y+v.y);
	}
	
	Vector operator - (const Vector v) const {
		return Vector2D(x-v.x,y-v.y);
	}
	
	Vector operator += (const Vector v) {
		return (*this = *this + v);
	}
	
	Vector operator -= (const Vector v) {
		return (*this = *this - v);
	}
	
	Vector operator * (const T mul) const {
		return Vector2D(x * mul, y * mul);
	}
	
	Vector operator *= (const T mul) {
		return (*this = *this * mul);
	}
	
	Vector operator * (const Vector v) {
		return Vector2D(x*v.x,y*v.y);
	}
	
	Vector operator *= (const Vector v) {
		return (*this = *this * v);
	}
	
	Vector operator / (const T mul) const {
		return Vector2D(x / mul, y / mul);
	}
	
	Vector operator /= (const T mul) {
		return (*this = *this / mul);
	}
	
	Vector operator - () {
		return Vector2D(-x,-y);
	}
	
	Vector operator + () {
		return Vector2D(+x,+y);
	}
	
	bool operator == (const Vector v) const {
		return x == v.x && y == v.y;
	}
	
	bool operator != (const Vector v) const {
		return !(*this == v);
	}
	
	double squareLength() const {
		return x*x + y*y;
	}
	
	double length() const {
		return sqrt(squareLength());
	}
	
};

typedef Vector2D<int> IntegerVector;
typedef Vector2D<double> RealVector;
typedef IntegerVector IntegerPoint;
typedef RealVector RealPoint;
