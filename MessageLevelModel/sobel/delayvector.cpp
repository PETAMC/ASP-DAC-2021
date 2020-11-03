#include <delayvector.hpp>
#include <cstdlib>
#include <iostream>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics_double.h>
#include <algorithm>  


DelayVector::DelayVector(const char *path, DISTRIBUTION distribution)
    : delayindex(0)
    , rng(NULL)
    , distribution(distribution)
{
    this->ReadDelayVector(path);
    this->rng = gsl_rng_alloc(gsl_rng_default);
    // By default, the random number generator gets seeded with 0
    // This shall not be changed to get reproducible results
}

DelayVector::DelayVector(std::string path, DISTRIBUTION distribution)
    : delayindex(0)
    , rng(NULL)
    , distribution(distribution)
{
    this->ReadDelayVector(path.c_str());
    this->rng = gsl_rng_alloc(gsl_rng_default);
    // By default, the random number generator gets seeded with 0
    // This shall not be changed to get reproducible results
}



sc_core::sc_time DelayVector::GetDelay()
{
    double delay;

    switch(this->distribution)
    {
        case DISTRIBUTION::INJECTED:
            delay = this->GetInjectedDelay();
            break;

        case DISTRIBUTION::GAUSSIAN:
            delay = this->GetGaussianDelay();
            break;

        case DISTRIBUTION::UNIFORM:
            delay = this->GetUniformDelay();
            break;

        case DISTRIBUTION::WCET:
            delay = this->GetWCETDelay();
            break;
    }

    return sc_core::sc_time(delay, sc_core::SC_NS);
}



double DelayVector::GetInjectedDelay()
{
    double delay = this->delayvector[this->delayindex];
    this->delayindex = (this->delayindex + 1) % this->delayvector.size();

    return delay;
}



double DelayVector::GetGaussianDelay()
{
    auto delay = gsl_ran_gaussian(this->rng, this->sigma);
    delay += this->mu;

    return delay;
}



double DelayVector::GetUniformDelay()
{
    auto delay = gsl_ran_flat(this->rng, this->BCET, this->WCET);

    return delay;
}



double DelayVector::GetWCETDelay()
{
    return this->WCET;
}



void DelayVector::ReadDelayVector(const char *path)
{
    std::ifstream ifs;
    ifs.open(path);

    // Initialize min/max with opposite extremes
    // to approach to the actual values from the file
    this->WCET = 0.0;
    this->BCET = std::numeric_limits<decltype(this->BCET)>::max();

    for(std::string line; std::getline(ifs, line); )
    {
        double time;
        try
        {
            time = std::stod(line);
        }
        catch(std::exception &e)
        {
            std::cerr << "Parsing line \""
                      << line
                      << "\" failed."
                      << "This line will be ignored!\n";
        }

        this->delayvector.emplace_back(time);
        if(time > this->WCET)
            this->WCET = time;
        if(time < this->BCET)
            this->BCET = time;

    }

    // Shuffle delay vector to make sure it is not just a repetition of the measured data
    std::random_shuffle(this->delayvector.begin(), this->delayvector.end());

    // Get further properties from the measured data
    double *data = this->delayvector.data();
    size_t  n    = this->delayvector.size();
    this->sigma  = gsl_stats_sd(  data, 1, n);
    this->mu     = gsl_stats_mean(data, 1, n);

    ifs.close();
    return;
}


// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

