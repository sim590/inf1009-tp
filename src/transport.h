//---------------------------------------------------------
// Fichier: transport.h    Auteur(s): Simon DESAULNIERs
// Date: 2013-04-12
//---------------------------------------------------------
// Fichier d'entete decrivant le comportment de la couche
// transport.
//---------------------------------------------------------
#ifndef TRANSPORT_4R0LNUHE

#define TRANSPORT_4R0LNUHE
#define S_LEC "./S_LEC"
#define S_ECR "./S_ECR"
#define I_AM "ET"

#include <transNnet.h>



int getMessageFromBuffer(char buffer[],char*);
int writeResults(char*,char*);

#endif /* end of include guard: TRANSPORT_4R0LNUHE */
