# [FR] Verbes irréguliers en Allemand

## Résumé

Ce programme simple en ligne de commande permettra de visualiser et de s'exercer à la connaissance des verbes irréguliers en Allemand.

Ces verbes irréguliers sont composés de l'infinitif, de l'accord à la 3PS du prétérit, de l'accord à la 3PS du parfait avec l'auxiliaire, de la traduction française. Il est permis d'avoir plusieurs possibilités à chaque fois.

Au lancement, il lit un fichier CSV contenant les verbes irréguliers et les stocke en mémoire.

L'utilisateur a une interface graphique lui permettant de choisir son activité, parmi :

* Visualiser par lettre initiale les verbes sous forme de liste.
* Rechercher, par une sous-chaîne, un ou des verbes (résultats à l'écran en temps réel).
* Compléter une ligne affichée incomplète. La recherche est aléatoire, ainsi que la partie masquée de la ligne. L'utilisateur dispose d'une façon de demander la solution.

## Suggestion d'implémentation

* Langage : C
* Affichage : CLI avec (n)curses
* Stockage en mémoire des verbes : SQLite
* Utilisation du MVC
* Pas de threads
* Contrôleur avec design pattern State
* Génération du CSV des verbes : script Python qui récupère depuis `https://www.allemandfacile.com/exercices/exercice-allemand-2/exercice-allemand-22677.php` (1 seule fois, en cas d'absence du fichier).
