- Stocker l'état en échec ou pas de chaque roi dans la board ??

## A faire

### Core

Notes: Polyglot Opening Books, CAISSA Book Creator, EndGame TableBases

-!!!! Coups illégaux constatés quand la TT est utilisée 
- S'assurer que toutes les méta du board sont dans le hash (par exemple le compteur de demi-coups pour la règle des 50 coups)
- Pénaliser la profondeur (ex pour vpouvoir mater en moins de coups)
- 

- Enregistrer les parties dans un format lisible et conventionnel (PGN)
- Implémenter le protocole UCI (Universal Chess Interface) :

### Interface
- Améliorer la TUI pour afficher mettre le dernier mouvement en surbrillance
- Implémenter une GUI

### Améliorations

- Problème: même avec assez de matérial, l'IA se conce dans une spirale de répétition de coups. Il faut donc implémenter un système pour détecter les répétitions de coups et les pénaliser dans l'évaluation de la position.
- 
- Ajouter un moteur d'évaluation plus avancé (apprentissage automatique, heuristiques, etc.)


