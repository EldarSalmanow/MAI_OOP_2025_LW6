#include <fstream>
#include <iostream>
#include <sstream>

#include <lab6/game.h>
#include <lab6/visitor.h>


Game::Game(NPCFactoryPtr factory)
        : _npcFactory(std::move(factory)) {}

auto Game::StartBattle(double distance) -> int32_t {
    DumpObjects(std::cout);

    Battle battle(*this);

    for (auto &defender : _npcs) {
        battle.SetTarget(defender);

        for (auto &attacker : _npcs) {
            if (!attacker->CanAttack(*defender, distance)) {
                continue;
            }

            attacker->Accept(&battle);

            if (defender->GetKilled()) {
                break;
            }
        }
    }

    std::erase_if(_npcs, [] (const NPCPtr &npc) -> bool {
        return npc->GetKilled();
    });

    DumpObjects(std::cout);

    return 0;
}

auto Game::AddNPC(NPCType type,
                  Point point,
                  const std::string &name) -> int32_t {
    auto npc = _npcFactory->CreateNPC(type,
                                             point,
                                             name);

    if (!npc) {
        return 1;
    }

    return AppendNPC(npc);
}

auto Game::SaveObjects(const std::string &filename) const -> int32_t {
    std::ofstream file(filename);

    if (!file.is_open()) {
        return 1;
    }

    DumpObjects(file);

    file.close();

    return 0;
}

auto Game::LoadObjects(const std::string &filename) -> int32_t {
    std::ifstream file(filename);

    if (!file.is_open()) {
        return 1;
    }

    while (auto npc = _npcFactory->LoadNPC(file)) {
        if (AppendNPC(npc)) {
            break;
        }
    }

    file.close();

    return 0;
}

auto Game::DumpObjects(std::ostream &ostream) const -> void {
    for (const auto &npc : _npcs) {
        std::ostringstream string_stream;

        auto type = npc->GetType();
        const auto &name = npc->GetName();
        auto point = npc->GetPoint();

        string_stream << "[" << NPCTypeToString(type) << "] "
                      << name << " [" << point.GetX() << "," << point.GetY() << "]" << std::endl;

        ostream << string_stream.str();
    }
}

auto Game::AddObserver(const ObserverPtr &observer) -> void {
    _observers.emplace_back(observer);
}

auto Game::NotifyKill(const NPC &killer,
                      const NPC &killed) -> void {
    for (auto &observer : _observers) {
        observer->OnKill(killer,
                         killed);
    }
}

auto Game::AppendNPC(const NPCPtr &npc) -> int32_t {
    if (std::ranges::find_if(_npcs,
                             [npc](const NPCPtr &tmp) -> bool {
                                 return tmp->GetName() == npc->GetName();
                             }) != _npcs.end()) {
        return 1;
    }

    _npcs.emplace_back(npc);

    return 0;
}
