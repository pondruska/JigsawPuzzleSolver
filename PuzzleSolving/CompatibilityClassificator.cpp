/**
 * Computes the basic compatibility scores for shape and colour,
 * optimized for using with scaled edges
 * Usage:
 * 1. create scaled versions of two edges (edge + several edges in a lower resolution)
 * 2. initialize the instance with this scaled edges
 * 3. each call of recomputeScore returns score of edges computed
 *    using higher resolution than previous call
 */
class CompatibilityClassificator {
	
	ShapeAligner aligner;
	
	const ScaledEdge &edge1;
	const ScaledEdge &edge2;
	ShapeAlign align;
	int level;
	
	typedef struct {
		double H, S, L;
	} ColorScore;
	
	// distance of points on circle
	inline double circleDist(double a, double b) {
		if (a > b) swap(a,b);
		return min(b-a,1.0-b+a);
	}
	
	inline double squareDist(double x) {
		return x * x;
	}
	
	// computes baseic shape score
	double shapeScore(const Shape &shape1, const Shape &shape2, const Permutation &pairs) {
		double sum = 0.0;
		for (unsigned int i = 0; i < pairs.size(); i++) {
			sum += (shape1[i]-shape2[pairs[i]]).squareLength();
		}
		return sum;
	}
	
	// computes basic colour scores
	ColorScore colorScore(const ColorSignature &color1, const ColorSignature &color2, const Permutation &pairs) {
		ColorScore score = { 0.0, 0.0, 0.0 };
		for (unsigned int i = 0; i < pairs.size(); i++) {
			ColorHSL c1 = color1[i];
			ColorHSL c2 = color2[pairs[i]];
			score.H += circleDist(c1.hue(),c2.hue());
			score.S += squareDist(c1.saturation()-c2.saturation());
			score.L += squareDist(c1.luminosity()-c2.luminosity());
		}
		return score;
	}
	
	// create from optimal resolution iin lower resolution an optimal align in higher resolution
	void rescaleAlign() {
		typedef SignalProcessor<int> ISP;
		int len1A = edge1[level-1]->shape.size();
		int len1B = edge1[level]->shape.size();
		int len2A = edge2[level-1]->shape.size();
		int len2B = edge2[level]->shape.size();
		
		for (unsigned int i = 0; i < align.pairs12.size(); i++) {
			align.pairs12[i] *= double(len2B-1)/(len2A-1);
		}
		for (unsigned int i = 0; i < align.pairs21.size(); i++) {
			align.pairs21[i] *= double(len1B-1)/(len1A-1);
		}
		align.pairs12 = ISP::resample(align.pairs12,len1B);
		align.pairs21 = ISP::resample(align.pairs21,len2B);
	}
	
	// create scaled version of the edge
	static EdgeRef scaleEdge(EdgeRef edge, double scale) {
		Edge *e  = new Edge;
		e->id    = edge->id;
		e->prev  = edge->prev;
		e->next  = edge->next;
		e->piece = edge->piece;
		e->type  = edge->type;
		e->shape = SignalProcessor<RealPoint>::resample(edge->shape,scale);
		e->color = SignalProcessor<Color>::resample(edge->color,scale);
		return e;
	}
	
	public:
	
	// initialize the instance with two scaled edges
	CompatibilityClassificator(const ScaledEdge &edge1, const ScaledEdge &edge2) :
		edge1(edge1), edge2(edge2), level(0) {
	}
	
	// recompute the score using higher resolution
	Score recomputeScore() {
		const Shape &shape1 = edge1[level]->shape;
		const Shape &shape2 = edge2[level]->shape;
		const ColorSignature &color1 = edge1[level]->color;
		const ColorSignature &color2 = edge2[level]->color;
		
		if (level == 0) {
			align = aligner.shapeAlign(shape1, shape2);
		} else {
			rescaleAlign();
			align = aligner.shapeAlign(shape1, shape2, align);
		}
		
		int length = align.pairs12.size() + align.pairs21.size();
		Shape shapeT = Geometry2D::transform(shape2,align.t);
		
		Score score = { 0.0, 0.0, 0.0, 0.0 };
		score.shape += shapeScore(shape1,shapeT,align.pairs12);
		score.shape += shapeScore(shapeT,shape1,align.pairs21);
		score.shape /= length;
		
		ColorScore c1 = colorScore(color1,color2,align.pairs12);
		ColorScore c2 = colorScore(color2,color1,align.pairs21);
		score.H += (c1.H + c2.H) / length;
		score.S += (c1.S + c2.S) / length;
		score.L += (c1.L + c2.L) / length;
		
		level++;
		return score;
	}
	
	// determines if two given edges can logically fit together
	static bool compatibleTypes(EdgeRef edge1, EdgeRef edge2) {
		if (edge1->piece == edge2->piece) return false;
		if (edge1->type == edge2->type) return false;
		if (edge1->type != -edge2->type) return false;
		if (bool(edge1->next->type) != bool(edge2->prev->type)) return false;
		if (bool(edge1->prev->type) != bool(edge2->next->type)) return false;
		return true;
	}

	// create set of scaled edges in lower resolution for the given edge
	static ScaledEdge createScaledEdge(EdgeRef edge) {
		ScaledEdge scaledEdge;
		scaledEdge.push_back(edge);
		for (int i = 1; i < RESOLUTION_DEPTH; i++) {
			double scale = 1.0 - double(i) / RESOLUTION_DEPTH;
			scaledEdge.push_back(scaleEdge(edge,scale));
		}
		return scaledEdge;
	}
	
	// dispose the set of scaled edges
	static void deleteScaledEdge(ScaledEdge &edge) {
		for (int i = 1; i < RESOLUTION_DEPTH; i++)
			delete edge[i];
		edge.clear();
	}
	
};
