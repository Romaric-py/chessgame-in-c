- Stocker dans la structure board, le compte des types de pièces pour chaque couleur, et penser à la mettre à jour, à chaque fois que set_piece est appelé (ou move)

- Stocker l'état en échec ou pas de chaque roi dans la board

- Permettre l'indexation du méta state de la board par color (BLACK / WHITE)

## A faire

### Core

Notes: Polyglot Opening Books, CAISSA Book Creator, 

- Refactor generale, utilisation de Move,unmake promotion
- Simplifier la description des droits de roque à 4 booléens au lieu de 6 
    et permettre l'indexation du méta state de la board par color (BLACK / WHITE)
- Mettre à jour leur utilisation à travers la base de code


- Implémenter la sauvegarde et le chargement du plateau en FEN
- Implémnter l'usage des openings books (special zobrist hashing, book format, fetching in log(n))
- Enregistrer les 

### Interface
- Améliorer la TUI pour afficher mettre le dernier mouvement en surbrillance
- Implémenter une GUI



## 10 premiers coups (ia vs ia)
========================================================
     CHESS ENGINE BENCHMARK (Depth 1 -> 7, Moves: 10)   
========================================================
| Depth    | Min (s)      | Max (s)      | Avg (s)      |
|----------|--------------|--------------|--------------|
| 1        |     0.000071 |     0.000107 |     0.000081 |
| 2        |     0.000372 |     0.000453 |     0.000396 |
| 3        |     0.002285 |     0.008297 |     0.004190 |
| 4        |     0.008254 |     0.111160 |     0.024797 |
| 5        |     0.208170 |     1.296248 |     0.477392 |
| 6        |     1.142452 |     5.786894 |     3.163870 |
| 7        |    15.433225 |    63.135949 |    28.831633 |
========================================================


## REfactor progress

- [x] chessboard.h
- [x] chessboard.c
- [x] move_check.c
- [x] move_check.h
- [x] special_move.c
- [x] special_move.h
- [x] movelist.c
- [x] movelist.h
- [ ] game