# Tema1_APD
*ALEXANDRESCU MARIUS-CRISTIAN - 331CB*

*Tema 1 - APD*

Citesc din fisierul dat ca input numele celorlalte fisiere de input si le pun intr-o lista.
Aceasta lista impreuna cu alte valori se afla intr-o structura alocata dinamic. Aceasta structura
mai contine numarul de mapperi (citit de la input), numarul de reduceri (citit de la input),
un map in care key-ul reprezinta numarul care este ridicat la putere. Creez apoi
aux->mappers threaduri pentru mappers. Fiecare thread va lua fisierele de la start la end,
aceste valori fiind calculate ca in laboratorul 1. Citind din fisier, iau fiecare valoare
si rulez functia isPower. Aceasta functie realizeaza o aproximare a radicalului de ordin k,
folosind o functie invatata la Metode Numerice in anul 1. Pentru aceasta functie am scris si
o functie de ridicare la putere mai rapida ca functia pow gasita pe internet. Folosesc si un
mutex la scrierea in vector pentru ca poate aparea un race condition la functia de push_back.
Dupa ce toate threadurile mappers au ajuns la bariera, pot porni si threadurile reducers.
Acestea iau fiecare cate o intrare din map pentru o anumita baza si elimina toate numerele
care apar de mai multe ori, apoi scriu in fisierul out*.txt marimea vectorului (numarul de elemente).
Mai am si o structura pentru a lua thread_id-ul threadurilor.
