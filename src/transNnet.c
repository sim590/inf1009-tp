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
        redFromPipe = read(fd,buffer,sizeof(buffer));
        if (count && count < MAX_WAIT_TIME) {
            sleep(1);
        }
        else if (count > MAX_WAIT_TIME) {
            fprintf(stderr, "Erreur: Aucune réponse après %i secondes d'écoute du tuyau de descripteur de fichier %i..\n",MAX_WAIT_TIME,fd);
            return -1;
        }
        count++;
    }
    while (redFromPipe < 1);

    // Reconstruction du paquet
    p->prim = buffer[0];
    switch(p->prim)
    {
        // Paquet d'établissement de connexion
        case N_CONNECT_req:
        case N_CONNECT_ind:
        case N_CONNECT_resp:
        case N_CONNECT_conf:
            p->con_prim_packet.src_addr = buffer[1];
            p->con_prim_packet.dest_addr = buffer[2];
            p->con_prim_packet.con_number = buffer[3];
            break;
        // Paquet de transfert de données
        case N_DATA_req:
        case N_DATA_ind:
            p->data_prim_packet.con_number = buffer[1]; // 6e octet.. On saute le \0 après les 4 premiers octets
            strcpy(p->data_prim_packet.transaction,buffer+2);
            break;
        // Paquet de libération de connexion
        case N_DISCONNECT_req:
        case N_DISCONNECT_ind:
            p->rel_prim_packet.con_number = buffer[1];
            strcpy(p->rel_prim_packet.reason,buffer+2);
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
    int writtenToPipe, count = 0;
    char buffer[MAX_PRIM_PACKET_SIZE];

    // Construction d'un buffer
    buffer[0] = (char) p->prim;
    switch(p->prim)
    {
        // Paquet d'établissement de connexion
        case N_CONNECT_req:
        case N_CONNECT_ind:
        case N_CONNECT_resp:
        case N_CONNECT_conf:
            buffer[1] = p->con_prim_packet.src_addr;
            buffer[2] = p->con_prim_packet.dest_addr;
            buffer[3] = p->con_prim_packet.con_number;
            break;
        // Paquet de transfert de données
        case N_DATA_req:
        case N_DATA_ind:
            buffer[1] = p->data_prim_packet.con_number;
            strcpy(buffer+2,p->data_prim_packet.transaction);
            break;
        // Paquet de libération de connexion
        case N_DISCONNECT_req:
        case N_DISCONNECT_ind:
            buffer[1] = p->rel_prim_packet.con_number;
            strcpy(buffer+2,p->rel_prim_packet.reason);
            break;
    }

    do
    {
        writtenToPipe = write(fd,buffer,sizeof(buffer));
    
        if (count > 3) {
            fprintf(stderr, "Erreur: Impossible d'écrire dans le tuyau de descripteur de fichier %i\n",fd);
            return -1;
        }
        count++;
    }
    while (writtenToPipe < 0);
    
    return writtenToPipe;
}

//------------------------------
// Vide la liste de connexions
//------------------------------
void deleteAllConnections()
{
    Connection* node = first_con_node, *prev_node = first_con_node;
    
    while (node != NULL) {
        node = node->ncon.next;
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
        if (node->ncon.con_number == con_number) {
            return node;
        }
        node = node->ncon.next;
    }

    return NULL;
}

//---------------------------------------
// Ajout d'un noeud de connexion à la
// liste chaînée de connexions
//---------------------------------------
Connection* add_connection(char con_number, char* src_addr, char* dest_addr)
{
    Connection* node = (Connection*) malloc(sizeof(Connection));
    node->ncon.con_number = con_number;
    node->ncon.state = 0x00;
    if (src_addr != NULL && dest_addr != NULL) {
        node->ncon.src_addr = *src_addr;
        node->ncon.dest_addr = *dest_addr;
    }
    node->ncon.next = NULL;

    // Si la liste est vide
    if (first_con_node == last_con_node && first_con_node == NULL) {
        first_con_node = last_con_node = node;
        return node;
    }

    last_con_node->ncon.next = node;
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
        if (node->ncon.con_number == con_number) {
            // Le seul élément de la liste
            if (node == first_con_node && node == last_con_node) {
                free(node);
                first_con_node = last_con_node = NULL;
                return;
            }
            // Le premier, mais pas le dernier
            else if (node == first_con_node) {
                first_con_node = node->ncon.next;
                free(node);
                return;
            }
            // Le dernier, mais pas le premier
            else if (node == last_con_node) {
                last_con_node = prev_node;
                prev_node->ncon.next = NULL;
                free(node);
                return;
            }
            // Pas le premier ni le dernier
            else {
                prev_node->ncon.next = node->ncon.next;
                free(node);
                return;
            }
        }
        // On avance
        if (node != first_con_node)
            prev_node = node;
        node = node->ncon.next;
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
         if (node->ncon.con_number == con_number) {
            return node->ncon.state; // 0x00 ou 0x01
         }
    }

    return -1;
}




