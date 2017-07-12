#ifndef JOBSHOPSCHEDULER_H
#define JOBSHOPSCHEDULER_H

#include <random>
#include <string>
#include <utility>

#include "Schedule.h"
#include "DisjunctiveGraph.h"

class JobShopScheduler
{
    public:
        JobShopScheduler(std::string instance);
        ~JobShopScheduler();

        void set_niterations(int value);
        void set_nants(int value);
        void set_alpha(double value);
        void set_beta(double value);
        void set_p(double value);
        void set_pbest(double value);
        void set_seed(unsigned new_seed);
        Schedule* schedule();
    private:
        int njobs, nmachines, noperations, nnodes;
         // AS parameters
        int niterations, nants;
        double Q, alpha, beta, p, tmin, tmax, pbest;
        // AS parameters
        unsigned seed;
        Schedule* global_best;
        double** trail;
        operation** operations;
        Schedule** ants;
        DisjunctiveGraph* pert_model;
        DisjunctiveGraph** perts;
        std::mt19937 generator;

        void load_instance(std::string file);
        void start_trails();
        void create_ants();
        void clear_paths();
        void update_trails();
        void destroy_ants();
        void build_schedule_for(int ant_index);
        double normalize_trail_value(double value);
        double get_priority(operation from, operation to);
        Schedule* build_schedule_nn();
};

#endif // JOBSHOPSCHEDULER_H
