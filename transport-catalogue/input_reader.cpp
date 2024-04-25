#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>
#include <optional>


namespace input_pars{
using namespace geo_math;
namespace parsers{

size_t RetPosSymbol(std::string_view str, size_t pos, int step, char symbol ){
    size_t start = pos;
    for(int i=0; i < step; i++){
        start=str.find(symbol,start);
        if(start == str.npos){
            return str.npos;
        }
        start=str.find_first_not_of(symbol, start);
        start=str.find_first_not_of(' ', start);
        
    }
    return start;
}

std::optional<std::vector<std::pair<std::string_view, int>>> ParserDistance(std::string_view str){
    //возможно необходимо будет поделить на отдельные функции и упростить
    size_t pos = 0;
    std::vector<size_t> poses;
    std::vector<size_t> counts;
    std::vector<std::string_view> words;
    while (pos != str.npos){
        pos = RetPosSymbol(str, pos, 2, ',');
        poses.push_back(pos);
    }
    if (poses.size() >1 ){
            size_t count = poses[0];
            while (count != str.npos){
                counts.push_back(count);
                count = RetPosSymbol(str, count, 1, ',');           
        }
        counts.push_back(str.npos);
        std::vector<std::pair<std::string_view, int>> dist;
        for (size_t i = 0;i < counts.size(); i++){
            if (counts[i] != str.npos){
                int lenght = counts[i + 1] - counts[i];
                if (i != counts.size() -1){
                    lenght -= 2;
                }
               std::string_view request = str.substr(counts[i], lenght);
               size_t point=RetPosSymbol(request, 0, 1, ' ');
               std::string_view integer = request.substr(0, point - 2);
               std::string_view to = request.substr(point + 3);
               int distance = std::stod(std::string(integer));
               dist.push_back({to, distance});
            }
        }
        return dist;
    }
    return std::nullopt;
}
/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}
}

void InputReader::ParseLine(std::string_view line) {
    using namespace parsers;
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
    // Реализуйте метод самостоятельно
    using namespace parsers;
    Coordinates point;
    for(auto& com : commands_){ 
        if(com.command =="Stop"){
            point  =ParseCoordinates(com.description);
            catalogue.AddStop(com.id, point.lat, point.lng, true);
            //com.description
            auto rez=ParserDistance(com.description);
            if(rez!=std::nullopt){
                catalogue.SetStopNeighbour(com.id,rez.value());
            }
        }
    }
    for (auto& com : commands_){ 
        if (com.command =="Bus"){
            std::vector<std::string_view> stops = ParseRoute(com.description);
            catalogue.AddBus(com.id, stops);
        }
     }
}

void InputReader::ReadingLines(TransportCatalogue& catalogue, std::istream& inputput ){
    size_t lines;
    inputput >> lines >>  std::ws;
    for (size_t i = 0; i < lines; ++i) {
            std::string line;
            getline(inputput, line);
            ParseLine(line);
        }
    ApplyCommands(catalogue);
}

}