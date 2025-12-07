/**
 * IMS projekt 2025 - 04 - Model služeb v oblasti sport
 * Matej Menich (xmenicm00)
 * Samuel Durec (xdurecs00)
 * */

#include <simlib.h>
//#include "generator.h"
#include "lyziar.hpp"


class Generator : public Event {
public:
    Generator(int total) : N_total(total) { Activate(); }

    void Behavior() {
        if (generated >= N_total) {
            return;
        }
        

        double time = Time;
        double interval;
        int N1 = 0.55 * N_total; // počet lyžiarov prichádzajúcich v prvých 3 hodinach
        int N2 = 0.35 * N_total; // počet lyžiarov prichádzajúcich v nasledujúcich 3 hodinach
        int N3 = 0.09 * N_total; // počet lyžiarov prichádzajúcich v nasledujúcich 1 hodinach
        int N4 = 0.01 * N_total; // počet lyžiarov prichádzajúcich v poslednej hodine

        if(time < 180) interval = 180.0 / N1;
        else if(time < 360) interval = 180.0 / N2;
        else if(time < 420) interval = 60.0 / N3;
        else interval = 60.0 / N4;

        // vytvor lyžiara
        (new Skier())->Activate();
        generated++;

        // aktivuj ďalšieho lyžiara
        if (generated < N_total)
        Activate(Time + Exponential(interval));
    }

private:
    int N_total;
    int generated = 0;
};
