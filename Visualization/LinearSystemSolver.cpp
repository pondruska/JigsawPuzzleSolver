/**
 * Solver of the overdetermined linear system
 * usage:
 * 1. initialize the solver with the numer of variales in the system
 * 2. add dependecies to the system of type: x-y = constant
 *    add value to the system of type: x = constant
 * 3. call solve() to find the solution of the system which minimizes the error
 */
template <class T> class LinearSystemSolver {
	
	int columns;
	map<T,int> variableIndex;
	
	struct VDep {
		int index1, index2;
		double val;
	};
	
	struct VVal {
		int index;
		double val;
	};
	
	vector<VDep> dep;
	vector<VVal> val;
	
	// index of th given variable
	int index(T var) {
		if (!variableIndex.count(var)) {
			int i = variableIndex.size();
			variableIndex[var] = i;
		}
		return variableIndex[var];
	}
	
	public:
	
	// creates an instance of the system with N variables
	LinearSystemSolver(int numVariables) {
		columns = numVariables;
	}
	
	// add value to the system of type: x-y = constant
	void addDependency(T var1, T var2, double d) {
		dep.push_back((VDep) { index(var1), index(var2), d });
	}
	
	// add value to the system of type: x = constant
	void addValue(T var, double v) {
		val.push_back((VVal) { index(var), v });
	}
	
	PieceValues solve() {
		using namespace arma;
		// create matrix
		mat matrix = zeros<mat> (dep.size()+val.size(),columns);
		vec right = zeros<vec> (dep.size()+val.size());
		int p = 0;
		// add dependency equations
		for (unsigned int i = 0; i < dep.size(); i++) {
			matrix(p,dep[i].index1) = +1;
			matrix(p,dep[i].index2) = -1;
			right(p) = dep[i].val;
			p++;
		}
		// add value equations
		for (unsigned int i = 0; i < val.size(); i++) {
			matrix(p,val[i].index) = +1;
			right(p) = val[i].val;
			p++;
		}
		vec res = arma::solve(matrix,right);
		// solve
		PieceValues result;
		FOREACH(it,variableIndex) result[it->first] = res(it->second);
		return result;
	}
	
};
