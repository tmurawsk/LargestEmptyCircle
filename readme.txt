TOMASZ MURAWSKI
PROJEKT PW.7


[Wywołanie programu]
./largestEmptyCircle [TRYB] [x_lewej_ściany] [x_prawej_ściany] [y_dolnej_ściany] [y_górnej_ściany] [PARAMETRY]


[TRYB]
    1 - Odczytanie punktów z pliku wejściowego i wykonanie na nich obliczeń.
        [PARAMETRY]
            [nazwa_pliku] - nazwa pliku .txt ze współrzędnymi punktów

    2 - Wygenerowanie pewnej liczby losowych punktów oraz wykonanie na nich obliczeń.
        [PARAMETRY]
            [liczba_punktów] - liczba punktów do wygenerowania

    3 - Automatyczne, cykliczne generowanie punktów wraz z pomiarami czasowymi dla obu algorytmów.
        [PARAMETRY]
            [liczba_startowa]   - liczba punktów do wygenerowania, od jakiej ma się zacząć algorytm
            [interwał]          - liczba punktów, o jaką ma wzrastać liczba generowanych punktów co kolejną generację
            [liczba_generacji]  - liczba kolejnych generacji punktów do wygenerowania
            [liczba_w_generacji]- liczba różnych zestawów punktów losowanych w ramach jednej generacji

    4 - Automatyczne, cykliczne generowanie punktów wraz z pomiarami czasowymi dla algorytmu Fortune'a.
        [PARAMETRY]
            [liczba_startowa]   - liczba punktów do wygenerowania, od jakiej ma się zacząć algorytm
            [interwał]          - liczba punktów, o jaką ma wzrastać liczba generowanych punktów co kolejną generację
            [liczba_generacji]  - liczba kolejnych generacji punktów do wygenerowania
            [liczba_w_generacji]- liczba różnych zestawów punktów losowanych w ramach jednej generacji


[Pliki źródłowe]
    main.cpp    - główny moduł programu, odpowiedzialny za zarządzanie przebiegiem programu oraz za oprawę graficzną (konsolową)
    Voronoi.cpp - plik odpowiedzialny za realizację całej części algorytmicznej, zarówno algorytmu Fortune'a, jak i brute-force
    Voronoi.h   - plik nagłówkowy do pliku zawierającego implementację algorytmów
    Point.h     - zawiera strukturę opisującą punkt, a także prymitywne metody
    Parabola.h  - zawiera strukturę opisującą parabolę, będącą częścią linii brzegowej
    Edge.h      - zawiera strukturę opisującą krawędź diagramu Woronoja (skierowaną w jednym kierunku - technicznie więc półprosta)
    Event.h     - zawiera strukturę opisującą wydarzenia okręgu (circle events)