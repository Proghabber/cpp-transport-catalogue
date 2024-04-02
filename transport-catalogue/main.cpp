#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    TransportCatalogue catalogue;

   

    {
        //не совсем понял что требеутся , но  вроде ответственность за заполнение класса TransportCatalogue
        //теперь возложена на InputReader. А вывод информации возложен на функциях из  namespace utility.
        //Что более пригодно для проведения юнит тестов если, такие понадобятся.
        using namespace input_pars;
        InputReader reader;//имелись ввиду вызовы cin и  cout?
        reader.ReadingLines(catalogue, std::cin);

    }
    {
        using namespace utility;
        ReturnRsult(catalogue, std::cout, std::cin);
    }
    
    
}