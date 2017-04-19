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
void init_tab_find(char*);
static void purger1(void);
void afficher_tab1(char*);
int jouer_tour(char*,char*);
int partie(char*,char*,int);

void serveur_appli (char *service);   /* programme serveur */


char buffer[512];
int ma_socket;
/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/
void afficher_tab1(char* tab){
	for (int i = 0; i <4; ++i)
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

	char* tab_user=malloc(sizeof(char)*4);
	char* tab_to_find=malloc(sizeof(char)*4);
	init_tab_find(tab_to_find);
 	read(client_socket,tab_user,sizeof(tab_user));
 	afficher_tab1(tab_user);
 	int partie1=partie(tab_user,tab_to_find,client_socket);
 	if (partie1==0)
 	{
 		printf("Echec\n");
 	}
 	else
 		printf("c'est gagné\n");
    //printf("le serveur a recu: %s\n",buffer);
    //sprintf(buffer,"%s du serveur",buffer);
    //write(client_socket,buffer, 512);
    shutdown(client_socket,2);
    close(client_socket);
}


/******************************************************************************/	



void init_tab_find(char* tab){
	 srand(time(NULL));
	 char couleur[5]={'r','v','n','j','b'};
	 int indice=0;
	for (int i = 0; i <4; ++i)
	{	//fonction random pour faire un tabfind aléatoire
		indice= rand()%5;
		*(tab+i)=couleur[indice];
	}
}




int jouer_tour(char* tab_user,char* tab_to_find){
	int nbr_valide=0;
	int nbr_semi_valide=0;
	int nbr_invalide=0;
	int j=0;
	/*do{	
		for (int i = 0; i <4; ++i)
		{			
			if(tab_user[j]==tab_to_find[i] && i==j){
				nbr_valide++;
				printf("la couleur %c en position %d  est valide  et à la bonne place\n",tab_user[j],j );
				j++;
				i=4;
			}
			else if(tab_user[j]==tab_to_find[i] && i!=j){
				nbr_semi_valide++;
				printf("la couleur %c en position %d  est valide mais pas à la bonne place\n",tab_user[j],j );
				j++;
				i=4;
			}
			else
				nbr_invalide++;
		}
	j++;
	}while(j<4); */
	//VERIFIE SI C'EST BIEN PLACE
	for (int i = 0; i < 4; ++i)
	{
		if (tab_user[i]==tab_to_find[i])
		{
			printf("Le couleur position %d est bien place\n",i );
			nbr_valide++;

		}
	}
	
	if (nbr_valide==4)
	{
		return 4;
	}
	else
		return nbr_valide;

}




int partie(char* tab_user,char* tab_to_find,int client_socket)
{	//on cree le tableaux à trouver
	
	//affiche le tableaux à trouver(utile pour tester le programme)
	printf("(REPONSE= \n");
	afficher_tab1(tab_to_find);
	printf(")");
	int test=0;
	//effectue un tour(test si tab user== tab find)
	test=jouer_tour(tab_user,tab_to_find);
	if (test==4)
	{
		printf("Vous avez gagne :)\n");
		return 1;
	}
	else{
		printf("Vous avez trouve %d couleurs \n",test);
		printf("nbr envoyé = %d\n",test );
		send(client_socket,&test,sizeof(test),0);
		free(tab_to_find);
		return 0;
	}
		 // si partie retourne 0, il faut que le serveur renvoie le nombre de batonnet valide et redemande un tableaux user.	

}