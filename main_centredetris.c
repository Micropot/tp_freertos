/*
 * TP Centre de tri de colis
 * 2/5/2019 - YA
 */
 
// includes Kernel
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// includes Standard 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// includes Simulateur (redirection de la sortie printf)
#include "simulateur.h"

// Définition du nombre d'éléments pour les files
#define x 10 // nombre d'éléments pour la File_tapis_arrivee 
#define y 10 // nombre d'éléments pour les File_depart_national et File_depart_international
#define z 10 // nombre d'éléments pour la File_relecture 

// Définition des timeout pour les files en ms
#define TIMEOUT_FILE_TAPIS_ARRIVEE 200
#define RELECTURE 500

// Création du sémaphore pour l'affichage 
xSemaphoreHandle affichage;


// Création des files
xQueueHandle File_tapis_arrivee;
xQueueHandle File_depart_national;
xQueueHandle File_depart_international;
xQueueHandle File_tapis_relecture;

//********************************************************
//* Tâche arrivée 
//*
//* Répète de façon cyclique une liste de colis à deposer
//*
//* B2=1 -> le colis est passé par « tache-relecture », sinon B2=0
//* B1=1 -> étiquette non lisible sinon B1=0
//* B0=1 -> marché international / B0=0 -> marché international
//* 
//********************************************************

void affiche_message(char *texte, unsigned int colis){
	if(xSemaphoreTake(affichage, (10/portTICK_RATE_MS))== pdTRUE){
		printf("%s : compteur %d B2 = %d B1 = %d B0 = %d\n", texte, colis>>3, (colis & (1<<2))>>2, (colis & (1<<1))>>1, (colis & (1<<0))>>0);
		
		xSemaphoreGive(affichage);
	}
	
}



void Tache_arrivee( void *pvParameters )
{//gerenerer une etiquette aleatoire entre 0 et 3 et delay aléatoire en mutiple de 50 entre 0 et 500
	unsigned int liste_colis[]={  1,   3,   1,   2,   3,   0}; // Etat des bits B2, B2 et B0 -> 0 à 3 en décimal car B2=0
	unsigned int liste_colis_rand;
	unsigned int liste_delai[]={  5, 100,   0,   0, 400,  50}; // Temps d'attente en ms pour le colis suivant
	unsigned int num_colis = 0; // Numéro de colis
	unsigned int colis; // colis (étiquette)
	unsigned int colis_rand; // colis (étiquette)
	unsigned int liste_delai_rand;
	char* numero = malloc(sizeof(char));
	
	
	
	/*while(1)
		{ 
			liste_colis_rand = rand()%3;
			printf("LISTE COLIS RAND : %d\n", liste_colis_rand);
			colis = (num_colis<<3) + liste_colis[num_colis % (sizeof(liste_colis)/sizeof(unsigned int))];
			colis_rand = (num_colis<<3) + liste_colis_rand;
			printf("_______________COLIS RAND n°%d : %d\n", colis>>3,colis_rand);
			xQueueSendToBack(File_tapis_arrivee, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE); // Pour simplifier, on considère que la transmissions s'est bien effectuée
			affiche_message("Tache_arrivee", colis);
			vTaskDelay(liste_delai[num_colis % (sizeof(liste_colis)/sizeof(unsigned int))]/portTICK_RATE_MS); // Attente entre deux colis en ms
			printf("\nLe colis No %d est depose sur le tapis roulant et il porte l'etiquette %d\n",num_colis, colis);	
			num_colis++;
			
			
			
			
	}*/
		
	
		while(1)
		{ 
			liste_colis_rand = rand()%4;
			liste_delai_rand = (rand()%10)*50;
			printf("LISTE COLIS RAND : %d\n", liste_colis_rand);
			printf("LISTE DELAI RAND : %d\n", liste_delai_rand);
			colis_rand = (num_colis<<3) + liste_colis_rand;
			//printf("_______________COLIS RAND n°%d : %d\n", colis>>3,colis_rand);
			xQueueSendToBack(File_tapis_arrivee, &colis_rand, TIMEOUT_FILE_TAPIS_ARRIVEE); // Pour simplifier, on considère que la transmissions s'est bien effectuée
			affiche_message("Tache_arrivee", colis_rand);
			vTaskDelay(liste_delai_rand/portTICK_RATE_MS); // Attente entre deux colis en ms
			printf("\nLe colis No %d est depose sur le tapis roulant et il porte l'etiquette %d\n",num_colis, colis_rand);	
			num_colis++;
			
			
			
			
	}
	vTaskDelete( NULL );
}

