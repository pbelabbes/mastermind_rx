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
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"


void client_appli (char *serveur, char *service);
static void purger(void);
void afficher_tab(char*);
void start_client();
void level_choice();
void remplir_tab_user();
void show_header(int with);
void show_tab_user(int with);
void show_tab_answer(int with);
void show_screen();

int TAB_SIZE;
struct winsize w;
char* tab_user;
char* tab_answer;
int show_indice;

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
    while ((c = getchar()) != '\n' && c != EOF);
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

void remplir_tab_user(){
	char inserer=0;
	int i;
	for (i = 0; i < TAB_SIZE; ++i)
	{
		printf("Les couleurs sont :\nr : red \ny ; yellow \ng : green \nb : blue \no : orange \np : purple \nf : fushia \nw : white\n");
		printf("Choisissez le pion numéro %d :  ",i+1);
		scanf("%c",&inserer);
		purger();
		*(tab_user+i)=inserer;
		show_screen();
	}
}

void start_client()
{      
	show_indice = 2;
	level_choice();
	purger();
	remplir_tab_user(tab_user);
}

int check_tab(){
  int i;
  for (i=0 ; i<TAB_SIZE;i++ ){
    if ( tab_answer[i] != 'r') return 0;
  }
  return 1;
}

//Gère l'affichage à l'écran
void show_header(int with){
	int i;
	for (i = 0; i < ((with - 19)/2);i++ ) printf("%c",' '); 
	printf("%s\n","-------------------" );
	for (i = 0; i < ((with - 19)/2);i++ ) printf("%c",' '); 
	printf("%s\n","Mastermind en ligne" );
	for (i = 0; i < ((with - 19)/2);i++ ) printf("%c",' '); 
	printf("%s\n","-------------------" );
}

void show_tab_user(int with){
	
	printf("%s\n","Votre grille : " );
	afficher_tab(tab_user);
}

void show_tab_answer(int with){
	printf("%s\n","La grille réponse : " );
	afficher_tab(tab_answer);
	printf("%s\n","r : bonne position, w : présent mais pas bonne position, case vide : absent de la grille" );
}

void show_screen(){
	system("clear");
	ioctl(0, TIOCGWINSZ, &w);
	int with = w.ws_col;
	int height = w.ws_row;
	switch(show_indice){
		case 1 :
			show_header(with);
			break;	
		case 2 :
			show_header(with);
			show_tab_user(with);
			break;
		case 3 : 
			show_header(with);
			show_tab_user(with);
			show_tab_answer(with); 
			break;
		case 4 : 
			show_header(with);
			show_victory(with,height); 
	}
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
	
	printf("L'adresse du serveur est %s et le service est %s\n",serveur,service);

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	client_appli(serveur,service);
}

/*****************************************************************************/
void client_appli (char *serveur,char *service) /* procedure correspondant au traitement du client de votre application */
{
  int to_server_socket = -1;
  struct sockaddr_in serverSockAddr;
  struct hostent *serverHostEnt;
  long hostAddr;
  
  
  bzero(&serverSockAddr,sizeof(serverSockAddr));
  hostAddr = inet_addr(serveur);
  
  serverSockAddr.sin_port = htons(30000);
  serverSockAddr.sin_family = AF_INET;
  //remplit la structure de notre socket avec l'ip du serveur
  inet_aton(serveur, &serverSockAddr.sin_addr.s_addr);
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
  show_indice = 1;
  show_screen();
  tab_user=malloc(sizeof(char)*TAB_SIZE);
  start_client(); // Récupère le niveau de difficulté et la grille de départ du joueur
 
  tab_answer = malloc(sizeof(char)*TAB_SIZE); // Crée la grille dans laquelle seront reçus les réponses
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

  show_indice = 3;
 
  while(!gagner){ //tant que l'utilisateur ne gagne pas
    gagner = check_tab();
    if(gagner) break;
    show_screen();
    remplir_tab_user();

    /* envoie de donne et reception */
    h_writes(to_server_socket,tab_user,sizeof(tab_user));
   
    printf("attente de réception de la grille réponse ...\n");
    // réception du nombre de couleurs trouvé
    h_reads(to_server_socket,tab_answer, sizeof tab_answer);
    
  }
  
  show_indice = 4;
  show_screen();
  
  /* fermeture de la connection */
  h_shutdown(to_server_socket,2);
  h_close(to_server_socket);
}
/*****************************************************************************/

