#--------------------------------------------------
# Fichier: Makefile 	Auteur: Simon DESAULNIERS
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
	gcc -g -c main.c -I. -O0

transport: transport.o transNnet.o bindir
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/transport-entity transport.o transNnet.o

transport.o: $(SRCDIR)/transport.c transNnet.h
	cd $(SRCDIR)/; \
	gcc -g -c transport.c -I. -O0

network: network.o transNnet.o bindir
	cd $(SRCDIR)/; \
	gcc -o ../$(BINDIR)/network-entity network.o transNnet.o

network.o: $(SRCDIR)/network.c transNnet.h
	cd $(SRCDIR)/; \
	gcc -g -c network.c -I. -O0

transNnet.o: transNnet.h
	cd $(SRCDIR)/; \
	gcc -g -c transNnet.c -I. -O0

transNnet.h: $(SRCDIR)/transNnet.h $(SRCDIR)/transport.h $(SRCDIR)/network.h

clean:
	rm -f $(SRCDIR)/*.o

cleanall: clean
	rm -f $(BINDIR)/inf1009-tp $(BINDIR)/transport-entity $(BINDIR)/network-entity

bindir:
	mkdir -p $(BINDIR)
