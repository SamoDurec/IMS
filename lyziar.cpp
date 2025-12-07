/**
 * IMS projekt 2025 - 04 - Model služeb v oblasti sport
 * Matej Menich (xmenicm00)
 * Samuel Durec (xdurecs00)
 *  */ 


#include "lyziar.hpp" 

Queue ticket_queue("Rada na pokladnu");
Facility ticket_counter("Pokladna");
Stat stat_wait_ticket("Statistika cakania na pokladnu");

Queue rental_queue("Rada na vypozicanie vybavenia");
Facility rental_counter("Vypozicovna");

Queue lift_queue("Rada na lanovku");
Histogram hist_wait_lift("Cakanie na lanovku", 0.0, 1.0, 100);
Stat stat_wait_lift("Statistika cakania na lanovku");
Stat stat_lift_queue_length("Velkost radu na lanovku");
Facility ski_lift("Lanovka");

Store equipment_store("Sklad lyziarskeho vybavenia", 50); // Kapacita skladu 50 jednotiek vybavenia TODO
Store Kotvy("Sklad kotiev", 100); // TODO

const double jedna_cesta = 780.0/3.5/60.0; 

Skier::Skier() {
    Activate();

    hasOwnEquipment = (Random() < 0.7); // TODO
    startTime = Time;
    wait_start_lift = 0.0;
    durationOfStay = Normal(240.0, 90.0);
    if (durationOfStay < 30.0) {
        durationOfStay = 30.0;
    }

}

void Skier::Behavior() {
    
    // Prichod k pokladni
    //HandleTicket();
    HandleTicket();
    
}

void Skier::ActivateQueue(Queue &queue) {
    if (queue.Empty()) {
        return;
    }

    Skier *skier = (Skier *)queue.GetFirst();
    skier->Activate();
}

//void Skier::HandleTicket() {
void Skier::HandleTicket() {
    double startWait = Time;
    // Cakanie v rade na pokladnu
    if( ticket_counter.Busy() ) {
        ticket_queue.Insert(this);
        this->Passivate();
    }

    // Záznam čakania do histogramu a štatistiky
    double waiting = Time - startWait;
    stat_wait_ticket(waiting);
     
    // Obsluha na pokladni
    Seize(ticket_counter);
    Wait(Exponential(1.0)); // Simulacia nakupu listka
    Release(ticket_counter);


    // Aktivacia dalsieho lyziara v rade
    ActivateQueue(ticket_queue);
    
    if (hasOwnEquipment) {
        // Pokračovanie na lanovku
        HandleLift();
    } else {
        // Pokračovanie do vypožičovne
        HandleRental();
    }   
}

void Skier::HandleRental() {
    // Cakanie v rade na vypozicovanie vybavenia
    if( rental_counter.Busy() ) {
        rental_queue.Insert(this);
        this->Passivate();
    }
     
    // Obsluha vo vypozicovni
    Seize(rental_counter);
    // Skontrolovať sklad
    if(equipment_store.Capacity() > 0) {
        Enter(equipment_store);   // zoberie 1 jednotku
        Wait(Uniform(5, 15)); // simulácia vybavenia
    } else {
        // sklad je prázdny, lyžiar čaká
        Wait(Uniform(1, 3)); // krátke čakanie a pokus znova TODO mozno dat sancu ze bude cakat pokial nepride
        Release(rental_counter);
        Activate(); // opätovná aktivácia lyžiara
        return;
    }
    Release(rental_counter);

    // Aktivacia dalsieho lyziara v rade
    ActivateQueue(rental_queue);

    // Pokracovanie na lanovku
    HandleLift();
}


void Skier::HandleLift() {
    double startWait = Time;

    // Cakanie v rade na lanovku
    if( ski_lift.Busy() ) {
        wait_start_lift = Time;
        lift_queue.Insert(this);
        stat_lift_queue_length(lift_queue.Length());
        this->Passivate();
    }


    // Doba cakania
    double waiting = Time - startWait;
    hist_wait_lift(waiting);
    stat_wait_lift(waiting);
     
    // Jazda na lanovke
    Seize(ski_lift);
    Wait(Uniform(3, 7)); // Simulacia jazdy na lanovke TODO + lanovka sa vracia dole
    while (1){
        Enter(Kotvy, 1);
        if (Random()<=0.05) {
        // nezdareny start
            (new KotvaBezi(2))->Activate();
        
        } else {
            break;
        }
    }
    
    Release(ski_lift);

    Wait(jedna_cesta);
    //dobaCesty(Time-time);
    (new KotvaBezi(1))->Activate();

    // Aktivacia dalsieho lyziara v rade
    ActivateQueue(lift_queue);

    // Aktualizuj štatistiku veľkosti fronty po odchode
    stat_lift_queue_length(lift_queue.Length());

    // Pokracovanie na svah
    HandleSlope();
}


void Skier::HandleSlope() {
    // Jazda na svahu
    double speed = Normal(3.31, 10.95); // ms-1
    if (speed < 0.5)
        speed = 0.5; // minimalna rychlost
    double lenghtOfSlope = 1000; // m TODO mozno dat ako vstup parameter
    double timeOnSlope = lenghtOfSlope / speed;
    Wait(timeOnSlope/60); // Simulacia jazdy na svahu

    if (Time - startTime >= durationOfStay) {
        if ( hasOwnEquipment == false ){
            Leave(equipment_store); // Vratenie jednotky vybavenia
        }
        return;
    } else {
        HandleLift();
    }


    // Rozhodnutie ci pauza TODO

}



KotvaBezi::KotvaBezi(int t) : Process() {
    T = t;
    Activate(); // aktivuj proces hneď po vytvorení (rovnako ako Skier)
}

void KotvaBezi::Behavior() {
    // jedna_cesta by mala byť definovaná globálne (čas jednej cesty v minútach)
    Wait(jedna_cesta * T);
    // po dokončení cesty vrátime kotvu do skladu (uvolníme 1 jednotku)
    Leave(Kotvy, 1);
    // process končí automaticky pri návrate z Behavior()
}