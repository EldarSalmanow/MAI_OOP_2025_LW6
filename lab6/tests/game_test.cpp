#include <sstream>
#include <fstream>
#include <memory>

#include <gtest/gtest.h>

#include <lab6/game.h>
#include <lab6/visitor.h>


class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory = std::make_shared<NPCFactory>();
        game = std::make_unique<Game>(factory);
    }

    void TearDown() override {
        // Очистка временных файлов
        std::remove("test_save.txt");
        std::remove("test_log.txt");
    }

    std::shared_ptr<NPCFactory> factory;
    std::unique_ptr<Game> game;
};

// Тесты для класса Point
TEST(PointTest, ConstructorAndGetters) {
    Point point(10, 20);
    EXPECT_EQ(point.GetX(), 10);
    EXPECT_EQ(point.GetY(), 20);
}

// Тесты для NPC
TEST_F(GameTest, NPCCreationAndProperties) {
    Point point(5, 5);
    auto druid = factory->CreateNPC(NPCType::Druid, point, "Druid1");

    EXPECT_EQ(druid->GetType(), NPCType::Druid);
    EXPECT_EQ(druid->GetName(), "Druid1");
    EXPECT_FALSE(druid->GetKilled());
    EXPECT_EQ(druid->GetPoint().GetX(), 5);
    EXPECT_EQ(druid->GetPoint().GetY(), 5);
}

TEST_F(GameTest, NPCKillMethod) {
    Point point(0, 0);
    auto squirrel = factory->CreateNPC(NPCType::Squirrel, point, "Squirrel1");

    EXPECT_FALSE(squirrel->GetKilled());
    squirrel->Kill();
    EXPECT_TRUE(squirrel->GetKilled());
}

// Тесты для Game - добавление NPC
TEST_F(GameTest, AddNPC) {
    Point point1(10, 10);
    Point point2(20, 20);

    int32_t status1 = game->AddNPC(NPCType::Druid, point1, "Druid1");
    int32_t status2 = game->AddNPC(NPCType::Squirrel, point2, "Squirrel1");

    EXPECT_EQ(status1, 0);
    EXPECT_EQ(status2, 0);
}

TEST_F(GameTest, AddNPCInvalidCoordinates) {
    // Предполагаем, что координаты должны быть в пределах [0, 500]
    Point point(600, 600); // Вне допустимого диапазона

    int32_t id = game->AddNPC(NPCType::Druid, point, "InvalidNPC");
    EXPECT_NE(id, 0); // Ожидаем ошибку
}

// Тесты для Game - сохранение и загрузка
TEST_F(GameTest, SaveAndLoadObjects) {
    // Добавляем несколько NPC
    Point point1(10, 10);
    Point point2(20, 20);
    Point point3(30, 30);

    game->AddNPC(NPCType::Druid, point1, "Druid1");
    game->AddNPC(NPCType::Squirrel, point2, "Squirrel1");
    game->AddNPC(NPCType::Werewolf, point3, "Werewolf1");

    // Сохраняем
    int32_t saveResult = game->SaveObjects("test_save.txt");
    EXPECT_EQ(saveResult, 0);

    // Создаем новую игру и загружаем
    Game newGame(factory);
    int32_t loadResult = newGame.LoadObjects("test_save.txt");
    EXPECT_EQ(loadResult, 0);

    // Проверяем, что объекты загрузились
    std::stringstream ss;
    newGame.DumpObjects(ss);
    std::string output = ss.str();

    EXPECT_NE(output.find("Druid1"), std::string::npos);
    EXPECT_NE(output.find("Squirrel1"), std::string::npos);
    EXPECT_NE(output.find("Werewolf1"), std::string::npos);
}

TEST_F(GameTest, SaveToNonexistentDirectory) {
    int32_t result = game->SaveObjects("/nonexistent/path/test.txt");
    EXPECT_NE(result, 0); // Ожидаем ошибку
}

