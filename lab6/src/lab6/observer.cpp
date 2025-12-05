#include <iostream>
#include <sstream>

#include <lab6/observer.h>


Observer::~Observer() = default;

auto Observer::OnKillMessage(const NPC &killer,
                             const NPC &killed) -> std::string {
    std::ostringstream string_stream;

    string_stream << "[" << killed.GetName() << "] killed by [" << killer.GetName() << "]!";

    return string_stream.str();
}

Logger::Logger(std::ofstream file)
        : _file(std::move(file)) {}

auto Logger::OnKill(const NPC &killer,
                    const NPC &killed) -> void {
    _file << OnKillMessage(killer, killed) << std::endl;

    _file.flush();
}

auto Screen::OnKill(const NPC &killer,
                    const NPC &killed) -> void {
    std::cout << OnKillMessage(killer, killed) << std::endl;
}
