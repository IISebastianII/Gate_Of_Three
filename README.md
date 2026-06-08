# Gate of Three

`Gate of Three` to projekt gry platformowej 2D tworzony w C++17 z użyciem biblioteki SFML 2.x. Gra jest rozwijana etapami, tak aby każdy kolejny milestone dodawał konkretną część rozgrywki albo architektury projektu.

Aktualna wersja skupia się na fundamencie gry: poruszaniu postacią, podstawowej fizyce, pokojach, przejściach między lokacjami oraz strukturze kodu przygotowanej pod dalszą rozbudowę.

## Wymagania

- C++17
- SFML 2.x
- Qt Creator
- MinGW zgodny z używaną wersją SFML

Projekt można otworzyć w Qt Creatorze przez plik:

```text
GateOfThree.pro
```

Alternatywnie dostępny jest również plik:

```text
CMakeLists.txt
```

Domyślna konfiguracja zakłada, że SFML znajduje się w katalogu:

```text
C:/SFML
```

Jeżeli SFML jest zainstalowany w innym miejscu, trzeba zmienić ścieżkę w konfiguracji projektu.

## Sterowanie

- `A` / `Left` - ruch w lewo
- `D` / `Right` - ruch w prawo
- `W` / `Up` - skok
- `Space` - atak
- `LAlt` - slide
- `E` - interakcja z przejściem przy znaku
- `Esc` - zamknięcie gry

## Obecny stan gry

W aktualnej wersji gracz może poruszać się po pokoju tutorialowym, skakać, korzystać z podstawowych akcji postaci oraz przechodzić do drugiego pokoju przez interakcję ze znakiem. Drugi pokój pełni rolę prostego battle roomu testowego, w którym znajduje się pierwszy przeciwnik.

Projekt zawiera już podstawowy system walki gracza oraz pierwszego przeciwnika z HP i animacjami reakcji na obrażenia. Rozbudowane AI, ataki przeciwników, mana, bossowie, skrzynie i zapis gry są przewidziane na kolejne etapy.

## Struktura projektu

Kod jest podzielony na pliki `.h` i `.cpp`. Główne klasy zostały przygotowane tak, aby później można było rozwijać projekt bez przepisywania podstaw.

Najważniejsze elementy:

- `Game` - główna klasa gry, odpowiada za okno, pętlę gry, aktualizację i renderowanie
- `Player` - klasa gracza, zawiera ruch, skok, grawitację, slide i atak
- `GameObject` - wspólny interfejs dla obiektów gry
- `Entity` - baza dla obiektów posiadających pozycję i kolizję
- `Room` - bazowa klasa pokoju
- `RoomManager` - zarządza aktualnym pokojem i zmianą lokacji
- `RoomExit` - obiekt przejścia między pokojami
- `Npc`, `Enemy`, `Boss`, `Chest`, `Projectile`, `Spell` - klasy startowe pod przyszłe systemy gry

W projekcie są również przygotowane różne typy pokojów:

- `TutorialRoom`
- `BattleRoom`
- `HealRoom`
- `ChestRoom`
- `BossRoom`

Na ten moment grywalne są przede wszystkim `TutorialRoom` i prosty `BattleRoom`.

## Milestone 1 - podstawowy fundament gry

W pierwszym etapie powstała działająca baza projektu. Celem było uruchomienie grywalnego pokoju oraz sprawdzenie podstawowych mechanik ruchu.

Zaimplementowano:

- działający projekt C++17 + SFML
- strukturę plików `.h` i `.cpp`
- klasę `Game` z pętlą gry
- obsługę delta time
- okno gry i renderowanie
- pierwszy pokój `TutorialRoom`
- gracza widocznego na ekranie
- ruch w lewo i prawo
- skok
- grawitację
- kolizję z podłożem
- prostą kamerę śledzącą gracza
- podstawowe assety pokoju tutorialowego

Po tym etapie gra uruchamiała się, pokazywała pierwszy pokój i pozwalała sterować postacią.

## Milestone 2 - architektura projektu

Drugi etap skupił się na uporządkowaniu kodu i przygotowaniu projektu pod dalszą rozbudowę. Nie chodziło jeszcze o dużą ilość zawartości, tylko o stworzenie czytelnego szkieletu.

Zaimplementowano:

- bazową klasę `GameObject`
- bazową klasę `Entity`
- bazową klasę `Room`
- wspólny kontener obiektów gry oparty o smart pointery
- `RoomManager` jako klasę zarządzającą pokojami
- podstawowy podział pokojów według typu
- klasy startowe dla NPC, przeciwników, bossów, skrzyń, pocisków i zaklęć
- użycie dziedziczenia i polimorfizmu
- strukturę przygotowaną pod kolejne systemy gameplayowe

Po tym etapie projekt miał już bazę, która pozwala dodawać kolejne typy obiektów i pokojów bez trzymania całej logiki w jednym miejscu.

## Milestone 3 - przejścia między pokojami

Trzeci etap dodał pierwszą realną eksplorację między lokacjami. Gracz nie jest już ograniczony tylko do jednego pokoju.

Zaimplementowano:

- drugi grywalny pokój `BattleRoom`
- obiekt `RoomExit`
- przejście z `TutorialRoom` do `BattleRoom`
- powrót z `BattleRoom` do `TutorialRoom`
- interakcję klawiszem `E`
- ustawianie pozycji gracza po zmianie pokoju
- podstawowe tło i podłoże w battle roomie
- poprawione skalowanie tła w obu pokojach
- dopasowanie wysokości podłogi między pokojami

Po tym etapie gracz może wejść w interakcję ze znakiem, zmienić pokój i wrócić do poprzedniej lokacji.

## Milestone 4 - podstawowy system walki

Czwarty etap dodał pierwszy działający system walki oraz przeciwnika testowego w battle roomie.

Zaimplementowano:

- atak gracza pod `Space`
- hitbox ataku przed graczem
- pierwszego przeciwnika `skeleton` w `BattleRoom`
- punkty życia przeciwnika
- animację `idle` przeciwnika
- animację otrzymywania obrażeń `hurt`
- animację śmierci `die`
- usuwanie przeciwnika po zakończeniu animacji śmierci
- krótką niewrażliwość przeciwnika po trafieniu
- radialny wskaźnik HP przeciwnika
- czerwone koło aktualnego HP i szary fragment utraconego HP
- czarną obręcz wskaźnika HP
- klasę `RadialHealthIndicator`

Po tym etapie gracz może przejść do battle roomu, zaatakować skeletona, zmniejszyć jego HP i pokonać go.

## Plan dalszego rozwoju

Kolejne milestone'y będą rozwijały właściwą rozgrywkę. W planie są między innymi:

- AI przeciwników
- ataki przeciwników
- obrażenia i punkty życia
- skrzynie
- zaklęcia
- system many
- pokoje specjalne
- boss room
- HUD
- zapis gry

Projekt jest rozwijany stopniowo, aby każda większa mechanika była dodawana na stabilnym fundamencie.
