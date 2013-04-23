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
#define MAX_WAIT_TIME 3600
#define MAX_PRIM_PACKET_SIZE 262 // octets

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

//-------------------------
// Primitives lors de la
// de l'échange entre la
// couche ET et ER
//-------------------------
typedef enum {
    // Paquet d'établissement de connexion
    N_CONNECT_req,
    N_CONNECT_ind,
    N_CONNECT_resp,
    N_CONNECT_conf,
    // Paquet de transfert de données
    N_DATA_req,
    N_DATA_ind,
    // Paquet de libération de connexion
    N_DISCONNECT_req,
    N_DISCONNECT_ind
} PRIMITIVE;

//-------------------------------------------
// Paquets de communication entre la couche
// transport et réseau
//-------------------------------------------
typedef struct _CON_PRIM_PACKET {
    PRIMITIVE prim;
    char src_addr;
    char dest_addr;
    char con_number;
} CON_PRIM_PACKET;

typedef struct _DATA_PRIM_PACKET {
    PRIMITIVE prim;
    char con_number;
    char transaction[256];
} DATA_PRIM_PACKET;

typedef struct _REL_PRIM_PACKET {
    PRIMITIVE prim;
    char con_number;
    char reason[32];
} REL_PRIM_PACKET;

//----------------------------
// Paquet pour communication
// transport-réseau
//----------------------------
typedef union {
    PRIMITIVE prim;
    CON_PRIM_PACKET con_prim_packet;
    DATA_PRIM_PACKET data_prim_packet;
    REL_PRIM_PACKET rel_prim_packet;
} PRIM_PACKET;

//---------------------------------
// Noeud de la table de connexions
// de la couche transport
//---------------------------------
typedef struct _TRANS_CON {
    union _Connection* next;
    char con_number;
    char state;
} TRANS_CON;

//---------------------------------
// Noeud de la table de connexions
// de la couche réseau
//---------------------------------
typedef struct _NET_CON {
    union _Connection* next;
    char con_number;
    char state;
    char src_addr;
    char dest_addr;
} NET_CON;

typedef union _Connection {
    TRANS_CON tcon;
    NET_CON ncon;
} Connection;

Connection* first_con_node;
Connection* last_con_node;

// Fonctions décrivant l'interface entre les couches
int getPacketFromInterface(PRIM_PACKET*, int);
int sendPacketToInterface(PRIM_PACKET*,int);
void deleteAllConnections();
Connection* findConnection(char);
Connection* add_connection(char,char*,char*);
void remove_connection(char);
int whatsConState(char);


#endif /* end of include guard: TRANSNNET_MDHCQJ5S */
