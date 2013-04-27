inf1009-tp
==========

Travail pratique de la session hiver 2013 du cours INF1009 à l'Université du Québec à Trois-Rivières

MODE D'EMPLOI
-------------

Afin de compiler le programme, il suffit d'utiliser le programme «make» avec la règle "all". 
    
    make all

Ceci devrait compiler les **3** programmes nécessaire à l'exécution, c-à-d **inf1009-tp**, **transport-entity** et
**network-entity** dans le répertoire **./bin**. De plus, cela créé un fichier de transaction nommé *S_LEC*.

Afin de démarrer le programme et observer l'exécution, il suffit de démarrer le programme **inf1009-tp** et le laisser
appeller les deux autres processus.
    
    ./inf1009-tp

Il sera possible de remarquer certains résultats à la sortie standard et d'observer les fichiers *S_ECR*, *L_LEC* et 
*L_ECR* qui seront créé durant l'exécution. L'écriture est en mode "append", alors les exécution successive ne
supprimmeront pas les résultats précédents.

BESOIN DE PLUS D'INFORMATION?
-----------------------------

Compilez le rapport (./tex/rapport.tex) afin d'analyser la description du programme complète.


