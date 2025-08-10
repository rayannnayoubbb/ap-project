#include "agent.h"

Agent::Agent(Type type, const QString& name, const QString& imagePath,
             int hp, int mobility, int damage, int attackRange)
    : m_type(type), m_name(name), m_imagePath(imagePath), m_hp(hp),
    m_mobility(mobility), m_damage(damage), m_attackRange(attackRange),
    m_player(PLAYER1), m_position(-1, -1) {}

bool Agent::canMoveToWater() const {
    return m_type == WATER_WALKING || m_type == FLOATING;
}

bool Agent::canMoveToLand() const {
    return true; // All agents can move on land
}

bool Agent::canMoveToRock() const {
    return m_type == FLYING || m_type == FLOATING;
}

bool Agent::canStandOnWater() const {
    return m_type == WATER_WALKING || m_type == FLOATING;
}

bool Agent::canStandOnLand() const {
    return true; // All agents can stand on land
}

bool Agent::canStandOnRock() const {
    return m_type == FLOATING;
}
