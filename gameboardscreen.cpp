#include "gameboardscreen.h"
#include "agent.h" // Include your Agent class header
#include <QDebug>
#include <queue>
#include <QSet>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QTimer>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDrag>
#include <QMimeData>
#include <cmath>
#include <iostream>
#include <queue>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>


static const int HEX_SIZE = 25;
static const int HEX_WIDTH = HEX_SIZE * 2;
static const double HEX_HEIGHT = HEX_SIZE * sqrt(3);

GameBoardScreen::GameBoardScreen(const QString &player1Name, const QString &player2Name,
                                 const QList<Agent*>& player1Agents, const QList<Agent*>& player2Agents,
                                 QWidget *parent)
    : QWidget(parent),
    m_player1Name(player1Name),
    m_player2Name(player2Name),
    m_isPlayer1Turn(true),
    m_currentTurn(1),
    m_gameStarted(false),
    m_agentSelected(false),
    m_isAnimating(false),
    m_isAttackAnimation(false),
    m_animationProgress(0.0f),
    m_flashActive(false)
{



  
    const int numColumns = 9;
    m_board.resize(numColumns);
    for (int col = 0; col < numColumns; ++col) {
        int rows = (col % 2 == 0) ? 5 : 4;
        m_board[col].resize(rows);
        for (int row = 0; row < rows; ++row) {
            m_board[col][row].col = col;
            m_board[col][row].row = row;
            m_board[col][row].type = "";
            m_board[col][row].agentName = "";
            m_board[col][row].hovered = false;
        }
    }

    for (Agent* agent : player1Agents) {
        Agent* newAgent = Agent::createAgent(agent->getName(), Agent::PLAYER1);
        if (newAgent) {
            newAgent->loadPixmap();
            m_player1AvailableAgents.append(newAgent); // Use the member variable
            m_agentMap[newAgent->getName()] = newAgent;
        }
    }

    for (Agent* agent : player2Agents) {
        Agent* newAgent = Agent::createAgent(agent->getName(), Agent::PLAYER2);
        if (newAgent) {
            newAgent->loadPixmap();
            m_player2AvailableAgents.append(newAgent); // Use the member variable
            m_agentMap[newAgent->getName()] = newAgent;
        }
    }

  
    setupUI();

    
    loadBoardFromFile(":/grids/grid1.txt");

    
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &GameBoardScreen::updateAnimation);
    m_animationTimer->start(16);

    updateStatusDisplay();
}
GameBoardScreen::~GameBoardScreen() {
    
    for (Agent* agent : m_agentMap.values()) {
        delete agent;
    }
    m_agentMap.clear();

    
    for (Agent* agent : m_player1AvailableAgents) {
        delete agent;
    }
    m_player1AvailableAgents.clear();

    for (Agent* agent : m_player2AvailableAgents) {
        delete agent;
    }
    m_player2AvailableAgents.clear();
}


