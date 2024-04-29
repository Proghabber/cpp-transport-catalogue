#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"


namespace utility{
void ParseAndPrintStat(const catalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);

void ExecuteStatRequests(catalogue::TransportCatalogue& tansport_catalogue, std::ostream& output, std::istream& input);

}