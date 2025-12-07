#include <iostream>
#include <cstdlib>
#include <simlib.h>
#include "lyziar.hpp"
#include "generator.cpp"

using namespace std;

int main(int argc, char **argv) {
    // Default parametre (možno zmeniť cez argv)
    int N_total = 200;          // počet lyžiarov ktoré sa majú vygenerovať
    double sim_end = 480.0;     // dĺžka simulácie v minútach
    double slope_len = 1000.0;  // dĺžka svahu v metroch

    if (argc >= 2) N_total = atoi(argv[1]);
    if (argc >= 3) sim_end = atof(argv[2]);
    if (argc >= 4) slope_len = atof(argv[3]);

   
    RandomSeed(time(0)); 
    // inicializácia simulačného času
    Init(0.0, sim_end);

    // vytvor generator, ktorý bude generovať N_total lyžiarov
    Generator *gen = new Generator(N_total);

    // spusti simuláciu
    Run();

    // vypíš relevantné výsledky / štatistiky
    hist_wait_lift.Output();
    stat_wait_ticket.Output();
    stat_wait_lift.Output();
    stat_lift_queue_length.Output();

    ticket_counter.Output();
    rental_counter.Output();
    ski_lift.Output();
    stat_lyziari_v_systeme.Output();
    stat_cas_na_svahu.Output();
    stat_rental_queue_length.Output();
    //equipment_store.Output();

    return 0;
}