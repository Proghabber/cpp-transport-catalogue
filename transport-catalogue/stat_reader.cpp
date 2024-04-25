#include "stat_reader.h"
#include <iostream>

namespace utility{

void PrintBus(const TransportCatalogue& tansport_catalogue, std::string_view left, std::string_view rigth, std::ostream& output  ){
     InfoBus rez;
    rez = tansport_catalogue.ReturnBus(rigth);
    if (!rez.IsEmpty()){
            double distance =tansport_catalogue.CountDist(rigth);
            double curvature= distance/rez.length;
            output<<left<<" "<<rigth<<": "<<rez.amount<<" stops on route, "<<rez.unique<<" unique stops, "<<distance<<" route length, "<<curvature<<" curvature" ;
    } else {
        
        output<<left<<" "<<rigth<<": not found";
    }

}

void PrintStop(const TransportCatalogue& tansport_catalogue, std::string_view left, std::string_view rigth, std::ostream& output  ){
    std::set<std::string>  answer =tansport_catalogue.ReturnStop(rigth);
        output<<left<<" "<<rigth<<":";
        if(!answer.count("not found") && !answer.count("no buses")){
            output<<" buses";
        }
        for (std::string s : answer){
            output<<" "<<s;
        }
}

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {   
    std::string_view answer;                  
    size_t pos = request.find(' '); 
    std::string_view left = request.substr(0,pos);    
    std::string_view rigth = request.substr(pos+1);

    if (left == "Bus"){
        PrintBus(tansport_catalogue, left, rigth, output);    
    } else if (left == "Stop"){
        PrintStop(tansport_catalogue, left, rigth, output);
    } 
    output<<std::endl;
}

void ReturnRsult(TransportCatalogue& catalogue, std::ostream& output, std::istream& input ){
    size_t lines;
    input >> lines >> std::ws;
    for (size_t i = 0; i < lines; ++i) {
        std::string line;
        getline(input, line);
        ParseAndPrintStat(catalogue, line, output);
    }
}

}