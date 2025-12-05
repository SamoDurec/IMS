#include "skier.hpp"
#include <iostream>
#include <iomanip>

extern long liftRideCount;

int Skier::nextId() {
    static int id = 0;
    return ++id;
}

Skier::Skier(
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
    int maxRides,
    double probabilityOwnEquipment
) :
    parking_(parking),
    ticketOffice_(ticketOffice),
    rentalShop_(rentalShop),
    liftQueue_(liftQueue),
    parkingStat_(parkingStat),
    ticketStat_(ticketStat),
    rentalStat_(rentalStat),
    liftWaitStat_(liftWaitStat),
    liftRideStat_(liftRideStat),
    slopeStat_(slopeStat),
    totalTimeStat_(totalTimeStat),
    parkingSampler_(parkingSampler),
    ticketSampler_(ticketSampler),
    rentalSampler_(rentalSampler),
    liftRideSampler_(liftRideSampler),
    slopeSampler_(slopeSampler),
    maxRides_(maxRides),
    probabilityOwnEquipment_(probabilityOwnEquipment),
    id_(nextId())
{
    arrivalTime_ = Time; // Uloženie času príchodu
}

void Skier::Behavior() {
    // 1. PARKOVANIE
    parking_->Enter(this, 1); 
    double parkingDuration = parkingSampler_();
    Wait(parkingDuration);
    if (parkingStat_) (*parkingStat_)(parkingDuration);
    parking_->Leave(1);
    
    // 2. NÁKUP LÍSTKU
    ticketOffice_->Enter(this, 1);
    double ticketDuration = ticketSampler_();
    Wait(ticketDuration);
    if (ticketStat_) (*ticketStat_)(ticketDuration);
    ticketOffice_->Leave(1);
    
    // 3. PRENÁJOM VÝSTROJE (ak treba)
    bool needsRental = (Uniform(0.0, 1.0) > probabilityOwnEquipment_);
    if (needsRental && rentalShop_) {
        rentalShop_->Enter(this, 1);
        double rentalDuration = rentalSampler_();
        Wait(rentalDuration);
        if (rentalStat_) (*rentalStat_)(rentalDuration);
        rentalShop_->Leave(1);
    }
    
    // 4. LYŽOVANIE
    int ridesCompleted = 0;
    while (ridesCompleted < maxRides_) {
        // Čakanie na vlek
        double waitStart = Time;
        liftQueue_->Enter(this, 1);
        double waitEnd = Time;
        double waitDuration = waitEnd - waitStart;
        if (liftWaitStat_) (*liftWaitStat_)(waitDuration);
        
        // Jazda na vleku
        double rideDuration = liftRideSampler_();
        Wait(rideDuration);
        if (liftRideStat_) (*liftRideStat_)(rideDuration);
        liftQueue_->Leave(1);
        liftRideCount++;
        
        // Zjazd
        double slopeDuration = slopeSampler_();
        Wait(slopeDuration);
        if (slopeStat_) (*slopeStat_)(slopeDuration);
        
        ridesCompleted++;
        
        // Pravdepodobnosť ukončenia
        if (Uniform(0.0, 1.0) < 0.15) break;
    }
    
    // 5. CELKOVÝ ČAS
    double totalTime = Time - arrivalTime_;
    if (totalTimeStat_) (*totalTimeStat_)(totalTime);
    
    // Debug výstup
    // std::cout << "Skier " << id_ << ": " << ridesCompleted 
    //           << " rides, total " << totalTime << " min" << std::endl;
}

std::string Skier::toString() const {
    std::ostringstream oss;
    oss << "Skier[id=" << id_ 
        << ", maxRides=" << maxRides_
        << ", ownEq=" << std::fixed << std::setprecision(2) 
        << probabilityOwnEquipment_ << "]";
    return oss.str();
}

std::string Skier::toCsv() const {
    std::ostringstream oss;
    oss << id_ << "," << maxRides_ << "," 
        << std::fixed << std::setprecision(3) << probabilityOwnEquipment_;
    return oss.str();
}