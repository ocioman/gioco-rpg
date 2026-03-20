#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "vector.h"

// costanti varie
#define MAX_STANZE 10
#define MAX_ORCHI 3

// STRUCT VARIE
/**
 * @brief Abbiamo implementato per la gestione del protagonista una struttura (struct) "Player".
 * In questa struct troviamo:
 * Stats -> come gli HP (punti vita) massimi, quelli correnti e le monete.
 * Delle Flag -> per l'inventario e per capire se la struct player � stata istanziata o meno.
 */
struct player{
    int max_hp; //health points (max 20)
    int current_hp;
    int monete;
    int num_oggetti; // Numero di oggetti (per salvataggio , se lo implementiamo)
    int num_pozioni;
    int missioni_completate; // Missioni completate (per salvataggio , se lo implementiamo)
    bool has_sword;
    bool has_armor;
    bool has_hero_sword;
    bool has_key;
    bool is_initialized; // per i controlli -> ovviamente non instanziamo 2 volte il player a caso xD
};

/**
 * @brief stanza_livello (stanza) -> Invece quest'ultima e l'implementazione effettiva
 * in game, ha dei parametri in pi� come "visitata" e "obiettivo_missione" per tracciare i
 * progressi del giocatore in game.
 */
typedef struct stanza_livello { // Struttura della stanza nel dungeon (istanza)
    const char *nome; // Esempio: "Generale Orco" o "Acquitrino Velenoso"
    const char *tipologia; // Esempio: "Combattimento" o "Trappola"
    int colpo_fatale;
    int danno;
    int moneta;
    bool obiettivo_missione; // true se questa stanza � necessaria per la missione
    bool visitata; // Potrebbe essere utile per la logica di esplorazione
} stanza;

/**
 * @brief stanza_base -> Un array statico che funge come "database" per i modelli predefiniti
 */
struct stanza_base{ // Stanza/Nemico/Trappola base
    const char *nome; // il nome -> puntatore perch� e una stringa
    const char *tipologia; // e una trappola o meno -> puntatore perch� e una stringa
    int colpo_fatale;
    int danno;
    int moneta;
};

struct negozio{
    int costo_pozione;
    int costo_spada;
    int costo_armatura;
};

// array di struct per avere delle stanze con mostri o trappole gi� implementate
struct stanza_base stanza_palude[] = {
        {"", "", 0, 0, 0}, // indice: 0 (Non usato)
        {"Cane Selvaggio", "Combattimento", 2, 1, 1}, // 1
        {"Goblin", "Combattimento", 3, 2, 2}, // 2
        {"Scheletro", "Combattimento", 4, 2, 4}, // 3
        {"Orco", "Combattimento", 3, 4, 6}, // 4
        {"Acquitrino Velenoso", "Trappola", 0, 0, 0}, // 5
        {"Generale Orco", "Combattimento", 6, 3, 12}, // 6
};

struct stanza_base stanza_magione[] = {
        {"", "", 0, 0, 0}, // indice: 0 (Non usato)
        {"Botola Buia", "Trappola", 0, 3, 0}, // 1
        {"Pipistrello", "Combattimento", 2, 2, 1}, // 2
        {"Zombie", "Combattimento", 3, 2, 2}, // 3
        {"Fantasma", "Combattimento", 5, 2, 4}, // 4
        {"Demone Custode", "Combattimento", 4, 6, 10}, // 5
        {"Vampiro Superiore", "Combattimento", 4, 4, 7}, // 6
};

struct stanza_base stanza_grotta_c[] = {
        {"", "", 0, 0, 0}, // indice: 0 (Non usato)
        {"Stanza Vuota", "Vuota", 0, 0, 0}, // 1
        {"Cristalli Cadenti", "Trappola", 0, 2, 0}, // 2
        {"Ponte Pericolante", "Trappola", 0, 0, -3}, // 3
        {"Forziere Misterioso", "Trappola", 0, 2, 10}, // 4
        {"Rupe scoscesa", "Trappola", 0, 1, 0}, // 5
        {"Drago Antico", "Combattimento", 5, 10, 12}, // 6
};

//GAME LOOP
void gameLoop();
//VAR GLOBALI
int orchi_sconfitti = 0;

// FUNZIONI DEI MENU'
int StampaOpzioniMenu();
int StampaMenuIniziale();
int StampaMenuViaggi();
int rollareDadi(bool is_dice);
int padovan(int n);
bool is_in_padovan(int num , int n);
void clearScreen();

void MenuPartita();
void MenuViaggi();
void instanziamento_player();

//MODIFICATE
void generaStanze(stanza stanze_livello[], int *num_stanze, int missione_selezionata);
void gen_stanze_palude(stanza stanze_livello[], int *num_stanze);
void gen_stanze_magione(stanza stanze_livello[], int *num_stanze);
void gen_stanze_grotta(stanza stanze_livello[], int *num_stanze);


void Negozio();
bool saveToFile();
bool loadFromFile(size_t nSave);
void loadSavesVector();
bool deleteSave(size_t nSave);
bool deleteFromIndex(fileFormat delete);
void renameFiles(size_t nSave);

//NUOVE-COMBATTIMENTO
void stampa_opzioni_bossFinale();
void combattimento_drago(stanza stanza_da_esplorare);
void boss_finale();
void esplora_stanze(stanza stanze_livello[],int num_stanze);
void infliggi_danno(int danno);
void gestisci_combattimento(stanza stanza_da_esplorare);
void assegna_ricompense(int monete);
int menuMissione(stanza stanzaN,int num_stanza);
int opzioniMissione(int num_stanza);
void Inventario();
int calcola_danno();
void aspetta_invio();
int controlla_risultato(int giocatore,int nemico);

//NUOVO-KONAMI
void checkKonami(char array[], int arraySize, bool isKonami);
void Konami(); //pre esistente, AGGIORNATO

// VARIABILI GLOBALI
struct player Player; // Player
struct negozio Negozio_obj;
bool gameLoopFlag = true;
vector* saves=NULL;  //Vector globale per i salvataggi

bool konamiActivate = false; //Variabile iniziale per attivare l'opzione dei trucchi, inserendo da tastiera:  w w s s a d a d b a [Spazio], la variabile switcha a true e si abilita nel menu l'opzione dei trucchi
char codiceKonami[] = {'w', 'w', 's', 's', 'a', 'd', 'a', 'd', 'b', 'a', ' '};


int main(){
    saves=vCreate();


    //setto il random
    srand(time(NULL));

    //carico i salvataggi esistenti all'avvio
    loadSavesVector();
    gameLoop();

    //libero saves
    vFree(saves);
    return 0;
}

