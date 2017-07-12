#ifndef DISJUNCTIVEGRAPH_H
#define DISJUNCTIVEGRAPH_H

#include<stack>

#include "Node.h"

class DisjunctiveGraph
{
    public:
        DisjunctiveGraph(int num_nodes);
        ~DisjunctiveGraph();

        void add_node(int node, int ptime);
        void add_arc(int from, int to);
        void add_edge(int from, int to);
        void reset_visited();
        void set_as_source(int node);
        int len_of_longest_path();

        DisjunctiveGraph* clone();
    private:
        int order;
        bool *visited;
        Node** node_map;

        void topological_sort(int node, std::stack<int> &ordered_graph);
};

#endif // DISJUNCTIVEGRAPH_H
