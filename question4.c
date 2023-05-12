#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <ctype.h>


#define NUMBER_THREAD 4
#define SIZE_BUFFER 5
//Creation des deux sémaphores
sem_t *semC, *semP;
//Variable en volatile pour quelle soit modifier par n'importe quel processus.
volatile char buffer[SIZE_BUFFER];

//caractère produit 
char str = 'a';



void *producteur(void *args){
    //Initialisation de l'id ou on stock le caractère dans le buffer
    int id = 0;


    while(1){
        //Production

        //sleep pour voir l'évolution (on force le producteur à mettre au moins 1 seconde à s'exécuter)
        sleep(1);
        //On attend que le consomatteur est fini
        sem_wait(semC);
        buffer[id] = str;

        //On previent que le producteur a fini
        sem_post(semP);

        printf("On produit la lettre %c à la place %d du buffer \n",str,id);

        //Caractère suivant
        str++;

        //Ainsi que l'emplacement suivant du buffer
        id++;

        //On verifie si on est arrivé à la taille max du buffer, si oui on recommence depuis le début de ce dernier
        //On va donc écrire par dessus les informations précédentes les plus vieilles
        if(id == SIZE_BUFFER){
            id=0;
        }
    }
}


void* consommateur(void* args) {
        //Initialisation de l'id ou on stock le caractère dans le buffer
    int id = 0;

    while (1) {
        //Consommation
        char c;
        char c_maj;

        sem_wait(semP);
        c = buffer[id];
        //passe le char en majuscule
        c_maj = toupper(c);
        sem_post(semC);

        
        // Consomme
        printf("On consomme la lettre %c situé dans le buffer à la place %d \n",c,id);
        printf("Ce qui donne en majuscule: %c \n",c_maj);
        
        // Si on est à la case 10 -> on revient à la case 0
        id++;

        if (id == SIZE_BUFFER) {
            id = 0;
        }

        sleep(1);

    }
}


int main(int argc, char* argv[]) {
    srand(time(NULL));
    pthread_t th[NUMBER_THREAD];

    semC = sem_open("semC", O_CREAT, 0644, 1);

    semP = sem_open("semP", O_CREAT, 0644, 0);

    int i;
    //Le modulo permet de créé les thread avec une alternance entre producteur et consommateur pour
    //en avoir le même nombre
    for (i = 0; i < NUMBER_THREAD; i++) {
        if (i%2==0) {
            if (pthread_create(&th[i], NULL, &producteur, NULL) != 0) {
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &consommateur, NULL) != 0) {
                perror("Failed to create thread");
            }
        }
    }

    for (i = 0; i < NUMBER_THREAD; i++) {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }

    sem_close(semP);
    sem_close(semC);

    return 0;
}







