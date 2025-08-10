#ifndef AGENT_H
#define AGENT_H

#include <QString>
#include <QPoint>

class Agent {
public:
    enum Type { WATER_WALKING, GROUNDED, FLYING, FLOATING };
    enum Player { PLAYER1, PLAYER2 };

    Agent(Type type, const QString& name, const QString& imagePath,
          int hp, int mobility, int damage, int attackRange);

    // Getters
    Type getType() const { return m_type; }
    QString getName() const { return m_name; }
    QString getImagePath() const { return m_imagePath; }
    int getHp() const { return m_hp; }
    int getMobility() const { return m_mobility; }
    int getDamage() const { return m_damage; }
    int getAttackRange() const { return m_attackRange; }
    Player getPlayer() const { return m_player; }
    QPoint getPosition() const { return m_position; }

    // Setters
    void setPlayer(Player player) { m_player = player; }
    void setPosition(QPoint pos) { m_position = pos; }

    // Movement capabilities
    bool canMoveToWater() const;
    bool canMoveToLand() const;
    bool canMoveToRock() const;
    bool canStandOnWater() const;
    bool canStandOnLand() const;
    bool canStandOnRock() const;

private:
    Type m_type;
    QString m_name;
    QString m_imagePath;
    int m_hp;
    int m_mobility;
    int m_damage;
    int m_attackRange;
    Player m_player;
    QPoint m_position;
};

#endif // AGENT_H
