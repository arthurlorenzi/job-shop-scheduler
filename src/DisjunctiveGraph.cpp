#include "DisjunctiveGraph.h"

#include <climits>

DisjunctiveGraph::DisjunctiveGraph(int num_nodes)
{
    order = num_nodes;
    visited = new bool[order];
    node_map = new Node*[order];

    for (int i = 0; i < order; ++i)
    {
        visited[i] = false;
        node_map[i] = nullptr;
    }
}

DisjunctiveGraph::~DisjunctiveGraph()
{
    for (int i = 0; i < order; ++i)
        delete node_map[i];

    delete [] node_map;
    delete [] visited;
}

void DisjunctiveGraph::add_node(int node, int ptime)
{
    node_map[node] = new Node(ptime);
}

void DisjunctiveGraph::add_arc(int from, int to)
{
    Node* tail = node_map[from];

    tail->add_adj(to);
}

void DisjunctiveGraph::add_edge(int from, int to)
{
    Node *tail = node_map[from];
    Node *head = node_map[to];

    tail->add_adj(to);
    head->add_adj(from);

    /// These are the last added Arc objects
    Arc *direction = tail->get_adj_list();
    Arc *opposite = head->get_adj_list();

    direction->set_opposite(opposite);
    opposite->set_opposite(direction);
}

void DisjunctiveGraph::reset_visited()
{
    for(int i = 0; i < order; ++i)
        visited[i] = false;
}

void DisjunctiveGraph::set_as_source(int node)
{
    int adj;

    for (Arc *it = node_map[node]->get_adj_list(); it != nullptr; it = it->get_next())
    {
        adj = it->get_head_node();

        if (!visited[adj])
            it->edge_to_arc();
    }

    visited[node] = true;
}

void DisjunctiveGraph::topological_sort(int node, std::stack<int> &ordered_graph)
{
    int adj;

    visited[node] = true;

    for (Arc *it = node_map[node]->get_adj_list(); it != nullptr; it = it->get_next())
    {
        if (it->is_active())
        {
            adj = it->get_head_node();

            if (!visited[adj])
                topological_sort(adj, ordered_graph);
        }
    }

    ordered_graph.push(node);
}

int DisjunctiveGraph::len_of_longest_path()
{
    int node, adj, longest = INT_MIN;
    int *dist = new int[order];
    std::stack<int> ordered_graph;

    dist[0] = 0; // source
    for (int i = 1; i < order; ++i)
    {
        dist[i] = INT_MIN;
        visited[i] = false;
    }

    topological_sort(0, ordered_graph);

    while(!ordered_graph.empty())
    {
        node = ordered_graph.top();
        ordered_graph.pop();

        for (Arc *it = node_map[node]->get_adj_list(); it != nullptr; it = it->get_next())
        {
            if (it->is_active())
            {
                adj = it->get_head_node();

                if (dist[adj] < dist[node] + node_map[adj]->get_ptime())
                {
                    dist[adj] = dist[node] + node_map[adj]->get_ptime();
                    if (dist[adj] > longest) longest = dist[adj];
                }
            }
        }

    }

    delete [] dist;

    return longest;
}

DisjunctiveGraph* DisjunctiveGraph::clone()
{
    int adj;
    DisjunctiveGraph* cp = new DisjunctiveGraph(order);

    for (int i = 0; i < order; ++i)
    {
        visited[i] = false;
        cp->add_node(i, node_map[i]->get_ptime());
    }

    for (int i = 0; i < order; ++i)
    {
        for (Arc *it = node_map[i]->get_adj_list(); it != nullptr; it = it->get_next())
        {
            adj = it->get_head_node();

            if (it->is_edge_component())
            {
                if (!visited[adj])
                    cp->add_edge(i, adj);
            }
            else
                cp->add_arc(i, adj);
        }

        visited[i] = true;
    }

    return cp;
}
