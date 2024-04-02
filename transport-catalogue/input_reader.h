#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

namespace input_pars{
struct CommandDescription {
    // Определяет, задана ли команда (поле command непустое)
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды
};

using namespace catalogue;

class InputReader {
public:
    
    /**
     * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
     */
    void ParseLine(std::string_view line);

    /**
     * Наполняет данными транспортный справочник, используя команды из commands_
     */
    void ApplyCommands(TransportCatalogue& catalogue) const;

    void ReadingLines(size_t lines, TransportCatalogue& catalogue, std::istream& inputput );

private:
    std::vector<CommandDescription> commands_;
};
}