void GameBoardScreen::initializeBoard() {
    m_placedPlayer1Agents.clear();
    m_placedPlayer2Agents.clear();
    m_board.clear();

    const int numColumns = 9;
    m_board.resize(numColumns);
    for (int col = 0; col < numColumns; ++col) {
        int rows = (col % 2 == 0) ? 5 : 4; // Even columns have 5 rows, odd have 4
        m_board[col].resize(rows);

        for (int row = 0; row < rows; ++row) {
            m_board[col][row].col = col;
            m_board[col][row].row = row;
            m_board[col][row].type = "";
            m_board[col][row].agentName = "";
            m_board[col][row].hovered = false;
        }
    }
}
void GameBoardScreen::setupUI() {
    setFixedSize(800, 450);
    setMouseTracking(true);
    setAcceptDrops(true);

    // Main layout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    
    m_leftPanel = new QWidget(this);
    m_leftPanel->setFixedWidth(200);
    m_leftPanel->setStyleSheet("background-color: rgba(0, 0, 0, 0); border-radius: 10px;");

    QVBoxLayout *leftLayout = new QVBoxLayout(m_leftPanel);
    leftLayout->setAlignment(Qt::AlignTop);
    leftLayout->setContentsMargins(10, 10, 10, 10);
    leftLayout->setSpacing(10);

    QLabel *leftTitle = new QLabel(m_player1Name + "'s Agents", m_leftPanel);
    leftTitle->setStyleSheet("color: #f786e0; font-size: 16px; font-weight: bold;");
    leftLayout->addWidget(leftTitle);

    // Create scroll area for player 1 agents
    QScrollArea *leftScroll = new QScrollArea(m_leftPanel);
    leftScroll->setWidgetResizable(true);
    leftScroll->setStyleSheet("background: transparent; border: none;");
    leftScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *leftScrollWidget = new QWidget();
    QVBoxLayout *leftScrollLayout = new QVBoxLayout(leftScrollWidget);
    leftScrollLayout->setAlignment(Qt::AlignTop);
    leftScrollLayout->setSpacing(10);

    // Add agent cards for player 1
    for (Agent* agent : m_player1AvailableAgents) {
        QWidget *card = createAgentCard(agent, true);
        leftScrollLayout->addWidget(card);
    }

    leftScroll->setWidget(leftScrollWidget);
    leftLayout->addWidget(leftScroll);

    // Center area (game board)
    QWidget *centerWidget = new QWidget(this);
    centerWidget->setStyleSheet("background: transparent;");

    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(10);

    // Player info labels
    QWidget *infoWidget = new QWidget(centerWidget);
    QHBoxLayout *infoLayout = new QHBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(20);

    QLabel *p1Label = new QLabel("Player 1: " + m_player1Name, infoWidget);
    QLabel *p2Label = new QLabel("Player 2: " + m_player2Name, infoWidget);
    QLabel *currentPlayerLabel = new QLabel("Current: " + m_player1Name, infoWidget);
    QLabel *turnLabel = new QLabel("Turn: 1", infoWidget);

    p1Label->setStyleSheet("font-size: 14px; color: #f7d2f0; font-weight: bold;");
    p2Label->setStyleSheet("font-size: 14px; color: #c6bae3; font-weight: bold;");
    currentPlayerLabel->setStyleSheet("font-size: 14px; color: white; font-weight: bold;");
    turnLabel->setStyleSheet("font-size: 14px; color: white; font-weight: bold;");

    infoLayout->addWidget(p1Label);
    infoLayout->addWidget(p2Label);
    infoLayout->addWidget(currentPlayerLabel);
    infoLayout->addWidget(turnLabel);
    infoLayout->addStretch();

    centerLayout->addWidget(infoWidget);

    // Status display
    m_statusLabel = new QLabel("Place your agents on the board", centerWidget);
    m_statusLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold; background: rgba(0,0,0,150); padding: 8px; border-radius: 5px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setMinimumHeight(35);
    centerLayout->addWidget(m_statusLabel);

    // Grid widget
    m_gridWidget = new QWidget(centerWidget);
    m_gridWidget->setFixedSize(600, 450);
    m_gridWidget->setStyleSheet("background: rgba(0,0,0,100); border-radius: 10px;");
    m_gridWidget->installEventFilter(this); // Install event filter for grid interactions
    centerLayout->addWidget(m_gridWidget, 1, Qt::AlignCenter);

 
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    m_backButton = new QPushButton("Back to Menu", centerWidget);
    m_backButton->setFixedSize(120, 40);
    m_backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF6B6B;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 8px;"
        "   border-radius: 5px;"
        "   border: none;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover { background-color: #FF4757; }"
        );
    connect(m_backButton, &QPushButton::clicked, this, &GameBoardScreen::backToMenuRequested);

    m_startGameButton = new QPushButton("Start Game", centerWidget);
    m_startGameButton->setFixedSize(120, 40);
    m_startGameButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #777;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 8px;"
        "   border-radius: 5px;"
        "   border: none;"
        "   font-size: 12px;"
        "}"
        "QPushButton:enabled {"
        "   background-color: #4CAF50;"
        "}"
        "QPushButton:hover:enabled { background-color: #45a049; }"
        "QPushButton:disabled { background-color: #555; color: #999; }"
        );
    m_startGameButton->setEnabled(false);
    connect(m_startGameButton, &QPushButton::clicked, this, &GameBoardScreen::startGame);

    m_endTurnButton = new QPushButton("End Turn", centerWidget);
    m_endTurnButton->setFixedSize(120, 40);
    m_endTurnButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #777;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 8px;"
        "   border-radius: 5px;"
        "   border: none;"
        "   font-size: 12px;"
        "}"
        "QPushButton:enabled {"
        "   background-color: #FFE66D;"
        "   color: #333;"
        "}"
        "QPushButton:hover:enabled { background-color: #FFD700; }"
        "QPushButton:disabled { background-color: #555; color: #999; }"
        );
    m_endTurnButton->setEnabled(false);
    connect(m_endTurnButton, &QPushButton::clicked, this, &GameBoardScreen::endTurn);

    buttonLayout->addWidget(m_backButton);
    buttonLayout->addWidget(m_startGameButton);
    buttonLayout->addWidget(m_endTurnButton);
    buttonLayout->addStretch();

    centerLayout->addLayout(buttonLayout);

    // Right panel (Player 2 available agents)
    m_rightPanel = new QWidget(this);
    m_rightPanel->setFixedWidth(200);
    m_rightPanel->setStyleSheet("background-color: rgba(0, 0, 0, 0); border-radius: 10px;");

    QVBoxLayout *rightLayout = new QVBoxLayout(m_rightPanel);
    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->setContentsMargins(10, 10, 10, 10);
    rightLayout->setSpacing(10);

    QLabel *rightTitle = new QLabel(m_player2Name + "'s Agents", m_rightPanel);
    rightTitle->setStyleSheet("color: #c086f7; font-size: 16px; font-weight: bold;");
    rightLayout->addWidget(rightTitle);

    // Create scroll area for player 2 agents
    QScrollArea *rightScroll = new QScrollArea(m_rightPanel);
    rightScroll->setWidgetResizable(true);
    rightScroll->setStyleSheet("background: transparent; border: none;");
    rightScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *rightScrollWidget = new QWidget();
    QVBoxLayout *rightScrollLayout = new QVBoxLayout(rightScrollWidget);
    rightScrollLayout->setAlignment(Qt::AlignTop);
    rightScrollLayout->setSpacing(10);

    // Add agent cards for player 2
    for (Agent* agent : m_player2AvailableAgents) {
        QWidget *card = createAgentCard(agent, false);
        rightScrollLayout->addWidget(card);
    }

    rightScroll->setWidget(rightScrollWidget);
    rightLayout->addWidget(rightScroll);

    // Add panels to main layout
    mainLayout->addWidget(m_leftPanel);
    mainLayout->addWidget(centerWidget, 1);
    mainLayout->addWidget(m_rightPanel);

    setLayout(mainLayout);
}

QWidget* GameBoardScreen::createAgentCard(Agent* agent, bool belongsToPlayer1) {
    QWidget *card = new QWidget;
    card->setFixedSize(220, 110); 


    card->setStyleSheet(
        QString("QWidget {"
                " background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                " stop:0 rgba(200,120,200,230), stop:1 rgba(120,60,150,230));"
                " border-radius: 10px;"
                " border-left: 6px solid %1;"
                "} QLabel { background: transparent; border: none; }")
            .arg(belongsToPlayer1 ? "#FF77DD" : "#BB66FF"));

    card->setProperty("agent", QVariant::fromValue(agent));
    card->setProperty("isPlayer1", belongsToPlayer1);
    card->installEventFilter(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(card);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(10);

  
    QLabel *img = new QLabel(card);
    QPixmap pix = agent->getPixmap();
    if (!pix.isNull()) {
        img->setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    img->setAlignment(Qt::AlignCenter);
    img->setFixedSize(80, 80);
    mainLayout->addWidget(img, 0, Qt::AlignVCenter);

   
    QWidget *infoBox = new QWidget(card);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoBox);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(4);

    QString baseFont = "font-family:'Segoe UI'; color: white;";

 
    QLabel *name = new QLabel(agent->getName(), infoBox);
    name->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    name->setStyleSheet(baseFont + " font-weight: bold; font-size: 12px;");
    infoLayout->addWidget(name);

    QLabel *type = new QLabel(infoBox);
    QString t;
    switch (agent->getType()) {
    case Agent::WATER_WALKING: t = "🌊 Water"; break;
    case Agent::GROUNDED:      t = "🏔️ Ground"; break;
    case Agent::FLYING:        t = "🦅 Flying"; break;
    case Agent::FLOATING:      t = "👻 Floating"; break;
    default:                   t = "❓ Unknown";
    }
    type->setText(t);
    type->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    type->setStyleSheet(baseFont + " font-size: 11px;");
    infoLayout->addWidget(type);

    
    auto makeStat = [&](const QString &text) {
        QLabel *lbl = new QLabel(text, infoBox);
        lbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lbl->setStyleSheet(
            baseFont +
            " font-size: 10px;"
            " border: 1px solid rgba(255,255,255,90);"
            " border-radius: 5px;"
            " padding: 1px 4px;"
            " background-color: rgba(255,255,255,25);"
            );
        return lbl;
    };

    infoLayout->addWidget(makeStat(QString("❤️ %1").arg(agent->getHp())));
    infoLayout->addWidget(makeStat(QString("🚶 %1").arg(agent->getMobility())));
    infoLayout->addWidget(makeStat(QString("⚔️ %1").arg(agent->getDamage())));
    infoLayout->addWidget(makeStat(QString("🎯 %1").arg(agent->getAttackRange())));

    infoLayout->addStretch();
    mainLayout->addWidget(infoBox, 1);

    return card;
}




