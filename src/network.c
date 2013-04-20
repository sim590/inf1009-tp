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


    
    
    return 0;
}

TANT QUE LIRE_TUYAU_ET
    PERMUTTER PRIM_PACKET_TYPE
        SI N_CONNECT_REQ
            SI TESTE_VALIDE
                ÉCRIRE_TEST_VALIDE(L_ECR)

                GEN_REPONSE_B

                ÉCRIRE_RÉPONSE_B(L_LEC)

                RÉPONDRE_ET_OUI
            SINON
                RÉPONDRE_ET_NON
        SINON SI N_DATA_REQ
            
        SINON SI N_DISCONNECT_REQ
            
        FIN SI
    FIN PERMUTTER
FIN TANT QUE
