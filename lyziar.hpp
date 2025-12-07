#ifndef LYZIAR_HPP
#define LYZIAR_HPP

#include <iostream>
#include <simlib.h>

// Extern polia ukazovateľov
extern Queue** ticket_queues;
extern Facility** ticket_counters;
extern Stat** stat_wait_ticket;

extern Queue** rental_queues;
extern Facility** rental_counters;
extern Stat** stat_rental_queue_length;

extern Queue** lift_queues;
extern Facility** ski_lifts;
extern Stat** stat_lift_queue_length;
extern Stat** stat_wait_lift;

extern Stat stat_lyziari_v_systeme;
extern Stat stat_cas_na_svahu;
extern int lyziari_v_systeme;
extern Store equipment_store;
extern Store Kotvy;
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
        void ActivateQueue(Queue *queue);
        void HandleTicket();
        void HandleRental();
        void HandleLift();
        void HandleSlope();
};

class KotvaBezi : public Process {
public:
    KotvaBezi(int t);
    void Behavior() override;
private:
    int T;
};
#endif