# SK_testhttp
Programu testhttp_raw zawierający oprogramowanie klienta oraz skrypt testhttp. Zadaniem tych programów jest zapewnienie narzędzia do prostego testowania stron WWW. Używamy protokołu warstwy aplikacji HTTP w wersji 1.1, protokołu warstwy transportu TCP i warstwy sieci IPv4.

Klient po zinterpretowaniu argumentów wiersza poleceń łączy się ze wskazanym adresem i portem, wysyła do serwera HTTP żądanie podania wskazanej strony, odbiera odpowiedź od serwera, analizuje wynik i podaje raport.
Opis komunikacji

Techniczny opis formatu żądań i odpowiedzi HTTP znajduje się w dokumencie RFC7230. Klient łączy się ze wskazanym adresem i portem, a następnie wysła odpowiednie żądanie HTTP. Adres i port połączenia nie muszą się zgadzać z adresem i portem wskazanym w adresie testowanej strony.

Jeśli odpowiedź serwera jest inna niż 200 OK (np. 202 Accepted) klient ma podaje raport w postaci zawartości wiersza statusu uzyskanej odpowiedzi. Jeśli odpowiedź serwera jest 200 OK, raport ma składa się z dwóch części: zestawienia ciasteczek oraz rzeczywistej długości przesyłanego zasobu. Techniczny opis formatu pola nagłówkowego Set-Cookie znajduje się w dokumencie RFC2109. Należy tutaj pamiętać, że jedna odpowiedź HTTP może zawierać wiele pól Set-Cookie. Należy przyjąć, że jedno pole nagłówkowe Set-Cookie ustawia jedno ciasteczko. Jeśli w jednym polu ustawiane jest wiele ciasteczek należy ciasteczka poza pierwszym pominąć. Jeśli implementacja przyjmuje ograniczenia na liczbę przyjmowanych ciasteczek i ich długość, to ograniczenia te powinny zostać dobrane zgodnie z założeniami przyjętymi w standardach HTTP dla rozwiązań ogólnego przeznaczenia. Dodatkowo przy liczeniu długości przesyłanego zasobu należy uwzględnić możliwość, że zasób był wysłany w częściach (kodowanie przesyłowe chunked).

Opis wypisywanego raportu

Raport składa się z dwóch następujących bezpośrednio jedna po drugiej części: zestawienia ciasteczek oraz rzeczywistej długości przesyłanego zasobu. Zestawienie ciasteczek składa się z liczby linii równej liczbie ciasteczek. Każde ciasteczko wypisywane jest w osobnym wierszu w formacie klucz=wartość. Rzeczywista długość przesyłanego zasobu składa się z jednego wiersza postaci Dlugosc zasobu: , gdzie to zapisana w systemie dziesiętnym długość zasobu.

Opis wiersza poleceń

Wywołanie programu klienta ma ogólną postać:

testhttp_raw <adres połączenia>:<port> <plik ciasteczek> <testowany adres http>

gdzie

    <adres połączenia> to adres, z którym klient ma się połączyć;
    <port> to numer portu, z którym klient ma się podłączyć;
    <plik ciasteczek> to plik zawierający ciasteczka wysyłane do serwera HTTP, format opisany poniżej;
    <testowany adres http> to adres strony, która ma być zaserwowana przez serwer HTTP.

W pliku zawierającym ciasteczka każde ciasteczko jest zawarte w osobnym wierszu w formacie klucz=wartość (bez spacji naokoło znaku =).

Przykładowe wywołania:

  ./testhttp_raw www.mimuw.edu.pl:80 ciasteczka.txt http://www.mimuw.edu.pl/;

  ./testhttp_raw 127.0.0.1:30303 ciasteczka.txt https://www.mimuw.edu.pl/;

  ./testhttp <plik ciasteczek> <testowany adres http>.
