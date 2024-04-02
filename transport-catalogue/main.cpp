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
        size_t base_request_count;//Врятли будет столько запросов, но все возможно.
        cin >> base_request_count >> ws;
        InputReader reader;
        reader.ReadingLines(base_request_count, catalogue, cin);

    }
    {
        using namespace utility;
        size_t stat_request_count;
        cin >> stat_request_count >> ws;
        ReturnRsult(stat_request_count, catalogue, cout, cin);
    }
    
    
}