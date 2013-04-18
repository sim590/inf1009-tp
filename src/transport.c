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

int main(int argc,char** argv)
{
    FILE* transaction_file = fopen(S_LEC,"r");
    if (transaction_file == NULL) {
        fprintf(stderr,"Impossible d'ouvrir le fichier S_LEC\n");
        return -1;
    }
    char line_buffer[256];
    int i;

    // Récupération des références au tuyaux
    // (respectivement l'écriture et l'écoute)
    //transToNet_pipe = atoi(argv[1]); netToTrans_pipe = atoi(argv[2]);
    
    // Lecture du fichier S_LEC
    while(fgets(line_buffer, 256, transaction_file));
    {
       // 1. LIRE FICHIER
       // 2. ENVOYER DEMANDE À LA COUCHE RÉSEAU
       //   - CONNECTION.REQ
       //   - DATA.REQ
       //   - REL.REQ
       // 3. ÉCOUTER LA RÉPONSE DE LA COUCHE RÉSEAU
       //   - LIRE TUYAU
       //   - SI RÉPONSE
       //       TRAITER LA RÉPONSE
       //     SINON
       //       RETOURNER À 1. ET REVENIR POUR UNE AUTRE LECTURE
    }
    return 0;
}

//---------------------------------------
// Ajout d'un noeud de connexion à la
// liste chaînée de connexions
//---------------------------------------
void add_connection(char con_number)
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
            return node->state[0] - '0'; // 0x00 ou 0x01
         }
    }

    return -1;
}