void gameLoop(){
    int i = 0;
    size_t nSave;
    i = StampaMenuIniziale();
    bool loaded;
    bool deleted;
    int opzSave;
    char confirmDeletion[2];

    switch(i){
        case 1:
            clearScreen();
            instanziamento_player();
            MenuPartita();
            break;
        case 2:
            if(saves->size!=0){
                printf("\nCarica salvataggio: \n");
                vPrint(saves);
                printf("\nSeleziona un salvataggio [1-%zu]:\t\t", saves->size);

                scanf("%zu", &nSave);

                if(nSave-1>saves->size){
                    printf("\nNumero di salvataggio invalido");
                }else{
                    printf("\nSeleziona un'opzione per il salvataggio %zu:", nSave);
                    printf("\n1. Carica");
                    printf("\n2. Elimina");
                    printf("\nSeleziona un'opzione [1-2]:\t\t");

                    scanf("%d", &opzSave);
                    getchar();

                    switch(opzSave){
                        case 1:
                            loaded=loadFromFile(nSave);

                            if(loaded){
                                printf("\nSalvataggio caricato correttamente");
                                MenuPartita();
                            }
                            break;
                        case 2:
                            printf("\nSei sicuro di voler eliminare il salvataggio %zu? [Si/No]\t\t", nSave);
                            scanf("%[^\n]", confirmDeletion);

                            if(strcmp(confirmDeletion, "Si")==0){
                                deleted= deleteSave(nSave);
                                if(deleted){
                                    printf("\nSalvataggio eliminato con successo");
                                }else{
                                    printf("\nErrore: errore nell'eliminazione del salvataggio");
                                }
                            }else if(strcmp(confirmDeletion, "No")!=0){
                                printf("\nOpzione invalida");
                            }
                            break;
                        default:
                            printf("\nOpzione invalida");
                    }
                }
            }else{
                printf("\nNessun salvataggio presente");
            }
            break;
        case 3:
            gameLoopFlag = false;
            break;
        case 4:
            Konami();
            break;
        default:
            printf("\nOpzione non valida.");
            break;
    }
    if(gameLoopFlag)
        gameLoop();
}


int StampaOpzioniMenu(){
    int i = 1;
    printf("Menu del Villaggio : \n");
    do{
        printf("\n 1 Intraprendi una missione");
        printf("\n 2 Riposati");
        printf("\n 3 Inventario");
        printf("\n 4 Salva la partita");
        printf("\n 5 Esci\n\n>");
        printf("Seleziona una delle opzioni del menu [1-5]:\t");
        scanf("%d",&i);
        getchar();
    }while(i>5 || i<1);
    return i;
}


void Konami(){  //aggiornato
    if(!Player.is_initialized){
        instanziamento_player();
    }

    size_t nSave;
    int scelta;
    int nuovoValore;

    printf("\n--- MENU TRUCCHI (KONAMI ACTIVATED) ---\n");

    if(saves->size != 0){
        printf("Salvataggi disponibili:\n");
        vPrint(saves);
        printf("\nSeleziona il numero del salvataggio da caricare e modificare [1-%zu]: ", saves->size);
        scanf("%zu", &nSave);

        if(!loadFromFile(nSave)){
            printf("Errore nel caricamento del salvataggio per i trucchi.\n");
            return;
        }
    } else {
        printf("Nessun salvataggio trovato. Modifiche applicate alla sessione corrente.\n");
    }

    bool exitTrucchi = false;
    do {
        printf("\nModifica parametri per %s:\n", Player.is_initialized ? "Giocatore Attivo" : "Salvataggio");
        printf("1. Imposta Punti Vita (Attuali: %d)\n", Player.current_hp);
        printf("2. Imposta Monete (Attuali: %d)\n", Player.monete);
        printf("3. Sblocca Missione Finale (Castello del Signore Oscuro)\n");
        printf("0. Salva modifiche ed Esci\n");
        printf("Scelta: ");
        scanf("%d", &scelta);

        switch(scelta) {
            case 1:
                printf("Inserisci nuovi HP: ");
                scanf("%d", &nuovoValore);
                Player.current_hp = nuovoValore;
                if(nuovoValore > Player.max_hp) Player.max_hp = nuovoValore;
                printf("HP modificati!\n");
                break;
            case 2:
                printf("Inserisci nuovo numero monete: ");
                scanf("%d", &nuovoValore);
                Player.monete = nuovoValore;
                printf("Monete modificate!\n");
                break;
            case 3:
                Player.has_key = true;
                printf("Missione finale sbloccata!\n");
                break;
            case 0:
                exitTrucchi = true;
                saveToFile();
                break;
            default:
                printf("Opzione non valida.\n");
        }
    } while(!exitTrucchi);

    printf("\nTrucchi applicati con successo!\n");
}

int StampaMenuIniziale(){  // MANCA IMPLEMENTAZIONE DEL CODICE : WWDDAABA per fare vedere il punto 3. <-- FATTO
    int i;
    int size = 3;
    char c;
    printf("\nMenu Principale : \n");
    do{
        printf("\n 1. Nuova partita");
        printf("\n 2. Carica salvataggio");
        printf("\n 3. Esci dal gioco");
        if(konamiActivate == true){
            size = 4;
            printf("\n 4. Trucchi \n\n>");
        }
        
        printf("\nSeleziona una delle opzioni del menu [1-3]:\t");
        if(!konamiActivate){
            scanf(" %c",&c);
            i = c-'0';
        }else{
            scanf(" %d", &i);
        }
        if(c == 'w' && !konamiActivate){
            checkKonami(codiceKonami, sizeof(codiceKonami), true);
        }
        //printf("\n %d", i); debug
    }while(i > size || i < 1);
    return i;
}

//Funzione generica per controllare combinazioni di caratteri, per ora utilizzata solo per il codice Konami
void checkKonami(char array[], int arraySize, bool isKonami){
    bool flag = true;
    int j = 1;
    char c;
    while(flag && j < arraySize){ //Continua solo se non hai sbagliato lettera o devi inserire altre lettere
        printf("\nInserisci lettera: %d ->", j+1);
        if(j == arraySize-1 && isKonami){ // per il codice Konami serve questo if extra per catturare lo spazio finale
            scanf("%c", &c); //legge invio
            scanf("%c", &c); //legge spazio
        }else{
            scanf(" %c", &c); //per controllare le altre lettere fino alla penultima
        }
        if(c != array[j]){
            flag = false;
        }else{
            j++;
        }

        //printf("\n %d e %d", j, arraySize-1 ); debug
        //printf("\n %c e %c", c, array[j-1]); debug
    }
    if(flag){
        konamiActivate = true;
    }else{
        printf("Codice errato \n\n\n");
    }
}

