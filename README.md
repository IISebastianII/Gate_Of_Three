# Gate of Three

Gate of Three to krótka gra platformowo-akcyjna 2D stworzona jako projekt studencki w C++17 z użyciem biblioteki SFML 2.x.

Gracz przemierza kolejne pokoje, walczy z przeciwnikami i stopniowo przygotowuje się do walki z bossem. Projekt powstawał etapami. Najpierw został zrobiony podstawowy ruch i kolizje, a później dodane zostały pokoje, przeciwnicy, walka, zaklęcie, leczenie oraz finałowy boss.

## Rozgrywka

Gra zaczyna się w spokojnym pokoju tutorialowym, w którym można sprawdzić sterowanie i spotkać Cat NPC. Przy znaku znajdującym się po prawej stronie można przejść do pierwszego pokoju walki.

W pokojach walki znajdują się dwa rodzaje przeciwników:

- szkielety walczące z bliska,
- mushroom enemy, który porusza się i strzela pociskami.

Przejście do następnego pokoju jest możliwe dopiero po pokonaniu wszystkich przeciwników. Po pierwszej walce gracz trafia do pokoju ze skrzynią. Otwarcie skrzyni odblokowuje zaklęcie dalekiego zasięgu.

Następnie gracz przechodzi przez drugi pokój walki i trafia do Heal Roomu. Znajduje się tam uzdrowisko, które po interakcji przywraca całe zdrowie gracza. Ostatnią lokacją jest Boss Room, gdzie czeka Crystal Golem. Po jego pokonaniu na ekranie pojawia się napis `YOU WIN!`.

Jeśli gracz straci całe zdrowie, wraca do Tutorial Roomu i może rozpocząć grę ponownie.

## Sterowanie

| Klawisz | Akcja |
| --- | --- |
| `A` / strzałka w lewo | Ruch w lewo |
| `D` / strzałka w prawo | Ruch w prawo |
| `W` / strzałka w górę | Skok |
| `Space` | Podstawowy atak |
| `Q` | Zaklęcie dalekiego zasięgu po odblokowaniu |
| `LAlt` | Slide / unik |
| `E` | Interakcja ze znakiem, skrzynią lub uzdrowiskiem |
| `Esc` | Zamknięcie gry |

## Najważniejsze mechaniki

- ruch postaci liczony z użyciem delta time,
- skok, grawitacja i kolizje z podłożem,
- obsługa zwykłych platform oraz nachylonych powierzchni,
- kamera podążająca za graczem,
- podstawowy atak wręcz,
- slide działający jako unik,
- zdrowie oraz mana gracza,
- regeneracja jednego punktu many co 4 sekundy,
- zaklęcie dalekiego zasięgu kosztujące 2 punkty many,
- HUD pokazujący zdrowie, manę i odblokowane zaklęcie,
- animacje gracza i przeciwników,
- AI przeciwników walczących z bliska i na dystans,
- blokowanie przejścia, dopóki w pokoju żyją przeciwnicy,
- skrzynia odblokowująca zaklęcie,
- uzdrowisko przywracające całe zdrowie,
- walka z bossem,
- ekran `GAME OVER`, restart gry oraz ekran zwycięstwa,
- muzyka odtwarzana podczas gry.

## Kolejność pokojów

```text
Tutorial Room
    -> Battle Room 1
    -> Chest Room
    -> Battle Room 2
    -> Heal Room
    -> Boss Room
```

Każdy pokój posiada własną klasę i własne ustawienie tilemapy, dekoracji, kolizji oraz obiektów.

## Struktura projektu

Kod został podzielony na pliki nagłówkowe w folderze `include` oraz pliki źródłowe w folderze `src`.

Najważniejsze klasy:

- `Game` - tworzy okno, obsługuje główną pętlę gry, zdarzenia, kamerę, HUD i zmianę stanu gry,
- `Player` - odpowiada za ruch, walkę, animacje, zdrowie, manę i zaklęcie,
- `GameObject` - wspólna klasa bazowa dla obiektów występujących w pokojach,
- `Entity` - klasa bazowa dla obiektów posiadających pozycję i hitbox,
- `Room` - wspólna baza dla wszystkich pokojów,
- `RoomManager` - tworzy i zmienia aktualny pokój,
- `RoomExit` - obsługuje przejścia pomiędzy pokojami,
- `Enemy` - podstawowe AI, animacje, obrażenia oraz walka przeciwników,
- `Mushroom` - przeciwnik dystansowy posiadający własne pociski,
- `Boss` - Crystal Golem znajdujący się w ostatnim pokoju,
- `Chest` - interaktywna skrzynia odblokowująca zaklęcie,
- `HealingSanctuary` - obiekt leczący gracza,
- `Projectile` oraz `Spell` - obsługa zaklęcia gracza.

Obiekty w pokojach są przechowywane przy użyciu smart pointerów. Projekt korzysta również z dziedziczenia i polimorfizmu, dzięki czemu nowe typy przeciwników lub obiektów można dodać bez przebudowy całej gry.

## Wymagania

- C++17,
- SFML 2.x,
- Qt Creator z kompilatorem MinGW.

Projekt był tworzony z SFML znajdującym się w katalogu:

```text
C:/SFML
```

Jeśli biblioteka znajduje się w innym miejscu, trzeba zmienić ścieżki w pliku `GateOfThree.pro` albo `CMakeLists.txt`.

## Uruchomienie w Qt Creator

1. Otwórz plik `GateOfThree.pro` w Qt Creatorze.
2. Wybierz zestaw MinGW zgodny z używaną wersją SFML.
3. Uruchom konfigurację qmake.
4. Zbuduj i uruchom projekt.

Podczas budowania wymagane biblioteki SFML oraz `openal32.dll` są kopiowane do katalogu z plikiem wykonywalnym. Folder `Assets` również jest kopiowany automatycznie.

Projekt można alternatywnie zbudować przy użyciu pliku `CMakeLists.txt`.

## Zawartość projektu

```text
GateOfThree/
|-- Assets/          grafiki, animacje, muzyka i pozostałe zasoby
|-- include/         pliki nagłówkowe
|-- src/             pliki źródłowe
|-- CMakeLists.txt   konfiguracja CMake
|-- GateOfThree.pro  konfiguracja qmake / Qt Creator
|-- .gitignore
`-- README.md
```

Aktualna wersja zawiera pełną krótką ścieżkę rozgrywki od tutorialu do pokonania bossa. Projekt nadal można rozwijać o kolejne pokoje, przeciwników, zaklęcia oraz bardziej rozbudowaną fabułę.