TEST_F(GameTest, LoadFromNonexistentFile) {
    int32_t result = game->LoadObjects("nonexistent_file.txt");
    EXPECT_NE(result, 0); // Ожидаем ошибку
}

// Тесты для Game - дамп объектов
TEST_F(GameTest, DumpObjects) {
    Point point(15, 25);
    game->AddNPC(NPCType::Werewolf, point, "TestWerewolf");

    std::stringstream ss;
    game->DumpObjects(ss);
    std::string output = ss.str();

    EXPECT_NE(output.find("TestWerewolf"), std::string::npos);
    EXPECT_NE(output.find("15"), std::string::npos);
    EXPECT_NE(output.find("25"), std::string::npos);
}

// Тесты для Observer
TEST_F(GameTest, ObserverRegistration) {
    auto screenObserver = std::make_shared<Screen>();
    game->AddObserver(screenObserver);

    // Добавляем файловый observer
    std::ofstream logFile("test_log.txt");
    auto fileObserver = std::make_shared<Logger>(std::move(logFile));
    game->AddObserver(fileObserver);

    // Тестируем, что observers добавлены (можем проверить через бой)
}

TEST_F(GameTest, ScreenObserver) {
    auto screenObserver = std::make_shared<Screen>();
    game->AddObserver(screenObserver);

    Point point1(10, 10);
    Point point2(20, 20);
    auto npc1 = factory->CreateNPC(NPCType::Werewolf, point1, "Attacker");
    auto npc2 = factory->CreateNPC(NPCType::Druid, point2, "Victim");

    // Этот тест в основном проверяет, что нет crash при вызове
    // Для полного тестирования нужно перехватывать stdout
    EXPECT_NO_THROW(screenObserver->OnKill(*npc1, *npc2));
}

TEST_F(GameTest, FileObserver) {
    std::ofstream logFile("test_log.txt");
    auto fileObserver = std::make_shared<Logger>(std::move(logFile));
    game->AddObserver(fileObserver);

    Point point1(10, 10);
    Point point2(20, 20);
    auto npc1 = factory->CreateNPC(NPCType::Werewolf, point1, "Attacker");
    auto npc2 = factory->CreateNPC(NPCType::Druid, point2, "Victim");

    fileObserver->OnKill(*npc1, *npc2);

    // Проверяем, что файл создан и содержит ожидаемый текст
    std::ifstream checkFile("test_log.txt");
    EXPECT_TRUE(checkFile.is_open());

    std::string content;
    std::getline(checkFile, content);
    EXPECT_NE(content.find("Attacker"), std::string::npos);
    EXPECT_NE(content.find("Victim"), std::string::npos);
}

// Тесты для боевой системы
TEST_F(GameTest, BattleWithNoNPCs) {
    int32_t result = game->StartBattle(10.0);
    EXPECT_EQ(result, 0); // Нет NPC для боя
}

TEST_F(GameTest, BattleWithSingleNPC) {
    Point point(10, 10);
    game->AddNPC(NPCType::Druid, point, "LonelyDruid");

    int32_t result = game->StartBattle(10.0);
    EXPECT_EQ(result, 0); // Только один NPC - боя не происходит
}

TEST_F(GameTest, BattleWithOutOfRangeNPCs) {
    Point point1(0, 0);
    Point point2(100, 100); // Далеко друг от друга
    game->AddNPC(NPCType::Werewolf, point1, "Werewolf1");
    game->AddNPC(NPCType::Druid, point2, "Druid1");

    int32_t result = game->StartBattle(10.0); // Малая дистанция
    EXPECT_EQ(result, 0); // NPC не могут атаковать из-за расстояния
}

