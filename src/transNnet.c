//-------------------------------------------------------
// Fichier: transNnet.c    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-20
//-------------------------------------------------------
// Interface entre la couche réseau et transport.
//-------------------------------------------------------

#include <transNnet.h>


//-----------------------------------------
// Écoute la réponse de la couche désirée
// -1: erreur..
// redFromPipe: Nombre de caractères
// lus du pipe
//-----------------------------------------
int getPacketFromInterface(PRIM_PACKET* p, int fd)
{
    int redFromPipe, count = 0;
    char buffer[MAX_PRIM_PACKET_SIZE];

    do
    {
        redFromPipe = read(fd,buffer,sizeof(PRIM_PACKET));
        if (count && count < 4) {
            sleep(1);
        }
        else if (count > MAX_WAIT_TIME) {
            fprintf(stderr, "Erreur: Aucune réponse du fournisseur réseau pour une N_CONNECT.REQ après %i secondes..\n",MAX_WAIT_TIME);
            return -1;
        }
        count++;
    }
    while (redFromPipe < 1);

    // Reconstruction du paquet
    p->prim = atoi(buffer); // 4 premiers octets du buffer
    switch(p->prim)
    {
        // Paquet d'établissement de connexion
        case N_CONNECT_req:
        case N_CONNECT_ind:
        case N_CONNECT_resp:
        case N_CONNECT_conf:
            p->con_prim_packet.src_addr = buffer[5];
            p->con_prim_packet.dest_addr = buffer[6];
            break;
        // Paquet de transfert de données
        case N_DATA_req:
        case N_DATA_ind:
            p->data_prim_packet.con_number = buffer[5]; // 6e octet.. On saute le \0 après les 4 premiers octets
            strcpy(p->data_prim_packet.transaction,buffer+6);
            break;
        // Paquet de libération de connexion
        case N_DISCONNECT_req:
        case N_DISCONNECT_ind:
            p->rel_prim_packet.con_number = buffer[5];
            strcpy(p->rel_prim_packet.reason,buffer+6);
            break;
    }
   
    return redFromPipe;
}

//-----------------------------------------
// Envoie d'un paquet à la couche désirée
// Retourne:
// -1: erreur..
// writtenToPipe: Nombre de caractères
// écrits dans le pipe
//-----------------------------------------
int sendPacketToInterface(PRIM_PACKET* p, int fd)
{
    int writtenToPipe, count = 0, i;
    char buffer[MAX_PRIM_PACKET_SIZE];

    // Construction d'un buffer
    for (i = 0; i < 4; i++)
        buffer[i] = (p->prim << i*8) >> 24;
    buffer[4] = '\0';
    switch(p->prim)
    {
        // Paquet d'établissement de connexion
        case N_CONNECT_req:
        case N_CONNECT_ind:
        case N_CONNECT_resp:
        case N_CONNECT_conf:
            buffer[5] = p->con_prim_packet.src_addr;
            buffer[6] = p->con_prim_packet.dest_addr;
            break;
        // Paquet de transfert de données
        case N_DATA_req:
        case N_DATA_ind:
            buffer[5] = p->data_prim_packet.con_number;
            strcpy(buffer+6,p->data_prim_packet.transaction);
            break;
        // Paquet de libération de connexion
        case N_DISCONNECT_req:
        case N_DISCONNECT_ind:
            buffer[5] = p->rel_prim_packet.con_number;
            strcpy(buffer+6,p->rel_prim_packet.reason);
            break;
    }


    do
    {
        writtenToPipe = write(fd,buffer,sizeof(PRIM_PACKET));
    
        if (count && count < 4) {
            sleep(1);
        }
        else if (count > MAX_WAIT_TIME) {
            fprintf(stderr, "Erreur: Impossible d'écrire dans le pipe après %i secondes.\n",MAX_WAIT_TIME);
            return -1;
        }
        count++;
    }
    while (writtenToPipe < 1);
    
    return writtenToPipe;
}

//------------------------------
// Vide la liste de connexions
//------------------------------
void deleteAllConnections()
{
    Connection* node = first_con_node, *prev_node = first_con_node;
    
    while (node != NULL) {
        node = node->next;
        free(prev_node);
        prev_node = node;
    }
}

//-----------------------------------------------
// Indique si la connexion est existante ou non
//-----------------------------------------------
Connection* findConnection(char con_number)
{
    Connection* node = first_con_node;
    
    while (node != NULL) {
        if (node->state[0] == con_number) {
            return node;
        }
    }

    return NULL;
}

//---------------------------------------
// Ajout d'un noeud de connexion à la
// liste chaînée de connexions
//---------------------------------------
Connection* add_connection(char con_number)
{
    Connection* node = malloc(sizeof(Connection));
    node->state[0] = con_number;
    node->state[1] = 0x00;
    node->next = NULL;

    // Si la liste est vide
    if (first_con_node == last_con_node && first_con_node == NULL) {
        first_con_node = last_con_node = node;
        return;
    }

    last_con_node->next = node;
    last_con_node = node;

    return node;
}

//--------------------------------
// Suppression d'un noeud de
// la liste chaînée de connexions
//--------------------------------
void remove_connection(char con_number)
{
    Connection* node = first_con_node, *prev_node = node;

    while (node != NULL) {
        if (node->state[0] == con_number) {
            // Le seul élément de la liste
            if (node == first_con_node && node == last_con_node) {
                free(node);
                first_con_node = last_con_node = NULL;
                return;
            }
            // Le premier, mais pas le dernier
            else if (node == first_con_node) {
                first_con_node = node->next;
                free(node);
                return;
            }
            // Le dernier, mais pas le premier
            else if (node == last_con_node) {
                last_con_node = prev_node;
                prev_node->next = NULL;
                free(node);
                return;
            }
            // Pas le premier ni le dernier
            else {
                prev_node->next = node->next;
                free(node);
                return;
            }
        }
        // On avance
        if (node != first_con_node)
            prev_node = node;
        node = node->next;
    }
}

//-----------------------------------
// Indique l'état de la connexion:
// 0: Non-connecté
// 1: Connecté
// -1: Non-existante
//-----------------------------------
int whatsConState(char con_number)
{
    Connection* node = first_con_node;

    while (node!=NULL) {
         if (node->state[0] == con_number) {
            return node->state[0]; // 0x00 ou 0x01
         }
    }

    return -1;
}



