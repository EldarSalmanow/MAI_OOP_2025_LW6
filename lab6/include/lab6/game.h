#ifndef MAI_OOP_2025_GAME_H
#define MAI_OOP_2025_GAME_H

#include <vector>

#include <lab6/npc.h>
#include <lab6/observer.h>


class Game final {
public:

    explicit Game(NPCFactoryPtr factory);

public:

    auto StartBattle(double distance) -> int32_t;

    auto AddNPC(NPCType type,
                Point point,
                const std::string &name) -> int32_t;

    auto SaveObjects(const std::string &filename) const -> int32_t;

    auto LoadObjects(const std::string &filename) -> int32_t;

    auto DumpObjects(std::ostream &ostream) const -> void;

    auto AddObserver(const ObserverPtr &observer) -> void;

    auto NotifyKill(const NPC &killer,
                    const NPC &killed) -> void;

private:

    auto AppendNPC(const NPCPtr &npc) -> int32_t;

private:

    std::vector<NPCPtr> _npcs;
    NPCFactoryPtr _npcFactory;
    std::vector<ObserverPtr> _observers;
};

#endif //MAI_OOP_2025_GAME_H