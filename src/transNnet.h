//---------------------------------------------------------
// Fichier: transNnet.h    Auteur(s): Simon DESAULNIERS
// Date: 2013-04-12
//---------------------------------------------------------
// Fichier d'entete decrivant la communication entre la
// couche transport et la couche reseau.
//---------------------------------------------------------

#ifndef TRANSNNET_MDHCQJ5S

#define TRANSNNET_MDHCQJ5S

#define DEBUG 0
#define MAX_WAIT_TIME 10
#define MAX_PRIM_PACKET_SIZE 262 // octets

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

//-------------------------
// Primitives lors de la
// de l'echange entre la
// couche ET et ER
//-------------------------
typedef enum {
    // Paquet d'etablissement de connexion
    N_CONNECT_req,
    N_CONNECT_ind,
    N_CONNECT_resp,
    N_CONNECT_conf,
    // Paquet de transfert de donnees
    N_DATA_req,
    N_DATA_ind,
    // Paquet de liberation de connexion
    N_DISCONNECT_req,
    N_DISCONNECT_ind
} PRIMITIVE;

//-------------------------------------------
// Paquets de communication entre la couche
// transport et reseau
//-------------------------------------------
typedef struct _CON_PRIM_PACKET {
    PRIMITIVE prim;
    char con_number;
    char src_addr;
    char dest_addr;
} CON_PRIM_PACKET;

typedef struct _DATA_PRIM_PACKET {
    PRIMITIVE prim;
    char con_number;
    char transaction[256];
} DATA_PRIM_PACKET;

typedef struct _REL_PRIM_PACKET {
    PRIMITIVE prim;
    char con_number;
} REL_PRIM_PACKET;

//----------------------------
// Paquet pour communication
// transport-reseau
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
// de la couche reseau
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

// Fonctions decrivant l'interface entre les couches
int getPacketFromInterface(PRIM_PACKET*, int);
int sendPacketToInterface(PRIM_PACKET*,int);
void deleteAllConnections();
Connection* findConnection(char);
Connection* add_connection(char,char*,char*);
void remove_connection(char);
void writePrimPacketToStdOut(PRIM_PACKET*,char*);


#endif /* end of include guard: TRANSNNET_MDHCQJ5S */
