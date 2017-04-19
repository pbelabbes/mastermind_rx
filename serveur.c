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
void afficher_tab1(char*);
int jouer_tour(char*,char*,char*);
int partie(char*,char*,char*,int);

void serveur_appli (char *service);   /* programme serveur */

int TAB_SIZE;

char buffer[512];
int ma_socket;
/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/
void afficher_tab1(char* tab){
  int i;
  for (i = 0; i <TAB_SIZE; ++i)
    {
      printf(" tab [%d] = %c\n",i,tab[i] );
    }
}

static void purger1(void)
{
  int c;
  
  while ((c = getchar()) != '\n' && c != EOF)
    {}
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
  
  /* service est le service (ou num�ro de port) auquel sera affect�
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
  mon_address.sin_port = htons(30000);
  mon_address.sin_family = AF_INET;
  mon_address.sin_addr.s_addr = htonl(INADDR_ANY);
  
  /* creation de socket */
  if ((ma_socket = socket(AF_INET,SOCK_STREAM,0))== -1)
    {
      printf("ca chie avec la creation\n");
      exit(0);
    }
  /* bind serveur - socket */
  bind(ma_socket,(struct sockaddr *)&mon_address,sizeof(mon_address));
  
  /* ecoute sur la socket */
  listen(ma_socket,5);
  
  /* accept la connexion */
  mon_address_longueur = sizeof(client_address);
  
  client_socket = accept(ma_socket,(struct sockaddr *)&client_address,&mon_address_longueur);
  
  //JEU
  
  /* Récupération de la taille de la grille */
  char tab_size_data[1];
  h_reads(client_socket,tab_size_data, sizeof(tab_size_data));
  TAB_SIZE = atoi (tab_size_data);

  
  char* tab_user=malloc(sizeof(char)*TAB_SIZE); // grille joueur
  char* tab_to_find=malloc(sizeof(char)*TAB_SIZE); // grille à trouver
  char* tab_answer=malloc(sizeof(char)*TAB_SIZE); // grille réponse
  
  init_tab_find(tab_to_find); //crée une grille à trouver de manière aléatoire
  
  int partie1=partie(tab_user,tab_to_find,tab_answer,client_socket);
  if (partie1==0) printf("Fin de la partie\n");
   
  //printf("le serveur a recu: %s\n",buffer);
  //sprintf(buffer,"%s du serveur",buffer);
  //write(client_socket,buffer, 512);
  shutdown(client_socket,2);
  close(client_socket);
}


/******************************************************************************/	



void init_tab_find(char* tab){
  srand(time(NULL));
  char couleur[NB_COLOUR]={'r','y','g','b','o','p','f'};//red, yellow, green, blue, orange, purple, fushia
  int indice=0;
  int i;
  for (i = 0; i <4; ++i)
    {	//fonction random pour faire un tabfind aléatoire
      indice= rand()%NB_COLOUR;
      *(tab+i)=couleur[indice];
    }
}




int jouer_tour(char* tab_user,char* tab_to_find,char* tab_answer){
  int r = 0;

  int used[TAB_SIZE];
  int tmp;
  for (tmp = 0; tmp < TAB_SIZE; tmp++) used[tmp] = 0;
  
  
  int i;
  for( i = 0; i<TAB_SIZE;i++){
    char cc = tab_user[i];
    if (tab_answer[i] != 'r' || tab_answer[i] != 'w'){
      
      if(cc = tab_to_find[i] && used[i] != 0){
	tab_answer[i] = 'r';
	used[i] = 1;
	r++;
      }else{
	int j;
	for (j = 0; j < TAB_SIZE;j++){
	  if(cc == tab_to_find[j] && used[j] != 0){
	    if(tab_user[j] == tab_to_find[j]){ 
	      tab_answer[j] = 'r';
	      r++;
	    } else {
	      tab_answer[i]= 'w';
	    } 
	    used[j] = 1;
	  }else tab_answer[i] = ' ';
	}
      }
    }
  }
  return (r == TAB_SIZE);
}




int partie(char* tab_user,char* tab_to_find,char* tab_answer,int client_socket)
{	
  //affiche le tableaux à trouver(utile pour tester le programme)
  printf("(REPONSE= \n");
  afficher_tab1(tab_to_find);
  printf(")");
  //effectue un tour(test si tab user== tab find)
  
  // ATTENTE DE LA GRILLE DU JOUEUR
  h_reads(client_socket,tab_user,sizeof(tab_user));
  
  while (!(jouer_tour(tab_user,tab_to_find,tab_answer))){
    
    // ATTENTE DE LA NOUVELLE GRILLE DU JOUEUR
    h_reads(client_socket,tab_user,sizeof(tab_user));
  
  } 
  return 0 ;
}
