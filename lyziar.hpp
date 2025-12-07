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
        void HandleTicket();
        void HandleRental();
        void HandleLift();
        void HandleSlope();
};

#endif