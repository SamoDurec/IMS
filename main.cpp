/**
 * IMS projekt 2025 - 04 - Model služeb v oblasti sport
 * Matej Menich (xmenicm00)
 * Samuel Durec (xdurecs00)
 */

#include <simlib.h>
#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include "lyziar.hpp"
#include "generator.cpp"

#define SIM_START 0.0
#define SIM_END (8.0 * 60.0)

// default hodnoty
#define DEFAULT_SKIERS 500
#define DEFAULT_EQUIPMENT 50
#define DEFAULT_KOTVY 30
#define DEFAULT_SLOPE_LENGTH 1000.0

using namespace std;

// Pomocná funkcia pre vypísanie helpu
void PrintHelp() {
    cerr << "Pouzitie:\n"
         << "\t-s, --skiers <N>          celkovy pocet lyziarov (default 500)\n"
         << "\t-e, --equipment <N>       kapacita skladu vybavenia (default 50)\n"
         << "\t-k, --kotvy <N>           pocet kotiev (default 30)\n"
         << "\t-l, --length <meters>     dlzka svahu v metroch (default 1000)\n";
}

extern Store equipment_store;
extern Store Kotvy;
extern double GLOBAL_SLOPE_LENGTH;

// =========================================================
//                   MAIN
// =========================================================
int main(int argc, char *argv[]) {

    unsigned long skiers = DEFAULT_SKIERS;
    unsigned long equipment = DEFAULT_EQUIPMENT;
    unsigned long kotvy = DEFAULT_KOTVY;
    double slope_length = DEFAULT_SLOPE_LENGTH;

    int opt;
    char *err;

    const char *shortOpts = "s:e:k:l:";
    const struct option longOpts[] = {
        {"skiers", required_argument, nullptr, 's'},
        {"equipment", required_argument, nullptr, 'e'},
        {"kotvy", required_argument, nullptr, 'k'},
        {"length", required_argument, nullptr, 'l'},
        {nullptr, 0, nullptr, 0}
    };

    // spracovanie argumentov
    while ((opt = getopt_long(argc, argv, shortOpts, longOpts, nullptr)) != -1) {
        switch (opt) {
            case 's':
                skiers = strtoul(optarg, &err, 10);
                if (*err != '\0' || skiers <= 0) {
                    cerr << "Neplatny pocet lyziarov.\n";
                    return EXIT_FAILURE;
                }
                break;

            case 'e':
                equipment = strtoul(optarg, &err, 10);
                if (*err != '\0' || equipment <= 0) {
                    cerr << "Neplatna kapacita skladu vybavenia.\n";
                    return EXIT_FAILURE;
                }
                break;

            case 'k':
                kotvy = strtoul(optarg, &err, 10);
                if (*err != '\0' || kotvy <= 0) {
                    cerr << "Neplatny pocet kotiev.\n";
                    return EXIT_FAILURE;
                }
                break;

            case 'l':
                slope_length = strtod(optarg, &err);
                if (*err != '\0' || slope_length <= 50.0) {
                    cerr << "Dlzka svahu musi byt > 50m.\n";
                    return EXIT_FAILURE;
                }
                break;

            default:
                PrintHelp();
                return EXIT_FAILURE;
        }
    }

    cout << "\n============================================\n"
         << "        START SIMULACIE LYZIARSKEHO STREDISKA\n"
         << "============================================\n"
         << "Pocet lyziarov:            " << skiers << "\n"
         << "Kapacita skladu vybavenia: " << equipment << "\n"
         << "Pocet kotiev:              " << kotvy << "\n"
         << "Dlzka svahu:               " << slope_length << " m\n"
         << "--------------------------------------------\n\n";

    // prenos do globalnych premennych
    equipment_store.SetCapacity(equipment);
    Kotvy.SetCapacity(kotvy);
    GLOBAL_SLOPE_LENGTH = slope_length;
    //ski_lift.SetCapacity(kotvy);

    // 5 opakovaní pre štatistickú spoľahlivosť
    for (int i = 1; i <= 1; i++) {

        cout << ">>> SPUSTAM BEH #" << i << "\n";

        Init(SIM_START, SIM_END);

        // generátor lyžiarov
        (new Generator(skiers))->Activate();

        Run();

        cout << "--- KONIEC BEHU #" << i << " ---\n\n";
        SIMLIB_statistics.Output();
    }

    cout << "============================================\n";
    cout << "          SIMULACIA UKONČENA\n";
    cout << "============================================\n";

    return EXIT_SUCCESS;
}
