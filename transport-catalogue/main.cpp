#include <iostream>

#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"

using namespace std;

int main() {
    
    readJson::JsonReader readders;
    catalogue::TransportCatalogue transport;
    render::SvgMaker make;
    handler::RequestHandler handl(transport,make);
    
    readders.ParseRequests(std::cin, handl);
    std::cout<<readders.Print(handl);
    return 0;
}