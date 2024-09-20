#include <iostream>

#include "json_reader.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "transport_router.h"

using namespace std;

int main() {
    readJson::JsonReader readders;
    catalogue::TransportCatalogue transport;
    render::SvgMaker make;
    transport_router::Transport_Router router(transport);
    handler::RequestHandler handl(transport, make, readders, router);
    handl.ReadJson(std::cin);
    handl.ReturnJson(std::cout);

    return 0;
}