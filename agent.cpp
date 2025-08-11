#include "agent.h"
#include <QDebug>

Agent::Agent(const QString& name, const QString& imagePath, int hp, int mobility,
             int damage, int attackRange, Player player) :
    m_name(name),
    m_imagePath(imagePath),
    m_hp(hp),
    m_maxHp(hp),
    m_mobility(mobility),
    m_damage(damage),
    m_attackRange(attackRange),
    m_player(player),
    m_position(-1, -1) {}

Agent* Agent::createAgent(const QString& name, Player player) {
    // 🌊 WATER WALKING AGENTS
    if (name == "Billy") return new WaterWalkingAgent("Billy", ":/new/prefix1/agents/Billy.png", 320, 3, 90, 1, player);
    if (name == "Reketon") return new WaterWalkingAgent("Reketon", ":/new/prefix1/agents/Reketon.png", 320, 2, 80, 2, player);
    if (name == "Angus") return new WaterWalkingAgent("Angus", ":/new/prefix1/agents/Angus.png", 400, 2, 100, 1, player);
    if (name == "Duraham") return new WaterWalkingAgent("Duraham", ":/new/prefix1/agents/Duraham.png", 320, 2, 100, 2, player);
    if (name == "Colonel Baba") return new WaterWalkingAgent("Colonel Baba", ":/new/prefix1/agents/Colonel_Baba.png", 400, 2, 100, 1, player);
    if (name == "Medusa") return new WaterWalkingAgent("Medusa", ":/new/prefix1/agents/Medusa.png", 320, 2, 90, 2, player);
    if (name == "Bunka") return new WaterWalkingAgent("Bunka", ":/new/prefix1/agents/Bunka.png", 320, 3, 100, 1, player);
    if (name == "Sanka") return new WaterWalkingAgent("Sanka", ":/new/prefix1/agents/Sanka.png", 320, 3, 100, 1, player);

    // 🏔️ GROUNDED AGENTS
    if (name == "Sir Lamorak") return new GroundedAgent("Sir Lamorak", ":/new/prefix1/agents/Sir_Lamorak.png", 320, 3, 110, 1, player);
    if (name == "Kabu") return new GroundedAgent("Kabu", ":/new/prefix1/agents/Kabu.png", 400, 2, 120, 1, player);
    if (name == "Rajakal") return new GroundedAgent("Rajakal", ":/new/prefix1/agents/Rajakal.png", 320, 2, 130, 1, player);
    if (name == "Salih") return new GroundedAgent("Salih", ":/new/prefix1/agents/Salih.png", 400, 2, 80, 1, player);
    if (name == "Khan") return new GroundedAgent("Khan", ":/new/prefix1/agents/Khan.png", 320, 2, 90, 1, player);
    if (name == "Boi") return new GroundedAgent("Boi", ":/new/prefix1/agents/Boi.png", 400, 2, 100, 1, player);
    if (name == "Eloi") return new GroundedAgent("Eloi", ":/new/prefix1/agents/Eloi.png", 240, 2, 100, 2, player);
    if (name == "Kanar") return new GroundedAgent("Kanar", ":/new/prefix1/agents/Kanar.png", 160, 2, 100, 2, player);
    if (name == "Elsa") return new GroundedAgent("Elsa", ":/new/prefix1/agents/Elsa.png", 320, 2, 140, 2, player);
    if (name == "Karissa") return new GroundedAgent("Karissa", ":/new/prefix1/agents/Karissa.png", 280, 2, 80, 2, player);
    if (name == "Sir Philip") return new GroundedAgent("Sir Philip", ":/new/prefix1/agents/Sir_Philip.png", 400, 2, 100, 1, player);
    if (name == "Frost") return new GroundedAgent("Frost", ":/new/prefix1/agents/Frost.png", 260, 2, 80, 2, player);
    if (name == "Tusk") return new GroundedAgent("Tusk", ":/new/prefix1/agents/Tusk.png", 400, 2, 100, 1, player);

    // 🦅 FLYING AGENTS
    if (name == "Rambu") return new FlyingAgent("Rambu", ":/new/prefix1/agents/Rambu.png", 320, 3, 120, 1, player);

    // 👻 FLOATING AGENTS
    if (name == "Sabrina") return new FloatingAgent("Sabrina", ":/new/prefix1/agents/Sabrina.png", 320, 3, 100, 1, player);
    if (name == "Death") return new FloatingAgent("Death", ":/new/prefix1/agents/Death.png", 240, 3, 120, 2, player);

    qWarning() << "Unknown agent name:" << name;
    return nullptr;
}

// Derived class constructors
WaterWalkingAgent::WaterWalkingAgent(const QString& name, const QString& imagePath, int hp,
                                     int mobility, int damage, int attackRange, Player player) :
    Agent(name, imagePath, hp, mobility, damage, attackRange, player) {}

GroundedAgent::GroundedAgent(const QString& name, const QString& imagePath, int hp,
                             int mobility, int damage, int attackRange, Player player) :
    Agent(name, imagePath, hp, mobility, damage, attackRange, player) {}

FlyingAgent::FlyingAgent(const QString& name, const QString& imagePath, int hp,
                         int mobility, int damage, int attackRange, Player player) :
    Agent(name, imagePath, hp, mobility, damage, attackRange, player) {}

FloatingAgent::FloatingAgent(const QString& name, const QString& imagePath, int hp,
                             int mobility, int damage, int attackRange, Player player) :
    Agent(name, imagePath, hp, mobility, damage, attackRange, player) {}
