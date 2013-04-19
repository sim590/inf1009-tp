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

typedef struct _connection {
    struct _connection* next;
    char state[2]; // [no_connexion,état_connexion]..
                    // L'état de connexion est 0x00 pour déconnecté et
                    // 0xFF pour connecté
} Connection;

Connection* first_con_node;
Connection* last_con_node;

void deleteAllConnections();
int sendPacketToNet(Connection*, PRIM_PACKET*);
int getMessageFromBuffer(char buffer[],char*);
int getPacketFromNet(PRIM_PACKET*);
Connection* findConnection(char);
Connection* add_connection(char);
void remove_connection(char);
int whatsConState(char);

#endif /* end of include guard: TRANSPORT_4R0LNUHE */
