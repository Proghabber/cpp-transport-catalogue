#include <iostream>
#include <string>

#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"



using namespace std;



int main() {
    /*catalogue::TransportCatalogue catalogue;

   

    {
        using namespace input_pars;
        InputReader reader;
        reader.ReadLines(catalogue, std::cin);

    }
    {
        using namespace utility;
        ExecuteStatRequests(catalogue, std::cout, std::cin);
    }*/
   
    //RequestHandler hendler;
    //std::cout<<readJson::ReturnRequest(std::cin).str();
    readJson::JsonReader readders;
    catalogue::TransportCatalogue transport;
    render::SvgMaker make;
    handler::RequestHandler handl(transport,make);
    
    
    readders.ParsingRequests(std::cin, handl);
    //handl.MakeImage(std::cout);
    
    
   std::cout<<readders.Print(handl);
    return 0;
}