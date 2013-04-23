//---------------------------------------------------------
// Fichier: transport.h    Auteur(s): Simon DÉSAULNIERs
// Date: 2013-04-12
//---------------------------------------------------------
// Fichier d'entête décrivant le comportment de la couche
// transport.
//---------------------------------------------------------
#ifndef TRANSPORT_4R0LNUHE

#define TRANSPORT_4R0LNUHE
#define S_LEC "./S_LEC"
#define S_ECR "./S_ECR"

#include <transNnet.h>



int getMessageFromBuffer(char buffer[],char*);
int writeResults(char*,char*);

#endif /* end of include guard: TRANSPORT_4R0LNUHE */
