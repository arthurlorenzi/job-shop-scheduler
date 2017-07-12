#include "JobShopScheduler.h"

#include <chrono>
#include <climits>
#include <fstream>

#include <iostream>

JobShopScheduler::JobShopScheduler(std::string file)
{
    load_instance(file);

    set_seed(std::chrono::system_clock::now().time_since_epoch().count());
    niterations = 2000;
    nants = noperations;
    Q = 1;
    alpha = 0.7617;
    beta = 3.7982;
    p = 0.7163; // persistence
    pbest = 0.3072;
}

JobShopScheduler::~JobShopScheduler()
{
    for (int i = 0; i < njobs; ++i)
        delete [] operations[i];

    for (int i = 0; i < noperations + 1; ++i)
        delete [] trail[i];

    delete [] operations;
    delete [] trail;
}

void JobShopScheduler::load_instance(std::string file)
{
    std::ifstream stream(file);
    int **ids_by_machine;

    stream >> njobs >> nmachines;
    noperations = njobs * nmachines;
    // one node for each operation plus source node of the graphs
    nnodes = noperations + 1;

    // PERT used in solution evaluation
    pert_model = new DisjunctiveGraph(nnodes);
    // source node
    pert_model->add_node(0, 0);

    // instantiate operations matrix
    operations = new operation*[njobs];
    // number of operations for each job = number of machines
    for (int i = 0; i < njobs; ++i)
        operations[i] = new operation[nmachines];

    // instantiate trail matrix
    trail = new double*[nnodes];
    for (int i = 0; i < nnodes; ++i)
        trail[i] = new double[nnodes];

     ids_by_machine = new int*[nmachines];
    // each machine executes one operation for each job
    for (int i = 0; i < nmachines; ++i)
        ids_by_machine[i] = new int[njobs];

    int machine, processing_time, operation_id;

    // read operation data from file
    for (int i = 0; i < njobs; ++i)
    {
        for (int j = 0; j < nmachines; ++j)
        {
            stream >> machine >> processing_time;
            operation_id = (noperations/njobs * i) + j + 1;

            operations[i][j].id = operation_id;
            operations[i][j].machine = machine;
            operations[i][j].processing_time = processing_time;

            ids_by_machine[machine][i] = operation_id;
            pert_model->add_node(operation_id, processing_time);
        }
    }

    // remaining job time for each operation
    for (int i = 0; i < njobs; ++i)
    {
        operations[i][nmachines-1].job_remaining_time = operations[i][nmachines-1].processing_time;
        for (int j = nmachines-2; j >= 0; --j)
            operations[i][j].job_remaining_time = operations[i][j].processing_time + operations[i][j+1].job_remaining_time;
    }

    // add arcs
    for (int i = 0; i < njobs; ++i)
    {
        // source node to first of each job
        pert_model->add_arc(0, operations[i][0].id);
        // dependency chain
        for (int j = 0; j < nmachines - 1; ++j)
            pert_model->add_arc(operations[i][j].id, operations[i][j+1].id);
    }

    // add edges
    for (int i = 0; i < nmachines; ++i)
        for (int j = 0; j < njobs; ++j)
            for (int k = j + 1 ; k < njobs; ++k)
                pert_model->add_edge(ids_by_machine[i][j], ids_by_machine[i][k]);

    // deallocation
    for (int i = 0; i < nmachines; ++i)
        delete [] ids_by_machine[i];

    delete [] ids_by_machine;
}

void JobShopScheduler::start_trails()
{
    int makespan = global_best->get_makespan();
    double n = noperations;
    tmax = 1.0/((1-p)*makespan);
    tmin = tmax*(1-pow(pbest, 1/n))/((njobs-1)*pow(pbest, 1/n));

    if (tmin > tmax) tmin = tmax;

    for (int i = 0; i < nnodes; ++i)
        for (int j = 0; j < nnodes; ++j)
            trail[i][j] = tmax;
}

void JobShopScheduler::create_ants()
{
    ants = new Schedule*[nants];
    perts = new DisjunctiveGraph*[nants];

    for (int i = 0; i < nants; ++i)
    {
        ants[i] = new Schedule(noperations);
        perts[i] = pert_model->clone();

        ants[i]->set_seed(generator());
    }
}

void JobShopScheduler::clear_paths()
{
    for (int i = 0; i < nants; ++i)
        ants[i]->clear();
}

void JobShopScheduler::update_trails()
{
    operation previous, current;
    Schedule* best;

    for (int i = 0; i < nnodes; ++i)
        for (int j = 0; j < nnodes; ++j)
            trail[i][j] = normalize_trail_value(trail[i][j] * p);

    best = ants[0];
    for (int i = 1; i < nants; ++i)
        if ( ants[i]->get_makespan() < best->get_makespan() )
            best = ants[i];

    previous.id = 0;
    for (int n = 0; n < noperations; ++n)
    {
        current = best->get_nth_operation(n);

        trail[previous.id][current.id] = normalize_trail_value(trail[previous.id][current.id] + Q/best->get_makespan());
        previous = current;
    }
}

