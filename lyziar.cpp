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

Stat stat_lyziari_v_systeme("Pocet lyziarov v systeme");
Stat stat_cas_na_svahu("Statistika casu jazdy na svahu");
Stat stat_rental_queue_length("Velkost radu na vypozicovni");
int lyziari_v_systeme = 0;

Store equipment_store("Sklad lyziarskeho vybavenia", 50); // Kapacita skladu 50 jednotiek vybavenia
Store Kotvy("Sklad kotiev", 85); // Kapacita skladu kotiev 85 jednotiek

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
        // Pokračovanie na lanovku
        HandleTicket();
    } else {
        // Pokračovanie do vypožičovne
        HandleRental();
    }  
    
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

    lyziari_v_systeme++;
    stat_lyziari_v_systeme(lyziari_v_systeme);
    
    HandleLift(); 
}

void Skier::HandleRental() {
    // Cakanie v rade na vypozicovanie vybavenia
    if( rental_counter.Busy() ) {
        rental_queue.Insert(this);
        stat_rental_queue_length(rental_queue.Length());
        this->Passivate();
    }

    // Obsluha vo vypozicovni
    Seize(rental_counter);

    // Počkaj, kým bude vybavenie dostupné (automaticky čaká, ak nie je)
    Enter(equipment_store);   // zoberie 1 jednotku
    Wait(Uniform(5, 15)); // simulácia vybavenia

    Release(rental_counter);

    // Aktivacia dalsieho lyziara v rade
    ActivateQueue(rental_queue);

    stat_rental_queue_length(rental_queue.Length());

    // Pokracovanie na lanovku
    HandleLift();
}


void Skier::HandleLift() {
    double startWait = Time;

    //std::cout << "Lyziar " << this << " prichadza na lanovku v case " << Time << std::endl;
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
    while (1){
        Enter(Kotvy, 1);
        Wait(0.25); // kratka doba na nastupenie
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
    
    // Kotva ide spat dole
    (new KotvaBezi(1))->Activate();

    // Aktivacia dalsieho lyziara v rade
    ActivateQueue(lift_queue);

    // Aktualizuj štatistiku veľkosti fronty po odchode
    stat_lift_queue_length(lift_queue.Length());

    // Pokracovanie na svah
    HandleSlope();
}


void Skier::HandleSlope() {
    double start = Time;
    // Výber svahu
    double lenghtOfSlope;
    double r = Random();
    if (r < 0.4) {
        lenghtOfSlope = 1000; // 40% šanca cervena zjazdovka
    } else if (r < 0.75) {
        lenghtOfSlope = 850;  // 35% šanca cervena zjazdovka
    } else {
        lenghtOfSlope = 700;  // 25% šanca cierna zjazdovka
    }

    double speed = Normal(3.31, 10.95); // ms-1
    if (speed < 0.5)
        speed = Uniform(0.5, 1.0); // minimalna rychlost
    double timeOnSlope = lenghtOfSlope / speed / 60.0; // premena na minuty
    Wait(timeOnSlope); // Simulacia jazdy na svahu
    double cas_jazdy = Time - start;
    stat_cas_na_svahu(cas_jazdy);

    if (Time - startTime >= durationOfStay) {
        if ( hasOwnEquipment == false ){
            Leave(equipment_store); // Vratenie jednotky vybavenia
        }
        lyziari_v_systeme--;
        stat_lyziari_v_systeme(lyziari_v_systeme);
        return;
    } else {
        if (Random() < 0.3) {
            Wait(Uniform(5.0, 10.0)); // pauza na oddych/jedlo
        }
        HandleLift();
    }
}



KotvaBezi::KotvaBezi(int t) : Process() {
    T = t;
    Activate(); // aktivuj proces hneď po vytvorení
}

void KotvaBezi::Behavior() {
    // jedna_cesta by mala byť definovaná globálne (čas jednej cesty v minútach)
    Wait(jedna_cesta * T);
    // po dokončení cesty vrátime kotvu do skladu (uvolníme 1 jednotku)
    Leave(Kotvy, 1);
}