bool GameBoardScreen::loadBoardFromFile(const QString &filename) {
    QFile file(filename);
   

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.trimmed().isEmpty()) {
            lines.append(line);
        }
    }
    file.close();

    if (lines.isEmpty()) {
        QMessageBox::warning(this, "Error", "Board file is empty: " + filename);
        return false;
    }

    // Initialize board with 9 columns, alternating 5/4 rows
    m_board.clear();
    m_board.resize(9);
    for (int col = 0; col < 9; col++) {
        int rowsInCol = (col % 2) ? 4 : 5;
        m_board[col].resize(rowsInCol);
        for (int row = 0; row < rowsInCol; row++) {
            m_board[col][row].col = col;
            m_board[col][row].row = row;
            m_board[col][row].type = ""; 
            m_board[col][row].agentName = ""; 
            m_board[col][row].hovered = false;
          
        }
    }

    
    for (int lineNum = 0; lineNum < lines.size(); lineNum++) {
        QString line = lines[lineNum];

        for (int charPos = 0; charPos < line.length(); charPos++) {
            QChar c = line[charPos];

            // Skip decorative characters
            if (c == ' ' || c == '_' || c == '/' || c == '\\') continue;

            // Calculate grid coordinates - HEX GRID MAPPING
            int col, row;
            if (lineNum % 2 == 1) { // Odd lines (1,3,5,7,9)
                col = (charPos - 1) / 3;
                row = (lineNum - 1) / 2;
            } else { // Even lines (2,4,6,8,10)
                col = (charPos - 1) / 3;
                row = (lineNum / 2) - 1;
            }

            // Boundary check
            if (col < 0 || col >= 9 || row < 0 || row >= m_board[col].size()) {
                continue;
            }

            
            switch (c.unicode()) {
            case '1':
                m_board[col][row].type = "1"; // PLAYER1_SPAWN
                break;
            case '2':
                m_board[col][row].type = "2"; // PLAYER2_SPAWN
                break;
            case 'W':
            case '~':
                m_board[col][row].type = "~"; // WATER
                break;
            case 'R':
            case '#':
                m_board[col][row].type = "#"; // ROCK
                break;
            default:
                m_board[col][row].type = "";  // NORMAL (empty string)
                break;
            }
        }
    }

    update();
    return true;
}

void GameBoardScreen::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    QPixmap bg(":/new/prefix1/Picsart_25-04-29_08-39-34-769.png");
    painter.drawPixmap(rect(), bg);


    drawSidePanels(painter);

    if (!m_board.isEmpty()) {
        QPoint gridTopLeft = m_gridWidget->mapTo(this, QPoint(0, 0));
        painter.translate(gridTopLeft);
        drawBoard(painter);
        painter.translate(-gridTopLeft);
    }
}


void GameBoardScreen::drawSidePanels(QPainter &painter) {
    QFont panelFont("Consolas", 11, QFont::Bold);
    painter.setFont(panelFont);

    QRect leftRect = m_leftPanel->geometry();
    QRect rightRect = m_rightPanel->geometry();

    QLinearGradient leftGrad(leftRect.topLeft(), leftRect.bottomRight());
    leftGrad.setColorAt(0, QColor(0, 0, 0, 0));   
    leftGrad.setColorAt(1, QColor(0, 0, 0, 0));  
    painter.fillRect(leftRect, leftGrad);

    painter.setPen(QColor(20, 255, 230));
    painter.drawText(QRect(leftRect.x(), leftRect.y() + 15, leftRect.width(), 30),
                     Qt::AlignCenter, m_player1Name);
    if (m_isPlayer1Turn) {
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(QRect(leftRect.x(), leftRect.y() + 45, leftRect.width(), 30),
                         Qt::AlignCenter, "▶ TURN ◀");
    }

    QLinearGradient rightGrad(rightRect.topLeft(), rightRect.bottomRight());
    rightGrad.setColorAt(0, QColor(0, 0, 0, 0));
    rightGrad.setColorAt(1, QColor(0, 0, 0, 0));
    painter.fillRect(rightRect, rightGrad);

    painter.setPen(QColor(255, 200, 150));
    painter.drawText(QRect(rightRect.x(), rightRect.y() + 15, rightRect.width(), 30),
                     Qt::AlignCenter, m_player2Name);
    if (!m_isPlayer1Turn) {
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(QRect(rightRect.x(), rightRect.y() + 45, rightRect.width(), 30),
                         Qt::AlignCenter, "▶ TURN ◀");
    }
}


void GameBoardScreen::drawHexagon(QPainter &painter, const QPoint &center, int size, const QString &cellType) {
    QPolygonF hexagon;
    for (int i = 0; i < 6; i++) {
        double angle = 2 * M_PI / 6 * i;
        hexagon << QPointF(center.x() + size * cos(angle), center.y() + size * sin(angle));
    }


    QColor cellColor;
    if (cellType == "~") { // WATER
        cellColor = QColor(100, 150, 255, 200);
    } else if (cellType == "#") { // ROCK
        cellColor = QColor(120, 120, 120, 200);
    } else if (cellType == "1") { // PLAYER1_SPAWN
        cellColor = QColor(255, 150, 220, 200); // pink
    } else if (cellType == "2") { // PLAYER2_SPAWN
        cellColor = QColor(150, 120, 255, 200); // purple
    } else { // NORMAL or empty string
        cellColor = QColor(200, 200, 150, 200);
    }

    painter.setBrush(cellColor);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawPolygon(hexagon);

}

