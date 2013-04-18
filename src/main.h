//---------------------------------------------------------
// Fichier: main.h    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-12
//---------------------------------------------------------
// Fichier d'entête du fichier main.c contenant toutes les
// importations de librairies nécessaires.
//---------------------------------------------------------
#ifndef MAIN_D1ZUXH2C

#define MAIN_D1ZUXH2C

#define TRANS_PATH "./transport-entity"
#define TRANS_PATH_LENGTH 18
#define NET_PATH "./network-entity"
#define NET_PATH_LENGTH 16
#define DEBUG 1


#include <transNnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char** makeArgArray(int,int);

#endif /* end of include guard: MAIN_D1ZUXH2C */
