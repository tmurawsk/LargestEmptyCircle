[Wywołanie programu]
./largestEmptyCircle [TRYB] [x_lewej_ściany] [x_prawej_ściany] [y_dolnej_ściany] [y_górnej_ściany] [PARAMETRY]


[TRYB]
    1 - Odczytanie punktów z pliku wejściowego i wykonanie na nich obliczeń.
        [PARAMETRY]
            [nazwa_pliku] - nazwa pliku .txt ze współrzędnymi punktów

    2 - Wygenerowanie pewnej liczby losowych punktów oraz wykonanie na nich obliczeń.
        [PARAMETRY]
            [liczba_punktów] - liczba punktów do wygenerowania

    3 - Automatyczne, cykliczne generowanie punktów wraz z pomiarami czasowymi.
        [PARAMETRY]
            [liczba_startowa]   - liczba punktów do wygenerowania, od jakiej ma się zacząć algorytm
            [interwał]          - liczba punktów, o jaką ma wzrastać liczba generowanych punktów co kolejną generację
            [liczba_generacji]  - liczba kolejnych generacji punktów do wygenerowania
            [liczba_w_generacji]- liczba różnych zestawów punktów losowanych w ramach jednej generacji