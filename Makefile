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
main: main.o
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/inf1009-tp main.o

main.o: $(SRCDIR)/main.c $(SRCDIR)/main.h
	cd $(SRCDIR)/; \
	gcc -g -c main.c -I.

transport: transport.o
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/transport-entity transport.o

transport.o: $(SRCDIR)/transport.c transNnet
	cd $(SRCDIR)/; \
	gcc -g -c transport.c -I.

network: network.o
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/network-entity network.o

network.o: $(SRCDIR)/network.c transNnet
	cd $(SRCDIR)/; \
	gcc -g -c network.c -I.

transNnet: $(SRCDIR)/transNnet.h $(SRCDIR)/transport.h $(SRCDIR)/network.h

clean:
	rm -f $(SRCDIR)/*.o

cleanall: clean
	rm -f $(BINDIR)/inf1009-tp $(BINDIR)/transport-entity $(BINDIR)/network-entity

