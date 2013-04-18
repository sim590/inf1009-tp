//-------------------------------------------------------
// Fichier: transport.c    Auteur(s): Simon DÉSAULNIERS
// Date: 2013-04-12
//-------------------------------------------------------
// Routines que la couche transport a à accomplir.
//-------------------------------------------------------

#include <transport.h>

int transToNet_pipe[2],NetToTrans_pipe[2];

int main(int argc,char** argv)
{
    FILE* transaction_file = (FILE*) atoi(argv[1]);
    char buffer[256];
    int i;

    // Lecture du fichier
    //while(!feof(transaction_file))
    //{
        //fgets(buffer, 256, transaction_file);

    //}

    return 0;
}
