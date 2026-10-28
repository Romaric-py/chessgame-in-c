~~1. Implémenter la copie de plateau (Board Cloning / Sandbox)~~

~~Écrire une fonction propre pour dupliquer l'état d'un Board (nécessaire pour simuler un coup sans corrompre le plateau actuel).~~

2. Créer le filtre de légalité (is_legal_move)

Prendre un coup validé géométriquement (pseudo-légal).

Le simuler sur la copie du plateau.

Vérifier si le Roi de la couleur active est menacé (is_square_attacked).

Règle : Si le roi est en échec après la simulation, le coup est illégal (bloquer les clouages et les suicides de roi).

3. Écrire la suite de tests unitaires (Legal_Move_Suite)

Tester un coup cloué (ex: une tour clouée par un fou adverse ne peut pas bouger).

Tester l'interdiction de mettre son propre roi en échec.

4. Préparer la détection de fin de partie (Bonus)

Esquisser la logique pour parcourir l'ensemble des coups légaux d'un camp afin de préparer la détection future du Mat et du Pat.


Cas de match nul
- après 100 demi-coups sans déplacement de pion et sans capture: on peut réclamer la partie nulle
- au délà de 150, la partie est automatiquement arrêté et considérée comme nulle
- répétition de la même position trois fois : possibilité de réclamer la nulle
- répétition cinq fois : nulle automatique.