//-------------------------------------------------------
// Fichier contenant le point d'entrée du programme.
//-------------------------------------------------------
#include <main.h>


// Point d'entré dans le programme
int main()
{
    FILE* transaction_file = fopen(S_LEC,"r");
    char** argv;
    int transToNet_pipe[2], NetToTrans_pipe[2],i;

    // Création des tuyaux de communication
    if (pipe(transToNet_pipe) == -1)
    {
        fprintf(stderr,"Problème lors de la création d'un tuyau!\n");
        return -1;
    }
    if (pipe(NetToTrans_pipe)) {
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
        // Préparation des arguments pour le processus
        // entité transport
        argv = makeArgArray(7,TRANS_PATH_LENGTH);
        strcpy(argv[0],TRANS_PATH);
        sprintf(argv[1],"%i",transaction_file);
        for (i = 0; i < 2; i++) sprintf(argv[i+2],"%i",transToNet_pipe[(i+2)%2]);
        for (i = 0; i < 2; i++) sprintf(argv[i+4],"%i",NetToTrans_pipe[(i+2)%2]);

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
        // Préparation des arguments pour le processus
        // entité réseau
        //argv = makeArgArray(2,NET_PATH_LENGTH);
        //strcpy(argv[0],NET_PATH);
        //for (i = 0; i < 2; i++) sprintf(argv[i+2],"%i",transToNet_pipe[(i+2)%2]);
        //for (i = 0; i < 2; i++) sprintf(argv[i+4],"%i",NetToTrans_pipe[(i+2)%2]);

        //// Exécute le processus entité réseau
        //execv(TRANS_PATH,argv);
        exit(EXIT_SUCCESS);
    }

    // On ferme les file descriptors qui ne nous sont pas utiles
    for (i = 0; i < 2; i++) close(transToNet_pipe[i%2]);
    for (i = 0; i < 2; i++) close(NetToTrans_pipe[i%2]);
    
    sleep(2);
    //fclose(transaction_file);
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