void JobShopScheduler::destroy_ants()
{
    for (int i = 0; i < nants; ++i)
    {
        delete ants[i];
        delete perts[i];
    }

    delete [] ants;
    delete [] perts;
}

void JobShopScheduler::build_schedule_for(int ant_index)
{
    double total, rnd, limit;
    operation previous, current;
    int* next_operation;
    Schedule* ant = ants[ant_index];
    DisjunctiveGraph* pert = perts[ant_index];

    // next operation that should be scheduled of each job
    next_operation = new int[njobs];
    for(int i = 0; i < njobs; ++i)
        next_operation[i] = 0;
    // source node
    previous.id = 0;

    pert->reset_visited();

    for(int k = 0; k < noperations; ++k)
    {
        total = limit = 0;

        for(int i = 0; i < njobs; ++i)
        {
            if (next_operation[i] < nmachines) // not all operations of job i were scheduled
            {
                current = operations[i][next_operation[i]];
                total += get_priority(previous, current);
            }
        }

        rnd = ant->next_random(total);

        for(int i = 0; i < njobs; ++i)
        {
            if (next_operation[i] < nmachines) // not all operations of job i were scheduled
            {
                current = operations[i][next_operation[i]];
                limit += get_priority(previous, current);

                if  (rnd < limit)
                {
                    ant->add_operation(current);
                    pert->set_as_source(current.id);
                    previous = current;
                    ++next_operation[i];
                    break;
                }
            }
        }
    }

    ant->set_makespan(pert->len_of_longest_path());

    delete [] next_operation;
}

double JobShopScheduler::normalize_trail_value(double value)
{
    if (value > tmax)
        return tmax;
    else if (value < tmin)
        return tmin;

    return value;
}

double JobShopScheduler::get_priority(operation from, operation to)
{
    return pow(trail[from.id][to.id], alpha) * pow(to.job_remaining_time, beta);
}

/// builds a schedule using nearest neighbour method
Schedule* JobShopScheduler::build_schedule_nn()
{
    int local_best_job;
    operation local_best, current;
    int* next_operation;
    Schedule* schedule = new Schedule(noperations);
    DisjunctiveGraph* pert = pert_model->clone();

    // next operation that should be scheduled of each job
    next_operation = new int[njobs];
    for(int i = 0; i < njobs; ++i)
        next_operation[i] = 0;

    pert->reset_visited();

    for(int k = 0; k < noperations; ++k)
    {
        local_best.processing_time = INT_MAX;

        for(int i = 0; i < njobs; ++i)
        {
            if (next_operation[i] < nmachines) // not all operations of job i were scheduled
            {
                current = operations[i][next_operation[i]];
                if (current.processing_time < local_best.processing_time)
                {
                    local_best = current;
                    local_best_job = i;
                }
            }
        }

        schedule->add_operation(current);
        pert->set_as_source(local_best.id);
        ++next_operation[local_best_job];
    }

    schedule->set_makespan(pert->len_of_longest_path());

    delete [] next_operation;
    delete pert;

    return schedule;
}

void JobShopScheduler::set_niterations(int value)
{
    niterations = value;
}

void JobShopScheduler::set_nants(int value)
{
    nants = value;
}

void JobShopScheduler::set_alpha(double value)
{
    alpha = value;
}

void JobShopScheduler::set_beta(double value)
{
    beta = value;
}

void JobShopScheduler::set_p(double value)
{
    p = value;
}

void JobShopScheduler::set_pbest(double value)
{
    pbest = value;
}

void JobShopScheduler::set_seed(unsigned new_seed)
{
    seed = new_seed;
    generator.seed(seed);
}

Schedule* JobShopScheduler::schedule()
{
    int total_it;
    Schedule* it_best;
    Schedule* nn_schedule = build_schedule_nn();

    global_best = nn_schedule;
    start_trails();
    create_ants();

    for (int i = 0; i < niterations; ++i)
    {
//        total_it = 0;
        it_best = global_best;

        // generator is being shared, which is a problem
        #pragma omp parallel for
        for (int j = 0; j < nants; ++j)
        {
            build_schedule_for(j);
            total_it += ants[j]->get_makespan();

            if (ants[j]->get_makespan() < it_best->get_makespan())
            {
                it_best = ants[j];
                tmax = 1.0/((1-p) * ants[j]->get_makespan());
            }
        }

//        if (i % 3 == 0)
//            std::cout << total_it/nants << std::endl;

        if (it_best->get_makespan() < global_best->get_makespan())
        {
            delete global_best;
            // make a copy because the schedule will be erased for next iteration
            global_best = it_best->clone();
        }

        update_trails();
        clear_paths();
    }

    destroy_ants();

    return global_best;
}
