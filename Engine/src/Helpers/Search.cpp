
#include <vector>

namespace Engine
{
	struct GraphNode
	{
		// Adjacency list
		std::vector<GraphNode*> m_Adjacent;
	};

	struct Graph
	{
		std::vector<GraphNode*> m_Nodes;
	};

	// For a weighted graph, each node stores its outgoing edges
	struct WeightedEdge
	{
		struct WeightedGraphNode* m_From;
		struct WeightedGraphNode* m_To;
		float m_Weight;
	};

	struct WeightedGraphNode
	{
		// Stores only outgoing edges
		std::vector<WeightedEdge*> m_Edges;
	};

	struct WeightedGraph
	{
		std::vector<WeightedGraphNode*> m_Nodes;
	};
}