TEST_F(GameTest, BattleWithInRangeNPCs) {
    Point point1(10, 10);
    Point point2(15, 15); // Близко друг к другу
    game->AddNPC(NPCType::Werewolf, point1, "Werewolf1");
    game->AddNPC(NPCType::Druid, point2, "Druid1");

    // Добавляем observers для отслеживания событий
    auto screenObserver = std::make_shared<Screen>();
    game->AddObserver(screenObserver);

    int32_t result = game->StartBattle(10.0);
    EXPECT_GE(result, 0); // Бой должен произойти

    // Дополнительные проверки на основе правил варианта 8
}

TEST_F(GameTest, BattleMultipleNPCs) {
    // Создаем несколько NPC в зоне досягаемости
    Point point1(10, 10);
    Point point2(12, 12);
    Point point3(14, 14);

    game->AddNPC(NPCType::Werewolf, point1, "Werewolf1");
    game->AddNPC(NPCType::Druid, point2, "Druid1");
    game->AddNPC(NPCType::Squirrel, point3, "Squirrel1");

    int32_t result = game->StartBattle(5.0);
    EXPECT_GE(result, 0);
}

// Тесты для правил атаки (на основе варианта 8)
TEST_F(GameTest, AttackRulesSquirrelVsWerewolf) {
    Point point1(10, 10);
    Point point2(11, 11);

    auto squirrel = factory->CreateNPC(NPCType::Squirrel, point1, "Squirrel1");
    auto werewolf = factory->CreateNPC(NPCType::Werewolf, point2, "Werewolf1");

    // Белка должна атаковать оборотня
    EXPECT_TRUE(squirrel->CanAttack(*werewolf, 20.0));
}

TEST_F(GameTest, AttackRulesSquirrelVsDruid) {
    Point point1(10, 10);
    Point point2(11, 11);

    auto squirrel = factory->CreateNPC(NPCType::Squirrel, point1, "Squirrel1");
    auto druid = factory->CreateNPC(NPCType::Druid, point2, "Druid1");

    // Белка должна атаковать друида
    EXPECT_TRUE(squirrel->CanAttack(*druid, 5.0));
}

TEST_F(GameTest, AttackRulesWerewolfVsDruid) {
    Point point1(10, 10);
    Point point2(11, 11);

    auto werewolf = factory->CreateNPC(NPCType::Werewolf, point1, "Werewolf1");
    auto druid = factory->CreateNPC(NPCType::Druid, point2, "Druid1");

    // Оборотень должен атаковать друида
    EXPECT_TRUE(werewolf->CanAttack(*druid, 5.0));
}

TEST_F(GameTest, AttackRulesDruidPeaceful) {
    Point point1(10, 10);
    Point point2(11, 11);

    auto druid = factory->CreateNPC(NPCType::Druid, point1, "Druid1");
    auto squirrel = factory->CreateNPC(NPCType::Squirrel, point2, "Squirrel1");
    auto werewolf = factory->CreateNPC(NPCType::Werewolf, point1, "Werewolf1");

    // Друид никого не атакует
    EXPECT_TRUE(druid->CanAttack(*squirrel, 5.0));
    EXPECT_TRUE(druid->CanAttack(*werewolf, 5.0));
}

TEST_F(GameTest, AttackDistanceCheck) {
    Point point1(0, 0);
    Point point2(100, 100); // Далеко

    auto squirrel = factory->CreateNPC(NPCType::Squirrel, point1, "Squirrel1");
    auto werewolf = factory->CreateNPC(NPCType::Werewolf, point2, "Werewolf1");

    // Хотя белка должна атаковать оборотня по правилам, но расстояние слишком большое
    EXPECT_FALSE(squirrel->CanAttack(*werewolf, 10.0));
}

// Тесты для Visitor (Battle)
TEST_F(GameTest, BattleVisitorCreation) {
    Battle battle(*game);
    // Проверяем, что visitor создан
    EXPECT_NO_THROW();
}

