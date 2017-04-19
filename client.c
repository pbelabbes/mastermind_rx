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
#include <curses.h> 		/* Primitives de gestion d'Žcran */
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
int level_choice();
void remplir_tab_user(char*);



/*****************************************************************************/
/*--------------- programme client -----------------------*/

void afficher_tab(char* tab){
	for (int i = 0; i <4; ++i)
	{
		printf(" tab [%d] = %c\n",i,tab[i] );
	}
}

static void purger(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
    {}
}

int level_choice(){
	int level;
	printf("Choose your level : 1 - Beginner  / 2- Medium / 3- Hard\n");
	scanf("%d",&level);
	if (level<1 || level>3)
	{
		return(level_choice());
	}
	else
		return level;
}

void remplir_tab_user(char* tab){
	char inserer=0;
	for (int i = 0; i <4; ++i)
	{	printf("Choisissez la %dieme couleur ",i);
		scanf("%c",&inserer);
		purger();
		*(tab+i)=inserer;
	}
}

void start_client(char* tab_user)
{	int level;
	level=level_choice();
	purger();
	remplir_tab_user(tab_user);
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
void client_appli (char *serveur,char *service)

/* procedure correspondant au traitement du client de votre application */

{int to_server_socket = -1;
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
if(connect( to_server_socket,
            (struct sockaddr *)&serverSockAddr,
            sizeof(serverSockAddr)) < 0 )
{
  printf("Echec demande de connection\n");
  exit(0);
}
char* tab_user=malloc(sizeof(char)*4);
start_client(tab_user);
/* envoie de donne et reception */
write(to_server_socket,tab_user,sizeof(tab_user));
//read(to_server_socket,buffer,512);
//printf("%s",buffer);
/* fermeture de la connection */
h_shutdown(to_server_socket,2);
h_close(to_server_socket);
}
/*****************************************************************************/

