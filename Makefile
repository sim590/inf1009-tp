#--------------------------------------------------
# Fichier: Makefile 	Auteur: Simon DÉSAULNIERS
# Projet: [INF1009] Travail de session
#
#--------------------------------------------------
# Makefile du code de la partie du projet de ma
# session.
#--------------------------------------------------

clean:
	cd src; rm -f *.o; cd ..
clean all: clean
	rm inf1009-tp

