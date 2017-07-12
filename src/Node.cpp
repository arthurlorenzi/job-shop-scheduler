#include "Node.h"

Node::Node(int ptime)
{
    this->ptime = ptime;
    adj_list = nullptr;
}

Node::~Node()
{
    Arc *aux, *arc = adj_list;

    while (arc != nullptr)
    {
        aux = arc;
        arc = arc->get_next();

        delete aux;
    }
}

Arc* Node::get_adj_list()
{
    return adj_list;
}

int Node::get_ptime()
{
    return ptime;
}

void Node::add_adj(int node)
{
    adj_list = new Arc(node, adj_list);
}
