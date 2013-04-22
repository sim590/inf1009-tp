//-------------------------------------------------------
// Fichier: network.c    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-12
//-------------------------------------------------------
// Routines de la couche réseau.
//-------------------------------------------------------

#include <network.h>

Connection* first_con_node = NULL;
Connection* last_con_node = NULL;

int transToNet_pipe,netToTrans_pipe;

// Point d'entrée dans le programme
int main(int argc, char** argv)
{
    // Ouverture des fichiers 
    FILE* toDataLink = fopen(L_ECR,"w");
    FILE* fromDataLink = fopen(L_LEC,"w");

    if (toDataLink == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s\n",L_ECR);
    }
    if (fromDataLink == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s\n",L_LEC);
    }

    int i=0, count = 0, flags,flag_response;
    PRIM_PACKET p;
    PACKET packet, sndPacket;
    char segmt[128];
    netToTrans_pipe = atoi(argv[1]); transToNet_pipe = atoi(argv[2]);

    // On s'assure que les fd sont en mode 0_NONBLOCK
    flags = fcntl(transToNet_pipe,F_GETFL,0);
    fcntl(transToNet_pipe,F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(netToTrans_pipe,F_GETFL,0);
    fcntl(netToTrans_pipe,F_SETFL, flags | O_NONBLOCK);

    if (DEBUG)
    {
        printf("NETWORK:\nMes FD sont:\n%i,%i\n",transToNet_pipe,netToTrans_pipe);
        if(getPacketFromInterface(&p,transToNet_pipe) > 0)
        {
            printf("%d\n",p.con_prim_packet.prim);
            return 0;
        }
        if(getPacketFromInterface(&p,transToNet_pipe) > 0)
        {
            printf("%d\n",p.con_prim_packet.prim);
            return 0;
        }


    }

    // ALGO
    //-------
    //TANT QUE LIRE_TUYAU_ET
        //SI N_CONNECT_REQ
            //SI ER_ACCEPTE
                // AJOUTER_CONNEXION

                //CONSTRUIRE_CONNECTION_PACKET
    
                //ÉCRIRE_CONNECTION_PACKET(L_ECR)
    
                //GEN_REPONSE_B
    
                //ÉCRIRE_RÉPONSE_B(L_LEC)
                //SI REPONSE_B_OUI
                    //RÉPONDRE_ET_OUI_B
                //SINON
                    //RÉPONDRE_ET_NON_B
            //SINON
                //RÉPONDRE_ET_NON
        //SINON SI N_DATA_REQ
            //SEGMENTER_MESSAGE
            //POUR TOUT SEGMENTS
                //ÉCRIRE_DATA_PACKET(L_ECR)
                //GEN_RÉPONSE_B
                
                //ÉCRIRE_RÉPONSE_B(L_LEC)
                //SI AUCUNE_RÉPONSE OU RÉPONSE_NEGATIVE
                    //ÉCRIRE_DATA_PACKET(L_ECR)
                    //GEN_RÉPONSE_B
                    
                    //ÉCRIRE_RÉPONSE_B
                //FIN SI
            //FIN POUR TOUT
        //SINON SI N_DISCONNECT_REQ
            //ÉCRIRE_REL_PACKET(L_ECR)
        //FIN SI
    //FIN TANT QUE
   
    // TODO:
    while (getPacketFromInterface(&p,transToNet_pipe) > -1) {
        switch(p.prim)
        {
            case N_CONNECT_req:
                // Refus de la connexion car l'adresse source est un multiple de 27
                if (((int)p.con_prim_packet.src_addr)%27 == 0) {
                    p.prim = N_DISCONNECT_ind;
                    
                    if(sendPacketToInterface(&p,netToTrans_pipe) < 0)
                        return -1;
                }

                // Acceptation au niveau de l'entité réseau
                add_connection(p.con_prim_packet.con_number,&p.con_prim_packet.src_addr,&p.con_prim_packet.dest_addr);

                // Construction d'un paquet pour envoie sur
                // la couche liaison
                packet.packet_type = 0; // CONNECTION_PACKET
                packet.con_packet.con_number = p.con_prim_packet.con_number;
                packet.con_packet.src_addr = p.con_prim_packet.src_addr;
                packet.con_packet.dest_addr = p.con_prim_packet.dest_addr;

                // Envoie à la couche liaison
                if (sendPacketToDataLinkLayer(&packet,toDataLink) == -1)
                    return -1;

                // Réponse du distant
                flag_response = genRemotePacketResponse(&packet);

                // Aucune réponse ou refus
                if (!flag_response || packet.con_packet.con_packet_type == REL_ind) {
                    // Abandon
                    p.prim = N_DISCONNECT_ind;
                    if(sendPacketToInterface(&p,netToTrans_pipe) == -1)
                        return -1;
                    break;
                }
                
                // Écriture de la réponse du distant dans L_LEC
                sendPacketToDataLinkLayer(&packet,fromDataLink);
                
                // Confirmation de la connexion
                p.prim = N_CONNECT_conf;
                if(sendPacketToInterface(&p,netToTrans_pipe) == -1)
                    return -1;

                break;
            case N_DATA_req:
                /*
                 * Lors de l'envoie d'un segment, ne pas oublier de garder
                 * une copie du paquet car la fonction genRemotePacketResponse(PACKET*)
                 * va modifier celui qu'on lui passe..
                 */

                //while () {
                    
                    //// Construction d'un paquet de DATA
                //}

                break;
            case N_DISCONNECT_req:
                break;
        }
    }
    
    return 0;
}

//----------------------------------
// Envoie d'un paquet à la couche
// liaison
//----------------------------------
int sendPacketToDataLinkLayer(PACKET* p, FILE* f)
{
    char buffer[MAX_PACKET_SIZE];
    int i, writtenToFile, count;

    // Construction d'un buffer à partir d'un PACKET
    for (i = 0; i < 4; i++) {
        buffer[i] = (p->packet_type << i*8) >> 24;
    }
    buffer[4] = '\0';
    switch(p->packet_type)
    {
        // CONNECTION_PACKET
        case 0:
            for (i = 0; i < 4; i++)
                buffer[i+5] = (p->con_packet.con_packet_type >> i*8) >> 24;
            buffer[8] = '\0';
            buffer[9] = p->con_packet.con_number;
            buffer[10] = p->con_packet.src_addr;
            buffer[11] = p->con_packet.dest_addr;

            if (p->con_packet.con_packet_type == REL_ind)
                for (i = 0; i < 4; i++)
                    buffer[i+12] = (p->con_packet.reason << i*8) >> 24;
            break;

        // DATA_PACKET
        case 1:
            buffer[5] = p->data_packet.type;
            buffer[6] = p->data_packet.con_number;
            strcpy(buffer+7,p->data_packet.segm_data);
            break;

        // RELEASE_PACKET
        case 2:
            buffer[5] = p->rel_packet.type;
            buffer[6] = p->rel_packet.con_number;
            buffer[7] = p->rel_packet.src_addr;
            buffer[8] = p->rel_packet.dest_addr;
            break;
    }
    
    do {

        writtenToFile = fwrite(buffer,sizeof(buffer),1,f);

        if (count && count < 4) {
            sleep(1);
        }
        else if (count > MAX_WAIT_TIME) {
            fprintf(stderr, "Erreur: Impossible d'écrire à la couche liaison après %i secondes.\n",MAX_WAIT_TIME);
            return -1;
        }
        count++;
    } while (!writtenToFile);

    return writtenToFile;
}

//-----------------------------------------
// Génère la réponse du système distant
// 
// Retourne:
//  0: Aucune réponse du distant
//  CONNECTION)
//      1: Réponse écrite dans PACKET* p
//  DATA)
//      1: Acquitement positif
//      -1: Acquitement négatif
//-----------------------------------------
int genRemotePacketResponse(PACKET* p)
{
    switch(p->packet_type)
    {
        // CONNECTION_PACKET => CON_req
        case 0:
                // Aucune réponse de la part du distant
                if (p->con_packet.src_addr%19 == 0)
                    break;
                
                /*
                 * swap des adresses car simulation de 
                 * l'envoie dans le sens inverse
                 */
                char tmp;
                tmp = p->con_packet.src_addr;
                p->con_packet.src_addr = p->con_packet.dest_addr;
                p->con_packet.dest_addr = tmp;
                
                // Refus de connexion de la part du distant
                if (p->con_packet.src_addr%13 == 0) {
                    p->con_packet.con_packet_type = REL_ind;
                    p->con_packet.reason = REMOTE_DENY;
                    return 1;
                }
                // Acceptation du distant
                else {
                    p->con_packet.con_packet_type = CON_conf;
                    return 1;
                }
            break;
        // DATA_PACKET
        case 1:
            // Émission d'aucun paquet d'acquitement
            if (findConnection(p->data_packet.con_number)->ncon.src_addr%15 == 0)
                break;
            // Émission d'un paquet d'acquitement négatif
            if (((p->data_packet.type << 4) >> 5)/*p(s)*/ == (rand()%8))
            {
                // Calcul du p(r) = (p(s)+1)%8 sur 3 bits acoté à gauche
                p->data_packet.type = (((((p->data_packet.type << 4) >> 4) + 0x02) << 4) >> 5)<<5;
                // Ajout du masque de bits sur les 5 bits à droite du p(r)
                p->data_packet.type += 0x09; // == 01001
                
                // On efface le message parce qu'il n'est pas nécessaire
                memset(p->data_packet.segm_data,0,sizeof(p->data_packet.segm_data)-1);
                return -1;
            }
            // Émission d'un paquet d'acquitement positif
            else {
                // Calcul du p(r) = (p(s)+1)%8 sur 3 bits acoté à gauche
                p->data_packet.type = (((((p->data_packet.type << 4) >> 4) + 0x02) << 4) >> 5)<<5;
                // Ajout du masque de bits sur les 5 bits à droite du p(r)
                p->data_packet.type += 0x01; // == 00001
                
                // On efface le message parce qu'il n'est pas nécessaire
                memset(p->data_packet.segm_data,0,sizeof(p->data_packet.segm_data)-1);
                return 1;
            }
            break;
    }
    
    // Aucune réponse
    return 0;
}
