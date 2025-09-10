#ifndef AGENTSELECTSCREEN_H
#define AGENTSELECTSCREEN_H

#include <QWidget>
#include <QList>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QButtonGroup>
#include "agent.h"

class AgentSelectScreen : public QWidget {
    Q_OBJECT
public:
    explicit AgentSelectScreen(const QString& player1Name, const QString& player2Name, QWidget* parent = nullptr);
    ~AgentSelectScreen();

    QList<Agent*> getPlayer1Agents() const { return m_player1Agents; }
    QList<Agent*> getPlayer2Agents() const { return m_player2Agents; }

signals:
    void agentsSelected();
    void backToPlayerNamesRequested();

private slots:
    void handleAgentSelection(int id);
    void confirmSelection();
    void switchPlayer();

private:
    void setupUI();
    void createAgentCards();
    void updateSelectionDisplay();
    void clearCurrentPlayerSelection();
    QString getAgentTypeString(Agent::Type type) const;
    void updateButtonStyles();

    QString m_player1Name;
    QString m_player2Name;
    QList<Agent*> m_allAgents;
    QList<Agent*> m_player1Agents;
    QList<Agent*> m_player2Agents;
    QList<Agent*> m_currentPlayerSelection;
    Agent::Player m_currentSelectingPlayer;

    // UI Elements
    QGridLayout* m_gridLayout;
    QButtonGroup* m_agentButtonGroup;
    QLabel* m_playerLabel;
    QLabel* m_selectionCountLabel;
    QPushButton* m_confirmButton;
    QPushButton* m_switchPlayerButton;
    QPushButton* m_backButton;
    QScrollArea* m_scrollArea;
    QWidget* m_container;
    QLabel* m_backgroundLabel;
};

#endif // AGENTSELECTSCREEN_H
