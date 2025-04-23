# Specyfikacja Protokołu

Komunikacja pomiędzy klientem a serwerem odbywa się przy pomocy strumieni TCP.

Klient wysyła wyrazy oddzielane pojedynczymi spacjami. Bezpośrednio za ostatnim wyrazem wysyła dwuznak `\r\n` oznaczający koniec zapytania.

Serwer na każde zapytanie odpowiada ciągiem znaków zakończonym dwuznakiem `\r\n`. Pozostałe znaki ciągu tworzą:

- A: ułamek (złożony z cyfr `0-9` i znaku `/`), którego licznik oznacza liczbę palindromów w zapytaniu, a mianownik liczbę wszystkich wyrazów w zapytaniu

- B: ciąg liter `ERROR`



## FAQ

Czy utożsamiamy wielkie i małe litery?
 - Tak, np. wyraz `Ala` traktowany jest jak palindrom.

Czy jednoliterowe wyrazy są palindromami?
- Tak.

Jak serwer odpowiada na puste zapytania?
- Odpowiedzią serwera na zapytanie `\r\n` jest `0/0`.

**Czy jest limit długości zapytań?**
- **Długość zapytania nie jest ograniczona, ale serwer może zdefiniować maksymalną długość wyrazu - w przypadku przekroczenia tej długości odpowiedzią serwera będzie ERROR.**

# Algorytm Serwera

```
while (serwuj kolejnych klientów) {
    b << 1 znak;
    switch(b) {
        case (litera):
            dodaj lowercase b na stos;
        case (spacja):
            jeśli kolejna, ustaw flagę ERROR;
            inaczej:
            przetwórz słowo na stosie;
            dodaj do wyniku;
        case ('\r'):
            zapamiętaj;
        case ('\n'):
            jeśli poprzednie było '\r', wyślij odpowiedź, zresetuj flagi i wynik;
            inaczej ustaw flagę ERROR;
        case (pozostałe):
            ustaw flagę ERROR;
    }
}
```