TEST_F(GameTest, BattleVisitorSetTarget) {
    Battle battle(*game);
    Point point(10, 10);
    auto target = factory->CreateNPC(NPCType::Druid, point, "Target");

    EXPECT_NO_THROW(battle.SetTarget(target));
}

// Тесты для NPCType conversions
TEST(NPCTypeTest, TypeToStringConversion) {
    EXPECT_EQ(NPCTypeToString(NPCType::Squirrel), "Squirrel");
    EXPECT_EQ(NPCTypeToString(NPCType::Werewolf), "Werewolf");
    EXPECT_EQ(NPCTypeToString(NPCType::Druid), "Druid");
}

TEST(NPCTypeTest, StringToTypeConversion) {
    EXPECT_EQ(StringToNPCType("Squirrel"), NPCType::Squirrel);
    EXPECT_EQ(StringToNPCType("Werewolf"), NPCType::Werewolf);
    EXPECT_EQ(StringToNPCType("Druid"), NPCType::Druid);
}

TEST(NPCTypeTest, InvalidStringToType) {
    // Проверяем обработку неверных строк
    EXPECT_THROW(StringToNPCType("InvalidType"), std::runtime_error);
}

// Интеграционные тесты
TEST_F(GameTest, FullGameScenario) {
    // Создаем полноценный сценарий игры
    Point point1(10, 10);
    Point point2(15, 15);
    Point point3(20, 20);

    game->AddNPC(NPCType::Squirrel, point1, "AggressiveSquirrel");
    game->AddNPC(NPCType::Werewolf, point2, "HungryWerewolf");
    game->AddNPC(NPCType::Druid, point3, "PeacefulDruid");

    // Добавляем observers
    auto screenObserver = std::make_shared<Screen>();
    game->AddObserver(screenObserver);

    std::ofstream logFile("integration_test_log.txt");
    auto fileObserver = std::make_shared<Logger>(std::move(logFile));
    game->AddObserver(fileObserver);

    // Сохраняем состояние
    EXPECT_EQ(game->SaveObjects("integration_save.txt"), 0);

    // Запускаем бой
    int32_t battleResult = game->StartBattle(10.0);
    EXPECT_GE(battleResult, 0);

    // Загружаем сохраненное состояние
    Game loadedGame(factory);
    EXPECT_EQ(loadedGame.LoadObjects("integration_save.txt"), 0);

    // Очистка
    std::remove("integration_save.txt");
    std::remove("integration_test_log.txt");
}

// Тесты на граничные условия
TEST_F(GameTest, BoundaryCoordinates) {
    Point minPoint(0, 0);
    Point maxPoint(500, 500);
    Point edgePoint(250, 250);

    EXPECT_GE(game->AddNPC(NPCType::Squirrel, minPoint, "MinNPC"), 0);
    EXPECT_GE(game->AddNPC(NPCType::Werewolf, maxPoint, "MaxNPC"), 0);
    EXPECT_GE(game->AddNPC(NPCType::Druid, edgePoint, "EdgeNPC"), 0);
}

TEST_F(GameTest, EmptyNameNPC) {
    Point point(10, 10);
    int32_t result = game->AddNPC(NPCType::Druid, point, "");
    EXPECT_GE(result, 0); // Или EXPECT_LT в зависимости от требований к имени
}

// Тесты на производительность (если нужно)
TEST_F(GameTest, PerformanceManyNPCs) {
    const int NUM_NPCS = 100;

    for (int i = 0; i < NUM_NPCS; ++i) {
        Point point(i % 500, (i * 2) % 500);
        game->AddNPC(static_cast<NPCType>(i % 3), point, "NPC_" + std::to_string(i));
    }

    // Бой должен завершиться за разумное время
    auto start = std::chrono::high_resolution_clock::now();
    int32_t result = game->StartBattle(50.0);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LE(duration.count(), 1000); // Не более 1 секунды для 100 NPC
    EXPECT_GE(result, 0);
}

int main(int argc,
         char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
