#include <lab6/game.h>
#include <lab6/visitor.h>


Visitor::~Visitor() = default;

Battle::Battle(Game &game)
        : _game(game),
          _target(nullptr) {}

auto Battle::SetTarget(const NPCPtr &target) -> void {
    _target = std::move(target);
}

auto Battle::Visit(Druid *druid) -> void {
    (void) druid;
}

auto Battle::Visit(Squirrel *squirrel) -> void {
    switch (_target->GetType()) {
        case NPCType::Werewolf:
        case NPCType::Druid:
            _target->Kill();
            _game.NotifyKill(*squirrel, *_target);

            break;
        default:
            break;
    }
}

auto Battle::Visit(Werewolf *werewolf) -> void {
    switch (_target->GetType()) {
        case NPCType::Druid:
            _target->Kill();
            _game.NotifyKill(*werewolf, *_target);

            break;
        default:
            break;
    }
}
