#ifndef ARC_H
#define ARC_H

class Arc
{
    public:
        Arc(int node, Arc* next);
        ~Arc();

        int get_head_node();
        Arc* get_next();
        void set_next(Arc* arc);
        void set_opposite(Arc* arc);
        void edge_to_arc();
        void deactivate();
        bool is_edge_component();
        bool is_active();
    private:
        bool active;
        int head_node;
        Arc* next;
        Arc* opposite;
};

#endif // ARC_H
