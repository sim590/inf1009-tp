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

//-------------------------
// Primitives lors de la
// phase de demande de
// connexion
//-------------------------
typedef enum {
    unsigned int type;
    N_CONNECT.req,
    N_CONNECT.ind,
    N_CONNECT.resp,
    N_CONNECT.conf
} CONNECTION_PRIMITIVE;

//-----------------------
// Primitives lors de la
// phase de transfert de
// données
//-----------------------
typedef enum {
    unsigned int type;
    N_DATA.req,
    N_DATA.ind
} DATA_PRIMITIVE;

//-------------------------
// Primitives lors de la
// phase de libération de
// connexion
//-------------------------
typedef enum {
    unsigned int type;
    N_DISCONNECT.req,
    N_DISCONNECT.ind
} REL_PRIMITIVE;

//--------------------------------
// Primitives envoyées entre 
// l'entité transport et l'entité
// réseau.
//--------------------------------
typedef union {
    unsigned int type;
    CONNECTION_PRIMITIVE con_prim;
    DATA_PRIMITIVE data_prim;
    REL_PRIMITIVE rel_prim;
} TRANS_NET_LAYER_PRIMITIVE;

#endif /* end of include guard: TRANSNNET_MDHCQJ5S */
