#ifndef LYZIAR_HPP
#define LYZIAR_HPP

#include <iostream>
#include <simlib.h>

extern Queue ticket_queue;
extern Facility ticket_counter;
extern Queue rental_queue;
extern Facility rental_counter;
extern Queue lift_queue;
extern Facility ski_lift;
extern Queue queue_lift;
extern Histogram hist_wait_lift;
extern Stat stat_wait_lift;
extern Stat stat_lift_queue_length;
extern Stat stat_wait_ticket;
extern const double jedna_cesta;

class Skier : public Process {
    private:
        bool hasOwnEquipment;
        double startTime;
        double durationOfStay;
        double wait_start_lift;


    public:
        Skier();

        void Behavior();
        void ActivateQueue(Queue &queue);
        void HandlerTicket();
        void HandleRental();
        void HandleLift();
        void HandleSlope();
};


class KotvaBezi : public Process {
public:
    KotvaBezi(int t);        // konštruktor
    void Behavior() override; // správanie procesu
private:
    int T; // T=1 - jedna cesta, T=2 - cesta tam a zpet
};
#endif