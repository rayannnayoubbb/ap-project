#ifndef GAMEBOARDSCREEN_H
#define GAMEBOARDSCREEN_H

#include <QWidget>
#include <QVector>
#include <QPoint>

class QLabel;
class QPushButton;

class GameBoardScreen : public QWidget
{
    Q_OBJECT

public:
    enum Player { PLAYER1, PLAYER2 };
    enum CellType { NORMAL, PLAYER1_SPAWN, PLAYER2_SPAWN, WATER, ROCK };

    struct GameCell {
        int row;
        int col;
        CellType type;

        bool operator==(const GameCell& other) const {
            return row == other.row && col == other.col && type == other.type;
        }
    };

    struct Agent {
        QString name;
        int power;
        QString ability;
        Player player;
        QPoint position;
        QLabel* card;

        bool operator==(const Agent& other) const {
            return name == other.name &&
                   power == other.power &&
                   ability == other.ability &&
                   player == other.player &&
                   position == other.position;
        }
    };

    explicit GameBoardScreen(const QString &player1Name, const QString &player2Name, QWidget *parent = nullptr);
    bool loadBoardFromFile(const QString &filename);
CellType charToCellType(QChar c);
signals:
    void backToMenuRequested();

protected:


    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:

    void initializeAgents();
    void initializeBoard();
    void drawHexagon(QPainter &painter, const QPoint &center, int size, const QBrush &brush);
    void drawBoard(QPainter &painter);
    void drawSidePanels(QPainter &painter);
    QPoint hexagonCenter(int row, int col) const;
    void updateAgentCardsPosition();
    QLabel* createAgentCard(const Agent& agent);
    void handleAgentDrop(Agent &agent, const QPoint &dropPos);
    bool isValidMove(const Agent *agent, int toRow, int toCol) const;
    GameCell* getCellAtPosition(const QPoint &pos);
    Agent* getAgentAtPosition(const QPoint &pos);

    QString m_player1Name;
    QString m_player2Name;
    Player m_currentPlayer;
    QVector<QVector<GameCell>> m_board;
    QVector<Agent> m_agents;
    QPushButton *m_backButton;


    Agent *m_draggingAgent = nullptr;
    QPoint m_dragStartPosition;
    QPoint m_dragOffset;
    QPoint m_dragPosition;


};

#endif // GAMEBOARDSCREEN_H
