#ifndef NODE_H
#define NODE_H

#include "Arc.h"

class Node
{
    public:
        Node(int ptime);
        ~Node();

        Arc* get_adj_list();
        int get_ptime();
        void add_adj(int node);
    private:
        int ptime;
        Arc* adj_list;
};

#endif // NODE_H
