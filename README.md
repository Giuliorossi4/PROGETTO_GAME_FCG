Compilation:
```
cmake -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build

```
Execution:
```
./build/DungeonCleanser

```
Struttura del Progetto e Tappe (Git Tags)

Il progetto non utilizza cartelle separate per le varie fasi di sviluppo, ma è versionato interamente tramite Git Tags. Ogni tappa è compilabile ed eseguibile.

Per visionare e testare una specifica tappa del progetto, apri il terminale nella root del progetto e digita uno dei seguenti comandi per la specifica tappa:

git checkout Tappa-01--Finestra-Vuota
git checkout Tappa-02
git checkout Tappa-03
git checkout Tappa-04
git checkout Tappa-05
git checkout Tappa-06
git checkout Tappa-07
git checkout Tappa-08
git checkout Tappa-09
git checkout Tappa-10
git checkout Tappa-11
git checkout Tappa-12
git checkout Tappa-13


```
Comandi:

Dalla versione 00 alla versione 10:
```
Enter = avvio del gioco e pausa;

W = muovi il player verso l'alto;
A = muovi il player verso sinistra;
S = muovi il player verso il basso;
D = muovi il player verso destra;

Freccia su = spara un pugnale che nasce dal player e va verso l'alto;
Freccia sinistra = spara un pugnale che nasce dal player e va verso sinistra;
Freaccia giu' = spara un pugnale che nasce dal player e va verso il basso;
Freccia destra = spara un pugnale che nasce dal player e va verso destra;

```

Dalla versione 11 alla versione finale:
```
Per avviare o mettere in pausa il gioco si puo' usare anche il tasto SPACE;

```

GAMEPLAY:
```
L'obbiettivo del gioco e' eliminare i nemici per andare avanti nei livelli,
per farlo bisogna sparargli e colpirli con i pugnali.
Ci sono due tipi di nemici, i ragni e i serpenti.
I ragni sono piu' tenaci, per eliminarli bisogna colpirli esattamente 2 volte,
i serpenti invece sono meno resistenti, ma molto piu' veloci, per eliminarli basta un solo colpo.
Avanzando nei round il numero dei nemici aumenta, insieme anche alla loro velocita'.
Ogni volta che carica il nuovo round bisogna premere il tasto per avviare il gioco per iniziare il nuovo round,
cosi' per dare il tempo al giocatore per ambientarsi.
Ogni 5 round la vita del giocatore si ripristinera' e aumentera' con una vita extra, invece ogni 10 round ci sara' una boss fight con delle orde speciali, con meno nemici del solito, ma con un boss molto lento, grande e con molta vita, serviranno 40 colpi per abbatterlo.

```