/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/	

#include <time.h>
#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'残ran */
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"


void client_appli (char *serveur, char *service);
static void purger(void);
void afficher_tab(char*);
void start_client(char*);
void level_choice();
void remplir_tab_user(char*);

int TAB_SIZE;


/*****************************************************************************/
/*--------------- programme client -----------------------*/

void afficher_tab(char* tab){
  int i;	
  printf("\n[");
  for (i = 0; i <TAB_SIZE; ++i)
	{
		printf(" %c |",tab[i] );
	}
  printf("]\n");
}

static void purger(void)
{
    int c;
    printf("Dans la purge\n");
    while ((c = getchar()) != '\n' && c != EOF)
    {
      printf("Purge de %c\n",c);
    }
}

void level_choice(){
	int level;
	printf("Choose your level : 1 - Beginner  / 2- Medium / 3- Hard\n");
	scanf("%d",&level);
	switch(level){
	  
	case 1 : 
	  TAB_SIZE = 4;
	  break;
	case 2 :
	  TAB_SIZE = 5;
	  break;
	case 3 : 
	  TAB_SIZE = 6;
	  break;
	default : 
	  level_choice();
	}
}

void remplir_tab_user(char* tab){
	char inserer=0;
	printf("Les couleurs sont : r,y,g,b,o,p,f,w");
	int i;
	for (i = 0; i < TAB_SIZE; ++i)
	{	printf("Choisissez la %dieme couleur ",i);
		scanf("%c",&inserer);
		purger();
		*(tab+i)=inserer;
	}
	}

void start_client(char* tab_user)
{      
	level_choice();
	purger();
	remplir_tab_user(tab_user);
	printf("\n Taille de la grille : %d\n",TAB_SIZE);
	afficher_tab(tab_user);
}

int check_tab(char* tab_answer){
  int i;
  for (i=0 ; i<TAB_SIZE;i++ ){
    if ( tab_answer[i] != 'r') return 0;
  }
  return 1;
}


int main(int argc, char *argv[])
{

	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	client_appli(serveur,service);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service) /* procedure correspondant au traitement du client de votre application */
{
  int to_server_socket = -1;
  char *server_name = serveur;
  struct sockaddr_in serverSockAddr;
  struct hostent *serverHostEnt;
  long hostAddr;
  long status;
  char buffer[512];
  
  
  bzero(&serverSockAddr,sizeof(serverSockAddr));
  hostAddr = inet_addr(serveur);
  
  serverSockAddr.sin_port = htons(30000);
  serverSockAddr.sin_family = AF_INET;
  inet_aton(server_name, &serverSockAddr.sin_addr.s_addr);
/* creation de la socket */
  if ( (to_server_socket = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
      printf("Echec creation socket client\n");
      exit(0);
    }
  /* requete de connexion */
  if(connect( to_server_socket, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr)) < 0 )
    {
      printf("Echec demande de connection\n");
      exit(0);
    }

  // JEU
  char* tab_user=malloc(sizeof(char)*TAB_SIZE);
  start_client(tab_user); // Récupère le niveau de difficulté et la grille de départ du joueur
 
  char* tab_answer = malloc(sizeof(char)*TAB_SIZE); // Crée la grille dans laquelle seront reçus les réponses
  int gagner = 0; // booléen de victoire
  
  /* envoie de la taille de la grille */
  char tab_size_data[1];
  tab_size_data[0] = '0'+TAB_SIZE;
  printf("Taille envoyée : %c",tab_size_data[0]);
  h_writes(to_server_socket,tab_size_data,sizeof(tab_size_data));
  
  /* envoie de donne et reception */
  h_writes(to_server_socket,tab_user,sizeof(tab_user));
  
  // réception du nombre de couleurs trouvé
  h_reads(to_server_socket,tab_answer, sizeof tab_answer);

   
  while(!gagner){
    gagner = check_tab(tab_answer);
    printf("Vous avez : %d",gagner);
    printf("La table de réponse est la suivante : \n");
    afficher_tab(tab_answer);
    printf("r : bonne position, w : présent dans la grille mais mauvaise position\n");
    remplir_tab_user(tab_user);
    
    printf("Envoie de la grille :\n");
    afficher_tab(tab_user);
    /* envoie de donne et reception */
    h_writes(to_server_socket,tab_user,sizeof(tab_user));
    
    //  printf("Début purge");
    // purger();

    printf("attente de réception de la grille réponse ...\n");
    // réception du nombre de couleurs trouvé
    h_reads(to_server_socket,tab_answer, sizeof tab_answer);
    
  }

  printf("Féliciation vous avez gagné !");

  /* fermeture de la connection */
  h_shutdown(to_server_socket,2);
  h_close(to_server_socket);
}
/*****************************************************************************/