void GameBoardScreen::drawBoard(QPainter &painter) {
    painter.setRenderHint(QPainter::Antialiasing);

    for (int col = 0; col < m_board.size(); col++) {
        for (int row = 0; row < m_board[col].size(); row++) {
            QPoint center = hexagonCenter(col, row);
            GameCell &cell = m_board[col][row];

    
            drawHexagon(painter, center, HEX_SIZE, cell.type);

            // Draw highlight if needed (on top of the base cell)
            if (m_highlightedCells.contains(QPoint(col, row))) {
                // Draw highlight overlay
                QPolygonF hexagon;
                for (int i = 0; i < 6; i++) {
                    double angle = 2 * M_PI / 6 * i;
                    hexagon << QPointF(center.x() + HEX_SIZE * cos(angle), center.y() + HEX_SIZE * sin(angle));
                }
                painter.setBrush(QColor(144, 238, 144, 150)); // Light green highlight
                painter.setPen(Qt::NoPen);
                painter.drawPolygon(hexagon);
            }

            if (m_flashActive && QPoint(col, row) == m_attackFlashCell) {
             
                QPolygonF hexagon;
                for (int i = 0; i < 6; i++) {
                    double angle = 2 * M_PI / 6 * i;
                    hexagon << QPointF(center.x() + HEX_SIZE * cos(angle), center.y() + HEX_SIZE * sin(angle));
                }
                painter.setBrush(QColor(255, 0, 0, 150)); // Red flash
                painter.setPen(Qt::NoPen);
                painter.drawPolygon(hexagon);
            }

            if (cell.hovered) {
               
                QPolygonF hexagon;
                for (int i = 0; i < 6; i++) {
                    double angle = 2 * M_PI / 6 * i;
                    hexagon << QPointF(center.x() + HEX_SIZE * cos(angle), center.y() + HEX_SIZE * sin(angle));
                }
                painter.setBrush(QColor(255, 255, 255, 80)); 
                painter.setPen(Qt::NoPen);
                painter.drawPolygon(hexagon);
            }

            // Draw cell type indicator (for empty cells)
            if (cell.agentName.isEmpty() && !cell.type.isEmpty() && cell.type != "1" && cell.type != "2") {
                painter.setPen(Qt::black);
                painter.drawText(QRect(center.x() - HEX_SIZE, center.y() - HEX_SIZE,
                                       HEX_SIZE * 2, HEX_SIZE * 2),
                                 Qt::AlignCenter, cell.type);
            }

            // Draw agent if present
            if (!cell.agentName.isEmpty() && m_agentMap.contains(cell.agentName)) {
                Agent* agent = m_agentMap[cell.agentName];
                QPixmap agentPixmap = agent->getPixmap();

                if (!agentPixmap.isNull()) {
                    // Draw selection circle if selected
                    if (m_agentSelected && m_selectedAgentName == cell.agentName) {
                        painter.setBrush(Qt::NoBrush);
                        painter.setPen(QPen(Qt::yellow, 3));
                        painter.drawEllipse(center, HEX_SIZE - 5, HEX_SIZE - 5);
                    }

                    // Draw player color border
                    QColor borderColor = (m_placedPlayer1Agents.contains(cell.agentName)) ?
                                             QColor(255, 100, 100) : QColor(100, 100, 255);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen(borderColor, 2));
                    painter.drawEllipse(center, HEX_SIZE - 2, HEX_SIZE - 2);

                    // Draw agent image
                    painter.drawPixmap(center.x() - 15, center.y() - 15, 30, 30, agentPixmap);

                    // Draw HP bar
                    int hpWidth = (agent->getHp() * 30) / agent->getMaxHp();
                    painter.setBrush(Qt::red);
                    painter.setPen(Qt::black);
                    painter.drawRect(center.x() - 15, center.y() + 20, 30, 5);
                    painter.setBrush(Qt::green);
                    painter.drawRect(center.x() - 15, center.y() + 20, hpWidth, 5);
                }
            }
        }
    }
}

void GameBoardScreen::mousePressEvent(QMouseEvent *event) {
  
    QWidget::mousePressEvent(event);
}


void GameBoardScreen::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void GameBoardScreen::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void GameBoardScreen::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasText()) {
        QString agentName = event->mimeData()->text();
        QPoint dropPos = event->pos();

        // Find the agent in the available agents
        Agent* agent = nullptr;
        QList<Agent*>* availableAgents = m_isPlayer1Turn ?
                                              &m_player1AvailableAgents : &m_player2AvailableAgents;

        for (Agent* a : *availableAgents) {
            if (a->getName() == agentName) {
                agent = a;
                break;
            }
        }

        if (agent) {
            handleDrop(dropPos, agent);
        }

        event->acceptProposedAction();
    }
}

void GameBoardScreen::handleDrop(const QPoint& position, Agent* agent) {
    QPoint gridPos = m_gridWidget->mapFrom(this, position);
    QPoint cellPos = convertToGridCoordinates(gridPos);

    if (cellPos.x() < 0 || cellPos.x() >= m_board.size() ||
        cellPos.y() < 0 || cellPos.y() >= m_board[cellPos.x()].size()) {
        return;
    }

    GameCell& cell = m_board[cellPos.x()][cellPos.y()];

    // Check if the cell is a valid spawn point for the current player
    bool validSpawn = false;
    if (m_isPlayer1Turn && cell.type == "1") {
        validSpawn = true;
    } else if (!m_isPlayer1Turn && cell.type == "2") {
        validSpawn = true;
    }

    // Check if the cell is empty
    bool cellEmpty = cell.agentName.isEmpty();

    if (validSpawn && cellEmpty) {
        // Place the agent on the board
        cell.agentName = agent->getName();

       
        QList<Agent*>* availableAgents = m_isPlayer1Turn ?
                                              &m_player1AvailableAgents : &m_player2AvailableAgents;
        availableAgents->removeOne(agent);

     
        if (m_isPlayer1Turn) {
            m_placedPlayer1Agents.insert(agent->getName());
        } else {
            m_placedPlayer2Agents.insert(agent->getName());
        }

        // Update UI
        update();

        // Check if both players have placed all their agents
        if (m_player1AvailableAgents.isEmpty() && m_player2AvailableAgents.isEmpty()) {
            m_startGameButton->setEnabled(true);
            updateStatusDisplay();
        }

      

        if (m_isPlayer1Turn &&
            m_player1AvailableAgents.isEmpty() &&
            !m_player2AvailableAgents.isEmpty()) {
            switchTurn();
        } else if (!m_isPlayer1Turn &&
                   m_player2AvailableAgents.isEmpty() &&
                   !m_player1AvailableAgents.isEmpty()) {
            switchTurn();
        } else {
            
            updateStatusDisplay();
        }
    }
}

