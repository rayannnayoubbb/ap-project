#include "gameboardscreen.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFont>
#include <QLabel>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <cmath>

GameBoardScreen::GameBoardScreen(const QString &player1Name, const QString &player2Name, QWidget *parent)
    : QWidget(parent), m_player1Name(player1Name), m_player2Name(player2Name), m_currentPlayer(PLAYER1) //cst ref la playername
{
    setFixedSize(800, 450);
    setMouseTracking(true);





    initializeAgents();
    initializeBoard();
}

void GameBoardScreen::initializeAgents()
{

    Agent agent1;
    agent1.name = "Healer";
    agent1.power = 70;
    agent1.ability = "Heal";
    agent1.player = PLAYER1;
    agent1.position = QPoint(-1, -1);
    agent1.card = createAgentCard(agent1);

    Agent agent2;
    agent2.name = "Sprinter";
    agent2.power = 60;
    agent2.ability = "Speed";
    agent2.player = PLAYER1;
    agent2.position = QPoint(-1, -1);
    agent2.card = createAgentCard(agent2);

    m_agents.append(agent1);
    m_agents.append(agent2);


    Agent agentA;
    agentA.name = "Shielder";
    agentA.power = 80;
    agentA.ability = "Shield";
    agentA.player = PLAYER2;
    agentA.position = QPoint(-1, -1);
    agentA.card = createAgentCard(agentA);

    Agent agentB;
    agentB.name = "Teleporter";
    agentB.power = 65;
    agentB.ability = "Teleport";
    agentB.player = PLAYER2;
    agentB.position = QPoint(-1, -1);
    agentB.card = createAgentCard(agentB);

    m_agents.append(agentA);
    m_agents.append(agentB);   //hydol lists

    updateAgentCardsPosition(); // updates their pos on screen
}

QLabel* GameBoardScreen::createAgentCard(const Agent& agent) //pointer to label
{
    QLabel* card = new QLabel(this);
    card->setAlignment(Qt::AlignCenter);
    card->setStyleSheet(QString(
                            "background-color: %1;"
                            "color: white;"
                            "border-radius: 10px;"
                            "border: 2px solid %2;"
                            "padding: 5px;"
                            "font-weight: bold;"
                            ).arg(agent.player == PLAYER1 ? "rgba(255, 150, 150, 200)" : "rgba(150, 150, 255, 200)")
                            .arg(agent.player == PLAYER1 ? "darkred" : "darkblue"));

    card->setText(QString("%1\nPower: %2\nAbility: %3")
                      .arg(agent.name)
                      .arg(agent.power)
                      .arg(agent.ability));

    card->setFixedSize(150, 80);
    card->installEventFilter(this);
    card->setAttribute(Qt::WA_TranslucentBackground);
    card->show();

    return card;
}

void GameBoardScreen::updateAgentCardsPosition()
{
    for (Agent& agent : m_agents) {
        if (agent.position == QPoint(-1, -1)) {
            // Position in side panel - adjusted for 800 width
            if (agent.player == PLAYER1) {
                agent.card->move(20, 80 + m_agents.indexOf(agent) * 90);
            } else {
                agent.card->move(800 - 170, 80 + (m_agents.indexOf(agent) - 2) * 90);
            }
        } else {
            // Position on board
            QPoint center = hexagonCenter(agent.position.x(), agent.position.y());
            agent.card->move(center.x() - agent.card->width()/2,
                             center.y() - agent.card->height()/2);
        }
    }
}
void GameBoardScreen::initializeBoard()
{
    m_board.clear();
    // 9 columns with alternating 5/4 rows
    const int numColumns = 9;
    m_board.resize(numColumns);

    for (int col = 0; col < numColumns; ++col) {
        int rows = (col % 2) ? 4 : 5; // Alternating 5 and 4 rows
        m_board[col].resize(rows);

        for (int row = 0; row < rows; ++row) {
            m_board[col][row].col = col;
            m_board[col][row].row = row;
            m_board[col][row].type = NORMAL;
        }
    }
}

QPoint GameBoardScreen::hexagonCenter(int col, int row) const
{
    const int hexSize = 30;
    const int hexWidth = hexSize * 2;
    const int hexHeight = static_cast<int>(hexSize * sqrt(3));

    // Center the grid in the 800x450 window
    int x = 400 + (col - 4) * hexWidth * 0.75;
    int y = 225;

    if (col % 2 == 0) {
        // Even columns (5 cells)
        y += (row - 2) * hexHeight;
    } else {
        // Odd columns (4 cells)
        y += (row - 1.5) * hexHeight;
    }

    return QPoint(x, y);
}

