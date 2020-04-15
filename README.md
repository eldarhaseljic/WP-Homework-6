# WP-Homework-6
Fakultet elektrotehnike Tuzla -  Windows programiranje 

Zadatak 1
Kreirati CodeBlocks projekat za realizaciju klijentskog administrativnog alata za sqlite bazu. Potrebno je zadovoljiti sljedeće specifikacije:
    1. Aplikacija treba da se sastoji od 2 prozora. U jedan prozor se unosi sql naredba, dok se u drugom prozoru ispisuje rezultat izvršenja te naredbe.
    2. Drugi prozor je posjedovani prozor prvog prozora. Ukoliko se zatvori bilo koji od ova dva prozora, zatvaraju se oba prozora. Oba prozora su uvijek vidljiva.
    3. Samo jedna naredba se može izvršiti u datom trenutku.
    4. Program je namijenjen samo za izvršavanje osnovnih CRUD operacija. SELECT naredba uključuje osnovne SELECT naredbe, na nivou tabele, bez joinsa. Obični select po atributu ili svim atributima (*).
    5. U slučaju errora, ispisati error u drugi prozor. 
    6. Za slučaj INSERT, DELETE i UPDATE naredbi ispisati u drugom prozoru samo da je naredba izvršena uspješno.
    7. Za ispis rezultata SELECT naredbe u drugom prozoru, može se koristiti lista kao i na vježbama.
    8. Prvi prozor posjeduje dugme na čiji klik se izvršava naredba.
    9. Aplikacija radi sa bazom za vođenje evidencije o letovima.
    10. Bazu (i tabele) je moguće kreirati korištenjem sqlite3.exe alata, ili programski.
    11. Model baze je prikazan na slici:
