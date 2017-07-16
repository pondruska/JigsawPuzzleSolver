/**
 * Representation of one row of the cmpatibility table e.g. scores for 
 * each possible edge to one given edge
 */

class EdgeScores {
	
	public:
	
	struct EdgeState {
		EdgeRef edge;
		CompatibilityClassificator* classificator;
		Score score;
	};
	
	// sort the working versions of the edges only by shape score component
	static bool sortByShapeScore(const EdgeState &s1, const EdgeState &s2) {
		return s1.score.shape < s2.score.shape;
	}
	
	EdgeRef edge;
	Score best;
	vector<Score> score;	
	
	// get the working versions of edges, this version contains also the optimal layout and matching points
	// and lives only during a computation of its score
	vector<EdgeState> getEdgeStates(const vector<ScaledEdge> &edges) {
		vector<EdgeState> edgeStates;
		for (unsigned int i = 0; i < edges.size(); i++) {
			if (CompatibilityClassificator::compatibleTypes(edge,edges[i][0])) {
				EdgeState state;
				state.edge = edges[i][0];
				state.classificator = new CompatibilityClassificator(edges[edge->id],edges[i]);
				edgeStates.push_back(state);
			} else {
				disableEdge(edges[i][0]);
			}
		}
		return edgeStates;
	}
	
	// dispose the edges allocated in getEdgeStates()
	void deleteEdgeStates(vector<EdgeState> &states) {
		for (unsigned int i = 0; i < states.size(); i++) {
			delete states[i].classificator;
		}
		states.clear();
	}
	
	// finds the best score for each knd of the score
	void recompute() {
		using Utils::DOUBLE_INF;
		best = (Score){ DOUBLE_INF, DOUBLE_INF, DOUBLE_INF, DOUBLE_INF };
		for (unsigned int i = 0; i < score.size(); i++) {
			best.shape = min(best.shape,score[i].shape);
			best.H = min(best.H,score[i].H);
			best.S = min(best.S,score[i].S);
			best.L = min(best.L,score[i].L);
		}
	}
	
	public:
	
	// initalize the row for given edge
	EdgeScores(EdgeRef edge) : edge(edge) {
	}
	
	// returns a score for the given edge
	double getScore(EdgeRef edge) const {
		Score s = score[edge->id];
		return SHAPE_WEIGHT   * (1 - best.shape / s.shape)
		  + HUE_WEIGHT        * (1 - best.H / s.H)
		  + SATURATION_WEIGHT * (1 - best.S / s.S)
		  + LUMINOSITY_WEIGHT * (1 - best.L / s.L);
	}
	
	// computes the scores using the lower resolution versions of each edge
	void init(const vector<ScaledEdge> &edges) {
		score.resize(edges.size());
		vector<EdgeState> edgeStates = getEdgeStates(edges);
		int numEdges = edgeStates.size();
		
		int k = numEdges;
		// fraction of edges keept in every round
		const double scale = pow(double(numEdges)/BASE_SIZE,-1.0/RESOLUTION_DEPTH);
		for (int i = 0; i < RESOLUTION_DEPTH; i++) {
			// recompute the score using higher resolution
			for (int j = 0; j < k; j++) {
				EdgeState &s = edgeStates[j];
				s.score = s.classificator->recomputeScore();
			}
			// sort edges by scores
			sort(edgeStates.begin(),edgeStates.begin()+k,sortByShapeScore);
			// keep only some fraction to next round
			k = min(Utils::Convert(k*scale),numEdges);
		}
		// fill the comaptibility table
		for (int i = 0; i < numEdges; i++) {
			score[ edgeStates[i].edge->id ] = edgeStates[i].score;
		}
		deleteEdgeStates(edgeStates);
		
		recompute();
	}
	// disables one edge, so it won't be considered as the best possibility anymore
	void disableEdge(const EdgeRef &edge) {
		using Utils::DOUBLE_INF;
		score[edge->id] = (Score){ DOUBLE_INF, DOUBLE_INF, DOUBLE_INF, DOUBLE_INF };
		recompute();
	}

};