void GameBoardScreen::drawBoard(QPainter &painter)
{
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(10);
    painter.setFont(font);

    // Draw columns first
    for (int col = 0; col < m_board.size(); ++col) {
        // Then draw rows in each column
        for (int row = 0; row < m_board[col].size(); ++row) {
            const GameCell &cell = m_board[col][row];
            QPoint center = hexagonCenter(col, row);

            QBrush brush;
            switch (cell.type) {
            case PLAYER1_SPAWN: brush = QColor(255, 200, 200); break;
            case PLAYER2_SPAWN: brush = QColor(200, 200, 255); break;
            case WATER: brush = QColor(100, 200, 255); break;
            case ROCK: brush = Qt::gray; break;
            default: brush = Qt::white;
            }

            if (m_draggingAgent && isValidMove(m_draggingAgent, col, row)) {
                brush.setColor(brush.color().lighter(120));
            }

            drawHexagon(painter, center, 30, brush);

            QString text;
            switch (cell.type) {
            case PLAYER1_SPAWN: text = "1"; break;
            case PLAYER2_SPAWN: text = "2"; break;
            case WATER: text = "~"; break;
            case ROCK: text = "#"; break;
            default: text = "";
            }

            painter.drawText(QRect(center.x()-15, center.y()-15, 30, 30),
                             Qt::AlignCenter, text);
        }
    }
}

void GameBoardScreen::drawHexagon(QPainter &painter, const QPoint &center, int size, const QBrush &brush)
{
    QPolygon hexagon;
    for (int i = 0; i < 6; ++i) {
        double angle = 2 * M_PI / 6 * i;
        int x = static_cast<int>(center.x() + size * cos(angle));
        int y = static_cast<int>(center.y() + size * sin(angle));
        hexagon << QPoint(x, y);
    }

    painter.setBrush(brush);
    painter.setPen(Qt::black);
    painter.drawPolygon(hexagon);
}

void GameBoardScreen::drawSidePanels(QPainter &painter)
{
    QFont font = painter.font();
    font.setPointSize(10);
    font.setBold(true);
    painter.setFont(font);

    // Left panel: 0 to 150
    QRect leftPanel(0, 0, 150, height());
    QLinearGradient leftGradient(0, 0, 0, height());
    leftGradient.setColorAt(0, QColor(255, 230, 240));
    leftGradient.setColorAt(1, QColor(255, 200, 220));
    painter.fillRect(leftPanel, leftGradient);

    painter.setPen(Qt::darkMagenta);
    painter.drawText(QRect(0, 20, 150, 30), Qt::AlignCenter, m_player1Name);
    painter.drawText(QRect(0, 50, 150, 30), Qt::AlignCenter,
                     m_currentPlayer == PLAYER1 ? "Your turn" : "");

    // Right panel: ends exactly at 800px
    QRect rightPanel(650, 0, 150, height());
    QLinearGradient rightGradient(650, 0, 650, height());
    rightGradient.setColorAt(0, QColor(230, 240, 255));
    rightGradient.setColorAt(1, QColor(200, 220, 255));
    painter.fillRect(rightPanel, rightGradient);

    painter.setPen(Qt::darkBlue);
    painter.drawText(QRect(650, 20, 150, 30), Qt::AlignCenter, m_player2Name);
    painter.drawText(QRect(650, 50, 150, 30), Qt::AlignCenter,
                     m_currentPlayer == PLAYER2 ? "Your turn" : "");
}

