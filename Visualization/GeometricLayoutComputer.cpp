/**
 * Geometric layout computes for the given combinatoric soloution
 * the exact pixel positions and transformations of the pieces
 * together with the size of the resulting image
 */
class GeometricLayoutComputer {
	
	// virtual bottom-right corner
	PieceRef FramePiece;
	ShapeAligner aligner;
	
	int numPieces;
	
	// position dependency between neighbouring pieces
	struct PieceToPieceDependency {
		PieceRef piece1, piece2;
		RigidTransformation trans;
	};
	
	// position dependency between the piece without padding and the frame
	struct PieceToFrameDependency {
		PieceRef piece;
		RigidTransformation trans;
	};
	
	struct Dependencies {
		vector<PieceToPieceDependency> PDep;
		vector<PieceToFrameDependency> FDep[4];
	};
	
	typedef map<PieceRef,EdgeRef> PieceOrientations;
	
	// add one piece to piece dependency to the set if applicable
	void addPDep(Dependencies &dep, EdgeRef up1, EdgeRef up2, int dir) {
		if (up1 == NULL || up2 == NULL) return;
		
		EdgeRef edge1 = up1->next;
		EdgeRef edge2 = up2->prev;
		for (int i = 0; i < dir; i++) {
			edge1 = edge1->next;
			edge2 = edge2->next;
		}
		
		dep.PDep.push_back((PieceToPieceDependency) {
			up1->piece, up2->piece, aligner.shapeAlign(edge2->shape,edge1->shape).t
		});
	}
	
	// add one piece to edge dependency to the set if applicable
	void addFDep(Dependencies &dep, EdgeRef up, int dir) {
		if (up == NULL) return;
		
		EdgeRef edge = up->next;
		for (int i = 0; i < dir; i++) {
			edge = edge->next;
		}
		
		dep.FDep[dir].push_back((PieceToFrameDependency) {
			edge->piece, aligner.lineAlign(edge->shape,Utils::DegreesToRadians(90*(3-dir)))
		});
	}
	
	// returns all position dependencies defined by given geometric solution of the puzzle
	pair<Dependencies,PieceOrientations> GetDependencies(const PuzzleLayout &layout) {
		Dependencies dep;
		PieceOrientations top;
		for (int y = 0; y < layout.rows(); y++) {
			for (int x = 0; x < layout.columns(); x++) {
				if (layout.at(x,y) == NULL) continue;
				
				IntegerPoint p(x,y);
				top[layout.at(p)->piece] = layout.at(p);
				
				for (int d = 0; d < 4; d++) {
					IntegerPoint dp = p + Utils::Direction[d];
					if (layout.valid(dp)) {
						addPDep(dep,layout.at(p),layout.at(dp),d);
					} else {
						addFDep(dep,layout.at(p),d);
					}
				}
			}
		}
		
		return make_pair(dep,top);
	}
	
	// computes the approximately correct rotations of all pieces
	PieceValues baseRotations(const PieceOrientations &topEdge) {
		PieceValues rotations;
		FOREACH(it,topEdge) {
			rotations[it->first] = aligner.lineAlign(it->second->shape,0.0).rotationAngle;
		}
		return rotations;
	}
	