QVector<QPoint> GameBoardScreen::getHexNeighbors(int col, int row) const {
    QVector<QPoint> neighbors;

    if (col % 2 == 0) {
        // Even columns
        neighbors = {
            QPoint(col,     row - 1),  // up
            QPoint(col + 1, row - 1),  // up-right
            QPoint(col + 1, row),      // down-right
            QPoint(col,     row + 1),  // down
            QPoint(col - 1, row),      // down-left
            QPoint(col - 1, row - 1)   // up-left
        };
    } else {
        // Odd columns
        neighbors = {
            QPoint(col,     row - 1),  // up
            QPoint(col + 1, row),      // up-right
            QPoint(col + 1, row + 1),  // down-right
            QPoint(col,     row + 1),  // down
            QPoint(col - 1, row + 1),  // down-left
            QPoint(col - 1, row)       // up-left
        };
    }

    return neighbors;
}









void GameBoardScreen::startAgentAnimation(const QString &agentName, const QPoint &targetCell) {
    if (!m_agentMap.contains(agentName)) return;

    m_isAnimating = true;
    m_animatingAgentName = agentName;
    m_animationTarget = targetCell;
    m_isAttackAnimation = false; 


    QPoint currentPos(-1, -1);
    for (int col = 0; col < m_board.size(); col++) {
        for (int row = 0; row < m_board[col].size(); row++) {
            if (m_board[col][row].agentName == agentName) {
                currentPos = QPoint(col, row);
                break;
            }
        }
        if (currentPos.x() != -1) break;
    }

    if (currentPos.x() != -1) {
        m_animationStart = hexagonCenter(currentPos.x(), currentPos.y());
        m_animationEnd = hexagonCenter(targetCell.x(), targetCell.y());
        m_animationProgress = 0.0f;
    }
}



void GameBoardScreen::startAttackAnimation(const QString &attackerName, const QString &targetName) {
    if (!m_agentMap.contains(attackerName) || !m_agentMap.contains(targetName)) return;

    m_isAnimating = true;
    m_animatingAgentName = attackerName;
    m_isAttackAnimation = true;

    // Find positions
    QPoint attackerPos(-1, -1);
    QPoint targetPos(-1, -1);

    for (int col = 0; col < m_board.size(); col++) {
        for (int row = 0; row < m_board[col].size(); row++) {
            if (m_board[col][row].agentName == attackerName) {
                attackerPos = QPoint(col, row);
            }
            if (m_board[col][row].agentName == targetName) {
                targetPos = QPoint(col, row);
            }
        }
    }

    if (attackerPos.x() != -1 && targetPos.x() != -1) {
        m_animationStart = hexagonCenter(attackerPos.x(), attackerPos.y());
        m_animationEnd = hexagonCenter(targetPos.x(), targetPos.y());
        m_animationProgress = 0.0f;
    }
}

void GameBoardScreen::updateAnimation() {
    if (m_isAnimating) {
        m_animationProgress += 0.05f;

        if (m_animationProgress >= 1.0f) {
            m_isAnimating = false;
            m_animationProgress = 1.0f;

            if (m_isAttackAnimation) {
                completeAttackAnimation();
            } else {
                completeMoveAnimation();
            }
        }
        update();
    }
}

void GameBoardScreen::completeMoveAnimation() {
    if (m_animatingAgentName.isEmpty()) return;

    // Update board state - move agent to target cell
    GameCell* currentCell = nullptr;
    for (int col = 0; col < m_board.size(); col++) {
        for (int row = 0; row < m_board[col].size(); row++) {
            if (m_board[col][row].agentName == m_animatingAgentName) {
                currentCell = &m_board[col][row];
                break;
            }
        }
        if (currentCell) break;
    }

    if (currentCell && m_animationTarget.x() >= 0 && m_animationTarget.x() < m_board.size() &&
        m_animationTarget.y() >= 0 && m_animationTarget.y() < m_board[m_animationTarget.x()].size()) {

        GameCell& targetCell = m_board[m_animationTarget.x()][m_animationTarget.y()];
        targetCell.agentName = currentCell->agentName;
        currentCell->agentName.clear();
    }

    m_animatingAgentName.clear();
    m_animationTarget = QPoint(-1, -1);
    update();
}

void GameBoardScreen::completeAttackAnimation() {
   
    m_animatingAgentName.clear();
    update();
}


void GameBoardScreen::endTurn() {
    if (m_isAnimating) return; 

    switchTurn();
}

void GameBoardScreen::switchTurn() {
    m_isPlayer1Turn = !m_isPlayer1Turn;
    if (m_isPlayer1Turn) {
        m_currentTurn++;
    }

    // Clear any selection
    m_agentSelected = false;
    m_selectedAgentName.clear();
    clearHighlights();

    updateStatusDisplay();
    update();
}



void GameBoardScreen::updateStatusDisplay() {
    QString status;

    if (!m_gameStarted) {
        if (m_isPlayer1Turn) {
            status = QString("%1: Place your agents (%2/5 placed)")
            .arg(m_player1Name).arg(m_placedPlayer1Agents.size());
        } else {
            status = QString("%1: Place your agents (%2/5 placed)")
            .arg(m_player2Name).arg(m_placedPlayer2Agents.size());
        }
    } else {
        status = QString("%1's Turn - Turn %2")
        .arg(m_isPlayer1Turn ? m_player1Name : m_player2Name)
            .arg(m_currentTurn);

        if (m_agentSelected) {
            status += " - Select destination";
        }
    }

    m_statusLabel->setText(status);
}

