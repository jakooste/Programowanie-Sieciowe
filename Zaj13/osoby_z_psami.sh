#!/bin/sh

# Skrypt uruchamiający i demonstrujący działanie aplikacji z pliku
# rest_webapp.py. Skrypt przetestowano pod Debianem 7, czyli dystrybucją
# zainstalowaną w pracowniach studenckich.
#
# Przy ręcznym testowaniu  webaplikacji możesz chcieć użyć "curl -v" aby
# zobaczyć nagłówki zapytań i odpowiedzi HTTP.


# Zainicjuj bazę z danymi osób.

rm -f osoby.sqlite

sqlite3 osoby.sqlite "
PRAGMA foreign_keys = ON;
CREATE TABLE osoby (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    imie VARCHAR,
    nazwisko TEXT,
    telefon TEXT,
    adres TEXT
);
CREATE TABLE psy (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    imie VARCHAR,
    rasa TEXT,
    wlasciciel_id INTEGER,
    FOREIGN KEY(wlasciciel_id) REFERENCES osoby(id) ON UPDATE CASCADE ON DELETE SET NULL
);
INSERT INTO osoby VALUES (1, 'Anna', 'Nowak', '+48124569988',
    'Rynek Główny 2, 30-001 Kraków');
INSERT INTO osoby VALUES (2, 'Jan', 'Kowalski', '+48127770022',
    'ul. Podzamcze 1, 31-001 Kraków');
INSERT INTO osoby VALUES (3, 'Tim', 'Rogers', '+48123456789',
    'ul. Tokimeki 64, 32-002 Osaka');
INSERT INTO psy VALUES (1, 'Bibby Babbis', 'pomeranian', 3);
INSERT INTO psy VALUES (2, 'Reksio', 'kundel', 2);
INSERT INTO psy VALUES (3, 'Azor', 'samoyed', 1);
INSERT INTO psy VALUES (4, 'Mago', 'borzoi', 1);
"

# Dla pewności wypisz na ekran jej zawartość.

echo "Początkowa zawartość bazy:"
echo "Osoby:"
sqlite3 --header osoby.sqlite "SELECT * FROM osoby"
echo "Psy:"
sqlite3 --header osoby.sqlite "SELECT * FROM psy"

# Uruchom w tle serwer z webaplikacją.

env -i python3 -u osoby_z_psami_webapp.py > stdout.txt 2> stderr.txt &
server_pid=$!
sleep 1

# Testy aplikacji:

echo
echo "Test 0: pobieranie całej bazy"
curl http://127.0.0.1:8000/osoby

echo
echo "Test 1: pobieranie rekordu osoby"
curl http://127.0.0.1:8000/osoby/1

echo
echo "Test 2: uaktualnianie rekordu osoby"
printf "nazwisko\tadres\n" > dane.tsv
printf "Kowalska\tul. Podzamcze 1, 31-001 Kraków\n" >> dane.tsv
curl --upload-file dane.tsv \
        --header "Content-Type: text/tab-separated-values; charset=UTF-8" \
        http://127.0.0.1:8000/osoby/1
# użycie opcji --upload-file zmienia domyślną metodę na PUT

echo
echo "Test 3: usuwanie rekordu osoby"
curl --request DELETE http://127.0.0.1:8000/osoby/2

echo
echo "Test 4: dodawanie nowego rekordu osoby"
printf "imie\tnazwisko\ttelefon\tadres\n" > dane.tsv
printf "Adam\tWiśniewski\t+48120124433\tul. Reymonta 4\n" >> dane.tsv
curl --request POST --upload-file dane.tsv \
        --header "Content-Type: text/tab-separated-values; charset=UTF-8" \
        http://127.0.0.1:8000/osoby

echo
echo "Test 5: wyszukanie według imienia i nazwiska"
curl "http://127.0.0.1:8000/osoby/search?imie=Tim&nazwisko=Rogers"

echo
echo "Test 6: pobieranie tabeli z psami"
curl "http://127.0.0.1:8000/psy"

echo
echo "Test 7: pobieranie rekordu psa"
curl "http://127.0.0.1:8000/psy/1"

echo "Test 8: uaktualnianie rekordu psa"
printf "imie\trasa\twlasciciel_id\n" > dane.tsv
printf "Azor\towczarek niemiecki\t4\n" >> dane.tsv
curl --upload-file dane.tsv \
        --header "Content-Type: text/tab-separated-values; charset=UTF-8" \
        http://127.0.0.1:8000/psy/3
# użycie opcji --upload-file zmienia domyślną metodę na PUT

echo
echo "Test 9: usuwanie rekordu psa"
curl --request DELETE http://127.0.0.1:8000/psy/4

echo
echo "Test 10: dodawanie nowego rekordu psa"
printf "imie\trasa\twlasciciel_id\n" > dane.tsv
printf "Kuskus\tshiba inu\t4\n" >> dane.tsv
curl --request POST --upload-file dane.tsv \
        --header "Content-Type: text/tab-separated-values; charset=UTF-8" \
        http://127.0.0.1:8000/psy

echo
echo "Końcowa zawartość bazy:"
echo "Osoby:"
sqlite3 --header osoby.sqlite "SELECT * FROM osoby"
echo "Psy:"
sqlite3 --header osoby.sqlite "SELECT * FROM psy"

# Koniec testów, można wyłączyć serwer aplikacyjny.

kill $server_pid
