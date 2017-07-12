#include "Schedule.h"

#include <climits>
#include <iostream>

Schedule::Schedule(int noperations)
{
    this->noperations = noperations;
    ins_index = makespan = 0;

    operations = new operation[noperations];
}

Schedule::~Schedule()
{
    delete [] operations;
}

void Schedule::set_seed(unsigned new_seed)
{
    generator.seed(new_seed);
}

void Schedule::add_operation(operation operation)
{
    operations[ins_index] = operation;
    ++ins_index;
}

void Schedule::set_makespan(int value)
{
    makespan = value;
}

void Schedule::clear()
{
    ins_index = makespan = 0;
}

int Schedule::get_makespan()
{
    return makespan;
}

double Schedule::next_random(double max_value)
{
    std::uniform_real_distribution<> dist(0, max_value);
    return dist(generator);
}

operation Schedule::get_nth_operation(int n)
{
    return operations[n];
}

Schedule* Schedule::clone()
{
    Schedule* cp = new Schedule(noperations);

    for (int i = 0; i < noperations; ++i)
        cp->add_operation(operations[i]);

    cp->set_makespan(makespan);

    return cp;
}
