/**
 * IMS projekt 2025 - 04 - Model služeb v oblasti sport
 * Matej Menich (xmenicm00)
 * Samuel Durec (xdurecs00)
 *  */ 


#include "lyziar.hpp"

// Ostatné globálne objekty
Stat stat_lyziari_v_systeme("Pocet lyziarov v systeme");
Stat stat_cas_na_svahu("Statistika casu jazdy na svahu");
int lyziari_v_systeme = 0;
Store equipment_store("Sklad lyziarskeho vybavenia", 50);
Store Kotvy("Sklad kotiev", 85);
const double jedna_cesta = 780.0/3.5/60.0;

Skier::Skier() {
    Activate();
    hasOwnEquipment = (Random() < 0.7); 
    startTime = Time;
    wait_start_lift = 0.0;
    if(Random() < 0.2) {
        durationOfStay = Uniform(360.0, 480.0); 
    } else {
        durationOfStay = Normal(180.0, 90.0);
    }
    if (durationOfStay < 30.0) {
        durationOfStay = 30.0;
    }
}

void Skier::Behavior() {
    if (hasOwnEquipment) {
        HandleTicket();
    } else {
        HandleRental();
    }  
}

void Skier::ActivateQueue(Queue *queue) {
    if (queue->Empty()) return;
    Skier *skier = (Skier *)queue->GetFirst();
    skier->Activate();
}

void Skier::HandleTicket() {
    // Najdi index pokladne s najmensou radou
    extern int N_pokladni;
    int idx = 0;
    for (int i = 1; i < N_pokladni; ++i)
        if (ticket_queues[i]->Length() < ticket_queues[idx]->Length())
            idx = i;

    double startWait = Time;
    if( ticket_counters[idx]->Busy() ) {
        ticket_queues[idx]->Insert(this);
        this->Passivate();
    }

    double waiting = Time - startWait;
    stat_wait_ticket[idx]->operator()(waiting);

    Seize(*ticket_counters[idx]);
    Wait(Exponential(1.0));
    Release(*ticket_counters[idx]);

    ActivateQueue(ticket_queues[idx]);

    lyziari_v_systeme++;
    stat_lyziari_v_systeme(lyziari_v_systeme);

    HandleLift();
}

void Skier::HandleRental() {
    // Najdi index pozicovne s najmensou radou
    extern int N_pozicovni;
    int idx = 0;
    for (int i = 1; i < N_pozicovni; ++i)
        if (rental_queues[i]->Length() < rental_queues[idx]->Length())
            idx = i;

    if( rental_counters[idx]->Busy() ) {
        rental_queues[idx]->Insert(this);
        stat_rental_queue_length[idx]->operator()(rental_queues[idx]->Length());
        this->Passivate();
    }

    Seize(*rental_counters[idx]);
    Enter(equipment_store);
    Wait(Uniform(5, 15));
    Release(*rental_counters[idx]);

    ActivateQueue(rental_queues[idx]);
    stat_rental_queue_length[idx]->operator()(rental_queues[idx]->Length());

    HandleLift();
}

void Skier::HandleLift() {
    // Najdi index lanovky s najmensou radou
    extern int N_lanoviek;
    int idx = 0;
    for (int i = 1; i < N_lanoviek; ++i)
        if (lift_queues[i]->Length() < lift_queues[idx]->Length())
            idx = i;

    double startWait = Time;
    if( ski_lifts[idx]->Busy() ) {
        wait_start_lift = Time;
        lift_queues[idx]->Insert(this);
        stat_lift_queue_length[idx]->operator()(lift_queues[idx]->Length());
        this->Passivate();
    }

    double waiting = Time - startWait;
    stat_wait_lift[idx]->operator()(waiting);

    Seize(*ski_lifts[idx]);
    while (1){
        Enter(Kotvy, 1);
        Wait(0.25);
        if (Random()<=0.05) {
            (new KotvaBezi(2))->Activate();
        } else {
            break;
        }
    }
    Release(*ski_lifts[idx]);
    Wait(jedna_cesta);
    (new KotvaBezi(1))->Activate();

    ActivateQueue(lift_queues[idx]);
    stat_lift_queue_length[idx]->operator()(lift_queues[idx]->Length());

    HandleSlope();
}

void Skier::HandleSlope() {
    double start = Time;
    double lenghtOfSlope;
    double r = Random();
    if (r < 0.4) {
        lenghtOfSlope = 1000;
    } else if (r < 0.75) {
        lenghtOfSlope = 850;
    } else {
        lenghtOfSlope = 700;
    }

    double speed = Normal(3.31, 10.95);
    if (speed < 0.5)
        speed = Uniform(0.5, 1.0);
    double timeOnSlope = lenghtOfSlope / speed / 60.0;
    Wait(timeOnSlope);
    double cas_jazdy = Time - start;
    stat_cas_na_svahu(cas_jazdy);

    if (Time - startTime >= durationOfStay) {
        if ( hasOwnEquipment == false ){
            Leave(equipment_store);
        }
        lyziari_v_systeme--;
        stat_lyziari_v_systeme(lyziari_v_systeme);
        return;
    } else {
        if (Random() < 0.3) {
            Wait(Uniform(5.0, 10.0));
        }
        HandleLift();
    }
}

KotvaBezi::KotvaBezi(int t) : Process() {
    T = t;
    Activate();
}

void KotvaBezi::Behavior() {
    Wait(jedna_cesta * T);
    Leave(Kotvy, 1);
}