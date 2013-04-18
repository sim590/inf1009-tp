//---------------------------------------------------------
// Fichier: transNnet.h    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-12
//---------------------------------------------------------
// Fichier d'entête décrivant la communication entre la
// couche transport et la couche réseau.
//---------------------------------------------------------

#ifndef TRANSNNET_MDHCQJ5S

#define TRANSNNET_MDHCQJ5S

#include <network.h>
#include <transport.h>
#include <pthread.h>

//-------------------------
// Primitives lors de la
// phase de demande de
// connexion
//-------------------------
typedef enum {
    N_CONNECT_req,
    N_CONNECT_ind,
    N_CONNECT_resp,
    N_CONNECT_conf
} CONNECTION_PRIMITIVE;

//-----------------------
// Primitives lors de la
// phase de transfert de
// données
//-----------------------
typedef enum {
    N_DATA_req,
    N_DATA_ind
} DATA_PRIMITIVE;

//-------------------------
// Primitives lors de la
// phase de libération de
// connexion
//-------------------------
typedef enum {
    N_DISCONNECT_req,
    N_DISCONNECT_ind
} REL_PRIMITIVE;

//-------------------------------------------
// Paquets de communication entre la couche
// transport et réseau
//-------------------------------------------
typedef struct _CON_PRIM_PACKET {
    short int type;
    CONNECTION_PRIMITIVE prim;
} CON_PRIM_PACKET;

typedef struct _DATA_PRIM_PACKET {
    short int type;
    DATA_PRIMITIVE prim;
    char transaction[256];
} DATA_PRIM_PACKET;

typedef struct _REL_PRIM_PACKET {
    short int type;
    REL_PRIMITIVE prim;
} REL_PRIM_PACKET;

//----------------------------
// Paquet pour communication
// transport-réseau
//----------------------------
typedef union {
    short int type;
    CON_PRIM_PACKET con_prim_packet;
    DATA_PRIM_PACKET data_prim_packet;
    REL_PRIM_PACKET rel_prim_packet;
} PRIM_PACKET;

#endif /* end of include guard: TRANSNNET_MDHCQJ5S */
