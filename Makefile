#--------------------------------------------------
# Fichier: Makefile 	Auteur: Simon DÉSAULNIERS
# Projet: [INF1009] Travail de session
#
#--------------------------------------------------
# Makefile du code de la partie du projet de ma
# session.
#--------------------------------------------------

SRCDIR=./src
BINDIR=./bin

all: main transport network
main: $(SRCDIR)/main.c
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/inf1009-tp main.c -I.

transport: $(SRCDIR)/transport.c
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/transport-entity transport.c -I.

network: $(SRCDIR)/network.c
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/network-entity network.c -I.

clean:
	rm -f $(SRCDIR)/*.o

cleanall: clean
	rm -f $(BINDIR)/inf1009-tp $(BINDIR)/transport-entity $(BINDIR)/network-entity