int StampaMenuViaggi(){
    int i = 1;
    int max_missioni = 3;

    if(!Player.is_initialized){
        instanziamento_player();
    }

    if(Player.has_key){
        max_missioni = 4;
    }

    printf("Menu di selezione Missione : \n");
    do{
        printf("\n 1 Palude Putrescente");
        printf("\n 2 Magione Infestata");
        printf("\n 3 Grotta di Cristallo");
        if(Player.has_key){
            printf("\n 4 Castello del Signore Oscuro");
        }
        printf("\n\n>Seleziona una delle opzioni del menu [1-%d]:\t",max_missioni);
        scanf("%d",&i);
    }while(i > max_missioni || i < 1);
    return i;
}

void MenuPartita(){
    int i;
    bool saved;

    do{
        i=StampaOpzioniMenu();
        switch(i){
            case 1:
                clearScreen();
                printf(">il giocatore avvia una missione\n");
                aspetta_invio();
                MenuViaggi();
                break;
            case 2:
                printf(">il giocatore si riposa\n");
                Player.current_hp = Player.max_hp;
                break;
            case 3:
                printf(">il giocatore controlla l'inventario'");
                Inventario();
                break;
            case 4:
                saved = saveToFile();
                if(!saved){
                    printf("\n>Errore nel salvataggio del file\n");
                } else {
                    printf("\n>Partita salvata con successo!\n");
                }
                break;
            case 5:
                StampaMenuIniziale();
                break;
            default:
                printf("come sei finito qui?");
                break;
        }
    }while(i!=1&&i!=5);
}

void instanziamento_player(){
    Player.current_hp = 20; //modificato temporaneamente (i cambiamaneti sono stati annullati)
    Player.max_hp = 20;
    Player.monete = 90;
    Player.num_oggetti = 0;
    Player.num_pozioni = 0;
    Player.missioni_completate = 0;
    Player.has_sword = false;
    Player.has_armor = false;
    Player.has_key = false;
    Player.is_initialized = true;
}

void MenuViaggi(){
    int i;
    stanza stanze_livello_dungeon[MAX_STANZE];
    int num_stanze_effettive = 0;
    i=StampaMenuViaggi();

    switch(i){
        case 1:
            printf("\n**Sei entrato nella Palude Putrescente!**\n");
            generaStanze(stanze_livello_dungeon, &num_stanze_effettive, i);

            printf("\n hello1 %d \n",num_stanze_effettive);
            //aspetta_invio();
            esplora_stanze(stanze_livello_dungeon,num_stanze_effettive);
            break;
        case 2:
            printf("\n**Sei entrato nella Magione!**\n");
            generaStanze(stanze_livello_dungeon, &num_stanze_effettive, i);

            printf("\n hello2 %d \n",num_stanze_effettive);
            //aspetta_invio();
            esplora_stanze(stanze_livello_dungeon,num_stanze_effettive);
            break;
        case 3:
            printf("\n**Sei entrato nella Grotta di cristallo!**\n");
            generaStanze(stanze_livello_dungeon,&num_stanze_effettive, i);

            printf("\n hello3 %d \n",num_stanze_effettive);
            //aspetta_invio();
            esplora_stanze(stanze_livello_dungeon,num_stanze_effettive);
            break;
        case 4:
            if(Player.has_key){
                generaStanze(stanze_livello_dungeon, &num_stanze_effettive, i);
            }
            break;
        default:
            printf("\nOpzione non valida.");
            break;
    }
    if(Player.current_hp>0)
        MenuPartita();
    else
        gameLoop();
}
/**
 * @brief Lancio del dado -> Tramite la funzione "rollare Dadi(true)", il sistema estrae
 * un indice per pescare dai template delle stanze.
 */
int rollareDadi(bool is_dice) // is_dice = true usa dado a 6 facce is_dice = false lancio moneta. DI DEFAULT IS_DICE = TRUE
{
    int a;
    if(is_dice){ //se � dado
        a = (rand() % 6 )+ 1;
    }else{ // se � moneta
        a = rand()%2;
    }
    return a;
}

/**
 * @brief La funzione "generaStanze" popola un array di "Max_stanze" basandosi su quale missione il
 * giocatore ha scelto.
 * Il dungeon non � generato casualmente ma ha un vincolo di causalit�.
 */
void generaStanze(stanza stanze_livello[], int *num_stanze, int missione_selezionata){
    switch(missione_selezionata){
        case 1:
            gen_stanze_palude(stanze_livello, num_stanze);
            break;
        case 2:
            gen_stanze_magione(stanze_livello, num_stanze);
            break;
        case 3:
            gen_stanze_grotta(stanze_livello, num_stanze);
            break;
        case 4:
            if(Player.has_key){
                // missione finale
                //printf("Missione finale (ANCORA DA FINIRE)\n"); <- non pi�
                boss_finale();
            }
            break;
        default:
            printf("missione non ancora fatta");
            break;
    }
}

/**
 * @brief Nella grotta -> L'ultima stanza � impostata manualmente come
 * "Drago Antico" per garantire il che il giocatore arrivi ad avere la possibilit� di
 * ottenere "Spada dell'eroe".
 */
void gen_stanze_grotta(stanza stanze_livello[], int *num_stanze){
    printf("--- Generazione Dungeon Grotta di cristallo ---\n");
    *num_stanze = 0;
    bool is_dragon = false;
    for(int i = 0; i < MAX_STANZE ; i ++){
        int tiro_dado = rollareDadi(true);
        int index = *num_stanze;

        if(i == MAX_STANZE - 1 && !is_dragon){ // forzatura per la stanza del drago (Shenron ovviamente)
            tiro_dado = 6;
        }

        stanze_livello[index].nome = stanza_grotta_c[tiro_dado].nome;
        stanze_livello[index].tipologia = stanza_grotta_c[tiro_dado].tipologia;
        stanze_livello[index].colpo_fatale = stanza_grotta_c[tiro_dado].colpo_fatale;
        stanze_livello[index].danno = stanza_grotta_c[tiro_dado].danno;
        stanze_livello[index].moneta = stanza_grotta_c[tiro_dado].moneta;

        if(tiro_dado == 5) { // caso in cui il lancio del dado 1-6
            stanze_livello[index].danno = rollareDadi(true);
        }

        if(tiro_dado == 6){
            if(!is_dragon) {
                is_dragon = true;
            }else{ // forza una stanza vuota , se il drago � gi� uscito
                stanze_livello[index].nome = stanza_grotta_c[1].nome;
                stanze_livello[index].tipologia = stanza_grotta_c[1].tipologia;
                stanze_livello[index].colpo_fatale = stanza_grotta_c[1].colpo_fatale;
                stanze_livello[index].danno = stanza_grotta_c[1].danno;
                stanze_livello[index].moneta = stanza_grotta_c[1].moneta;
            }
        }
        (*num_stanze)++;
        //printf("DEBUG.Grotta.1 = Stanza %d: %s // generato casualmente (risultato del dado: %d)\n",index + 1, stanze_livello[index].nome, tiro_dado);
    }
}

