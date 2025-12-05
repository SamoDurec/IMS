#include <simlib.h>
#include "skier.hpp"
#include <iostream>

long liftRideCount = 0;
int main() {
    std::cout << "=== SKI RESORT SIMULATION ===" << std::endl;
    
    // Initialize simulation (0 to 480 minutes = 8 hours)
    Init(0, 480);
    
    // Create resources with capacities
    Store parking(150);       // 150 parking spots
    Store ticketOffice(5);    // 5 ticket counters
    Store rentalShop(3);      // 3 rental shops
    Store liftQueue(100);     // queue for ski lift
    
    // Create statistics collectors
    Stat totalTimeStat;
    Stat liftWaitStat;
    
    // Simulation parameters
    double currentTime = 0;
    int skierCount = 0;
    const int MAX_SKIERS = 100;
    const double SIMULATION_DURATION = 360; // 6 hours of arrivals
    
    // Generate skiers
    while (currentTime < SIMULATION_DURATION && skierCount < MAX_SKIERS) {
        // Create a new skier
        ( new Skier(
            &parking,
            &ticketOffice,
            &rentalShop,
            &liftQueue,
            nullptr,  // parking stat
            nullptr,  // ticket stat
            nullptr,  // rental stat
            &liftWaitStat,
            nullptr,  // lift ride stat
            nullptr,  // slope stat
            &totalTimeStat,
            []() { return Uniform(2, 8); },      // parking time: 2-8 min
            []() { return Exponential(3.0); },   // ticket purchase: exponential mean 3 min
            []() { return Uniform(5, 20); },     // rental time: 5-20 min
            []() { return Uniform(3, 7); },      // lift ride: 3-7 min
            []() { return Uniform(10, 30); },    // slope time: 10-30 min
            15,      // max rides per skier
            0.65     // 65% have own equipment
        ) )->Activate(currentTime);
        
        // Next arrival time (exponential distribution)
        currentTime += Exponential(2.5); // mean 2.5 minutes between arrivals
        skierCount++;
    }
    
    // Run the simulation
    Run();
    
    // Output results
    std::cout << "\n=== SIMULATION RESULTS ===" << std::endl;
    std::cout << "Total skiers generated: " << skierCount << std::endl;
    std::cout << "Final simulation time: " << Time << " minutes" << std::endl;
    

    std::cout << "Total time in resort:\n";
    totalTimeStat.Output();
    std::cout << "Lift waiting time:\n";
    liftWaitStat.Output();
    
    // Calculate and display some additional metrics
    std::cout << "\n=== ADDITIONAL METRICS ===" << std::endl;
    std::cout << "Average lifts per skier: " 
            << (skierCount > 0 ? (liftRideCount * 1.0 / skierCount) : 0) 
            << std::endl;
    
    // Resource utilization
    std::cout << "\n=== RESOURCE UTILIZATION ===" << std::endl;
    std::cout << "Parking utilization: " 
              << parking.Used() << "/" << parking.Capacity() 
              << " (" << (100.0 * parking.Used() / parking.Capacity()) << "%)" 
              << std::endl;
    std::cout << "Ticket office utilization: " 
              << ticketOffice.Used() << "/" << ticketOffice.Capacity() 
              << std::endl;
    std::cout << "Rental shop utilization: " 
              << rentalShop.Used() << "/" << rentalShop.Capacity() 
              << std::endl;
    std::cout << "Lift queue: " 
              << liftQueue.Used() << "/" << liftQueue.Capacity() 
              << std::endl;
    
    return 0;
}
