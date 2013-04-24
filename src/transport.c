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
    FILE* transaction_file = fopen(S_LEC,"a+");
    fclose(transaction_file);
    transaction_file = fopen(S_LEC,"r");

    if (transaction_file == NULL) {
        fprintf(stderr,"Impossible d'ouvrir le fichier %s\n",S_LEC);
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
            connection = (Connection*) add_connection(line_buffer[0],NULL,NULL);

            // Créaction d'un paquet contenant la primitive N_CONNECT_req
            p.prim = N_CONNECT_req;
            
            // Génère un seed pour une valeur pseudo-aléatoire
            // différente par rand()
            srand(time(NULL));
            p.con_prim_packet.src_addr = (char) rand();  // Adresses aléatoires pour la source
            p.con_prim_packet.dest_addr = (char) rand(); // et la destination..
            p.con_prim_packet.con_number = connection->tcon.con_number;
            
            // Envoie d'un paquet et écoute de la réponse de ER
            writePrimPacketToStdOut(&p,I_AM);
            if(sendPacketToInterface(&p,transToNet_pipe) == -1)
                return -1;
            if (getPacketFromInterface(&p,netToTrans_pipe) == -1)
                return -1;
            
            char result[256];
            // Action en conséquence
            switch(p.prim)
            {
                // CON_PRIM_PACKET reçu => N_CONNECT_conf
                case N_CONNECT_conf:
                    // Écriture des résutlats dans S_ECR
                    sprintf(result,"Réception de la primitive N_CONNECT.conf sur la connection %c\n", connection->tcon.con_number);
                    writeResults(result,S_ECR);
                    
                    // Confirmation de la connexion
                    connection->tcon.state = 0x01;

                    // Construction du paquet de DATA
                    p.prim = N_DATA_req;
                    getMessageFromBuffer(line_buffer,message);
                    
                    strcpy(p.data_prim_packet.transaction,message);
                    p.data_prim_packet.con_number = connection->tcon.con_number;
                    
                    // Envoie du paquet à l'ER
                    writePrimPacketToStdOut(&p,I_AM);
                    if(sendPacketToInterface(&p,transToNet_pipe) == -1)
                        return -1;
                    break;
                // REL_PRIM_PACKET reçu => N_DISCONNECT_ind
                case N_DISCONNECT_ind:
                    // Écriture des résultats dans S_ECR
                    sprintf(result,"Réception de la primitive N_DISCONNECT.ind pour la connexion %c\n",connection->tcon.con_number);
                    writeResults(result,S_ECR);
                    
                    // Retrait de la connexion de la table de connexions
                    remove_connection(connection->tcon.con_number);
                    break;
            }
        }
        else {
            // Construction du paquet de DATA
            p.prim = N_DATA_req;
            getMessageFromBuffer(line_buffer,message);
            strcpy(p.data_prim_packet.transaction,message);
            p.data_prim_packet.con_number = connection->tcon.con_number;
            
            // Envoyer N_DATA.req
            writePrimPacketToStdOut(&p,I_AM);
            if(sendPacketToInterface(&p,transToNet_pipe) == -1)
                return -1;
        }
    }

    // Construction d'un paquet REL_PRIM_PACKET
    // et vidage de la table de connexions
    p.prim = N_DISCONNECT_req;
    connection = first_con_node;
    while (connection) {
        p.rel_prim_packet.con_number = connection->tcon.con_number;
        writePrimPacketToStdOut(&p,I_AM);
        if(sendPacketToInterface(&p,transToNet_pipe) == -1)
            return -1;
        connection = connection->tcon.next;
    }

    deleteAllConnections();

    // Fermeture du fichier S_LEC
    fclose(transaction_file);
    
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
    for (i = 0; i < 254 ; i++) {
        if (buffer[i] == '\\') {
            break;
        }
    }

    for (j = 0; j < 255 - i; j++) {
        message[j] = buffer[i+j+1];
    }

    return j;
}

//---------------------------------------
// Écrit les résultats dans le fichier
// spécifié
//---------------------------------------
int writeResults(char* results, char* file_path)
{
    FILE* file = fopen(file_path,"a+");
    int writtenToFile;
    
    if((writtenToFile = fprintf(file,results)) < 1) {
        fprintf(stderr,"Impossible d'écrire dans le fichier %s",file_path);
        return -1;
    }
    
    fclose(file);

    return writtenToFile;
}
