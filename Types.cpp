
typedef vector<RealPoint> Shape;
typedef vector<Color> ColorSignature;

typedef vector<Shape> Shapes;

typedef vector<RealPoint> RealPoints;
typedef vector<IntegerPoint> IntegerPoints;

typedef vector<int> Permutation;
typedef vector<double> Weight;

typedef pair<double,RealPoint> WeightedPoint;
typedef pair<double,Shape> ScoredAlign;

typedef pair<int,int> Pair;

// piece structure definition

struct Piece;
struct Edge;

typedef const Piece* PieceRef;
typedef const Edge*  EdgeRef;

typedef vector<PieceRef> Pieces;
typedef vector<EdgeRef> Edges, ScaledEdge;

typedef pair<double,EdgeRef> ScoredEdge;

typedef array<EdgeRef,4> PieceEdges;

// instance of one puzzle piece
struct Piece {
	// name of the image the piece was extracted from
	string imageName;
	// position of the center in the source image
	IntegerPoint center;
	// id number of th epiece
	int id;
	// four edges of the piece
	PieceEdges edges;
};

typedef enum {
	OUTDENT = -1,
	FRAME = 0,
	INDENT = +1
} EdgeType;

// instance of one piece edge
struct Edge {
	// unique id of the edge
	int id;
	// pointer to the next and previous edge of the same piece
	// in the clockwise direction
	EdgeRef prev, next;
	// pointer to the owning piece
	PieceRef piece;
	
	// logial type of the edge
	EdgeType type;
	// sequence of points defining the edge
	Shape shape;
	// colour extracted for every point in shape
	ColorSignature color;
};

typedef tr1::array<int,4> Quadruplet;

// rigid transformation = translation + rotation
struct RigidTransformation {
	
	double rotationAngle;
	RealVector translation;
	
	RigidTransformation(double _rotationAngle = 0.0, double _transX = 0.0, double _transY = 0.0) {
		rotationAngle = _rotationAngle;
		translation = RealPoint(_transX,_transY);
	}
	
	RigidTransformation(double _rotationAngle, RealPoint _translation) {
		translation = _translation;
		rotationAngle = _rotationAngle;
	}
	
};

// combinatoric solution of the puzzle
// rectangular 2D array - for every position we store the edge facing north
typedef Array2D<EdgeRef> PuzzleLayout;

struct PiecePosition {
	PieceRef piece;
	RigidTransformation position;
};

// geometric layout of visualized solution
struct GeometricLayout {
	double width, height;
	map<PieceRef,RigidTransformation> positions;
};

// geometrical align of one edge along the other together with matching points
struct ShapeAlign {
	Permutation pairs12;
	Permutation pairs21;
	RigidTransformation t;
};

// score of four basic similarities: Shape, HUE, SATURATION, LUMINANCE
struct Score {
	float shape, H, S, L;
};

typedef map<PieceRef,double> PieceValues;

struct PieceLayout {
	IntegerPoint position;
	EdgeRef edge;
};

struct Solution {
	double score;
	IntegerPoints positions;
	Edges topEdges;
};
