/**
 *  Computes the combinatorial solution of the puzzle
 */
class Solver {
	// returns all edges of all pieces
	Edges extractEdges(const Pieces &pieces) {
		Edges edges(4*pieces.size());
		for (unsigned int i = 0; i < pieces.size(); i++) {
			for (int k = 0; k < 4; k++) {
				edges[pieces[i]->edges[k]->id] = pieces[i]->edges[k];
			}
		}
		return edges;
	}
	
	// determines if piece contain FRAME edge
	bool isFramePiece(PieceRef piece) const {
		for (int i = 0; i < 4; i++) {
			if (piece->edges[i]->type == FRAME)
				return true;
		}
		return false;
	}
	// solve the frame and return the PuzzleLyaout with filled frame positions
	PuzzleLayout solveFrame(CompatibilityTable *table, const Pieces &frame, const Pieces &interior) {
		FrameSolver frameSolver(table,frame,interior);
		return frameSolver.solve();
	}
	// fill te interior of the layout with filled frame positions
	PuzzleLayout solveInterior(CompatibilityTable *table, const PuzzleLayout &frameLayout, const Pieces &interior) {
		InteriorSolver interiorSolver(table,frameLayout,interior);
		return interiorSolver.solve();
	}
	
	public:
	// assemble the given pieces and return the combinatoric solution
	PuzzleLayout assemblePuzzle(const Pieces &pieces) {
		Edges edges = extractEdges(pieces);
		
		cout << "Computing compatibility table" << endl;
		CompatibilityTable table(edges);
		
		Pieces frame, interior;
		for (unsigned int i = 0; i < pieces.size(); i++) {
			if (isFramePiece(pieces[i])) {
				frame.push_back(pieces[i]);
			} else {
				interior.push_back(pieces[i]);
			}
		}
		
		cout << "Assembling the frame" << endl;
		PuzzleLayout frameLayout = solveFrame(&table,frame,interior);
		
		cout << "Assembling the interior" << endl;
		PuzzleLayout layout = solveInterior(&table,frameLayout,interior);
		
		return layout;
	}
	
};
