/**
 * generic 2D array containing elements of any type
 */
template<class T> class Array2D {
	vector<T> data;
	int r, c;
	
	public:
	
	int rows() const { return r; }
	int columns() const { return c; }
	IntegerPoint size() const { return IntegerPoint(c,r); }
	
	bool valid(int x, int y) const {
		return x >= 0 && x < columns() && y >= 0 && y < rows();
	}
	
	bool valid(IntegerPoint p) const {
		return valid(p.x,p.y);
	}
	
	void resize(int columns, int rows) {
		r = rows, c = columns;
		data.resize(rows*columns);
	}
	
	void resize(const IntegerPoint &size) {
		resize(size.x,size.y);
	}
	
	Array2D(int columns = 0, int rows = 0) {
		resize(rows,columns);
	}
	
	Array2D(const IntegerPoint &size) {
		resize(size);
	}
	
	const T& at(int x, int y) const {
		return data[y*c+x];
	}
	
	T& at(int x, int y) {
		return data[y*c+x];
	}
	
	const T& at(IntegerPoint p) const {
		return at(p.x,p.y);
	}
	
	T& at(IntegerPoint p) {
		return at(p.x,p.y);
	}
	
};