void gen_stanze_magione(stanza stanze_livello[], int *num_stanze){
    printf("--- Generazione Dungeon Magione ---\n");
    *num_stanze = 0;
    bool is_vamp = false;

    for(int i = 0 ; i < MAX_STANZE ; i++){
        int tiro_dado = rollareDadi(true);
        int index = *num_stanze;
        if(!is_vamp && tiro_dado == 6){
            stanze_livello[index].nome = stanza_magione[tiro_dado].nome;
            stanze_livello[index].tipologia = stanza_magione[tiro_dado].tipologia;
            stanze_livello[index].colpo_fatale = stanza_magione[tiro_dado].colpo_fatale;
            stanze_livello[index].danno = stanza_magione[tiro_dado].danno;
            stanze_livello[index].moneta = stanza_magione[tiro_dado].moneta;
            stanze_livello[index].obiettivo_missione = true; // true solo se � Vampiro
            is_vamp = true;
        }
        else{
            stanze_livello[index].nome = stanza_magione[tiro_dado].nome;
            stanze_livello[index].tipologia = stanza_magione[tiro_dado].tipologia;
            stanze_livello[index].colpo_fatale = stanza_magione[tiro_dado].colpo_fatale;
            stanze_livello[index].danno = stanza_magione[tiro_dado].danno;
            stanze_livello[index].moneta = stanza_magione[tiro_dado].moneta;
            stanze_livello[index].obiettivo_missione = false;
        }
        stanze_livello[index].visitata = false;

        //printf("DEBUG.Magione.1 = Stanza %d: %s // generato casualmente (risultato del dado: %d)\n",index + 1, stanze_livello[index].nome, tiro_dado);

        (*num_stanze)++;
    }
}

/**
 * @brief Nella palude- > Se non ci sono abbastanza "generali_orco" o se ci
 * sono poche stanze, il sistema smette di inserire con i dadi ma usa delle
 * forzature.
 */
void gen_stanze_palude(stanza stanze_livello[], int *num_stanze){ //#######################################
    printf("--- Generazione Dungeon Palude Putrescente ---\n");
    int generale_orco_count = 0;
    *num_stanze = 0;

    // 1. Ciclo di generazione casuale
    // Itera fino a MAX_STANZE, ma contiene condizioni di interruzione
    for(int i = 0 ; i < MAX_STANZE ; i++){

        // Condizione 1: Stop se la missione � gi� completa
        if(generale_orco_count == MAX_ORCHI){
            break; // Esempio 1: Se trovi 3 orchi alla Stanza 5, la generazione si ferma a 5.
        }

        // Condizione 2: Stop se le stanze rimanenti sono esattamente quelle necessarie per forzare gli Orchi.
        // Questo riserva gli slot finali per la forzatura (es. 7 stanze casuali, rimangono 3 slot, e mancano 3 orchi).
        int empty_slots_remaining = MAX_STANZE - *num_stanze;
        int orcs_still_needed = MAX_ORCHI - generale_orco_count;

        if (empty_slots_remaining <= orcs_still_needed) {
            // Se i posti vuoti rimasti sono minori o uguali agli orchi mancanti,
            // bisogna fermare la generazione casuale e passare alla forzatura.
            break; // Esempio 2: Se siamo a 7 stanze e ci mancano 3 orchi, 3 <= 3 � TRUE, si ferma qui (num_stanze = 7).
        }

        // Genera stanza casuale
        int tiro_dado = rollareDadi(true);
        if(tiro_dado == 6){ // Controlla se � un Generale Orco casuale
            generale_orco_count ++;
        }

        // Assegna i dati (indice � dato da num_stanze)
        int index = *num_stanze;
        stanze_livello[index].nome = stanza_palude[tiro_dado].nome;
        stanze_livello[index].tipologia = stanza_palude[tiro_dado].tipologia;
        stanze_livello[index].colpo_fatale = stanza_palude[tiro_dado].colpo_fatale;
        stanze_livello[index].danno = stanza_palude[tiro_dado].danno;
        stanze_livello[index].moneta = stanza_palude[tiro_dado].moneta;
        stanze_livello[index].obiettivo_missione = (tiro_dado == 6); // true solo se � Generale Orco
        stanze_livello[index].visitata = false;

        if(tiro_dado == 5){ //caso particolare per acquitrino che richiede danno randomico tra 1 e 6
            stanze_livello[index].danno = rollareDadi(true);
        }

        //printf("DEBUG = Stanza %d: %s // generato casualmente (risultato del dado: %d)\n",index + 1, stanze_livello[index].nome, tiro_dado);

        (*num_stanze)++; //incrementa il valore ()++; parentesi per evitare che il puntatore punti altrove
    }

    // 2. Ciclo di forzatura: Forzatura nel caso in cui il generale_orco_count non sia a 3
    while(generale_orco_count < MAX_ORCHI && *num_stanze < MAX_STANZE){
        int index = *num_stanze;

        // Forza Generale Orco (indice 6)
        stanze_livello[index].nome = stanza_palude[6].nome;
        stanze_livello[index].tipologia = stanza_palude[6].tipologia;
        stanze_livello[index].colpo_fatale = stanza_palude[6].colpo_fatale;
        stanze_livello[index].danno = stanza_palude[6].danno;
        stanze_livello[index].moneta = stanza_palude[6].moneta;
        stanze_livello[index].obiettivo_missione = true;
        stanze_livello[index].visitata = false;

        //printf("DEBUG.1 = Stanza %d: %s // forzato, non generato casualmente\n",index + 1, stanze_livello[index].nome);

        generale_orco_count++;
        num_stanze++;
    }
    //printf("DEBUG.2 = Generazione completata! Totale stanze: %d. Generali Orco trovati: %d.\n", *num_stanze, generale_orco_count);
}

/**
 * @brief Il negozio funge da "Hub" commerciale tra una missione e l'altra. La logica e progettata per
 * essere dinamica e sicura:
 * Verifica disponibilit� -> il menu mostra determinati oggetti come Spada, Armatura
 * solo se il giocatore non li possiede gi� ("!Player.has_sword").
 * Controllo Transizione -> Prima di ogni acquisto il sistema verificher� che il "Player"
 * abbia abbastanza monete per permettersi di comprare ci� che vuole, nel caso abbia
 * le monete giuste, le sottrae e aggiorna lo stato del "Player".
 * Interfaccia Dinamica -> "max_neg" � una variabile che limita gli input dell'utente
 * solo alle opzioni effettivamente visibili a schermo.
 */
