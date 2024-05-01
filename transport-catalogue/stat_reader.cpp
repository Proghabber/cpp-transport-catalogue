#include "stat_reader.h"
#include <iostream>

namespace utility{

void PrintBus(const catalogue::TransportCatalogue& tansport_catalogue, std::string_view left, std::string_view right, std::ostream& output){
    catalogue::InfoBus bus_info = tansport_catalogue.FindBus(right); 
    if (!bus_info.IsEmpty()){
        output<<left<<" "<<right<<": "<<bus_info.amount<<" stops on route, "<<bus_info.unique<<" unique stops, "<<bus_info.distance<<" route length, "<<bus_info.curvature<<" curvature" ;
    } else {      
        output<<left<<" "<<right<<": not found";
    }

}

void PrintStop(const catalogue::TransportCatalogue& tansport_catalogue, std::string_view left, std::string_view right, std::ostream& output){
    std::optional<std::set<std::string_view>>  answer =tansport_catalogue.ReturnBusesWithStop(right);
        output<<left<<" "<<right<<":";
        if(answer == std::nullopt){
            output<<" not found";
        } else {
            if(answer.value().size() == 0){
                output<<" no buses";
            } else { 
                output<<" buses";
                for (std::string_view s : answer.value()){
                    output<<" "<<s;
                }
            }
        }      
}

void ParseAndPrintStat(const catalogue::TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {   
    std::string_view answer;                  
    size_t pos = request.find(' '); 
    std::string_view left = request.substr(0,pos);    
    std::string_view right = request.substr(pos + 1);

    if (left == "Bus"){
        PrintBus(tansport_catalogue, left, right, output);    
    } else if (left == "Stop"){
        PrintStop(tansport_catalogue, left, right, output);
    } 
    output<<std::endl;
}

void ExecuteStatRequests(catalogue::TransportCatalogue& tansport_catalogue, std::ostream& output, std::istream& input){
    size_t lines;
    input >> lines >> std::ws;
    for (size_t i = 0; i < lines; ++i) {
        std::string line;
        getline(input, line);
        ParseAndPrintStat(tansport_catalogue, line, output);
    }
}

}