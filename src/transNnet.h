//---------------------------------------------------------
// Fichier: transNnet.h    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-12
//---------------------------------------------------------
// Fichier d'entête décrivant la communication entre la
// couche transport et la couche réseau.
//---------------------------------------------------------

#ifndef TRANSNNET_MDHCQJ5S

#define TRANSNNET_MDHCQJ5S

#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
    char src_addr;
    char dest_addr;
} CON_PRIM_PACKET;

typedef struct _DATA_PRIM_PACKET {
    short int type;
    DATA_PRIMITIVE prim;
    char transaction[256];
} DATA_PRIM_PACKET;

typedef struct _REL_PRIM_PACKET {
    short int type;
    REL_PRIMITIVE prim;
    char dest_addr;
    char reason[32];
} REL_PRIM_PACKET;

//----------------------------
// Paquet pour communication
// transport-réseau
//----------------------------
typedef union {
    short int type;
    CON_PRIM_PACKET con_prim_packet;// type = 0
    DATA_PRIM_PACKET data_prim_packet;// type = 1
    REL_PRIM_PACKET rel_prim_packet; // type = 2
} PRIM_PACKET;

#endif /* end of include guard: TRANSNNET_MDHCQJ5S */
