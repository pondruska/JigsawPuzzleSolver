/**
 * MinCostPerfect matching
 * Implemntation of the min cost perfect matching running in time O(N^3 log N)
 * usage:
 * 1. initialize the instance with the size of partitions of the graph.
 * 2. set the weights of the edges calling setCost()
 * 3. run getMinCostMatching() to compute min cost matching for given graph
 *
 * This implementation is not possible to understand without theoretical
 * background of the used algorithm, see bibliography for references.
 */
class MinCostMatching {
	
	typedef long long ll;
	
	// MinCostMatching internally uses MinCostMaxFlow algorithm
	class MinCostMaxFlow {
		
		static const double DINF = 1e50;
		static const ll IINF = 1000000000;
		
		public:
		typedef struct {
			int v, Cap, BackIndex;
			double Cost;
		} Edge;
		
		vector< vector<Edge> > Adj; // list of edges from given vertex
		vector<double> Dist; // distance to the vertex
		vector<int> From; // previous vertex on the shortest path
		vector<int> Push; // amount of low which can be delivered to the vertex
		
		// add the edge with given properties
		void AddEdge(int Src, int Dst, int Cap, double Cost) {
			int MaxIndex = max(Src,Dst)+1;
			if (int(Adj.size()) < MaxIndex) {
				Adj.resize(MaxIndex);
				Dist.resize(MaxIndex);
				From.resize(MaxIndex);
				Push.resize(MaxIndex);
			}
			Edge EA = { Dst, Cap, Adj[Dst].size(), +Cost };
			Edge EB = { Src,   0, Adj[Src].size(), -Cost };
			Adj[Src].push_back(EA);
			Adj[Dst].push_back(EB);
		}
		
		typedef pair<double,int> PDI;
		
		bool eq(double a, double b) {
			return abs(a-b) < 1e-6;
		}
		
		// find the shortest path from source to destination
		bool Dijkstra(int Src, int Dst) {
			priority_queue<PDI> Q;
			fill(Dist.begin(),Dist.end(),DINF);
			// insert base vertex
			Dist[Src] = 0;
			From[Src] = Src;
			Push[Src] = IINF;
			Q.push(PDI(Dist[Src],Src));
			while (!Q.empty()) {
				// process one event
				int v = Q.top().second;
				double dist = -Q.top().first;
				Q.pop();
				if (!eq(dist,Dist[v])) continue;
				// push the flow into unvisited neighbours
				for (unsigned int i = 0; i < Adj[v].size(); i++) {
					Edge &e = Adj[v][i];
					if (e.Cap > 0 && Dist[e.v] > Dist[v] + e.Cost + 1e-6) {
						Dist[e.v] = Dist[v] + e.Cost;
						From[e.v] = e.BackIndex;
						Push[e.v] = min(Push[v],e.Cap);
						Q.push(PDI(-Dist[e.v],e.v));
					}
				}
			}
			// check if we reached the final vertex
			return !eq(Dist[Dst],DINF);
		}
		
		// find the min cost max flow from the source to destination
		int getMinCostMaxFlow(int Src, int Dst) {
			if (int(Adj.size()) <= max(Src,Dst))
				return 0;
			
			double Cost = 0, SumCost = 0;
			
			int flow = 0;
			
			while (Dijkstra(Src,Dst)) {
				Cost += (SumCost += Dist[Dst]) * Push[Dst];
				
				// update cost of edges
				for (unsigned int i = 0; i < Adj.size(); i++) {
					for (unsigned int j = 0; j < Adj[i].size(); j++) {
						Adj[i][j].Cost += Dist[i] - Dist[ Adj[i][j].v ];
					}
				}
				
				// update along path
				int v = Dst;
				while (v != Src) {
					Adj[v][From[v]].Cap += Push[Dst];
					int BackIndex = Adj[v][From[v]].BackIndex;
					v = Adj[v][From[v]].v;
					Adj[v][BackIndex].Cap -= Push[Dst];
				}
				flow++;
			}
			return flow;
		}
		
	};
	
	const static double inf = 1e20;
	
	public:
	
	int size;
	arma::mat costs;
	
	MinCostMatching(int partitionSize) {
		size = partitionSize;
		costs.ones(size,size);
		costs *= inf;
	}
	// specify the cost of the edge from vertex u in the first partition
	// and vertex v in the second partition
	void setCost(int u, int v, double cost) {
		assert(cost>=0.0);
		costs(u,v) = cost;
	}

	double getCost(int u, int v) {
		return costs(u,v);
	}
	
	// compute the min cost matching and return the assignment
	vector<int> getMinCostMatching() {
		MinCostMaxFlow MCMF;
		// set cost of edges
		for (int u = 0; u < size; u++) {
			for (int v = 0; v < size; v++) {
				MCMF.AddEdge(u,size+v,1,costs(u,v));
			}
		}
		// add edges to the global source and sink
		for (int i = 0; i < size; i++) {
			MCMF.AddEdge(size+size+0,i,1,0);
		}
		for (int i = 0; i < size; i++) {
			MCMF.AddEdge(size+i,size+size+1,1,0);
		}
		// find the min-cost max-flow in the graph
		int matching = MCMF.getMinCostMaxFlow(size+size+0,size+size+1);
		
		if (matching != size) return vector<int> ();
		
		vector<int> pairs(size);
		// transform the flow into matching
		for (int i = 0; i < size; i++) {
			FOREACH(it,MCMF.Adj[i])
			if (it->v < size+size && it->Cap == 0) {
				pairs[i] = it->v-size;
			}
		}
		return pairs;
	}
	
};

