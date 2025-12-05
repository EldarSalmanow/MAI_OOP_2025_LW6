#ifndef MAI_OOP_2025_VISITOR_H
#define MAI_OOP_2025_VISITOR_H

#include <memory>

#include <lab6/npc.h>


class Visitor {
public:

    virtual ~Visitor();

public:

    virtual auto Visit(Druid *druid) -> void = 0;

    virtual auto Visit(Squirrel *squirrel) -> void = 0;

    virtual auto Visit(Werewolf *werewolf) -> void = 0;
};

using VisitorPtr = std::shared_ptr<Visitor>;

class Game;

class Battle : public Visitor {
public:

    explicit Battle(Game &game);

public:

    auto SetTarget(const NPCPtr &target) -> void;

public:

    auto Visit(Druid *druid) -> void override;

    auto Visit(Squirrel *squirrel) -> void override;

    auto Visit(Werewolf *werewolf) -> void override;

private:

    Game &_game;

    NPCPtr _target;
};

#endif //MAI_OOP_2025_VISITOR_H