#ifndef DELAYVECTOR_HPP
#define DELAYVECTOR_HPP

#include <vector>
#include <string>
#include <systemc.h>
#include <cstdlib>
#include <gsl/gsl_rng.h>

enum DISTRIBUTION
{
    INJECTED,
    GAUSSIAN,
    UNIFORM,
    WCET
};

// Important:
// Internal, this class works with doubles for the delays.
// A delay's unit is clock cycle.
// The external interfaces should be of type sc_time

class DelayVector
{
    public:
        DelayVector(const char *path, DISTRIBUTION distribution = DISTRIBUTION::INJECTED);
        DelayVector(std::string path, DISTRIBUTION distribution = DISTRIBUTION::INJECTED);

        sc_core::sc_time GetDelay();

    private:
        double GetInjectedDelay();
        double GetGaussianDelay();
        double GetUniformDelay();
        double GetWCETDelay();
        void ReadDelayVector(const char *path);

        // This vector holds all measured delay
        std::vector<double> delayvector;

        // These variables are properties of the measured delay are
        double BCET;  // Lowest delay
        double WCET;  // Highest delay
        double sigma;
        double mu;

        // Some variables for accessing the delays
        unsigned int delayindex; // Next delay in delay vector
        gsl_rng *rng;   // Random number generator for Normal/Uniform distribution
        DISTRIBUTION distribution;
};

#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

