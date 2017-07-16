/**
 * FrameSolver solves the frame of the puzzle - produces a partial solution
 * of placing frame pieces into a rectangular grid of size Width x Height
 * - a dimension of the final puzzle solution.
 * 
 * Frame pieces are pieces containing at least edge without padding.
 */
class FrameSolver {
	
	CompatibilityTable *table;
	// num all pieces of puzzle
	Pieces framePieces;
	
	int numPieces;
	// pieces containing FRAME edge
	
	
	typedef pair<EdgeRef,EdgeRef> EdgePair;
	
	// Determines matching edges of given FRAME piece
	// - matching edges are in a valid solution
	// always coincident with another FRAME pieces
	EdgePair matchingEdges(PieceRef piece) const {
		EdgePair res(NULL,NULL);
		for (int i = 0; i < 4; i++) {
			EdgeRef edge = piece->edges[i];
			if (edge->type != FRAME) {
				if (edge->prev->type == FRAME)
					res.first = edge;
				if (edge->next->type == FRAME)
					res.second = edge;
			}
		}
		assert(res.first != NULL && res.second != NULL);
		return res;
	}
	
	// determines if piece contain two consecutive FRAME edges
	bool isCornerPiece(PieceRef piece) const {
		EdgePair edges = matchingEdges(piece);
		return edges.first->next == edges.second;
	}
	
	// returns indices of corner pieces in given sequence of pieces
	vector<int> getCornerIndices(const Pieces &frameChain) const {
		vector<int> corners;
		for (unsigned int i = 0; i < frameChain.size(); i++) {
			if (isCornerPiece(frameChain[i]))
				corners.push_back(i);
		}
		return corners;
	}
	
	// determines the dimension of puzzle from sqeuence of pieces
	// along the frame or (-1,-1) if the sequence does not
	// represent a rectangular frame
	pair<int,int> getDimensions(const Pieces &frameChain) const {
		vector<int> corners = getCornerIndices(frameChain);
		int rows    = corners[1]-corners[0];
		int columns = corners[2]-corners[1];
		if (rows != corners[3]-corners[2])
			return make_pair(-1,-1);
		if (columns != corners[0]-corners[3]+int(frameChain.size()))
			return make_pair(-1,-1);
		return make_pair(columns+1,rows+1);
	}
	
	// return cycle defined by given permutation containing the
	// first element
	Pieces traceFirstCycle(const vector<int> &perm) const {
		Pieces chain;
		int i = 0;
		do {
			chain.push_back(framePieces[i]);
		} while ((i = perm[i]) != 0);
		return chain;
	}
	
	// crates from sequence of pieces along the correcty solved frame
	// the partial solution of placing this pieces into a grid of size Width x Height
	PuzzleLayout buildLayout(const Pieces &frameChain) const {
		int length = frameChain.size();
		pair<int,int> dimension = getDimensions(frameChain);
		printf("size of configuration: %dx%d\n", dimension.first, dimension.second);
		
		PuzzleLayout layout(dimension.first,dimension.second);
		
		int first = getCornerIndices(frameChain)[0];
		IntegerPoint pos(0,0);
		int dir = 3;
		
		for (int i = 0; i < length; i++) {
			PieceRef piece = frameChain[(first+i)%length];
			
			if (isCornerPiece(piece))
				dir = (dir+1)%4;
			layout.at(pos) = matchingEdges(piece).first->prev;
			for (int j = 0; j < dir; j++) {
				layout.at(pos) = layout.at(pos)->prev;
			}
			pos += Utils::Direction[dir];
		}
		
		return layout;
	}
	
	// determines if the sequence of pieces forms a valid solution
	// of the frame for given puzzle
	bool isCorrectSolution(const Pieces &chain) const {
		if (chain.size() != framePieces.size())
			return false;
		pair<int,int> dim = getDimensions(chain);
		if (dim == make_pair(-1,-1))
			return false;
		return dim.first * dim.second == numPieces;
	}
	
	public:
	
	// Creates an instance of SuccessiveMinCostMatching which generates
	// min cost permutations of frame pieces - potential solutions of the frame
	SuccessiveMinCostMatching getMatchingGenerator() const {
		vector<EdgePair> mEdges = MAP1(EdgePair,matchingEdges,framePieces);
		int numEdges = framePieces.size();
		
		SuccessiveMinCostMatching matching(numEdges);
		for (int i = 0; i < numEdges; i++) {
			for (int j = 0; j < numEdges; j++) {
				EdgeRef edge1 = mEdges[i].first;
				EdgeRef edge2 = mEdges[j].second;
				matching.setCost(i,j,table->score(edge1,edge2));
			}
		}
		
		return matching;
	}
	
	public:
	
	FrameSolver(CompatibilityTable *table, const Pieces &framePieces, const Pieces &interiorPieces)
	  : table(table), framePieces(framePieces), numPieces(framePieces.size() + interiorPieces.size()) {
	}
	
	PuzzleLayout solve() const {
		SuccessiveMinCostMatching generator = getMatchingGenerator();
		generator.init();
		
		// generate min-cost permutations until the permutation
		// fullfil the condition for a correctly solved frame
		int step = 0;
		Pieces chain;
		do {
			cout << "trying combination number " << ++step << endl;
			vector<int> pairs = generator.getNextMatching();
			printf("%.9lf\n", generator.cost(pairs)); //
			chain = traceFirstCycle(pairs);
		} while (!isCorrectSolution(chain));
		
		return buildLayout(chain);
	}
	
};
