//---------------------------------------------------------
// Fichier: network.h    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-12
//---------------------------------------------------------
// Fichier d'entête décrivant le comportement de la couche
// réseau.
//---------------------------------------------------------

#ifndef NETWORK_UNQBN59Z

#define NETWORK_UNQBN59Z

#define L_LEC "./L_LEC"
#define L_ECR "./L_ECR"
#define MAX_PACKET_SIZE 163

#include <transNnet.h>

// Type de paquet lors de la phase
// de connexion
    /* appel = 00001011
     * connexion établie = 00001111
     * libération = 00010011
     */ 
typedef enum {
    CON_req=0x0B,
    CON_conf=0x0F,
    REL_ind=0x13,
} CONNECTION_PACKET_TYPE;

typedef enum {
    REMOTE_DENY=0x01,
    NET_DENY=0x02
} REL_REASON;

//----------------------------------
// Paquets lors la phase 
// d'établissement de connexion
//----------------------------------
typedef struct _CONNECTION_PACKET {
    int packet_type;
    CONNECTION_PACKET_TYPE con_packet_type;
    char con_number;
    char src_addr; // adresses [0,249]
    char dest_addr;
    REL_REASON reason; // raison de la libération de connexion 
} CONNECTION_PACKET;

//----------------------------
// Paquets lors de la phase 
// du transfert de données
//----------------------------
/*type = [p(r),M,p(s),N]
*    SI DATA
*    ---------
*        p(r) et p(s) sont sur 3 bits chacun
*        p(r) et p(s) incrémentent modulo 8.. Je crois que 
*        p(r) = [(p(s)+1) mod 8]
*        M est un seul bit: 0 si dernier segment, 1 sinon
*    SI ACQUITEMENT OU ACQUITEMENT NÉGATIF
*    --------------------------------------
*       M=0
*       N=1
*       - POSITIF
*           p(s)=000
*       - NÉGATIF
*           p(s)=100
*/
typedef struct _DATA_PACKET {
    int packet_type;
    char type; 
    char con_number;
    char segm_data[128];
} DATA_PACKET;

//--------------------------
// Paquets lors de la phase
// de libération
//--------------------------
typedef struct _RELEASE_PACKET {
    int packet_type;
    char type;
    char con_number;
    char src_addr;
    char dest_addr;
} RELEASE_PACKET;

//----------------------------
// Network Protocol Data Unit
//----------------------------
typedef union {
    int packet_type; // 0: CONNECTION_PACKET, 1: DATA_PACKET, 2: RELEASE_PACKET
    CONNECTION_PACKET con_packet;
    DATA_PACKET data_packet;
    RELEASE_PACKET rel_packet;
} PACKET;

int sendPacketToDataLinkLayer(PACKET*,char*);
int genRemotePacketResponse(PACKET*);

#endif /* end of include guard: NETWORK_UNQBN59Z */