void GameBoardScreen::handleAgentDrop(Agent &agent, const QPoint &dropPos)
{
    const int hexSize = 20;
    const int hexWidth = hexSize * 2;
    const int hexHeight = static_cast<int>(hexSize * sqrt(3));

    for (int row = 0; row < m_board.size(); ++row) {
        for (int col = 0; col < m_board[row].size(); ++col) {
            QPoint center = hexagonCenter(row, col);
            QRect hexRect(center.x() - hexSize, center.y() - hexHeight / 2, hexWidth, hexHeight);

            if (hexRect.contains(dropPos)) {
                if (isValidMove(&agent, row, col)) {
                    agent.position = QPoint(row, col);
                    m_currentPlayer = (m_currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;
                    update();
                    return;
                }
                break;
            }
        }
    }


    agent.card->move(m_dragStartPosition);
}

bool GameBoardScreen::isValidMove(const Agent *agent, int toRow, int toCol) const
{
    if (toRow < 0 || toRow >= m_board.size() || toCol < 0 || toCol >= m_board[toRow].size()) {
        return false;
    }

    const GameCell &targetCell = m_board[toRow][toCol];

    if (targetCell.type == ROCK || targetCell.type == WATER) {
        return false;
    }

    if (agent->player == PLAYER1 && targetCell.type == PLAYER2_SPAWN) {
        return false;
    }
    if (agent->player == PLAYER2 && targetCell.type == PLAYER1_SPAWN) {
        return false;
    }

    for (const Agent &otherAgent : m_agents) {
        if (otherAgent.position == QPoint(toRow, toCol) && !(&otherAgent == agent)) {
            return false;
        }
    }

    if (agent->position == QPoint(-1, -1)) {
        return (agent->player == PLAYER1 && targetCell.type == PLAYER1_SPAWN) ||
               (agent->player == PLAYER2 && targetCell.type == PLAYER2_SPAWN);
    }

    int rowDiff = abs(toRow - agent->position.x());
    int colDiff = abs(toCol - agent->position.y());

    return (rowDiff <= 1 && colDiff <= 1);
}
bool GameBoardScreen::loadBoardFromFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open arena file:" << filename;
        return false;
    }

    // Initialize empty board (9 columns, alternating 5/4 rows)
    m_board.clear();
    m_board.resize(9);
    for (int col = 0; col < 9; col++) {
        m_board[col].resize((col % 2) ? 4 : 5);
        for (int row = 0; row < m_board[col].size(); row++) {
            m_board[col][row] = {col, row, NORMAL};
        }
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.trimmed().isEmpty()) {
            lines.append(line);
        }
    }
    file.close();

    // Process all lines with consistent hex grid mapping
    for (int lineNum = 0; lineNum < lines.size(); lineNum++) {
        QString line = lines[lineNum];

        for (int charPos = 0; charPos < line.length(); charPos++) {
            QChar c = line[charPos];

            // Skip decorative characters
            if (c == ' ' || c == '_' || c == '/' || c == '\\') continue;

            // Calculate grid coordinates - NEW HEX GRID MAPPING
            int col, row;
            if (lineNum % 2 == 1) { // Odd lines (1,3,5,7,9)
                col = (charPos - 1) / 3; // 3 chars per hex cell
                row = (lineNum - 1) / 2;
            } else { // Even lines (2,4,6,8,10)
                col = (charPos - 1) / 3;
                row = (lineNum / 2) - 1;
            }

            // Boundary check
            if (col < 0 || col >= 9 || row < 0 || row >= m_board[col].size()) {
                continue;
            }

            // Set cell type - ALL characters processed the same way
            switch (c.unicode()) {
            case '1':
                m_board[col][row].type = PLAYER1_SPAWN;
                break;
            case '2':
                m_board[col][row].type = PLAYER2_SPAWN;
                break;
            case '~':
                m_board[col][row].type = WATER;
                break;
            case '#':
                m_board[col][row].type = ROCK;
                break;
            default:
                break;
            }
        }
    }

    update();
    return true;
}
GameBoardScreen::CellType GameBoardScreen::charToCellType(QChar c)
{
    switch (c.unicode()) {
    case '1': return PLAYER1_SPAWN;
    case '2': return PLAYER2_SPAWN;
    case '~': return WATER;
    case '#': return ROCK;
    default: return NORMAL;
    }
}

GameBoardScreen::GameCell* GameBoardScreen::getCellAtPosition(const QPoint &pos)
{
    for (auto &row : m_board) {
        for (GameCell &cell : row) {
            QPoint center = hexagonCenter(cell.row, cell.col);
            QRect hexRect(center.x() - 20, center.y() - 20, 40, 40);
            if (hexRect.contains(pos)) {
                return &cell;
            }
        }
    }
    return nullptr;
}

void GameBoardScreen::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
        m_draggingAgent = getAgentAtPosition(event->pos());

        if (m_draggingAgent && m_draggingAgent->player == m_currentPlayer) {
            m_dragOffset = event->pos() - m_draggingAgent->card->pos();
            m_draggingAgent->card->raise();
        } else {
            m_draggingAgent = nullptr;
        }
    }
    QWidget::mousePressEvent(event);
}

void GameBoardScreen::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && m_draggingAgent) {
        m_draggingAgent->card->move(event->pos() - m_dragOffset);
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void GameBoardScreen::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_draggingAgent) {
        handleAgentDrop(*m_draggingAgent, event->pos());
        m_draggingAgent = nullptr;
    }
    QWidget::mouseReleaseEvent(event);
}

void GameBoardScreen::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    drawBoard(painter);
    drawSidePanels(painter);

    if (m_draggingAgent) {
        painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
        painter.drawEllipse(m_dragPosition, 20, 20);
    }
}

bool GameBoardScreen::eventFilter(QObject *obj, QEvent *event)  //allows widget to respond to clicks or hover
{
    return QWidget::eventFilter(obj, event);
}

GameBoardScreen::Agent* GameBoardScreen::getAgentAtPosition(const QPoint &pos)
{
    for (Agent &agent : m_agents) {
        if (agent.card->geometry().contains(pos)) {
            return &agent;
        }
    }
    return nullptr;
}