void Negozio(){  // si pu� migliorare ma intanto funziona
    clearScreen();

    // per debug , se no non si hanno modifiche sul player
    /*if(!Player.is_initialized){
        instanziamento_player();
    }*/

    Negozio_obj.costo_pozione = 4;
    Negozio_obj.costo_spada = 5;
    Negozio_obj.costo_armatura = 10;
    int max_neg = 1;
    bool exit = false;
    int i = 1;
    do{
        printf("\t\t**NEGOZIO**\t\tMonete correnti:\t%d\n",Player.monete);
        printf("1) - Pozione Curativa : \t\t %d monete\n",Negozio_obj.costo_pozione);
        if(!Player.has_sword){
            printf("2) - Spada : \t\t\t %d monete\n",Negozio_obj.costo_spada);
            max_neg = 2;
        }
        if(!Player.has_armor){
            printf("3) - Armatura : \t\t %d monete\n",Negozio_obj.costo_armatura);
            max_neg = 3;
        }
        printf(">0) Exit [0-%d]\t",max_neg);
        scanf("%d",&i);

        switch(i){
            case 0:
                exit = true;
                break;
            case 1:
                if(Player.monete >= Negozio_obj.costo_pozione){
                    printf("Hai acquistato una pozione curativa!\n");
                    Player.num_pozioni += 1;
                    Player.monete -= Negozio_obj.costo_pozione; 
                    exit = true;
                }
                else{
                    printf("Poche monete o oggetto gia' acquistato!\n");
                }
                break;
            case 2:
                if(!Player.has_sword && Player.monete >= Negozio_obj.costo_spada){
                    printf("Spada acqustata!\n");
                    Player.has_sword = true;
                    Player.monete -= Negozio_obj.costo_spada;
                    exit = true;
                }
                else{
                    printf("Poche monete o oggetto gia' acquistato!\n");
                }
                break;
            case 3:
                if(!Player.has_armor && Player.monete >= Negozio_obj.costo_armatura){
                    printf("Armatura acqustata!\n");
                    Player.has_armor = true;
                    Player.monete -= Negozio_obj.costo_armatura;
                    exit = true;
                }
                else{
                    printf("Poche monete o oggetto gia' acquistato!\n");
                }
                break;
            default:
                printf("errore!");
                exit = true;
                break;
        }
    }while(!exit);
}

/**
 * @brief Per la sfida contro il "Drago antico", si ha implementato come da consegna l'algoritmo della
 * successione di Padovan, che a differenza di Fibonacci questa sequenza cresce lentamente
 * ed e definita dalla seguente formula:
 * $P(n)=P(n-2)+P(n+3)$
 * con dei valori iniziali: P(0)) 1, P(1) = 1, $P(2)=1.$
 * la funzione padovan -> � stata calcolata in modo ricorsivo.
 */
int padovan(int n){
    if(n == 0 || n == 1 || n == 2){
        return 1;
    }
    return padovan(n - 2) + padovan(n - 3);
}

/**
 * @brief is_in_padonav(num, n) -> controlla se il numero inserito dal giocatore � un
 * termine valido della serie, permettendo al drago di convalidare la risposta
 * dell'indovinello.
 */
bool is_in_padovan(int num , int n){
    int p = padovan(n);
    if(p == num){
        return true;
    }
    if(p >= num){
        return false;
    }
    return is_in_padovan(num, n+1);
}

/**
 * @brief saveToFile: sono presenti due puntatori di tipo FILE, uno per la scrittura effettiva dei
 * dati del giocatore nel salvataggio ed uno per scrivere i dati del salvataggio nel file
 * index.
 * Per costruire il nome effettivo del salvataggio (salvataggio<n>.bin) � presente
 * un array di char chiamato fileName, formattato da sprintf per inserire il numero di
 * salvataggio (preso dal numero di elementi presenti nel vector).
 * Una volta generato il nome del file viene utilizzata fopen();
 * nella modalit� write binary, in modo da creare
 * (dato che ancora non esiste) e scrivere, tramite fwrite che va ad inserire un record
 * contenente una struct di tipo player, il file di salvataggio.
 * Terminato questo processo (e quindi dopo aver chiuso lo stream di fp, dato che non � possibile avere due stream
 * aperti contemporaneamente) si procede a scrivere i dati del salvataggio nel file indice
 * (tramite append binary, dato che write binary sovrascrive l'intero contenuto del file) e
 * ad aggiungerli all'interno di vector.
 * Ci� avviene alla fine perch� bisogna assicurarsi che le scritture su file abbiano avuto successo.
 */
bool saveToFile(){
    //dato che non posso inizializzare saves nelle variabili globali devo controllare che sia stato inizializzato
    if(saves==NULL){
        printf("ERRORE: Vector dei salvataggi non inizializzato!\n");
        return false;
    }

    FILE* fp=NULL;
    FILE* fpIndex=NULL;
    time_t now=time(NULL);
    struct tm *t=localtime(&now);
    char date[50];
    char fileName[50];

    //stringa della data formattata in giorni-mesi-anni ore-minuti-secondi
    strftime(date, sizeof(date), "%d-%m-%Y %H:%M:%S", t);

    //copio i dati che mi servono per l'identificativo del file nel vettore
    fileFormat file;
    strcpy(file.data, date);
    file.pVita=Player.current_hp;
    file.monete=Player.monete;
    file.oggetti=Player.num_oggetti;
    file.missioni=Player.missioni_completate;
    file.deleted=false;


    sprintf(fileName, "salvataggio%zu.bin", saves->size);

    //scrivo i dati del player
    fp=fopen(fileName, "wb");
    if(fp==NULL){
        printf("ERRORE: Impossibile creare il file %s\n", fileName);
        return false;
    }

    fwrite(&Player, sizeof(Player), 1, fp);
    fclose(fp);

    //aggiorno file indice
    fpIndex=fopen("index.bin", "ab");
    if(fpIndex==NULL){
        printf("ERRORE: Impossibile aprire index.bin\n");
        return false;
    }

    fwrite(&file, sizeof(fileFormat), 1, fpIndex);
    fclose(fpIndex);

    //Aggiungo a vector solo alla fine perch� devo assicurarmi che tutte le operazioni precedenti siano andate a buon fine
    vAdd(saves, file);
    return true;
}

/**
 * @brief loadFromFile: � presente un controllo iniziale per verificare che il salvataggio
 * richiesto dal player sia valido, dopodich� come nella funzione saveToFile viene
 * costruito all'interno di una stringa il nome (salvataggio<n>.bin) del file di salvataggio
 * a cui accedere.
 * Tramite la fread() si ottengono i dati della struct player (da sostituire a
 * quelli della struttura player attuale) per poi chiudere lo stream.
 */
