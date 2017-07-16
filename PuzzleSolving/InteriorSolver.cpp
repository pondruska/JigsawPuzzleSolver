/**
 * Interior solver fills the interior of the given puzzle layout with
 * filled frame positions and produces the final combinatoric solution
 * filling is done by placing one piece at a time at some unused position
 */
class InteriorSolver {
	// pieces which are not placed already
	
	CompatibilityTable *table;
	PuzzleLayout layout;
	Pieces pieces;
	
	// returns the score of coinciding edges for the given possibility
	double matchingScore(PieceEdges edges, EdgeRef topEdge) {
		topEdge = topEdge->next;
		double score = 0;
		for (int i = 0; i < 4; i++) {
			if (edges[i] != NULL) {
				score += table->score(edges[i], topEdge);
			}
			topEdge = topEdge->next;
		}
		return score;
	}
	
	// returns the already placed edges coninciding with given position
	PieceEdges placedEdges(IntegerPoint position) {
		PieceEdges edges;
		for (int d = 0; d < 4; d++) {
			IntegerPoint p = position + Utils::Direction[d];
			if (layout.valid(p) && layout.at(p) != NULL) {
				edges[d] = layout.at(p)->prev;
				for (int i = 0; i < d; i++) {
					edges[d] = edges[d]->next;
				}
			} else {
				edges[d] = NULL;
			}
		}
		return edges;
	}
	
	// returns the number of already placed edges coinciding with given position
	int numPlacedEdges(IntegerPoint position) {
		PieceEdges edges = placedEdges(position);
		int numEdges = 0;
		for (int i = 0; i < 4; i++) {
			if (edges[i] != NULL) {
				numEdges++;
			}
		}
		return numEdges;
	}
	
	// retursn the positions where can be placed the new piece into builded solution
	// = positions with maximal number of already occupied neigbouring positions
	IntegerPoints getFreePositions() {
		int maxCardinality = 2;
		IntegerPoints positions;
		for (int y = 0; y < layout.rows(); y++) {
			for (int x = 0; x < layout.columns(); x++) {
				IntegerPoint p(x,y);
				if (layout.at(p) == NULL) {
					int cardinality = numPlacedEdges(p);
					if (maxCardinality < cardinality) {
						maxCardinality = cardinality;
						positions.clear();
					}
					if (maxCardinality == cardinality) {
						positions.push_back(p);
					}
				}
			}
		}
		return positions;
	}
	
	// returns the best possibility of placing some piece at some unused position
	PieceLayout getBestChoice() {
		// get all free spots
		IntegerPoints positions = getFreePositions();
		PieceLayout bestLayout;
		double bestScore = Utils::DOUBLE_INF;
		// backtrack all possibilities and choose the best one
		for (unsigned int i = 0; i < positions.size(); i++) {
			PieceEdges edges = placedEdges(positions[i]);
			for (unsigned int j = 0; j < pieces.size(); j++) {
				for (int d = 0; d < 4; d++) {
					double score = matchingScore(edges,pieces[j]->edges[d]);
					if (bestScore > score) {
						bestScore = score;
						bestLayout.edge = pieces[j]->edges[d];
						bestLayout.position = positions[i];
					}
				}
			}
		}
		
		return bestLayout;
	}
	
	// places given piece at given position
	void placePiece(IntegerPoint position, EdgeRef topEdge) {
		layout.at(position) = topEdge;
		
		// delete piece
		pieces.erase(find(pieces.begin(),pieces.end(),topEdge->piece));

		// disable the covered edges
		PieceEdges edges = placedEdges(position);
		topEdge = topEdge->next;
		for (int i = 0; i < 4; i++) {
			if (edges[i] != NULL) {
				table->disableEdge(edges[i]);
				table->disableEdge(topEdge);
			}
			topEdge = topEdge->next;
		}
	}
	
	public:
	
	// initalize with the layou having empty interior and free pices to place into this interior
	InteriorSolver(CompatibilityTable *table, const PuzzleLayout &frameLayout, const Pieces &pieces) :
		table(table), layout(frameLayout), pieces(pieces) { }
	
	// solves the interior and return the final combinatoric solution
	PuzzleLayout solve() {
		// place all pieces one at a time
		while (!pieces.empty()) {
			// choose the best possibility
			PieceLayout choice = getBestChoice();
			// place the piece at the position
			placePiece(choice.position, choice.edge);
		}
		
		return layout;
	}
	
};