void Tache_lecture_rapide( void *pvParametres){
	unsigned int colis;
	while(1){		//faire un if sur le receive pour voir si quleque chose arrive ou pas 
		if(xQueueReceive(File_tapis_arrivee, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)){
			//printf("COLIS ETIQUETTE: %d\n", colis);
			if(((colis & (1<<1))>>1)==1){ //etiquette non lisible 
				
				//xQueueSendToBack(File_tapis_relecture, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE);
				//printf("le colis va en RELECTURE\n");
				//affiche_message("RELECTURE", colis);
				if(xQueueSendToBack(File_tapis_relecture, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)==0){
					printf("!!!!!!ERREUR RELECTURE!!!!!!!!!\n");
				}
			}
			
			if(((colis & (1<<0))>>0)==1){
				
				//xQueueSendToBack(File_depart_international, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE);
				//printf("le colis va a l'INTERNATIONAL\n");
				//affiche_message("INTERNATIONAL", colis);
				if(xQueueSendToBack(File_depart_international, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)==0){
					printf("!!!!!!ERREUR INTERNATIONAL!!!!!!!!!\n");
				}
			
			}
			
			if(((colis & (1<<0))>>0)==0 ){
			
				//xQueueSendToBack(File_depart_national, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE);
				//printf("le colis va sur le territoire NATIONAL\n");
					//affiche_message("NATIONAL", colis);
				if(xQueueSendToBack(File_depart_national, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)==0){
					printf("!!!!!!ERREUR NATIONAL!!!!!!!!!\n");
				}
					
				
			
			}
			affiche_message("lecture rapide", colis);
			
		}
		
	
	}
	vTaskDelete(NULL);
}


void Tache_depart_national(void *pvParametres){
	unsigned int colis;
	
	while(1){
		if(xQueueReceive(File_depart_national, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)){
			affiche_message("LISTE NATIONAL", colis);
		}
		
	}
	
	vTaskDelete(NULL);

}

void Tache_depart_international(void *pvParametres){
	unsigned int colis;
	
	while(1){
		if(xQueueReceive(File_depart_international, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)){
			
				affiche_message("LISTE INTERNATIONAL", colis);
		}
	
	
	
	}
	

	vTaskDelete(NULL);
}

void Tache_relecure( void *pvParametres){
	unsigned int colis;
	while(1){
		if(xQueueReceive(File_tapis_relecture, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE)){
			affiche_message("avant la relcture", colis);
			colis&=~(1<<1);//force le bit 1 a passer a 0
			colis|=(1<<2);//force le bit 2 a passer a 1
			vTaskDelay(RELECTURE);
			//xQueueSendToFront(File_tapis_arrivee, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE);
			affiche_message("apres la relecture", colis);
			if(xQueueSendToFront(File_tapis_arrivee, &colis, TIMEOUT_FILE_TAPIS_ARRIVEE) == 0){
				printf("!!!!!!!!!!ERREUR ARRIVEE!!!!!!!\n");
			}
		}
	
		
	
	}
	vTaskDelete(NULL);
}


// Main()
int main(void)
{
	// ulTaskNumber[0] = Tache_arrivee
	// ulTaskNumber[1] = idle task
	
	//création des sémaphores binaires 
	affichage = xSemaphoreCreateMutex();
	
	
	
	File_tapis_arrivee = xQueueCreate(x, sizeof(unsigned int));
	File_depart_international = xQueueCreate(y, sizeof(unsigned int));
	File_depart_national = xQueueCreate(y, sizeof(unsigned int));
	File_tapis_relecture = xQueueCreate(z, sizeof(unsigned int));
	
	// Création de la tâche arrivée
	

	
	
	
	xTaskCreate( 	Tache_arrivee, 						// Pointeur vers la fonction
								"Tache arrivee",					// Nom de la tâche, facilite le debug
							  240, // Taille de pile (mots)
								NULL, 										// Pas de paramètres pour la tâche
								1, 												// Niveau de priorité 1 pour la tâche (0 étant la plus faible) 
								NULL ); 									// Pas d'utilisation du task handle
								
	xTaskCreate( 	Tache_lecture_rapide, 						// Pointeur vers la fonction
								"Tache_lecture_rapide",					// Nom de la tâche, facilite le debug
								240, // Taille de pile (mots)
								NULL, 										// Pas de paramètres pour la tâche
								1, 												// Niveau de priorité 1 pour la tâche (0 étant la plus faible) 
								NULL ); 									// Pas d'utilisation du task handle
	
	xTaskCreate( 	Tache_depart_national, 						// Pointeur vers la fonction
								"Tache_depart_national",					// Nom de la tâche, facilite le debug
								240, // Taille de pile (mots)
								NULL, 										// Pas de paramètres pour la tâche
								1, 												// Niveau de priorité 1 pour la tâche (0 étant la plus faible) 
								NULL ); 									// Pas d'utilisation du task handle
								
	xTaskCreate( 	Tache_depart_international, 						// Pointeur vers la fonction
						"Tache_depart_international",					// Nom de la tâche, facilite le debug
						240, // Taille de pile (mots)
						NULL, 										// Pas de paramètres pour la tâche
						1, 												// Niveau de priorité 1 pour la tâche (0 étant la plus faible) 
						NULL ); 							// Pas d'utilisation du task handle
						
	xTaskCreate( 	Tache_relecure, 						// Pointeur vers la fonction
					"	Tache_relecure",					// Nom de la tâche, facilite le debug
					240, // Taille de pile (mots)
					NULL, 										// Pas de paramètres pour la tâche
					1, 												// Niveau de priorité 1 pour la tâche (0 étant la plus faible) 
					NULL ); 							// Pas d'utilisation du task handle

	
	
	// Lance le scheduler et les taches associées 
	vTaskStartScheduler();
	// On n'arrive jamais ici sauf en cas de problèmes graves: pb allocation mémoire, débordement de pile par ex.
	while(1);
}