bool loadFromFile(size_t nSave) {
    if(nSave>saves->size){
        printf("\nERRORE: Numero di salvataggio invalido");
        return false;
    }

    FILE* fp=NULL;
    char filename[50];
    struct player readPlayer;

    sprintf(filename, "salvataggio%zu.bin", nSave-1);
    fp= fopen(filename, "rb");

    if(fp==NULL){
        printf("\nERRORE: Impossibile aprire il file di salvataggio");
        return false;
    }

    while(fread((&readPlayer), sizeof(struct player), 1, fp)==1){
        Player.monete=readPlayer.monete;
        Player.has_key=readPlayer.has_key;
        Player.current_hp=readPlayer.current_hp;
        Player.has_armor=readPlayer.has_armor;
        Player.has_sword=readPlayer.has_sword;
        Player.has_hero_sword=readPlayer.has_hero_sword;
        Player.is_initialized=readPlayer.is_initialized;
        Player.num_pozioni=readPlayer.num_pozioni;
        Player.missioni_completate=readPlayer.missioni_completate;
        Player.num_oggetti=readPlayer.num_oggetti;
        Player.max_hp=readPlayer.max_hp;
    }

    fclose(fp);

    return true;
}

/**
 * @brief loadSaves Vector: si occupa di leggere i dati dei salvataggi presenti all'interno del file
 * indice e di caricare le varie struct (ad eccezione di quelle flaggate come eliminate)
 * all'interno del vector utilizzando la funzione vAdd(v, data) della libreria vector.h.
 * L'apertura dello stream � stata effettuata tramite l'opzione "read binary" in quanto il
 * file non deve essere modificato.
 */
void loadSavesVector(){
    if(saves==NULL){
        printf("ERRORE: Vector non inizializzato!\n");
        return;
    }

    FILE *fp=fopen("index.bin", "rb");
    if(fp==NULL) {
        return;
    }

    fileFormat read;
    while(fread(&read, sizeof(fileFormat), 1, fp)==1){
        if(read.deleted==0){
            vAdd(saves, read);
        }
    }

    fclose(fp);
    printf("Caricati %zu salvataggi.\n", saves->size);
}

/**
 * @brief deleteSave: viene usato un flag is Deleted FromIndex il cui valore verr� assegnato dal
 * ritorno della funzione deleteFromIndex per assicurarsi che l'eliminazione del file
 * indice abbia avuto successo.
 * Si usa poi la stringa fileDelete e la funzione sprintf();
 * per costruire il nome del salvataggio da eliminare (nSave viene decrementato di uno
 * poich� � stato scelto un sistema zero-indexed per i nomi dei file di salvataggio, in
 * modo da essere coerenti con gli indici del vector), dopodich� viene eliminato tramite
 * la funzione remove();.
 * Viene utilizzata anche la funzione renameFiles per "shiftare" di
 * 1 i numeri <n> dei nomi dei salvataggi successivi a quello eliminato.
 */
bool deleteSave(size_t nSave){
    if(nSave>saves->size){
        printf("\nERRORE: Numero di salvataggio invalido");
        return false;
    }

    bool isDeletedFromIndex;
    char fileDelete[50];
    fileFormat delete= vGet(saves, nSave-1);

    vDelete(saves, nSave-1);

    isDeletedFromIndex=deleteFromIndex(delete);

    if(isDeletedFromIndex){
        sprintf(fileDelete, "salvataggio%zu.bin",nSave-1);
        remove(fileDelete);
        renameFiles(nSave);
    }else{
        return false;
    }

    return true;
}

/**
 * @brief deleteFromIndex: elimina dal file indice le informazioni del salvataggio da eliminare.
 * Lo stream viene aperto in "r+" per poter apportare delle modifiche ai record, nello
 * specifico il campo "deleted" della struct per flaggare che quel record non � pi�
 * presente.
 * La ricerca all'interno del file viene effettuata comparando la data poich�
 * univoca. Dato che fread();
 * sposta automaticamente il puntatore ad ogni lettura, �
 * necessario utilizzare fseek();
 * per tornare indietro di un record e modificare il flag
 * "deleted" (la fwrite sposter� poi di nuovo il puntatore in avanti di un record),
 */
bool deleteFromIndex(fileFormat delete){
    FILE *fp=fopen("index.bin", "r+");
    if(fp==NULL){
        printf("Errore: Impossibile aprire il file index.bin");
        return false;
    }

    fileFormat readFileFormat;

    while(fread(&readFileFormat, sizeof(fileFormat), 1, fp)==1){
        if(readFileFormat.deleted==0&& strcmp(delete.data,readFileFormat.data)==0){
            readFileFormat.deleted=1; //metto flag di cancellazione a 1
            fseek(fp, -(long)sizeof(fileFormat), SEEK_CUR); //torno indietro di un record nel file per poter scrivere la struct modificata
            fwrite(&readFileFormat, sizeof(fileFormat), 1, fp); //scrivo la struct nel file
            break; //non ha senso scorrere il resto del file
        }
    }

    fclose(fp);
    return true;
}

/**
 * @brief renameFiles: necessaria per l'eliminazione di un file di salvataggio. Quando un file
 * viene eliminato, viene rimosso anche dal vector e se non venisse aggiornato il valore
 * presente alla fine del nome del salvataggio si avrebbe una incoerenza tra i file
 * presenti nella directory e l'indice delle strutture salvate nel vector.
 * Nella stringa fileRename viene messo il nome del file da modificare, mentre nella stringa
 * newFileName il nuovo nome del file (<n> decrementato di 1), per poi rinominare
 * effettivamente il file con la funzione rename.
 */
void renameFiles(size_t nSave){
    char fileRename[50];
    char newFileName[50];
    size_t i = 0;
    for(i=nSave; i<=saves->size; i++){
        sprintf(fileRename, "salvataggio%zu.bin", i);
        sprintf(newFileName, "salvataggio%zu.bin", i-1);
        rename(fileRename, newFileName);
    }
}

/**
 * @brief Una volta scelto il dungeon da esplorare e generate le stanze viene chiamata la funzione
 * esplora stanze che prende in input il numero del livello (1 per palude, 2 per magione, 3 per
 * grotta) e la funzione ha un while che richiama il men� che si occupa delle decisioni
 * all'interno delle esplorazioni (menuMissione) che ritorna true se si ha scelto di affrontare una
 * stanza o false se il giocatore ha fatto un'altra azione questo per far avanzare il contatore
 * che gestisce le stanze.
 * Finito di esplorare le stanze ci sono dei controlli per verificare che il
 * player abbia completato gli obiettivi del dungeon oppure abbia abbandonato il dungeon
 * prima di aver completato gli obiettivi.
 */
