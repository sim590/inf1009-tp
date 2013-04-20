//-------------------------------------------------------
// Fichier: transport.c    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-12
//-------------------------------------------------------
// Routines que la couche transport a à accomplir.
//-------------------------------------------------------

#include <transport.h>

int transToNet_pipe,netToTrans_pipe;
Connection* first_con_node = NULL;
Connection* last_con_node = NULL;

// Point d'entrée dans le programme
int main(int argc,char** argv)
{
    FILE* transaction_file = fopen(S_LEC,"r");
    if (transaction_file == NULL) {
        fprintf(stderr,"Impossible d'ouvrir le fichier S_LEC\n");
        return -1;
    }
    char line_buffer[256], message[256];
    int i, flags;
    Connection* connection;
    PRIM_PACKET p;

    // Récupération des références au tuyaux
    // (respectivement l'écriture et l'écoute)
    transToNet_pipe = atoi(argv[1]); netToTrans_pipe = atoi(argv[2]);
    
    // On s'assure que les file descriptors sont en mode 0_NONBLOCK
    flags = fcntl(transToNet_pipe,F_GETFL,0);
    fcntl(transToNet_pipe,F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(netToTrans_pipe,F_GETFL,0);
    fcntl(netToTrans_pipe,F_SETFL, flags | O_NONBLOCK);

    if (DEBUG) printf("TRANSPORT\nMes fd sont:\n%i,%i\n",netToTrans_pipe,transToNet_pipe);
    // Lecture du fichier S_LEC et envoie des requêtes et écoute
    // de la réponse.
    
    //ALGO
    //--------
    //TANT QUE FICHIER_NON_VIDE
        //LIRE S_LEC
        
        //REGARDER_TABLE_DE_CONNEXIONS
        
        //SI CONNEXION_EXISTE_PAS
            //ENVOYER_CONNECTION_REQ
            //ECOUTER_REPONSE
            
            //SI REPONSE_POSITIVE
                //CHANGER_ETAT_CONNEXION_TABLE_CONNEXION
                //ENVOYER_DATA_REQ
            //SINON
                //SUPPRIMER_CONNEXION
            //FIN SI
        //SINON
            //ENVOYER_DATA_REQ
        //FIN SI
    //FIN TANT QUE
    
    //POUR TOUS LES CONNEXION
        //SUPPRIMMER_CONNEXION
    
    while(fgets(line_buffer, 256, transaction_file))
    {
        connection = (Connection*) findConnection(line_buffer[0]);
        
        // La connexion n'existe pas
        if (!connection/*==NULL*/) {
            
            // Engager une connexion
            connection = (Connection*) add_connection(line_buffer[0]);

            // Créaction d'un paquet contenant la primitive N_CONNECT_req
            p.type = 0; // CON_PRIM_PACKET
            p.con_prim_packet.prim = N_CONNECT_req;
            
            // Envoie d'un paquet et écoute de la réponse de ER
            if(sendPacketToNet(connection,&p) == -1 || getPacketFromNet(&p) == -1)
                return -1;
            
            // Action en conséquence
            switch(p.type)
            {
                // CONNECTION_PACKET reçu => N_CONNECT_conf
                case 0:
                    // Confirmation de la connexion
                    connection->state[1] = 0x01;

                    // Construction du paquet de DATA
                    p.type = 1;
                    p.data_prim_packet.prim = N_DATA_req;
                    getMessageFromBuffer(line_buffer,message);
                    strcpy(p.data_prim_packet.transaction,message);
                    
                    // Envoie du paquet à l'ER
                    if(sendPacketToNet(connection,&p) == -1)
                        return -1;
                    break;
                // wtf?
                case 1:
                    break;
                // REL_PRIM_PACKET reçu => N_DISCONNECT_ind
                case 2:
                    remove_connection(connection->state[0]);
                    break;
            }
        }
        else {
            // Construction du paquet de DATA
            p.type = 1;
            p.data_prim_packet.prim = N_DATA_req;
            getMessageFromBuffer(line_buffer,message);
            strcpy(p.data_prim_packet.transaction,message);
            
            // Envoyer N_DATA.req
            if(sendPacketToNet(connection,&p) == -1)
                return -1;
        }
    }

    // Vider la table de connexions
    deleteAllConnections();
    
    return 0;
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
//-----------------------------------
// Écoute la réponse de l'ER
// -1: erreur..
// redFromPipe: Nombre de caractères
// lus du pipe
//-----------------------------------
int getPacketFromNet(PRIM_PACKET* p)
{
    int redFromPipe, count = 0;
    // Réponse de l'ER
    do
    {
        redFromPipe = read(netToTrans_pipe,p,sizeof(PRIM_PACKET));
        if (count && count < 4) {
            sleep(1);
        }
        else if (count > 3) {
            fprintf(stderr, "Erreur: Aucune réponse du fournisseur réseau pour une N_CONNECT.REQ après 4 secondes..\n");
            return -1;
        }
        count++;
    }
    while (redFromPipe < 1/*On s'assure qu'on reçoit un PRIM_PACKET???*/); 
   
   return redFromPipe;
}

//---------------------------------------
// Envoie du data à l'ER.
// Retourne:
// -1: erreur..
// writtenToPipe: Nombre de caractères
// écrits dans le pipe
//---------------------------------------
int sendPacketToNet(Connection* connection, PRIM_PACKET* p)
{
    int writtenToPipe, count=0;
    
    // Envoie du N_DATA.req
    do
    {
        writtenToPipe = write(transToNet_pipe,p,sizeof(PRIM_PACKET));
    
        if (count && count < 4) {
            sleep(1);
        }
        else if (count > 3) {
            fprintf(stderr, "Erreur: Impossible d'écrire dans le pipe après 4 secondes.\n");
            return -1;
        }
        count++;
    }
    while (writtenToPipe < 1);
    
    return writtenToPipe;
}

//--------------------------------------
// Trouver le pointeur vers le premier
// caractère du message
// Retourne:
// -1: aucun message..
// j: Le nombre de caractères dans le message
//--------------------------------------
int getMessageFromBuffer(char buffer[], char message[])
{
    int i=0,j;

    // On trouve l'indice auquel se trouve
    while (i < 255 || buffer[i++] != '\\') {
    }

    if (!i<255) {
        return -1;
    }

    for (j = 0; j < 255 - i; j++) {
        message[j] = buffer[i+j+1];
    }

    return j;
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




