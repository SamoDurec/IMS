#include <iostream>
#include <cstdlib>
#include <cstring>
#include <simlib.h>
#include "lyziar.hpp"
#include "generator.cpp"

using namespace std;

// Globálne počty
int N_total = 0;
int N_pokladni = 1;
int N_lanoviek = 1;
int N_pozicovni = 1;

// Pole ukazovateľov na objekty
Queue** ticket_queues = nullptr;
Facility** ticket_counters = nullptr;
Stat** stat_wait_ticket = nullptr;

Queue** rental_queues = nullptr;
Facility** rental_counters = nullptr;
Stat** stat_rental_queue_length = nullptr;

Queue** lift_queues = nullptr;
Facility** ski_lifts = nullptr;
Stat** stat_lift_queue_length = nullptr;
Stat** stat_wait_lift = nullptr;

// Pomocná funkcia na parsovanie parametrov
bool parse_args(int argc, char **argv) {
    bool l_found = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-l") == 0 && i+1 < argc) {
            N_total = atoi(argv[++i]);
            l_found = true;
        } else if (strcmp(argv[i], "-p") == 0 && i+1 < argc) {
            N_pokladni = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-v") == 0 && i+1 < argc) {
            N_lanoviek = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-r") == 0 && i+1 < argc) {
            N_pozicovni = atoi(argv[++i]);
        } else {
            cout << "Neznamy alebo nekompletny parameter: " << argv[i] << endl;
            return false;
        }
    }
    if (!l_found || N_total <= 0 || N_pokladni <= 0 || N_lanoviek <= 0 || N_pozicovni <= 0) {
        cout << "Vsetky parametre musia byt kladne cele cisla a -l je povinny." << endl;
        return false;
    }
    return true;
}

int main(int argc, char **argv) {
    if (!parse_args(argc, argv)) {
        cout << "Pouzitie: " << argv[0]
             << " -l <pocet_lyziarov> [-p <pocet_pokladni>] [-v <pocet_lanoviek>] [-r <pocet_pozicovni>]" << endl;
        cout << "  -l <pocet_lyziarov>   povinny parameter (napr. 200)" << endl;
        cout << "  -p <pocet_pokladni>   volitelne, default 1" << endl;
        cout << "  -v <pocet_lanoviek>   volitelne, default 1" << endl;
        cout << "  -r <pocet_pozicovni>  volitelne, default 1" << endl;
        return 1;
    }

    // Inicializácia polí ukazovateľov
    ticket_queues = new Queue*[N_pokladni];
    ticket_counters = new Facility*[N_pokladni];
    stat_wait_ticket = new Stat*[N_pokladni];
    for (int i = 0; i < N_pokladni; ++i) {
        std::string qname = "Rada na pokladnu " + std::to_string(i+1);
        std::string fname = "Pokladna " + std::to_string(i+1);
        std::string sname = "Statistika cakania na pokladnu " + std::to_string(i+1);
        ticket_queues[i] = new Queue(qname.c_str());
        ticket_counters[i] = new Facility(fname.c_str());
        stat_wait_ticket[i] = new Stat(sname.c_str());
    }

    rental_queues = new Queue*[N_pozicovni];
    rental_counters = new Facility*[N_pozicovni];
    stat_rental_queue_length = new Stat*[N_pozicovni];
    for (int i = 0; i < N_pozicovni; ++i) {
        std::string qname = "Rada na vypozicovnu " + std::to_string(i+1);
        std::string fname = "Vypozicovna " + std::to_string(i+1);
        std::string sname = "Statistika radu na vypozicovnu " + std::to_string(i+1);
        rental_queues[i] = new Queue(qname.c_str());
        rental_counters[i] = new Facility(fname.c_str());
        stat_rental_queue_length[i] = new Stat(sname.c_str());
    }

    lift_queues = new Queue*[N_lanoviek];
    ski_lifts = new Facility*[N_lanoviek];
    stat_lift_queue_length = new Stat*[N_lanoviek];
    stat_wait_lift = new Stat*[N_lanoviek];
    for (int i = 0; i < N_lanoviek; ++i) {
        std::string qname = "Rada na lanovku " + std::to_string(i+1);
        std::string fname = "Lanovka " + std::to_string(i+1);
        std::string sname1 = "Statistika radu na lanovku " + std::to_string(i+1);
        std::string sname2 = "Statistika cakania na lanovku " + std::to_string(i+1);
        lift_queues[i] = new Queue(qname.c_str());
        ski_lifts[i] = new Facility(fname.c_str());
        stat_lift_queue_length[i] = new Stat(sname1.c_str());
        stat_wait_lift[i] = new Stat(sname2.c_str());
    }

    RandomSeed(time(0)); 
    Init(0.0, 480.0);

    Generator *gen = new Generator(N_total);

    Run();

    // Výpis štatistík
    for (int i = 0; i < N_pokladni; ++i) {
        stat_wait_ticket[i]->Output();
        ticket_counters[i]->Output();
    }
    for (int i = 0; i < N_pozicovni; ++i) {
        stat_rental_queue_length[i]->Output();
        rental_counters[i]->Output();
    }
    for (int i = 0; i < N_lanoviek; ++i) {
        stat_lift_queue_length[i]->Output();
        stat_wait_lift[i]->Output();
        ski_lifts[i]->Output();
    }

    return 0;
}