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
main: main.o bindir
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/inf1009-tp main.o

main.o: $(SRCDIR)/main.c $(SRCDIR)/main.h
	cd $(SRCDIR)/; \
	gcc -g -c main.c -I.

transport: transport.o transNnet.o bindir
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/transport-entity transport.o transNnet.o

transport.o: $(SRCDIR)/transport.c transNnet.h
	cd $(SRCDIR)/; \
	gcc -g -c transport.c -I.

network: network.o transNnet.o bindir
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/network-entity network.o transNnet.o

network.o: $(SRCDIR)/network.c transNnet.h
	cd $(SRCDIR)/; \
	gcc -g -c network.c -I.

transNnet.o: transNnet.h
	cd $(SRCDIR)/; \
	gcc -g -c transNnet.c -I.

transNnet.h: $(SRCDIR)/transNnet.h $(SRCDIR)/transport.h $(SRCDIR)/network.h

clean:
	rm -f $(SRCDIR)/*.o

cleanall: clean
	rm -f $(BINDIR)/inf1009-tp $(BINDIR)/transport-entity $(BINDIR)/network-entity

bindir:
	mkdir -p $(BINDIR)
	touch $(BINDIR)/S_LEC $(BINDIR)/S_ECR $(BINDIR)/L_LEC $(BINDIR)/L_ECR
