#include <cmath>
#include <sstream>

#include <lab6/npc.h>
#include <lab6/visitor.h>


auto NPCTypeToString(NPCType type) -> std::string_view {
    switch (type) {
        case NPCType::Squirrel:
            return "Squirrel";
        case NPCType::Werewolf:
            return "Werewolf";
        case NPCType::Druid:
            return "Druid";
    }

    return "<unknown>";
}

auto StringToNPCType(const std::string &string) -> NPCType {
    if (string == "Druid") {
        return NPCType::Druid;
    }
    else if (string == "Squirrel") {
        return NPCType::Squirrel;
    }
    else if (string == "Werewolf") {
        return NPCType::Werewolf;
    }

    throw std::runtime_error("[ERROR] Unknown type!");
}

NPC::NPC(Point point,
         std::string name)
    : _point(point),
      _name(std::move(name)),
      _killed(false) {}

NPC::~NPC() = default;

auto NPC::Kill() -> void {
    _killed = true;
}

auto NPC::CanAttack(const NPC &defender,
                    double distance) const -> bool {
    const double EPSILON = 1e-9;

    if (this == &defender) {
        return false;
    }

    if (GetKilled() || defender.GetKilled()) {
        return false;
    }

    auto x       = _point.GetX(), y                = _point.GetY();
    auto x_other = defender.GetPoint().GetX(), y_other    = defender.GetPoint().GetY();
    auto dist_x = x - x_other, dist_y = y - y_other;

    return (distance - std::sqrt(dist_x * dist_x + dist_y * dist_y)) > EPSILON;
}

auto NPC::GetPoint() const -> const Point & {
    return _point;
}

auto NPC::GetName() const -> const std::string & {
    return _name;
}

auto NPC::GetKilled() const -> bool {
    return _killed;
}

Druid::Druid(Point point,
             std::string name)
        : NPC(std::move(point),
              std::move(name)) {}

auto Druid::Accept(Visitor *visitor) -> void {
    return visitor->Visit(this);
}

auto Druid::GetType() const -> NPCType {
    return NPCType::Druid;
}

Squirrel::Squirrel(Point point,
                   std::string name)
        : NPC(point,
              std::move(name)) {}

auto Squirrel::Accept(Visitor *visitor) -> void {
    return visitor->Visit(this);
}

auto Squirrel::GetType() const -> NPCType {
    return NPCType::Squirrel;
}

Werewolf::Werewolf(Point point,
                   std::string name)
        : NPC(point,
              std::move(name)) {}

auto Werewolf::Accept(Visitor *visitor) -> void {
    return visitor->Visit(this);
}

auto Werewolf::GetType() const -> NPCType {
    return NPCType::Werewolf;
}

NPCFactory::~NPCFactory() = default;

auto NPCFactory::LoadNPC(std::istream &istream) const -> NPCPtr {
    std::string line;

    std::getline(istream, line);

    if (istream.eof()) {
        return nullptr;
    }

    std::istringstream string_stream(line);

    std::string str_type, name, str_point;
    string_stream >> str_type >> name >> str_point;

    NPCType type  = StringToNPCType(str_type.substr(1, str_type.size() - 2));
    std::string x = str_point.substr(1, str_point.find(','));
    std::string y = str_point.substr(str_point.find(',') + 1, str_point.size() - str_point.find(',') - 1);
    Point point(std::atoi(x.c_str()), std::atoi(y.c_str()));

    return CreateNPC(type, point, name);
}

auto NPCFactory::CreateNPC(NPCType type,
                           Point point,
                           const std::string &name) const -> NPCPtr {
    if (point.GetX() > 500 || point.GetY() > 500) {
        return nullptr;
    }

    switch (type) {
        case NPCType::Squirrel:
            return std::make_shared<Squirrel>(point,
                                              std::move(name));
        case NPCType::Werewolf:
            return std::make_shared<Werewolf>(point,
                                              std::move(name));
        case NPCType::Druid:
            return std::make_shared<Druid>(point,
                                           std::move(name));
    }

    throw std::runtime_error("[INTERNAL] Missed NPC type handling in NPCFactory::CreateNPC!");
}
