//-------------------------------------------------------
// Fichier contenant le point d'entrée du programme.
//-------------------------------------------------------
#include <main.h>


// Point d'entré dans le programme
int main()
{
    char** argv;
    int transToNet_pipe[2], netToTrans_pipe[2],i;

    // Création des tuyaux de communication
    if (pipe(transToNet_pipe) == -1)
    {
        fprintf(stderr,"Problème lors de la création d'un tuyau!\n");
        return -1;
    }
    if (pipe(netToTrans_pipe)) {
        fprintf(stderr,"Problème lors de la création d'un tuyau!\n");
        return -1;
    }
    
    pid_t pid = fork();

    if (pid == -1)
    {
        fprintf(stderr,"Impossible de créer un processus Entité Transport..\n");
        return -1;
    }
    else if (pid == 0) {
        if (DEBUG) printf("CHILD1: what's up!\n");
        
        // Fermeture des côté du tuyaux non-utiles
        close(transToNet_pipe[0]);
        close(netToTrans_pipe[1]);

        // Préparation des arguments pour le processus
        // entité transport
        argv = makeArgArray(4,TRANS_PATH_LENGTH);
        strcpy(argv[0],TRANS_PATH);
        sprintf(argv[1],"%i",transToNet_pipe[1]);
        sprintf(argv[2],"%i",netToTrans_pipe[0]);

        // Exécute le processus entité transport
        execv(TRANS_PATH,argv);
    }

    pid = fork();

    if (pid == -1)
    {
        fprintf(stderr,"Impossible de créer un processus Entité Réseau..\n");
        return -1;
    }
    else if (pid == 0) {
        if (DEBUG) printf("CHILD2: what's up dude!\n");

        // Fermeture des côté du tuyaux non-utiles
        //close(netToTrans_pipe[0]);
        //close(transToNet_pipe[1]);
        // Préparation des arguments pour le processus
        // entité réseau
        //argv = makeArgArray(4,NET_PATH_LENGTH);
        //strcpy(argv[0],NET_PATH);
        //sprintf(argv[1],"%i",netToTrans_pipe[1]);
        //sprintf(argv[2],"%i",transToNet_pipe[0]);
        //for (i = 0; i < 2; i++) sprintf(argv[i+2],"%i",transToNet_pipe[(i+2)%2]);
        //for (i = 0; i < 2; i++) sprintf(argv[i+4],"%i",netToTrans_pipe[(i+2)%2]);

        //// Exécute le processus entité réseau
        //execv(TRANS_PATH,argv);
        exit(EXIT_SUCCESS);
    }

    // On ferme les file descriptors qui ne nous sont pas utiles
    for (i = 0; i < 2; i++) close(transToNet_pipe[i%2]);
    for (i = 0; i < 2; i++) close(netToTrans_pipe[i%2]);
    
    sleep(2);
    return 0;    
}

//----------------------------------
// Remplir un tableau de string
// pour argument à passer à execv
//----------------------------------
char** makeArgArray(int height, int length)
{
    int i;
    char** array = malloc(sizeof(char*)*height);

    for (i = 0; i < height-1; i++) {
        array[i] = malloc(sizeof(char)*length);
    }
    array[height-1] = (char*) NULL;

    return array;
}