void esplora_stanze(stanza stanze_livello[],int num_stanze)
{
    int risultato;
    clearScreen();
    int i=0;
    while(i<num_stanze && Player.current_hp >0){
        risultato = menuMissione(stanze_livello[i],i);

        if(risultato==1){
            i++;
        }else if(risultato==2){
            return; 
        }
    }
    if(orchi_sconfitti == 3){
        Player.missioni_completate += 1;
    }
    if(Player.has_key){
        Player.missioni_completate += 1;
    }
    if(Player.has_hero_sword){
        Player.missioni_completate += 1;
    }
    clearScreen();
    if(Player.current_hp <=0){
        printf("\n=====SEI MORTO=====\n");
        aspetta_invio();
    }else{
        printf("\n=====DUNGEON COMLETATO=====\n");
        aspetta_invio();
    }
}

/**
 * @brief gestisci_combattimento: riceve in input la stanza da esplorare
 * in base al tipo avvia una delle boss fight un combattimento tradizionale oppure, nel caso di
 * una trappola, danneggia il giocatore
 * la funzione richiama funzioni helper per danneggiare il giocatore o assegnare delle
 * ricompense
 */
void gestisci_combattimento(stanza stanza_da_esplorare){
    bool nemico_sconfitto = false;
    int dmg;
    //questa funzione gestisce i combattimenti che sia contro boss o nemici normali
    if(strcmp(stanza_da_esplorare.tipologia, "Combattimento") == 0){
        printf("\n esplorando la stanza ti imbatti in %s",stanza_da_esplorare.nome);
        if(strcmp(stanza_da_esplorare.nome,"Drago Antico")==0)//drago
        {
            combattimento_drago(stanza_da_esplorare);
            return;
        }
        if(strcmp(stanza_da_esplorare.nome,"Signore Oscuro")==0)//boss finale
        {
            boss_finale();
            return;
        }

        //per altri nemici
        while(!nemico_sconfitto && Player.current_hp>0){
            dmg = calcola_danno();
            printf(" DANNO : %d VITA: %d",dmg,stanza_da_esplorare.colpo_fatale);
            //aspetta_invio(); //<--
            if(dmg > stanza_da_esplorare.colpo_fatale){
                printf("\n hai inflitto %d danni su %d HP a %s",dmg,stanza_da_esplorare.colpo_fatale,stanza_da_esplorare.nome); //debug
                nemico_sconfitto = true;
            } else{
                printf("\n\n %s ti attacca",stanza_da_esplorare.nome);
                infliggi_danno(stanza_da_esplorare.danno);
            }
            if(Player.current_hp>0)
                aspetta_invio(); //<-- qui volevo mettere un commento ma non ricordo cosa volessi scrivere

        }
        if(Player.current_hp>0){
            printf("\n Hai sconfitto %s",stanza_da_esplorare.nome);
            assegna_ricompense(stanza_da_esplorare.moneta);
            if(strcmp(stanza_da_esplorare.nome,"Generale Orco")==0){
                //printf("\n DEBUG GENERALE ORCO SCONFITTO");
                orchi_sconfitti += 1;
            }
            if(strcmp(stanza_da_esplorare.nome,"Vampiro Superiore")==0){
                //printf("\n DEBUG GENERALE ORCO SCONFITTO");
                Player.has_key = true;
            }
            if(strcmp(stanza_da_esplorare.nome,"Drago Antico")==0){
                //printf("\n DEBUG GENERALE ORCO SCONFITTO");
                Player.has_hero_sword = true;
            }
        }
        else
            printf("\n %s ti ha sconfitto \n",stanza_da_esplorare.nome);

        aspetta_invio(); //<--
    }

}

int calcola_danno(){
    int dmg;
    dmg = rollareDadi(true);

    if(Player.has_sword)
        dmg += 1;
    else if(Player.has_hero_sword)
        dmg += 2;

    return dmg;
}

void infliggi_danno(int danno){
    printf("\n\n>il giocatore subisce %d danni ",danno);
    Player.current_hp -= danno;
    printf("\n>ti restano %d/%d HP \n\n",Player.current_hp,Player.max_hp);
}

void assegna_ricompense(int monete){
    printf("guadagnato %d monete ",monete);
    Player.monete += monete;
    printf(", monete nel tuo borsello: %d \n\n",Player.monete);
}

/**
 * @brief II menuMissione permette al giocatore di fare varie cose in particolare:
 * curarsi: permette di usare una pozione di cura senza passare per l'inventario
 * usare il negozio: per acquistare armi armature e pozioni di cura
 * abbandonare missione: spendendo monete � possibile abbandonare il dungeon
 * esplorare una stanza: esplora una stanza generata
 */
int menuMissione(stanza stanzaN,int num_stanza){ //permette al giocaotre di fare compere e ripristinare punti vita tra le stanze
    int i = opzioniMissione(num_stanza);
    clearScreen();
    int hp_recuperati;
    switch(i){
        case 0:
            //printf("\n %d",num_stanza); debug
            //printf("\nTi trovi davanti a: %s", stanzaN.nome);
            //printf("\nTipologia: %s", stanzaN.tipologia);

            // Qui dovrai inserire la logica di combattimento o trappola
            if (strcmp(stanzaN.tipologia, "Combattimento") == 0) {
                gestisci_combattimento(stanzaN);
                // EseguiCombattimento(&stanze_livello_dungeon[j]);
            }else if (strcmp(stanzaN.tipologia, "Trappola") == 0){
                printf("\n Ti sei imbattuto in %s!", stanzaN.nome);
                infliggi_danno(stanzaN.danno);
            }
            return 1;
            break;
        case 1:
            if(Player.num_pozioni<=0){ //se non ha pozioni salta tutto il resto
                printf("\n #>NON HAI POZIONI DI CURA");
                break;
            }

            hp_recuperati = rollareDadi(true);
            if(hp_recuperati+Player.current_hp > Player.max_hp){ // se la cura superi il limite di hp
                hp_recuperati = hp_recuperati-((Player.max_hp + hp_recuperati) - Player.max_hp);
                Player.current_hp = Player.max_hp;
            }else{ //tutti gli altri casi

                Player.current_hp += hp_recuperati;
            }
            printf("\n>hai recuperato %d punti vita",hp_recuperati);
            aspetta_invio();
            Player.num_pozioni -= 1; //sottraggo pozione usata
            return 0;
            break;

        case 2:
            Negozio();
            return 0;
            break;

        case 3:
            Inventario();
            return 0;
            break;

        case 4:
            if(Player.monete>=50){
                orchi_sconfitti = 0;
                Player.monete -= 50;
                return 2;
            }else{
                printf("\n non ha abbastanza monete, resisti!");
                aspetta_invio();
                return 0;
            }
            break;

        default:
            printf("\nOpzione non valida.");
            break;

    }
    if(i==0)
        return true;
    else
        return false;
}

