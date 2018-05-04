Tema 2 - Sistem monetar Internet Banking

Nume: Mihalache Florin-Razvan
Grupa: 323CC

Client:
    - se trimite tot ce se citeste de la tastatura la server si afiseaza ce
primeste de la server la consola (inputul e trimis pe socketul TCP, mai putin
comenzile legate de unlock, care sunt pe UDP -> aici salvez numarul de card de
la ultimul login ca sa il folosesc la unlock + variabila booleana daca se
asteapta parola sau nu in caz ca sunt indeplinite conditiile pentru unlock)

Server:
    - am realizat multiplexarea cu select si FD_SET si parsarea comenzilor cu
sscanf (era lejer asa).
    - login (TCP) - verific conditiile in urmatoarea ordine: daca exista un
client deja logat in sesiunea curenta, daca exista numarul de card dat, daca
cardul e blocat, daca exista pinul la cardul respectiv (incerc de maxim 3 ori,
la a treia incercare cardul este blocat).
    - logout (TCP) - vad daca am client autentificat si daca da -> delogare
    - listsold (TCP) - vad daca am client autentificat si daca da -> afisez
cat are in cont
    - transfer (TCP) - aici am facut o manarie ca sa imi mearga ca lumea (inainte
mergea aiurea, trebuia sa raspund la intrebarea de confirmare a transferului
inainte sa apara aceasta pe ecran - WTF) -> vad daca ultima comanda daca este
transfer. Mai intai verific urmatoarele conditii: daca sunt logat, daca exista
contul la care vreau sa transfer (adica cardul), daca am fonduri suficiente,
daca suma data e mai mica sau egala cu 0 si daca totul e ok, pastrez suma pe care
vreau sa o transfer si indicii clientilor intre care sa fac transferul. Apoi
vad daca ultima comanda e transfer si daca da efectuez transferul (ciudat asa,
dar hei, merge!).
    - unlock (UDP) - verific mai intai daca: exista cardul respectiv, nu e blocat
contul respectiv -> afisez sa se trimita parola secreta + setez daca se asteapta
parola. Daca se asteapta parola, verific daca parola data la tastatura corespunde
cu contul si daca da -> deblocare