void GameBoardScreen::startGame() {
    if (m_placedPlayer1Agents.size() >= 4 && m_placedPlayer2Agents.size() >= 4) {
        m_gameStarted = true;
        m_startGameButton->setEnabled(false);
        m_startGameButton->setVisible(false);
        m_endTurnButton->setEnabled(true);

        // Remove numbers from spawn cells
        for (auto &column : m_board) {
            for (auto &cell : column) {
                if ((cell.type == "1" || cell.type == "2") && cell.agentName.isEmpty()) {
                    cell.type = "";
                }
            }
        }

        updateStatusDisplay();
        update();
    }
}

QPoint GameBoardScreen::hexagonCenter(int col, int row) const {
    double x = HEX_WIDTH + (HEX_WIDTH * 0.75 * col);
    double y = HEX_HEIGHT + (HEX_HEIGHT * row) + (col % 2 ? HEX_HEIGHT / 2 : 0);
    return QPoint(static_cast<int>(x), static_cast<int>(y));
}


GameBoardScreen::GameCell* GameBoardScreen::getCellAtPosition(const QPoint &pos) {
    QPoint gridPos = convertToGridCoordinates(pos);

    // Validate the position
    if (gridPos.x() >= 0 && gridPos.x() < m_board.size() &&
        gridPos.y() >= 0 && gridPos.y() < m_board[gridPos.x()].size()) {
        return &m_board[gridPos.x()][gridPos.y()];
    }

    return nullptr; // Invalid position
}

GameBoardScreen::GameCell* GameBoardScreen::getCellAtHexCoord(int col, int row) {
    if (col < 0 || col >= m_board.size() || row < 0 || row >= m_board[col].size()) {
        return nullptr;
    }
    return &m_board[col][row];
}



int GameBoardScreen::hexDistance(const QPoint &a, const QPoint &b) const {
    // Convert to cube coordinates
    auto toCube = [](const QPoint &p) {
        int x = p.x();
        int z = p.y() - ((p.x() - (p.x() & 1)) / 2);
        int y = -x - z;
        return std::make_tuple(x, y, z);
    };

    auto [ax, ay, az] = toCube(a);
    auto [bx, by, bz] = toCube(b);

    return (std::abs(ax - bx) + std::abs(ay - by) + std::abs(az - bz)) / 2;
}
























