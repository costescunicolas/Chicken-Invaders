# Chicken Invaders Clone & Ship Editor - Tema 1 EGC

## Descriere Proiect
Acest proiect reprezintă o implementare a jocului clasic **Chicken Invaders**, dezvoltată în C++ folosind un framework de grafică 2D. Proiectul include un modul avansat de tip **Ship Editor**, unde utilizatorul își poate construi și valida nava spațială înainte de a intra în luptă.

## Funcționalități Principale

### 1. Editorul de Nave (Ship Editor)
Permite personalizarea navei pe un grid de 17x9 folosind piese individuale:
* **Componente:** Blocuri structurale, Motoare (portocalii, cu flacără) și Tunuri (gri, cu țeavă lungă).
* **Sistem Drag & Drop:** Piesele sunt selectate prin click stânga și plasate pe grid.
* **Validare (Constraints):** Jocul poate începe doar dacă nava respectă regulile de inginerie:
    * Structura trebuie să fie conexă (toate piesele legate).
    * Maxim 10 blocuri în total.
    * Obligatoriu cel puțin un motor și un tun.
    * **Restricții Poziționare:** Nu se pot pune piese deasupra tunurilor sau sub motoare pentru a nu bloca funcționarea acestora.

### 2. Gameplay (Chicken Invaders)
* **Inamici:** Două rânduri de pui care se deplasează sinusoidal și coboară progresiv.
* **Atac și Apărare:**
    * Nava trage cu toate tunurile instalate.
    * Puii aruncă ouă în mod aleatoriu.
    * Proiectilele jucătorului pot distruge ouăle inamice la impact.
* **Progresie:** Pe măsură ce rundele avansează, crește viteza ouălor, frecvența atacurilor și viteza de coborâre a puilor.
* **Sistem de Vieți:** Jucătorul are 3 vieți reprezentate prin inimi. Coliziunea cu un ou scade o viață.

## Detalii Tehnice
* **Randare Procedurală:** Toate elementele (pui, inimi, componente navă, ouă) sunt generate ca mesh-uri 2D prin cod.
* **Transformări:** Utilizarea matricelor de translație și scalare pentru animații și poziționarea corectă a elementelor în universul virtual (1280x720).
* **Sistem de Coordonate:** Conversia dinamică între pixelii ecranului și spațiul logic al lumii pentru gestionarea corectă a mouse-ului în editor.

## Controale
* **Editor:** * `Click Stânga`: Drag piese.
    * `Click Dreapta`: Plasează piesă (Release) sau șterge piesă de pe grid.
* **Joc:**
    * `Săgeți`: Deplasare (viteza depinde de numărul de motoare).
    * `Space`: Trage cu tunurile.
    * `Space` (după Game Over): Revenire la editor.

## Structura Repository-ului
* `Tema1.cpp/h`: Coordonarea stărilor de joc (Editor vs. Gameplay).
* `ShipEditor.cpp/h`: Logica de construcție și constrângerile navei.
* `ChickenInvaders.cpp/h`: Mecanicile de joc, coliziuni și inamici.
* `mesh2D.cpp/h`: Definirea geometrică a obiectelor grafice.
