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
    handler::RequestHandler handl(transport,make);

    readders.ParseRequests(std::cin, handl);
    transport_router::Transport_Router<double> router(handl);
    router.FindPath();
    readders.Print(std::cout, handl);
    return 0;
}