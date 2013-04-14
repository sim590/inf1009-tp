//---------------------------------------------------------
// Fichier: network.h    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-12
//---------------------------------------------------------
// Fichier d'entête décrivant le comportement de la couche
// réseau.
//---------------------------------------------------------

#ifndef NETWORK_UNQBN59Z

#define NETWORK_UNQBN59Z

#include <transNnet.h>


// Type de paquet lors de la phase
// de connexion
    /* appel = 00001011
     * connexion établie = 00001111
     * libération = 00010011
     */ 
typedef enum {
    CALLING=0x09,
    CON_ind=0x0F,
    REL_ind=0x13,
    NET_REFUSED=0x01,
    USER_REFUSED=0x02
} CONNECTION_PACKET_TYPE;

//typedef struct _COMM_ET_ER {
//    CONNECTION_PRIMITIVE prim;
//    char src_addr;
//    char dest_addr;
//} COMM_ET_ER;

//----------------------------------
// Paquets lors la phase 
// d'établissement de connexion
//----------------------------------
typedef struct _CONNECTION_PACKET {
    unsigned int type;
    CONNECTION_PACKET_TYPE con_type; // {CALL,CONTD,REL} 
    char con_number;
    char source_addr; // adresses [0,249]
    char dest_addr;
    char reason[32]; // raison de la libération de connexion 
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
    unsigned int type;
    char con_number;
    char type; 
    char segm_data[128];
} DATA_PACKET;

//--------------------------
// Paquets lors de la phase
// de libération
//--------------------------
typedef struct _RELEASE_PACKET {
    unsigned int type;
    char con_number;
    char type=0x13;
    char source_addr;
    char dest_addr;
} RELEASE_PACKET;

//----------------------------
// Network Protocol Data Unit
//----------------------------
typedef union {
    unsigned int type;
    CONNECTION_PACKET con_packet;
    DATA_PACKET data_packet;
    RELEASE_PACKET rel_packet;
} NPDU;

#endif /* end of include guard: NETWORK_UNQBN59Z */
