/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/	

#include <time.h>
#include <stdio.h>
#include <curses.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>



#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define NB_COLOUR 8
void init_tab_find(char*);
static void purger1(void);
int creation_processus();
void afficher_tab1(char*);
int jouer_tour(char*,char*,char*);
int partie(char*,char*,char*,int);

void serveur_appli (char *service);   /* programme serveur */

int TAB_SIZE;

char buffer[512];
int socket_passive;
/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/
void afficher_tab1(char* tab){
  printf("\n[");
  int i;
  for (i = 0; i <TAB_SIZE; ++i)
    {
      printf("%c | ",tab[i] );
    }
  printf("]\n");
}

static void purger1(void)
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}


int main(int argc,char *argv[])
{
  
  char *service= SERVICE_DEFAUT; /* numero de service par defaut */
  
  
  /* Permet de passer un nombre de parametre variable a l'executable */
  switch (argc)
    {
    case 1:
      printf("defaut service = %s\n", service);
      break;
    case 2:
      service=argv[1];
      break;
      
    default :
      printf("Usage:serveur service (nom ou port) \n");
      exit(1);
    }
  
  /* service est le service (ou numŽro de port) auquel sera affectŽ
     ce serveur*/
  
  serveur_appli(service);
}


/******************************************************************************/	
void serveur_appli(char *service)
  
/* Procedure correspondant au traitemnt du serveur de votre application */
  
{
  
  int client_socket;
  struct sockaddr_in mon_address, client_address;
  int mon_address_longueur, lg;
  
  bzero(&mon_address,sizeof(mon_address));
  //on remplit la structure de notre adresse
  mon_address.sin_port = htons(30000);
  mon_address.sin_family = AF_INET;
  mon_address.sin_addr.s_addr = htonl(INADDR_ANY);
  
  //creation de socket
  if ((socket_passive = socket(AF_INET,SOCK_STREAM,0))== -1)
    {
      exit(0);
    }
  // bind serveur - socket
  bind(socket_passive,(struct sockaddr *)&mon_address,sizeof(mon_address));
  
  // ecoute sur la socket

  listen(socket_passive,2);

  //accept la connexion
    mon_address_longueur = sizeof(client_address);
    for (;;) // on boucle 
    {
      //on accepte la connexion avec le client
      client_socket = accept(socket_passive,(struct sockaddr *)&client_address,&mon_address_longueur);
    int pid=creation_processus();
    if (pid==0){
      // processus fils, on ferme la socket serveur
      close(socket_passive);
    }
    else
      //processus père, on ferme la socket client
      close(client_socket);
      //Jeu
  
  //on récup la taille de la grille
  char tab_size_data[1];
  //lit la donnée envoyé par le client
  h_reads(client_socket,tab_size_data, sizeof(tab_size_data));
  TAB_SIZE = atoi (tab_size_data);
  printf("Taille du tableau : %c\n", tab_size_data[0]);
  
  char* tab_user=malloc(sizeof(char)*TAB_SIZE); // grille joueur
  static char* tab_to_find=malloc(sizeof(char)*TAB_SIZE); // grille à  trouver
  char* tab_answer=malloc(sizeof(char)*TAB_SIZE); // grille réponse
  
  init_tab_find(tab_to_find); //créee une grille à  trouver de manière aléatoire
  
  int partie1=partie(tab_user,tab_to_find,tab_answer,client_socket);
  if (partie1==0) printf("Fin de la partie\n");
     
    }
    
  shutdown(client_socket,2);
  //ferme la socket client
  close(client_socket);
}


/******************************************************************************/	
int creation_processus(){
  int pid;
  //on crée notre processus avec le pid associé?(0 pour le fils)
  pid=fork();
  return pid;
}


void init_tab_find(char* tab){
  srand(time(NULL));
  char couleur[NB_COLOUR]={'r','y','g','b','o','p','f','w'};//red, yellow, green, blue, orange, purple, fushia,white
  int indice=0;
  int i;
  for (i = 0; i <TAB_SIZE; ++i)
    {	//fonction random pour faire un tabfind alÃ©atoire
      indice= rand()%NB_COLOUR;
      *(tab+i)=couleur[indice];
    }
}

void nettoyerReponse(char* tab_answer){
  int i;
  for (i = 0; i < TAB_SIZE; i++) tab_answer[i] = ' ';
}


int jouer_tour(char* tab_user,char* tab_to_find,char* tab_answer){
  int r = 0;

  int used[TAB_SIZE];
  int tmp;
  for (tmp = 0; tmp < TAB_SIZE; tmp++) used[tmp] = 0;
  
  
  int i;
  for( i = 0; i<TAB_SIZE;i++){
    char cc = tab_user[i];
    printf("Caractère courant : %d est %c\n",i,tab_user[i]);
    afficher_tab1(tab_answer);
    if (tab_answer[i] != 'r' && tab_answer[i] != 'w'){
      //printf("tab_answer[%d] != 'r' et 'w'\n",i);
      for (tmp = 0; tmp<TAB_SIZE; tmp++) printf("%d|",used[tmp]);
      printf("\n");

      if(cc == tab_to_find[i] && used[i] == 0){
	printf("%c = %c pour i = %d\n",tab_to_find[i],tab_user[i],i);
	tab_answer[i] = 'r';
	used[i] = 1;
	r++;
      }else{
	int j;
	for (j = 0; j < TAB_SIZE;j++){
	  if(cc == tab_to_find[j] && used[j] == 0){
	    if(tab_user[j] == tab_to_find[j]){ 
	      printf("%c = %c pour j = %d\n",tab_to_find[j],tab_user[j],j);
	      tab_answer[j] = 'r';
	      r++;
	    } else {
	      tab_answer[i]= 'w';
	      printf("%c = %c pour i = %d et j =%d\n",tab_to_find[j],tab_user[i],i,j);
	    } 
	    used[j] = 1;
	    break;
	  }else tab_answer[i] = ' ';
	}
      }
    }
  }
  printf("Table réponse\n");
  afficher_tab1(tab_answer);
  return (r == TAB_SIZE);
}




int partie(char* tab_user,char* tab_to_find,char* tab_answer,int client_socket)
{	
  //affiche le tableaux Ã  trouver(utile pour tester le programme)
  printf("(Grille à  trouver = ");
  afficher_tab1(tab_to_find);
  //effectue un tour(test si tab user== tab find)
  
  // ATTENTE DE LA GRILLE DU JOUEUR
  h_reads(client_socket,tab_user,sizeof(tab_user));
  printf("Grille reçue");
  afficher_tab1(tab_user);
  int continu = 1;
  while (continu){
    continu = !(jouer_tour(tab_user,tab_to_find,tab_answer));
    printf("\nEnvoie tab_answer : \n");
    afficher_tab1(tab_answer);
    h_writes(client_socket,tab_answer,sizeof(tab_answer));

    nettoyerReponse(tab_answer);
    
    if( !continu ) break;
    // ATTENTE DE LA NOUVELLE GRILLE DU JOUEUR
    printf("Attente d'un nouvelle table ...\n");
    h_reads(client_socket,tab_user,sizeof(tab_user));
    
  } 
  return 0 ;
}
