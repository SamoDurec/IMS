#ifndef SKIER_HPP
#define SKIER_HPP

#include <simlib.h>
#include <functional>
#include <string>
#include <sstream>

class Skier : public Process {
public:
    using Sampler = std::function<double()>;
    
    Skier(
        Store *parking,
        Store *ticketOffice,
        Store *rentalShop,
        Store *liftQueue,
        Stat *parkingStat,
        Stat *ticketStat,
        Stat *rentalStat,
        Stat *liftWaitStat,
        Stat *liftRideStat,
        Stat *slopeStat,
        Stat *totalTimeStat,
        Sampler parkingSampler,
        Sampler ticketSampler,
        Sampler rentalSampler,
        Sampler liftRideSampler,
        Sampler slopeSampler,
        int maxRides = 10,
        double probabilityOwnEquipment = 0.6
    );
    
    void Behavior() override;
    
    std::string toString() const;
    std::string toCsv() const;
    
private:
    // Stores
    Store *parking_;
    Store *ticketOffice_;
    Store *rentalShop_;
    Store *liftQueue_;
    
    // Statistics
    Stat *parkingStat_;
    Stat *ticketStat_;
    Stat *rentalStat_;
    Stat *liftWaitStat_;
    Stat *liftRideStat_;
    Stat *slopeStat_;
    Stat *totalTimeStat_;
    
    // Samplers
    Sampler parkingSampler_;
    Sampler ticketSampler_;
    Sampler rentalSampler_;
    Sampler liftRideSampler_;
    Sampler slopeSampler_;
    
    // Parameters
    int maxRides_;
    double probabilityOwnEquipment_;
    int id_;
    double arrivalTime_;
    
    static int nextId();
};

#endif