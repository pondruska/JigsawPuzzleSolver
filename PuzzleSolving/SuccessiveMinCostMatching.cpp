/**
 * SuccessiveMinCost Matching
 * Implemntation of Murphy's algorithm
 * usage:
 * 1. initialize the instance with the size of partitions of the graph.
 * 2. set the weights of the edges calling setCost()
 * 3. run init()
 * 4. each call of getNextMatching() computes the next lowest cost matching in order
 *
 * This implementation is not possible to understand without theoretical
 * background of the used algorithm, see bibliography for references.
 */

// restricted solution is a solution to problem of finding minimum cost assignment,
// when some edges has to be in matching and som can not
struct RestrictedSolution {
	double cost;
	Permutation perm;
	vector<Pair> forced; // must be in matching
	vector<Pair> free; // can not be in matching
};

bool operator < (const RestrictedSolution &a, const RestrictedSolution &b) {
	return a.cost > b.cost;
}

class SuccessiveMinCostMatching {

	static const int INF = 10000000;
	
	public:
	
	int size;
	Array2D<double> costs;
	priority_queue<RestrictedSolution> Q;
	
	// initializes the instance with the size of one partititon
	SuccessiveMinCostMatching(int N) {
		size = N;
		costs.resize(N,N);
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				costs.at(i,j) = Utils::DOUBLE_INF;
			}
		}
	}
	
	// cost of the assignment
	double cost(Permutation &p) {
		double c = 0;
		for (int i = 0; i < size; i++) {
			c += costs.at(i,p[i]);
		}
		return c;
	}
	
	// returns the solution of the restricted problem
	RestrictedSolution solve(const vector<Pair> &forced, const vector<Pair> &free) {
		
		MinCostMatching m(size);
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				m.setCost(i,j,costs.at(i,j));
			}
		}
		
		for (unsigned int i = 0; i < forced.size(); i++) {
			int a = forced[i].first;
			int b = forced[i].second;
			for (int k = 0; k < size; k++) {
				m.setCost(a,k,INF);
			}
			for (int k = 0; k < size; k++) {
				m.setCost(k,b,INF);
			}
			m.setCost(a,b,costs.at(a,b));
		}
		
		for (unsigned int i = 0; i < free.size(); i++) {
			int a = free[i].first;
			int b = free[i].second;
			m.setCost(a,b,INF);
		}
		
		RestrictedSolution s;
		s.perm = m.getMinCostMatching();
		s.cost = cost(s.perm);
		for (int i = 0; i < size; i++) {
			if (m.getCost(i,s.perm[i]) > INF/2) {
				s.cost = INF;
			}
		}
		s.forced = forced;
		s.free = free;
		return s;
	}
	
	public:
	// set the cost of the edge between vertices a in the first partitition
	// and vertex b in the second partition
	void setCost(int a, int b, double cost) {
		costs.at(a,b) = cost;
	}
	
	// returns the cost of the edge between vertices a in the first partitition
	// and vertex b in the second partition
	double getCost(int a, int b) {
		return costs.at(a,b);
	}
	
	void init() {
		vector<Pair> forced, free;
		Q.push(solve(forced,free));
	}
	
	// returns next matching in order of cost
	Permutation getNextMatching() {
		RestrictedSolution s = Q.top(); Q.pop();
		for (int i = 0; i < size-1; i++) {
			s.free.push_back(Pair(i,s.perm[i]));
			Q.push(solve(s.forced,s.free));
			s.free.pop_back();
			s.forced.push_back(Pair(i,s.perm[i]));
		}
		return s.perm;
	}
	
};

