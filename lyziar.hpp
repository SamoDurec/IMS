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

class Skier : public Process {
    private:
        bool hasOwnEquipment;
        double startTime;
        double durationOfStay;
        

    public:
        Skier();

        void Behavior();
        void ActivateQueue(Queue &queue);
        void HandlerTicket();
        void HandleRental();
        void HandleLift();
        void HandleSlope();
};

#endif