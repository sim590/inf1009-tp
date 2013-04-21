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
    FILE* results_file = fopen(S_ECR,"w");

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
    
    // Lecture du fichier S_LEC, envoie des requêtes et écoute
    // de la réponse.
    while(fgets(line_buffer, 256, transaction_file))
    {
        connection = (Connection*) findConnection(line_buffer[0]);
        
        // La connexion n'existe pas
        if (!connection/*==NULL*/) {
            
            // Engager une connexion
            connection = (Connection*) add_connection(line_buffer[0]);

            // Créaction d'un paquet contenant la primitive N_CONNECT_req
            p.prim = N_CONNECT_req;
            p.con_prim_packet.src_addr = (char) rand();  // Adresses aléatoires pour la source
            p.con_prim_packet.dest_addr = (char) rand(); // et la destination..
            
            // Envoie d'un paquet et écoute de la réponse de ER
            if(sendPacketToInterface(&p,transToNet_pipe) == -1 || getPacketFromInterface(&p,netToTrans_pipe) == -1)
                return -1;
            
            char result[256];
            // Action en conséquence
            switch(p.prim)
            {
                // CON_PRIM_PACKET reçu => N_CONNECT_conf
                case N_CONNECT_conf:
                    // Écriture des résutlats dans S_ECR
                    sprintf(result,"Réception de la primitive N_CONNECT.conf sur la connection %i\n", connection->state[0]);
                    fwrite(result,1,sizeof(result),results_file);
                    // Confirmation de la connexion
                    connection->state[1] = 0x01;

                    // Construction du paquet de DATA
                    p.prim = N_DATA_req;
                    getMessageFromBuffer(line_buffer,message);
                    strcpy(p.data_prim_packet.transaction,message);
                    p.data_prim_packet.con_number = connection->state[0];
                    
                    // Envoie du paquet à l'ER
                    if(sendPacketToInterface(&p,transToNet_pipe) == -1)
                        return -1;
                    break;
                // REL_PRIM_PACKET reçu => N_DISCONNECT_ind
                case N_DISCONNECT_ind:
                    // Écriture des résultats dans S_ECR
                    sprintf(result,"Réception de la primitive N_DISCONNECT.ind pour la connexion %i\n",connection->state[0]);
                    // Retrait de la connexion de la table de connexions
                    remove_connection(connection->state[0]);
                    fwrite(result,1,sizeof(result),results_file);
                    break;
            }
        }
        else {
            // Construction du paquet de DATA
            p.prim = N_DATA_req;
            getMessageFromBuffer(line_buffer,message);
            strcpy(p.data_prim_packet.transaction,message);
            p.data_prim_packet.con_number = connection->state[0];
            
            // Envoyer N_DATA.req
            if(sendPacketToInterface(&p,transToNet_pipe) == -1)
                return -1;
        }
    }

    // Construction d'un paquet REL_PRIM_PACKET
    // et vidage de la table de connexions
    p.prim = N_DISCONNECT_req;
    connection = first_con_node;
    while (connection) {
        sprintf(p.rel_prim_packet.reason,"Fin des requêtes sur la connexion %i\n",connection->state[0]);
        p.rel_prim_packet.con_number = connection->state[0];
        if(sendPacketToInterface(&p,transToNet_pipe) == -1)
            return -1;
    }

    deleteAllConnections();

    // Fermeture des fichiers S_ECR, S_LEC
    fclose(transaction_file);
    fclose(results_file);
    
    return 0;
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
    while (i++ < 255 || buffer[i-1] != '\\') {
    }

    if (!i<255) {
        return -1;
    }

    for (j = 0; j < 255 - i; j++) {
        message[j] = buffer[i+j+1];
    }

    return j;
}

