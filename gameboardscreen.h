#ifndef GAMEBOARDSCREEN_H
#define GAMEBOARDSCREEN_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QSet>
#include <QPoint>
#include <QMap>
#include "agent.h"


class GameBoardScreen : public QWidget
{
    Q_OBJECT

public:
    explicit GameBoardScreen(const QString &player1Name, const QString &player2Name,
                             const QList<Agent*>& player1Agents, const QList<Agent*>& player2Agents,
                             QWidget *parent = nullptr);
    ~GameBoardScreen();

  
    bool loadBoardFromFile(const QString &filename);

    
    bool m_gameStarted = false;
    bool m_agentSelected = false;
    QString m_selectedAgentName;
    QPoint m_selectedStartPos;
    QVector<QPoint> m_highlightedCells;
    QPoint m_attackFlashCell;
    bool m_flashActive = false;

   

    struct GameCell {
        int col;
        int row;
        QString type;        // Cell type as string: "", "1", "2", "#", "~"
        QString agentName;   // Agent name if occupied, empty string otherwise
        QRect rect;          // Visual rectangle for hit testing
        bool hovered = false; // For mouse hover effects

    
    };
    GameCell* getCellAtPosition(const QPoint &pos);
GameCell* getCellAtHexCoord(int col, int row);

signals:
    void backToMenuRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void startGame();
    void endTurn();
    void switchTurn();
    void updateAnimation();

private:
    // UI components
    QWidget *m_leftPanel;
    QWidget *m_rightPanel;
    QWidget *m_gridWidget;
    QLabel *m_statusLabel;
    QPushButton *m_backButton;
    QPushButton *m_startGameButton;
    QPushButton *m_endTurnButton;
    void initializeBoard();
    // Game data
    QString m_player1Name;
    QString m_player2Name;
    bool m_isPlayer1Turn = true;
    int m_currentTurn = 1;



    QList<Agent*> m_player1AvailableAgents;
    QList<Agent*> m_player2AvailableAgents;
    QSet<QString> m_placedPlayer1Agents;
    QSet<QString> m_placedPlayer2Agents;
    QMap<QString, Agent*> m_agentMap;
    QVector<QVector<GameCell>> m_board;

  
    QTimer *m_animationTimer;
    bool m_isAnimating = false;
    float m_animationProgress = 0.0f;
    QPointF m_animationStart;
    QPointF m_animationEnd;
    QString m_animatingAgentName;
    QPoint m_animationTarget;



    // Helper functions
    void setupUI();
    QWidget* createAgentCard(Agent* agent, bool isPlayer1);
    QPoint hexagonCenter(int col, int row) const;
    // In gameboardscreen.h
    void drawHexagon(QPainter &painter, const QPoint &center, int size, const QString &cellType);
    void drawBoard(QPainter &painter);
    void drawSidePanels(QPainter &painter);

    // Simplified functions (like your friend)
    QPoint convertToGridCoordinates(const QPoint &screenPos);
    void handleGridClick(const QPoint &gridPos);
    void handleAgentSelection(Agent* agent);
    void highlightMovesForAgent(const QPoint &startPos, Agent* agent);
    void clearHighlights();
    void moveAgentOnBoard(const QPoint &start, const QPoint &end);
    void performAttack(const QPoint &attackerPos, const QPoint &defenderPos);
    int hexDistance(const QPoint &a, const QPoint &b) const;
    QVector<QPoint> getHexNeighbors(int col, int row) const;
bool canStandOnCellForAgentType(const QString &agentType, const QString &cellType) const;
    void removeAgentByName(const QString &name);
    void checkForWinner();
    void updateStatusDisplay();


    void startAgentAnimation(const QString &agentName, const QPoint &targetCell);
    void completeMoveAnimation();
    void completeAttackAnimation();
    void handleDrop(const QPoint& position, Agent* agent);
     bool m_isAttackAnimation = false; // ADD THIS LINE



  void startAttackAnimation(const QString &attackerName, const QString &targetName);
  void startDrag(Agent* agent);
  int getMaxRows() const ;
  void animateAgentMove(QWidget* agentCard, const QPoint &targetPos);
  void animateAgentDisappear(QWidget* agentCard);



  bool m_isDeathAnimation;
  QString m_dyingAgentName;
  float m_deathAnimationProgress;
  QPoint m_deathPosition;
};

#endif // GAMEBOARDSCREEN_H
