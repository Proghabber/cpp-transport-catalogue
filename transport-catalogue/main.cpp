#include <iostream>
#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"

using namespace std;

int main() {
    readJson::JsonReader readders;
    catalogue::TransportCatalogue transport;
    render::SvgMaker make;
    handler::RequestHandler handl(transport, make, readders);
    handl.ReadJson(std::cin);
    handl.ReturnJson(std::cout);

    return 0;
}