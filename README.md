## 🧭 Navigation dans le jeu

1. **Chargement** : Barre de progression avec fond animé
2. **Page Histoire** : Introduction au jeu (avec bouton Skip)
3. **Menu principal** :
   - `Jouer`
   - `Options`
   - `Quitter`
4. **Options** :
   - Changer la langue
   - Gérer le volume
   - Choisir la musique
   - Crédits
5. **Mode de jeu** :
   - Joueur 1 vs Joueur 2
   - Joueur 1 vs Ordinateur (IA facile)
6. **Sélection des personnages** :
   - Chaque joueur choisit 3 persos
   - Une fiche personnage s’affiche à chaque sélection
7. **Page de confirmation** :
   - Vue des deux équipes
8. **Combat (page de jeu)** :
   - Attaques, défense, tours, profils affichés, points PT, etc.

---

## 🎮 Contrôles

| **Action**                    | **Contrôle**                    |
|-------------------------------|---------------------------------|
| Naviguer dans les menus       | Souris                          |
| Sélectionner un perso         | souris                          |
| Skipper l’intro               | Bouton flèche                   |
| Jouer une action              | Clic sur un des 5 boutons       |

---

## 🛠️ Raccourcis développeur (Debug)

| **Touche**      | **Action**                               |
|-----------------|------------------------------------------|
| **P**           | Donne 10 PT au personnage actif          |
| **M**           | Rétablit toute la vie du personnage      |
| **Suppr**       | Met les PV du personnage à zéro (KO)     |

---

## 🧠 IA intégrée (facile)

- Fonctionne en **mode J1 vs Ordinateur**
- L’IA choisit aléatoirement :
  - Une attaque spéciale si elle a assez de PT
  - Défense ou attaque basique sinon
- Cible un personnage vivant aléatoire dans l’équipe adverse
- Petit délai d’action entre chaque tour pour plus de réalisme

---

## 🌐 Langues disponibles

- Français 🇫🇷
- Anglais 🇬🇧
- Espagnol 🇪🇸
- Allemand 🇩🇪

---


## 🧪 Compilation
### Sous Linux (WSL ou Debian)
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
make clean
make jeu
./exec/jeu
