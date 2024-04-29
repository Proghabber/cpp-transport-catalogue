#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue catalogue;

   

    {
        using namespace input_pars;
        InputReader reader;
        reader.ReadLines(catalogue, std::cin);

    }
    {
        using namespace utility;
        ExecuteStatRequests(catalogue, std::cout, std::cin);
    }
    
    
}