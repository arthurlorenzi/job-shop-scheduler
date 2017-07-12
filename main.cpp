#include <iostream>

#include "JobShopScheduler.h"

static void print_usage()
{
    std::cout << "usage: JSSP [-h] [-n <nants>] [-i <niterations>] [-a <alpha>]"
        << "[-b <beta>] [-p <persistence>] [-pb <pbest>] [-s <seed>] instance" << std::endl
        << "  instance        instance file" << std::endl
        << "  -h              help" << std::endl
        << "  nants           number of ants" << std::endl
        << "  niterations     number of iterations" << std::endl
        << "  alpha           alpha coefficient" << std::endl
        << "  beta            beta coefficient" << std::endl
        << "  persistence     trail persistence rate" << std::endl
        << "  pbest           global best probability used in definition of tmin" << std::endl
        << "  seed            seed for PRNG" << std::endl;
}

int main(int argc, char* argv[])
{
    unsigned seed;
    int i = 1, nants, niterations;
    double alpha, beta, p, pbest;
    std::string instance, opt;
    std::vector<bool> read;

    // one position for each optional argument, except -h
    for (int i = 0; i < 7; ++i)
        read.push_back(false);

    while (i < argc)
    {
        opt = argv[i];

        if (opt == "-h")
        {
            print_usage();
            return 1;
        }
        else if (opt == "-n") {
            nants = std::stoi(argv[++i]);
            read[0] = true;
        } else if (opt == "-i") {
            niterations = std::stoi(argv[++i]);
            read[1] = true;
        } else if (opt == "-a") {
            alpha = std::stod(argv[++i]);
            read[2] = true;
        } else if (opt == "-b") {
            beta = std::stod(argv[++i]);
            read[3] = true;
        } else if (opt == "-p") {
            p = std::stod(argv[++i]);
            read[4] = true;
        } else if (opt == "-pb") {
            pbest = std::stod(argv[++i]);
            read[5] = true;
        } else if (opt == "-s") {
            seed = std::stoi(argv[++i]);
            read[6] = true;
        } else
            instance = argv[i];

        ++i;
    }

    JobShopScheduler* sched = new JobShopScheduler(instance);

    // set parameters
    if (read[0]) sched->set_nants(nants);
    if (read[1]) sched->set_niterations(niterations);
    if (read[2]) sched->set_alpha(alpha);
    if (read[3]) sched->set_beta(beta);
    if (read[4]) sched->set_p(p);
    if (read[5]) sched->set_pbest(pbest);
    if (read[6]) sched->set_seed(seed);
    // end parameters

    Schedule* solution = sched->schedule();
    std::cout <<  solution->get_makespan() << std::endl;

    delete sched;
    delete solution;

    return 0;
}
