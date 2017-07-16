/**
 * Compatibility table stores compatibility scores for every pair of edges
 */
class CompatibilityTable {
	// rows of the table
	vector<EdgeScores> scores;
	
	public:
	// disable the given edge, the edge won't be considered as potentionally best matching edge anymore
	inline void disableEdge(EdgeRef edge) {
		Parallel::ForEach(scores,&EdgeScores::disableEdge,edge);
	}
	
	// returns the score for given pair of edges
	inline double score(EdgeRef edge1, EdgeRef edge2) const {
		return scores[edge1->id].getScore(edge2) + scores[edge2->id].getScore(edge1);
	}
	
	// Initializes the compatibility table for given set of edges
	CompatibilityTable(const Edges &edges) {
		// create for every edge the versions in lower resolution
		vector<ScaledEdge> scaledEdges = MAP1(
			ScaledEdge,CompatibilityClassificator::createScaledEdge,edges
		);
		// create rows of the table
		for (unsigned int i = 0; i < edges.size(); i++) {
			scores.push_back(EdgeScores(edges[i]));
		}
		// compute the scores in each row of the table
		Parallel::ForEach(scores,&EdgeScores::init,scaledEdges);
		// dispose the memory allocated for scaled versions of edges
		for (unsigned int i = 0; i < scaledEdges.size(); i++) {
			CompatibilityClassificator::deleteScaledEdge(scaledEdges[i]);
		}
	}
	
};