int opzioniMissione(int num_stanza){ //stampa e ritorna le opzioni del men� della missione
    int i;
    char c;

    clearScreen();

    printf("\n Menu Esplorazione : \n");
    do{
        printf("\n 0 Esplora stanza %d",num_stanza);
        printf("\n 1 Usa Pozione	(Hai %d / %d HP)",Player.current_hp,Player.max_hp);
        printf("\n 2 Fai Acquisti	(Hai %d monete)",Player.monete);
        printf("\n 3 Inventario");
        printf("\n 4 Fuggi	(Hai %d/50 monete)",Player.monete);
        printf("\nSeleziona una delle opzioni del menu [0-4]:\t");
        scanf(" %c",&c);
        getchar();
        i = c -'0';
    }while(i>5 || i<0);


    clearScreen();
    return i;
}

void Inventario(){ //stampa informazioni relative all'inventario, vita armatura spada etc...
    clearScreen();
    printf("\n -----PLAYER-----\n");
    printf(">HP: 			%d/%d\n",Player.current_hp,Player.max_hp);
    printf(">Denari: 		%d\n",Player.monete);
    printf(">pozioni cura:		%d\n",Player.num_pozioni);
    if(Player.has_armor)
        printf(">armatura: 		SI\n");
    else
        printf(">armatura: 		NO\n");

    if(Player.has_sword)
        printf(">spada: 		SI\n");
    else
        printf(">spada: 		NO\n");

    if(Player.has_hero_sword)
        printf(">spada Eroe: 		SI\n");
    else
        printf(">spada Eroe: 		NO\n");

    if(Player.has_key)
        printf(">chiave: 		SI\n");
    else
        printf(">chiave: 		NO\n");

    printf("\n-premi invio per continuare-\n");
    getchar();
    clearScreen();
    //menuMissione();
}


/**
 * @brief Drago caverna di cristalli:
 * usa la sequenza di Padovan, il giocatore dice un numero se appartiene alla sequenza vince
 * altrimenti subisce danni
 */
void combattimento_drago(stanza stanza_da_esplorare){
    bool EnemynotDefeated = true;
    int num=0;
    while(Player.current_hp>0 && EnemynotDefeated){
        while(num >=1 && num<=500){
            printf("\n il %s ti chiede di inserire un numero: ",stanza_da_esplorare.nome);
            scanf("%d", &num);
            getchar();
        }

        if(is_in_padovan(num,0)){
            printf("\n hai indovinato! ");
            int dmg = calcola_danno();
            if(dmg>stanza_da_esplorare.colpo_fatale){//piccola aggiunta mi ero dimenticato
                EnemynotDefeated = false;
                printf("\n =====DRAGO SCONFITTO=====");
            }
        }else{
            printf("\n hai sbagliato! ");
            infliggi_danno(stanza_da_esplorare.danno);
        }
    }
}

/**
 * @brief Signore Oscuro:
 * Sasso carta forbici e in 5 round il primo che raggiunge 3 vittorie vince se ci� non avviene c'�
 * un twist particolare.
 */
void boss_finale(){
    int vittorieP=0,vittorieN=0; // P = player, N = nemico
    int numero_casuale;
    int valore,ris;
    for(int i=0; i<5 && vittorieP<3 && vittorieN<3 ;i++){
        //stampa inizio turno
        printf("\n ROUND %i/5 - vittorie player: %d , vittorie signore oscuro: %d",i+1,vittorieP,vittorieN);
        //scelta giocatore
        do{
            stampa_opzioni_bossFinale();
            printf("\n \n cosa scegli?");
            scanf("%d", &valore);
            getchar();
        }while(valore <0 || valore >2);

        //scelta drago
        numero_casuale = rand() % 3;
        printf("\n Il Signore Oscuro sceglie ");
        switch(numero_casuale){
            case 0:
                printf("SCUDO");
                break;
            case 1:
                printf("SPADA");
                break;
            case 2:
                printf("MAGIA");
                break;
            default:
                break;
        }

        ris = controlla_risultato(valore,numero_casuale);
        if(ris <0){
            printf("\n HAI PERSO");
            vittorieN++;
        }else if(ris == 0){
            printf("\n PAREGGIO");
        }else{
            printf("\n HAI VINTO");
            vittorieP++;
        }
    }
    if(vittorieN==3){
        printf("\n L'eroe e' stato sconfitto, il signore oscuro regna sovrano");
    }else if(vittorieP==3){
        printf("\n Il signore oscuro e' stato sconfitto, l'eroe torna al villaggio trionfante");
    }else{
        printf("\n errore perche' sei qui?\n ad ogni modo i dev fixeranno questo bug nella prossima patch\n ma non prima di aver aggiunto un battlepass e un negozio di cosmetici");
    }
}

void stampa_opzioni_bossFinale(){//stampa opizioni
    printf("\n SCUDO [0]");
    printf("\n SPADA [1]");
    printf("\n MAGIA [2]");
}

void stampa_opzioni_bossFinale_guida(){//non usata ma serve a me come reference
    printf("\n SCUDO [0] vince su SPADA [1]");
    printf("\n SPADA [1] vince su MAGIA [2]");
    printf("\n MAGIA [2] vince su SCUDO [0]");
}

int controlla_risultato(int giocatore,int nemico){//per i risultati ed evitare di usare 9 if
    int risultati[3][3] = { //0 pareggio, 1 vittoria, -1 sconfitta, ordine -> scudo, magia spada
            { 0, -1,  1},
            { 1,  0, -1},
            {-1,  1,  0}};

    return risultati[giocatore][nemico];
}

/**
 * @brief Aspetta invio per permettere al giocatore di leggere i messaggi stampati dal gioco.
 */
void aspetta_invio() {//semplice funzione per permettere al giocatore di leggere i messaggi
    printf("\n-premi invio per continuare-\n");
    getchar();
}

/**
 * @brief Pulizia dello schermo: system("cls) (windows) e system("clear") (unix) non sono portabili,
 * abbiamo quindi optato per l'utilizzo di questa funzione:
 * -\033[H : sposta il cursore nella sua posizione predefinita
 * -\033[J : cancella i caratteri visualizzati nel terminale
 */
//pulizia dello schermo dato che system(cls) (windows) e system(clear) (unix) non sono portabili
void clearScreen(void) {
    printf("\033[H\033[J");
}