#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <random>
#include <utility>

typedef struct {
    int id;
    int machine;
    int processing_time;
    int job_remaining_time;
} operation;

class Schedule
{
    public:
        Schedule(int noperations);
        ~Schedule();

        void set_seed(unsigned new_seed);
        void add_operation(operation operation);
        void set_makespan(int value);
        void clear();
        int get_makespan();
        double next_random(double max_value);
        operation get_nth_operation(int n);
        Schedule* clone();

    private:
        int noperations, ins_index, makespan;
        operation* operations;
        std::mt19937 generator;
};

#endif // SCHEDULE_H
