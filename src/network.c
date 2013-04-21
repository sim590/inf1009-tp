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
    netToTrans_pipe = atoi(argv[1]); transToNet_pipe = atoi(argv[2]);
    PRIM_PACKET p;
    int i=0, count = 0, flags;

    // On s'assure que les fd sont en mode 0_NONBLOCK
    flags = fcntl(transToNet_pipe,F_GETFL,0);
    fcntl(transToNet_pipe,F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(netToTrans_pipe,F_GETFL,0);
    fcntl(netToTrans_pipe,F_SETFL, flags | O_NONBLOCK);

    if (DEBUG)
    {
        printf("NETWORK:\nMes FD sont:\n%i,%i\n",transToNet_pipe,netToTrans_pipe);
        while (count++ < 4) {
            if(read(transToNet_pipe,&p,sizeof(PRIM_PACKET)) > 0)
            {
                printf("%d\n",p.con_prim_packet.prim);
                return 0;
            }
            sleep(1);
        }
    }

    // ALGO
    //-------
    //TANT QUE LIRE_TUYAU_ET
        //SI N_CONNECT_REQ
            //SI ER_ACCEPTE
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
                }
                break;
            case N_DATA_req:
                break;
            case N_DISCONNECT_req:
                break;
        }
    }
    
    return 0;
}

