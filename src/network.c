//-------------------------------------------------------
// Fichier: network.c    Auteur(s): Simon DESAULNIERS
// Date: 2013-04-12
//-------------------------------------------------------
// Routines de la couche reseau.
//-------------------------------------------------------

#include <network.h>

Connection* first_con_node = NULL;
Connection* last_con_node = NULL;

int transToNet_pipe,netToTrans_pipe;

// Point d'entree dans le programme
int main(int argc, char** argv)
{
    int i=0, count = 0, flags,flag_response, length, response;
    char segmt[128], pr,ps;
    PRIM_PACKET p;
    PACKET packet, sndPacket;
    Connection* connection;

    netToTrans_pipe = atoi(argv[1]); transToNet_pipe = atoi(argv[2]);
    if (DEBUG) printf("NETWORK:\nMes fd sont:\n%i,%i\n",transToNet_pipe,netToTrans_pipe);
    
    // On s'assure que les fd sont en mode 0_NONBLOCK
    flags = fcntl(transToNet_pipe,F_GETFL,0);
    fcntl(transToNet_pipe,F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(netToTrans_pipe,F_GETFL,0);
    fcntl(netToTrans_pipe,F_SETFL, flags | O_NONBLOCK);

    // ALGO
    //-------
    //TANT QUE LIRE_TUYAU_ET
        //SI N_CONNECT_REQ
            //SI ER_ACCEPTE
                // AJOUTER_CONNEXION

                //CONSTRUIRE_CONNECTION_PACKET
    
                //ECRIRE_CONNECTION_PACKET(L_ECR)
    
                //GEN_REPONSE_B
    
                //ECRIRE_REPONSE_B(L_LEC)
                //SI REPONSE_B_OUI
                    //REPONDRE_ET_OUI_B
                //SINON
                    //REPONDRE_ET_NON_B
            //SINON
                //REPONDRE_ET_NON
        //SINON SI N_DATA_REQ
            //SEGMENTER_MESSAGE
            //POUR TOUT SEGMENTS
                //ECRIRE_DATA_PACKET(L_ECR)
                //GEN_REPONSE_B
                
                //ECRIRE_REPONSE_B(L_LEC)
                //SI AUCUNE_REPONSE OU REPONSE_NEGATIVE
                    //ECRIRE_DATA_PACKET(L_ECR)
                    //GEN_REPONSE_B
                    
                    //ECRIRE_REPONSE_B
                //FIN SI
            //FIN POUR TOUT
        //SINON SI N_DISCONNECT_REQ
            //ECRIRE_REL_PACKET(L_ECR)
        //FIN SI
    //FIN TANT QUE
   
    while (getPacketFromInterface(&p,transToNet_pipe) > -1) {
        switch(p.prim)
        {
            case N_CONNECT_req:
                // Refus de la connexion car l'adresse source est un multiple de 27
                if (((int)p.con_prim_packet.src_addr)%27 == 0) {
                    p.prim = N_DISCONNECT_ind;
                    
                    writePrimPacketToStdOut(&p,I_AM);
                    if(sendPacketToInterface(&p,netToTrans_pipe) < 0)
                        return -1;
                    break;
                }

                // Acceptation au niveau de l'entite reseau
                connection = add_connection(p.con_prim_packet.con_number,&p.con_prim_packet.src_addr,&p.con_prim_packet.dest_addr);

                // Construction d'un paquet pour envoie sur
                // la couche liaison
                packet.packet_type = 0; // CONNECTION_PACKET
                packet.con_packet.con_packet_type = CON_req;
                packet.con_packet.con_number = p.con_prim_packet.con_number;
                packet.con_packet.src_addr = p.con_prim_packet.src_addr;
                packet.con_packet.dest_addr = p.con_prim_packet.dest_addr;

                // Envoie a la couche liaison
                if (sendPacketToDataLinkLayer(&packet,L_ECR) == -1)
                    return -1;

                // Reponse du distant
                flag_response = genRemotePacketResponse(&packet);

                // Aucune reponse ou refus
                if (!flag_response || packet.con_packet.con_packet_type == REL_ind) {
                    // Abandon
                    p.prim = N_DISCONNECT_ind;
                    writePrimPacketToStdOut(&p,I_AM);
                    if(sendPacketToInterface(&p,netToTrans_pipe) == -1)
                        return -1;
                    break;
                }
                
                // Ecriture de la reponse du distant dans L_LEC
                sendPacketToDataLinkLayer(&packet,L_LEC);
                
                // Confirmation de la connexion
                p.prim = N_CONNECT_conf;
                writePrimPacketToStdOut(&p,I_AM);
                if(sendPacketToInterface(&p,netToTrans_pipe) == -1)
                    return -1;
                connection->ncon.state = 0x01;

                break;
            case N_DATA_req:
                /*
                 * Lors de l'envoie d'un segment, ne pas oublier de garder
                 * une copie du paquet car la fonction genRemotePacketResponse(PACKET*)
                 * va modifier celui qu'on lui passe..
                 */
                pr=0x01, ps=0x00;

                length = (int) strlen(p.data_prim_packet.transaction);
                // Segmentation s'il y a lieu et envoie des segments
                for (i = 0; i < length; i+=128) {

                    // Construction d'un paquet de DATA
                    packet.packet_type = 1; // DATA_PACKET

                    // le type = [p(r),M,p(s),0]
                    if (i+128 < length)
                        packet.data_packet.type = ((pr << 5) + (ps << 1)) | 0x10;
                    else
                        packet.data_packet.type = (pr << 5) + (ps << 1);

                    packet.data_packet.con_number = p.data_prim_packet.con_number;

                    // Copie de 128 octets du message vers un segment
                    strncpy(packet.data_packet.segm_data,p.data_prim_packet.transaction,128);

                    // Envoie a la couche liaison
                    sendPacketToDataLinkLayer(&packet,L_ECR);
                    
                    // Sauvegarde du paquet pour la phase de la reponse du distant
                    // car on doit envoyer deux fois maximum
                    sndPacket = packet;
                    
                    // Aucune reponse ou reponse negative
                    if ((response = genRemotePacketResponse(&sndPacket)) <= 0) {
                        if (response < 0)
                            // Ecrire la reponse dans L_LEC
                            sendPacketToDataLinkLayer(&sndPacket,L_LEC);

                        // Deuxieme envoie
                        sendPacketToDataLinkLayer(&packet,L_ECR);
                        
                        // Reception d'un acquitement positif ou negatif
                        if(genRemotePacketResponse(&packet)) {
                            sendPacketToDataLinkLayer(&packet,L_LEC);
                        }
                    }
                    // Reponse positive
                    else
                        sendPacketToDataLinkLayer(&sndPacket,L_LEC);

                    ps = pr;
                    pr = (((ps+0x01) << 5) >> 5); // modulo
                }

                break;
            case N_DISCONNECT_req:
                connection = findConnection(p.rel_prim_packet.con_number);
                
                // Construction d'un paquet de liberation
                packet.packet_type = 2; // RELEASE_PACKET
                packet.rel_packet.type = 0x13;
                packet.rel_packet.con_number = p.rel_prim_packet.con_number;
                packet.rel_packet.src_addr = connection->ncon.src_addr;
                packet.rel_packet.dest_addr = connection->ncon.dest_addr;
                
                // Envoie d'un paquet d'indication de liberation
                sendPacketToDataLinkLayer(&packet,L_ECR);
                
                // Retrait de de la connexion de la table de connexions
                remove_connection(p.rel_prim_packet.con_number);
                break;
        }
    }

    return 0;
}

//----------------------------------
// Envoie d'un paquet a la couche
// liaison
//----------------------------------
int sendPacketToDataLinkLayer(PACKET* p, char* file_path)
{
    char buffer[MAX_PACKET_SIZE];
    int i, writtenToFile, count = 0;
    FILE* file;

    // Construction d'un buffer a partir d'un PACKET
    switch(p->packet_type)
    {
        // CONNECTION_PACKET
        case 0:
            sprintf(buffer,"%c/%p/%i/%i",p->con_packet.con_number,
                                            p->con_packet.con_packet_type,
                                            p->con_packet.src_addr,
                                            p->con_packet.dest_addr);

            if (p->con_packet.con_packet_type == REL_ind) {
                char tmp[2];
                sprintf(tmp,"%p",(char)p->con_packet.reason);
                strcat(buffer,tmp);
            }

            strcat(buffer,"\n");
            break;

        // DATA_PACKET
        case 1:
            sprintf(buffer,"%c/%p/%s\n",p->data_packet.con_number,p->data_packet.type,
                                          p->data_packet.segm_data);
            break;

        // RELEASE_PACKET
        case 2:
            sprintf(buffer,"%c/%p/%i/%i\n",p->rel_packet.con_number,
                                              p->rel_packet.type,
                                              p->rel_packet.src_addr,
                                              p->rel_packet.dest_addr);
            break;
    }
    
    // Ouverture du fichier
    file = fopen(file_path,"a+");
    
    if (file == NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s\n",L_ECR);
        return -1;
    }
    
    do {

        writtenToFile = fprintf(file,buffer);

        if (count && count < MAX_WAIT_TIME) {
            sleep(1);
        }
        else if (count > MAX_WAIT_TIME) {
            fprintf(stderr, "Erreur: Impossible d'ecrire a la couche liaison apres %i secondes.\n",MAX_WAIT_TIME);
            return -1;
        }
        count++;
    } while (writtenToFile < 0);

    fclose(file);

    return writtenToFile;
}

//-----------------------------------------
// Genere la reponse du systeme distant
// 
// Retourne:
//  0: Aucune reponse du distant
//  CONNECTION)
//      1: Reponse ecrite dans PACKET* p
//  DATA)
//      1: Acquitement positif
//      -1: Acquitement negatif
//-----------------------------------------
int genRemotePacketResponse(PACKET* p)
{
    switch(p->packet_type)
    {
        // CONNECTION_PACKET => CON_req
        case 0:
                // Aucune reponse de la part du distant
                if (p->con_packet.src_addr%19 == 0)
                    break;
                
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
            // Emission d'aucun paquet d'acquitement
            if (findConnection(p->data_packet.con_number)->ncon.src_addr%15 == 0)
                break;
            // Emission d'un paquet d'acquitement negatif
            if (((p->data_packet.type << 4) >> 5)/*p(s)*/ == (rand()%8))
            {
                // Calcul du p(r) = (p(s)+1)%8 sur 3 bits acote a gauche
                p->data_packet.type = ((((((p->data_packet.type << 4) >> 4) + 0x02) << 4) >> 5)<<5);
                // Ajout du masque de bits sur les 5 bits a droite du p(r)
                p->data_packet.type += 0x09; // == 01001
                
                // On efface le message parce qu'il n'est pas necessaire
                memset(p->data_packet.segm_data,0,sizeof(p->data_packet.segm_data)-1);
                return -1;
            }
            // Emission d'un paquet d'acquitement positif
            else {
                // Calcul du p(r) = (p(s)+1)%8 sur 3 bits acote a gauche
                p->data_packet.type = ((((((p->data_packet.type << 4) >> 4) + 0x02) << 4) >> 5)<<5);
                // Ajout du masque de bits sur les 5 bits a droite du p(r)
                p->data_packet.type += 0x01; // == 00001
                
                // On efface le message parce qu'il n'est pas necessaire
                memset(p->data_packet.segm_data,0,sizeof(p->data_packet.segm_data)-1);
                return 1;
            }
            break;
    }
    
    // Aucune reponse
    return 0;
}

