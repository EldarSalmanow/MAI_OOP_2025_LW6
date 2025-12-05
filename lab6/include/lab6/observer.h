#ifndef MAI_OOP_2025_OBSERVER_H
#define MAI_OOP_2025_OBSERVER_H

#include <fstream>

#include <lab6/npc.h>


class Observer {
public:

    virtual ~Observer();

protected:

    auto OnKillMessage(const NPC &iller,
                       const NPC &killed) -> std::string;

public:

    virtual auto OnKill(const NPC &killer,
                        const NPC &killed) -> void = 0;
};

using ObserverPtr = std::shared_ptr<Observer>;

class Logger : public Observer {
public:

    explicit Logger(std::ofstream file);

public:

    auto OnKill(const NPC &killer,
                const NPC &killed) -> void override;

private:

    std::ofstream _file;
};

class Screen : public Observer {
public:

    auto OnKill(const NPC &killer,
                const NPC &killed) -> void override;
};

#endif //MAI_OOP_2025_OBSERVER_H