bool GameBoardScreen::eventFilter(QObject *obj, QEvent *event) {
    // Paint the grid
    if (obj == m_gridWidget && event->type() == QEvent::Paint) {
        QPainter painter(static_cast<QWidget *>(obj));
        drawBoard(painter);
        return true;
    }

    // Mouse move → hover highlight
    if (obj == m_gridWidget && event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint gridPos = convertToGridCoordinates(mouseEvent->pos());

        for (auto &column : m_board) {
            for (auto &cell : column) {
                cell.hovered = false;
            }
        }

        if (gridPos.x() >= 0 && gridPos.x() < m_board.size() &&
            gridPos.y() >= 0 && gridPos.y() < m_board[gridPos.x()].size()) {
            m_board[gridPos.x()][gridPos.y()].hovered = true;
        }

        update();
        return true;
    }

    // Mouse leave → clear hover
    if (obj == m_gridWidget && event->type() == QEvent::Leave) {
        for (auto &column : m_board) {
            for (auto &cell : column) {
                cell.hovered = false;
            }
        }
        update();
        return true;
    }

    // Mouse button press
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() != Qt::LeftButton) {
            return QWidget::eventFilter(obj, event);
        }

        // Grid clicks
        if (obj == m_gridWidget) {
            QPoint gridPos = convertToGridCoordinates(mouseEvent->pos());

            if (gridPos.x() >= 0 && gridPos.x() < m_board.size() &&
                gridPos.y() >= 0 && gridPos.y() < m_board[gridPos.x()].size()) {
                handleGridClick(gridPos);
            }
            return true;
        }

        // Agent cards
        Agent* agent = nullptr;
        bool isPlayer1 = false;

        QWidget *cardWidget = qobject_cast<QWidget*>(obj);
        if (cardWidget && cardWidget->property("agent").isValid()) {
            agent = cardWidget->property("agent").value<Agent*>();
            isPlayer1 = cardWidget->property("isPlayer1").toBool();
        } else if (QLabel *label = qobject_cast<QLabel*>(obj)) {
            if (label->property("agent").isValid()) {
                agent = label->property("agent").value<Agent*>();
                QWidget *parent = label->parentWidget();
                while (parent && !parent->property("isPlayer1").isValid()) {
                    parent = parent->parentWidget();
                }
                if (parent) {
                    isPlayer1 = parent->property("isPlayer1").toBool();
                }
            }
        }

        if (agent) {
            if (!m_gameStarted) {
                if ((m_isPlayer1Turn && isPlayer1) || (!m_isPlayer1Turn && !isPlayer1)) {
                    startDrag(agent);
                }
            } else {
                handleAgentSelection(agent);
            }
            return true;
        }
    }

    // Drag enter
    if (event->type() == QEvent::DragEnter) {
        QDragEnterEvent *dragEvent = static_cast<QDragEnterEvent*>(event);
        if (dragEvent->mimeData()->hasText()) {
            dragEvent->acceptProposedAction();
            return true;
        }
    }

    // Drag move
    if (event->type() == QEvent::DragMove) {
        QDragMoveEvent *dragEvent = static_cast<QDragMoveEvent*>(event);
        dragEvent->acceptProposedAction();
        return true;
    }

    // Drop
    if (event->type() == QEvent::Drop) {
        QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
        if (dropEvent->mimeData()->hasText()) {
            QString agentName = dropEvent->mimeData()->text();
            QPoint widgetPos = dropEvent->pos();
            QPoint gridPos = convertToGridCoordinates(widgetPos);

            qDebug() << "Drop event for agent:" << agentName << "at widget pos:" << widgetPos
                     << "mapped grid:" << gridPos;

            Agent* agent = nullptr;
            QList<Agent*>* availableAgents = m_isPlayer1Turn ?
                                                  &m_player1AvailableAgents : &m_player2AvailableAgents;

            for (Agent* a : *availableAgents) {
                if (a->getName() == agentName) {
                    agent = a;
                    break;
                }
            }

            if (agent && gridPos.x() >= 0 && gridPos.y() >= 0) {
                handleDrop(gridPos, agent);
            }

            dropEvent->acceptProposedAction();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}







void GameBoardScreen::handleAgentSelection(Agent* agent) {
    if (!agent) return;

    // Find agent position on board
    for (int col = 0; col < m_board.size(); col++) {
        for (int row = 0; row < m_board[col].size(); row++) {
            if (m_board[col][row].agentName == agent->getName()) {
                m_selectedAgentName = agent->getName();
                m_selectedStartPos = QPoint(col, row);
                m_agentSelected = true;
                highlightMovesForAgent(m_selectedStartPos, agent);
                return;
            }
        }
    }
}




void GameBoardScreen::startDrag(Agent* agent) {
    if (!agent) return;

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(agent->getName());
    drag->setMimeData(mimeData);

    QPixmap dragPixmap = agent->getPixmap();
    if (!dragPixmap.isNull()) {
        // Scale the pixmap for dragging
        QPixmap scaledPixmap = dragPixmap.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        drag->setPixmap(scaledPixmap);
        drag->setHotSpot(QPoint(15, 15)); // Center of the 30x30 image
    }

    drag->exec(Qt::CopyAction);
}







QPoint GameBoardScreen::convertToGridCoordinates(const QPoint &widgetPos) {
    // Find the closest hexagon center to the widget position
    int closestCol = -1;
    int closestRow = -1;
    double minDistance = std::numeric_limits<double>::max();

    for (int col = 0; col < m_board.size(); col++) {
        for (int row = 0; row < m_board[col].size(); row++) {
            QPoint center = hexagonCenter(col, row);
            double distance = QLineF(widgetPos, center).length();

            if (distance < minDistance) {
                minDistance = distance;
                closestCol = col;
                closestRow = row;
            }
        }
    }

    // Only return valid grid coordinates if close enough
    if (minDistance < HEX_SIZE * 1.5) {
        return QPoint(closestCol, closestRow);
    }

    return QPoint(-1, -1);
}






void GameBoardScreen::performAttack(const QPoint &attackerPos, const QPoint &defenderPos) {
    if (attackerPos.x() < 0 || defenderPos.x() < 0) return;

    GameCell &attCell = m_board[attackerPos.x()][attackerPos.y()];
    GameCell &defCell = m_board[defenderPos.x()][defenderPos.y()];

    if (attCell.agentName.isEmpty() || defCell.agentName.isEmpty()) return;
    if (!m_agentMap.contains(attCell.agentName) || !m_agentMap.contains(defCell.agentName)) return;

    Agent* attacker = m_agentMap[attCell.agentName];
    Agent* defender = m_agentMap[defCell.agentName];

    // Convert Agent object to string type for retreat check
    QString attackerType;
    switch (attacker->getType()) {
    case Agent::WATER_WALKING: attackerType = "WaterWalking"; break;
    case Agent::GROUNDED: attackerType = "Grounded"; break;
    case Agent::FLYING: attackerType = "Flying"; break;
    case Agent::FLOATING: attackerType = "Floating"; break;
    default: attackerType = "Unknown";
    }

    // 1) Attacker deals damage
    int damageDealt = attacker->getDamage();
    defender->takeDamage(damageDealt);

    // 2) Defender counter-attacks (if alive)
    if (defender->isAlive()) {
        int counterDamage = defender->getDamage() / 2;
        attacker->takeDamage(counterDamage);
    }

    // 3) Move attacker to random adjacent cell (if alive)
    if (attacker->isAlive()) {
        QVector<QPoint> adjacentCells = getHexNeighbors(defenderPos.x(), defenderPos.y());
        QVector<QPoint> validRetreatCells;

        for (const QPoint &cellPos : adjacentCells) {
            if (cellPos.x() < 0 || cellPos.x() >= m_board.size() ||
                cellPos.y() < 0 || cellPos.y() >= m_board[cellPos.x()].size()) {
                continue;
            }

            GameCell &cell = m_board[cellPos.x()][cellPos.y()];
            if (!cell.agentName.isEmpty()) continue;

            // Use the string-based function
            if (canStandOnCellForAgentType(attackerType, cell.type)) {
                validRetreatCells.append(cellPos);
            }
        }

        if (!validRetreatCells.isEmpty()) {
            QPoint retreatPos = validRetreatCells[QRandomGenerator::global()->bounded(validRetreatCells.size())];
            moveAgentOnBoard(attackerPos, retreatPos);
        }
    }

    // Check if agents died
    if (!defender->isAlive()) {
        removeAgentByName(defCell.agentName);
    }
    if (!attacker->isAlive()) {
        removeAgentByName(attCell.agentName);
    }

    // Flash effect
    m_attackFlashCell = defenderPos;
    m_flashActive = true;
    QTimer::singleShot(500, this, [this]() {
        m_flashActive = false;
        update();
    });

    checkForWinner();
    switchTurn();
    update();
}
void GameBoardScreen::removeAgentByName(const QString &name) {
    // Remove from board
    for (auto &column : m_board) {
        for (auto &cell : column) {
            if (cell.agentName == name) {
                cell.agentName.clear();
                break;
            }
        }
    }

    // Remove from placed sets
    if (m_placedPlayer1Agents.contains(name)) {
        m_placedPlayer1Agents.remove(name);
    } else if (m_placedPlayer2Agents.contains(name)) {
        m_placedPlayer2Agents.remove(name);
    }

   
    update();
}





void GameBoardScreen::handleGridClick(const QPoint &gridPos) {
    GameCell &cell = m_board[gridPos.x()][gridPos.y()];

    if (!m_gameStarted) {
        // Placement phase logic
        return;
    }

    // Game phase logic
    if (!cell.agentName.isEmpty()) {
        bool isAgent1 = m_placedPlayer1Agents.contains(cell.agentName);
        bool isOwn = (m_isPlayer1Turn && isAgent1) || (!m_isPlayer1Turn && !isAgent1);

        if (isOwn) {
            // Select own agent
            Agent* agent = m_agentMap.value(cell.agentName);
            if (agent) {
                m_selectedAgentName = cell.agentName;
                m_selectedStartPos = gridPos;
                m_agentSelected = true;
                highlightMovesForAgent(m_selectedStartPos, agent);
            }
        } else {
            // Attack enemy agent
            if (m_agentSelected && !m_selectedAgentName.isEmpty()) {
                Agent* attacker = m_agentMap.value(m_selectedAgentName);
                if (attacker) {
                    int distance = hexDistance(m_selectedStartPos, gridPos);
                    if (distance <= attacker->getAttackRange()) {
                        performAttack(m_selectedStartPos, gridPos);
                        clearHighlights();
                        m_agentSelected = false;
                        m_selectedAgentName.clear();
                    }
                }
            }
        }
    } else {
        // Move to empty cell
        if (m_agentSelected && m_highlightedCells.contains(gridPos)) {
            moveAgentOnBoard(m_selectedStartPos, gridPos);
        }
        clearHighlights();
        m_agentSelected = false;
        m_selectedAgentName.clear();
    }
}






void GameBoardScreen::highlightMovesForAgent(const QPoint &startPos, Agent* agent) {
    clearHighlights();

    if (!agent) return;

    // Convert Agent object to string type
    QString agentType;
    switch (agent->getType()) {
    case Agent::WATER_WALKING: agentType = "WaterWalking"; break;
    case Agent::GROUNDED: agentType = "Grounded"; break;
    case Agent::FLYING: agentType = "Flying"; break;
    case Agent::FLOATING: agentType = "Floating"; break;
    default: agentType = "Unknown";
    }

    std::queue<std::pair<QPoint, int>> q;
    q.push({startPos, 0});

    QVector<QVector<bool>> visited(m_board.size());
    for (int i = 0; i < m_board.size(); ++i) {
        visited[i].resize(m_board[i].size(), false);
    }
    visited[startPos.x()][startPos.y()] = true;

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        QPoint pos = current.first;
        int dist = current.second;

        if (dist > agent->getMobility()) continue;
        if (dist > 0) m_highlightedCells.append(pos);

        QVector<QPoint> neighbors = getHexNeighbors(pos.x(), pos.y());

        for (const QPoint &neighbor : neighbors) {
            if (neighbor.x() < 0 || neighbor.x() >= m_board.size() ||
                neighbor.y() < 0 || neighbor.y() >= m_board[neighbor.x()].size()) {
                continue;
            }

            if (visited[neighbor.x()][neighbor.y()]) continue;

            const GameCell &cell = m_board[neighbor.x()][neighbor.y()];
            bool canMove = true;
            int moveCost = 1;

         
            canMove = canStandOnCellForAgentType(agentType, cell.type);
            if (cell.type == "~") moveCost = 2;

            if (!cell.agentName.isEmpty()) {
                // Handle occupied cells
                bool isEnemy = (m_isPlayer1Turn && m_placedPlayer2Agents.contains(cell.agentName)) ||
                               (!m_isPlayer1Turn && m_placedPlayer1Agents.contains(cell.agentName));

                if (isEnemy && canMove && (dist + moveCost) <= agent->getMobility()) {
                    visited[neighbor.x()][neighbor.y()] = true;
                    m_highlightedCells.append(neighbor);
                }
            } else {
                if (canMove && (dist + moveCost) <= agent->getMobility()) {
                    visited[neighbor.x()][neighbor.y()] = true;
                    q.push({neighbor, dist + moveCost});
                }
            }
        }
    }

    update();
}








