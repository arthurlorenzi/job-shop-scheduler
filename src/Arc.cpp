#include "Arc.h"

Arc::Arc(int node, Arc* next)
{
    head_node = node;
    this->next = next;
    active = true;
    opposite = nullptr;
}

Arc::~Arc()
{
    //dtor
}

int Arc::get_head_node()
{
    return head_node;
}

Arc* Arc::get_next()
{
    return next;
}

void Arc::set_next(Arc* arc)
{
    next = arc;
}

void Arc::set_opposite(Arc* arc)
{
    opposite = arc;
}

void Arc::edge_to_arc()
{
    active = true;

    if (opposite != nullptr)
        opposite->deactivate();
}

void Arc::deactivate()
{
    active = false;
}

bool Arc::is_edge_component()
{
    return opposite != nullptr;
}

bool Arc::is_active()
{
    return active;
}
