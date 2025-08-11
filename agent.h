#ifndef AGENT_H
#define AGENT_H

#include <QString>
#include <QPoint>

class Agent {
public:
    enum Player {
        PLAYER1,
        PLAYER2
    };

    Agent(const QString& name, const QString& imagePath, int hp, int mobility,
          int damage, int attackRange, Player player);
    virtual ~Agent() = default;

    // Movement capabilities
    virtual bool canMoveOnWater() const = 0;
    virtual bool canMoveOnRock() const = 0;
    virtual bool canStandOnWater() const = 0;
    virtual bool canStandOnRock() const = 0;

    // Player management
    Player getPlayer() const { return m_player; }
    void setPlayer(Player player) { m_player = player; }

    // Getters
    QString getName() const { return m_name; }
    QString getImagePath() const { return m_imagePath; }
    int getHp() const { return m_hp; }
    int getMaxHp() const { return m_maxHp; }
    int getMobility() const { return m_mobility; }
    int getDamage() const { return m_damage; }
    int getAttackRange() const { return m_attackRange; }
    QPoint getPosition() const { return m_position; }

    // Setters
    void setPosition(const QPoint& pos) { m_position = pos; }
    void takeDamage(int amount) { m_hp = qMax(0, m_hp - amount); }

    // Game mechanics
    int counterAttackDamage() const { return m_damage / 2; }
    bool isAlive() const { return m_hp > 0; }

    // Factory method
    static Agent* createAgent(const QString& name, Player player);

protected:
    QString m_name;
    QString m_imagePath;
    int m_hp;
    int m_maxHp;
    int m_mobility;
    int m_damage;
    int m_attackRange;
    Player m_player;
    QPoint m_position;
};

// Derived agent classes
class WaterWalkingAgent : public Agent {
public:
    WaterWalkingAgent(const QString& name, const QString& imagePath, int hp,
                      int mobility, int damage, int attackRange, Player player);

    bool canMoveOnWater() const override { return true; }
    bool canMoveOnRock() const override { return false; }
    bool canStandOnWater() const override { return true; }
    bool canStandOnRock() const override { return false; }
};

class GroundedAgent : public Agent {
public:
    GroundedAgent(const QString& name, const QString& imagePath, int hp,
                  int mobility, int damage, int attackRange, Player player);

    bool canMoveOnWater() const override { return false; }
    bool canMoveOnRock() const override { return false; }
    bool canStandOnWater() const override { return false; }
    bool canStandOnRock() const override { return false; }
};

class FlyingAgent : public Agent {
public:
    FlyingAgent(const QString& name, const QString& imagePath, int hp,
                int mobility, int damage, int attackRange, Player player);

    bool canMoveOnWater() const override { return true; }
    bool canMoveOnRock() const override { return true; }
    bool canStandOnWater() const override { return false; }
    bool canStandOnRock() const override { return false; }
};

class FloatingAgent : public Agent {
public:
    FloatingAgent(const QString& name, const QString& imagePath, int hp,
                  int mobility, int damage, int attackRange, Player player);

    bool canMoveOnWater() const override { return true; }
    bool canMoveOnRock() const override { return true; }
    bool canStandOnWater() const override { return true; }
    bool canStandOnRock() const override { return true; }
};

#endif // AGENT_H