void GameBoardScreen::clearHighlights() {
    m_highlightedCells.clear();
    update();
}








bool GameBoardScreen::canStandOnCellForAgentType(const QString &agentType, const QString &cellType) const {
    if (cellType == "#") return false; // rock - no one stands on rock

    if (agentType == "Grounded") {
        return (cellType != "~"); // grounded can't stand on water
    } else if (agentType == "WaterWalking") {
        return true; // waterWalking can be on water and ground
    } else if (agentType == "Floating") {
        return true; // floating can be on water/ground
    } else if (agentType == "Flying") {
        return true; // flying can be anywhere
    }

    return (cellType != "#"); // default: can't stand on rock
}





void GameBoardScreen::moveAgentOnBoard(const QPoint &start, const QPoint &end) {
    if (!m_gameStarted) return;
    if (start == end) return;

    GameCell &startCell = m_board[start.x()][start.y()];
    GameCell &endCell = m_board[end.x()][end.y()];
    QString agentName = startCell.agentName;

    if (agentName.isEmpty()) return;

    // Handle enemy capture
    if (!endCell.agentName.isEmpty()) {
        QString enemyName = endCell.agentName;

        // Remove from placed sets
        if (m_placedPlayer1Agents.contains(enemyName)) {
            m_placedPlayer1Agents.remove(enemyName);
        } else if (m_placedPlayer2Agents.contains(enemyName)) {
            m_placedPlayer2Agents.remove(enemyName);
        }

        // Remove from agent map
        if (m_agentMap.contains(enemyName)) {
            Agent* enemyAgent = m_agentMap[enemyName];
            m_agentMap.remove(enemyName);
            delete enemyAgent;
        }
    }

    // Move the agent
    endCell.agentName = agentName;
    startCell.agentName.clear();

    update();
    switchTurn();
}



void GameBoardScreen::checkForWinner() {
    bool player1Alive = false;
    bool player2Alive = false;
    
    // Check if any agents are still placed
    for (const auto &column : m_board) {
        for (const auto &cell : column) {
            if (!cell.agentName.isEmpty()) {
                if (m_placedPlayer1Agents.contains(cell.agentName)) {
                    player1Alive = true;
                } else if (m_placedPlayer2Agents.contains(cell.agentName)) {
                    player2Alive = true;
                }
            }
        }
    }
    
    if (!player1Alive || !player2Alive) {
        QString winner = !player1Alive ? m_player2Name : m_player1Name;
        QString loser = !player1Alive ? m_player1Name : m_player2Name;
        
        QMessageBox::information(this, "Game Over",
                                 QString("🎉 VICTORY FOR %1! You win!\n Better luck next time %2.")
                                     .arg(winner).arg(loser));
        
        m_gameStarted = false;
        m_endTurnButton->setEnabled(false);
    }
}