	// computes exact rotations of all pieces in the solution
	PieceValues computeRotations(int numPieces, const Dependencies &dep) {
		LinearSystemSolver<PieceRef> solver(numPieces);
		// dependencies between pieces
		for (unsigned int i = 0; i < dep.PDep.size(); i++) {
			const PieceToPieceDependency &d = dep.PDep[i];
			solver.addDependency(d.piece1,d.piece2,Utils::normAngle(d.trans.rotationAngle));
		}
		// dependecies to the frame
		for (int k = 0; k < 4; k++) {
			for (unsigned int i = 0; i < dep.FDep[k].size(); i++) {
				const PieceToFrameDependency &d = dep.FDep[k][i];
				solver.addValue(d.piece,Utils::normAngle(d.trans.rotationAngle));
			}
		}
		return solver.solve();
	}
	// computes x positions of all pieces in the solution
	PieceValues computeXpositions(int numPieces, const Dependencies &dep) {
		LinearSystemSolver<PieceRef> solver(numPieces+1);
		// dependencies between pieces
		for (unsigned int i = 0; i < dep.PDep.size(); i++) {
			const PieceToPieceDependency &d = dep.PDep[i];
			solver.addDependency(d.piece1,d.piece2,d.trans.translation.x);
		}
		// dependencies to the left frame
		for (unsigned int i = 0; i < dep.FDep[2].size(); i++) {
			const PieceToFrameDependency &d = dep.FDep[2][i];
			solver.addValue(d.piece,d.trans.translation.x);
		}
		// dependencies to the right frame
		for (unsigned int i = 0; i < dep.FDep[0].size(); i++) {
			const PieceToFrameDependency &d = dep.FDep[0][i];
			solver.addDependency(d.piece,FramePiece,d.trans.translation.x);
		}
		return solver.solve();
	}
	
	// computes y positions of all pieces in the solution
	PieceValues computeYpositions(int numPieces, const Dependencies &dep) {
		LinearSystemSolver<PieceRef> solver(numPieces+1);
		// dependencies between pieces
		for (unsigned int i = 0; i < dep.PDep.size(); i++) {
			const PieceToPieceDependency &d = dep.PDep[i];
			solver.addDependency(d.piece1,d.piece2,d.trans.translation.y);
		}
		// dependencies to the bottom frame
		for (unsigned int i = 0; i < dep.FDep[3].size(); i++) {
			const PieceToFrameDependency &d = dep.FDep[3][i];
			solver.addValue(d.piece,d.trans.translation.y);
		}
		// dependencies to the top frame
		for (unsigned int i = 0; i < dep.FDep[1].size(); i++) {
			const PieceToFrameDependency &d = dep.FDep[1][i];
			solver.addDependency(d.piece,FramePiece,d.trans.translation.y);
		}
		return solver.solve();
	}
	
	// recompute the dependencies if we rotate all elements by given angles
	Dependencies RotateElements(Dependencies dep, PieceValues angles) {
		for (unsigned int i = 0; i < dep.PDep.size(); i++) {
			PieceToPieceDependency &d = dep.PDep[i];
			RigidTransformation rotation1(-angles[d.piece1]);
			d.trans = Geometry2D::compositeTransformation(rotation1,d.trans);
			RigidTransformation rotation2(+angles[d.piece2]);
			d.trans = Geometry2D::compositeTransformation(d.trans,rotation2);
		}
		for (int k = 0; k < 4; k++) {
			for (unsigned int i = 0; i < dep.FDep[k].size(); i++) {
				PieceToFrameDependency &d = dep.FDep[k][i];
				RigidTransformation rotation(-angles[d.piece]);
				d.trans = Geometry2D::compositeTransformation(rotation,d.trans);
			}
		}
		return dep;
	}
	
	public:
	
	// computes the geometric layout of the given combinatoric solution
	GeometricLayout computeLayout(const PuzzleLayout &layout) {
		pair<Dependencies,PieceOrientations> dep = GetDependencies(layout);
		int numPieces = dep.second.size();
		// rotate the pieces in approximately correct direction
		PieceValues baseRotation = baseRotations(dep.second);
		dep.first = RotateElements(dep.first,baseRotation);
		// find exact values of rotation
		PieceValues rotation = computeRotations(numPieces,dep.first);
		dep.first = RotateElements(dep.first,rotation);
		// compute the translation of each piece
		PieceValues posX = computeXpositions(numPieces,dep.first);
		PieceValues posY = computeYpositions(numPieces,dep.first);
		// create the final geometric layout
		GeometricLayout geometricLayout;
		geometricLayout.width  = posX[FramePiece];
		geometricLayout.height = posY[FramePiece];
		
		FOREACH(it,baseRotation) {
			PieceRef p = it->first;
			geometricLayout.positions[p] = RigidTransformation(rotation[p]+baseRotation[p],posX[p],posY[p]);
		}
		
		return geometricLayout;
	}
	
};
