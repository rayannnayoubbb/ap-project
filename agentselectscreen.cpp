#include "agentselectscreen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QScrollArea>
#include <QGridLayout>
#include <QPixmap>
#include <QDebug>

AgentSelectScreen::AgentSelectScreen(const QString& player1Name, const QString& player2Name, QWidget* parent)
    : QWidget(parent),
    m_player1Name(player1Name),
    m_player2Name(player2Name),
    m_currentSelectingPlayer(Agent::PLAYER1) {

    // Set background
    QLabel *background = new QLabel(this);
    background->setGeometry(0, 0, 800, 450);
    QPixmap bgImage(":/new/prefix1/Picsart_25-04-29_08-39-34-769.png");
    if (!bgImage.isNull()) {
        bgImage = bgImage.scaled(800, 450, Qt::KeepAspectRatioByExpanding);
        background->setPixmap(bgImage);
    } else {
        background->setStyleSheet("background-color: black;");
        qWarning() << "Failed to load background image";
    }

    // Create all available agents
    m_allAgents = {
        // 🌊 WATER WALKING AGENTS
        new Agent(Agent::WATER_WALKING, "Billy", ":/new/prefix1/agents/Billy.png", 320, 3, 90, 1),
        new Agent(Agent::WATER_WALKING, "Reketon", ":/new/prefix1/agents/Reketon.png", 320, 2, 80, 2),
        new Agent(Agent::WATER_WALKING, "Angus", ":/new/prefix1/agents/Angus.png", 400, 2, 100, 1),
        new Agent(Agent::WATER_WALKING, "Duraham", ":/new/prefix1/agents/Duraham.png", 320, 2, 100, 2),
        new Agent(Agent::WATER_WALKING, "Colonel Baba", ":/new/prefix1/agents/Colonel_Baba.png", 400, 2, 100, 1),
        new Agent(Agent::WATER_WALKING, "Medusa", ":/new/prefix1/agents/Medusa.png", 320, 2, 90, 2),
        new Agent(Agent::WATER_WALKING, "Bunka", ":/new/prefix1/agents/Bunka.png", 320, 3, 100, 1),
        new Agent(Agent::WATER_WALKING, "Sanka", ":/new/prefix1/agents/Sanka.png", 320, 3, 100, 1),

        // 🏔️ GROUNDED AGENTS
        new Agent(Agent::GROUNDED, "Sir Lamorak", ":/new/prefix1/agents/Sir_Lamorak.png", 320, 3, 110, 1),
        new Agent(Agent::GROUNDED, "Kabu", ":/new/prefix1/agents/Kabu.png", 400, 2, 120, 1),
        new Agent(Agent::GROUNDED, "Rajakal", ":/new/prefix1/agents/Rajakal.png", 320, 2, 130, 1),
        new Agent(Agent::GROUNDED, "Salih", ":/new/prefix1/agents/Salih.png", 400, 2, 80, 1),
        new Agent(Agent::GROUNDED, "Khan", ":/new/prefix1/agents/Khan.png", 320, 2, 90, 1),
        new Agent(Agent::GROUNDED, "Boi", ":/new/prefix1/agents/Boi.png", 400, 2, 100, 1),
        new Agent(Agent::GROUNDED, "Eloi", ":/new/prefix1/agents/Eloi.png", 240, 2, 100, 2),
        new Agent(Agent::GROUNDED, "Kanar", ":/new/prefix1/agents/Kanar.png", 160, 2, 100, 2),
        new Agent(Agent::GROUNDED, "Elsa", ":/new/prefix1/agents/Elsa.png", 320, 2, 140, 2),
        new Agent(Agent::GROUNDED, "Karissa", ":/new/prefix1/agents/Karissa.png", 280, 2, 80, 2),
        new Agent(Agent::GROUNDED, "Sir Philip", ":/new/prefix1/agents/Sir_Philip.png", 400, 2, 100, 1),
        new Agent(Agent::GROUNDED, "Frost", ":/new/prefix1/agents/Frost.png", 260, 2, 80, 2),
        new Agent(Agent::GROUNDED, "Tusk", ":/new/prefix1/agents/Tusk.png", 400, 2, 100, 1),

        // 🦅 FLYING AGENTS
        new Agent(Agent::FLYING, "Rambu", ":/new/prefix1/agents/Rambu.png", 320, 3, 120, 1),

        // 👻 FLOATING AGENTS
        new Agent(Agent::FLOATING, "Sabrina", ":/new/prefix1/agents/Sabrina.png", 320, 3, 100, 1),
        new Agent(Agent::FLOATING, "Death", ":/new/prefix1/agents/Death.png", 240, 3, 120, 2)
    };

    setupUI();
}

void AgentSelectScreen::setupUI() {
    // Main container with semi-transparent background
    QWidget *container = new QWidget(this);
    container->setGeometry(0, 0, 800, 450);
    container->setStyleSheet("background-color: rgba(0, 0, 0, 100);");

    QVBoxLayout* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Player info area
    QHBoxLayout* infoLayout = new QHBoxLayout();
    m_playerLabel = new QLabel(m_player1Name + "'s Selection (0/5)", container);
    m_playerLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    infoLayout->addWidget(m_playerLabel, 0, Qt::AlignLeft);

    m_selectionCountLabel = new QLabel("Selected: 0/5", container);
    m_selectionCountLabel->setStyleSheet("color: white; font-size: 20px;");
    infoLayout->addWidget(m_selectionCountLabel, 0, Qt::AlignRight);

    mainLayout->addLayout(infoLayout);

    // Agent selection area
    m_scrollArea = new QScrollArea(container);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet("background-color: transparent; border: none;");

    QWidget* scrollWidget = new QWidget();
    scrollWidget->setStyleSheet("background-color: transparent;");

    m_gridLayout = new QGridLayout(scrollWidget);
    m_gridLayout->setSpacing(15);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);

    createAgentCards();

    m_scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(m_scrollArea);

    // Button area
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);

    m_backButton = new QPushButton("Back", container);
    m_backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(231, 76, 60, 200);"
        "   border: 2px solid white;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(192, 57, 43, 200);"
        "}"
        );
    connect(m_backButton, &QPushButton::clicked, this, &AgentSelectScreen::backToPlayerNamesRequested);
    buttonLayout->addWidget(m_backButton);

    m_switchPlayerButton = new QPushButton("Switch to " + m_player2Name, container);
    m_switchPlayerButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(52, 152, 219, 200);"
        "   border: 2px solid white;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(41, 128, 185, 200);"
        "}"
        );
    connect(m_switchPlayerButton, &QPushButton::clicked, this, &AgentSelectScreen::switchPlayer);
    buttonLayout->addWidget(m_switchPlayerButton);

    m_confirmButton = new QPushButton("Confirm Selection", container);
    m_confirmButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(46, 204, 113, 200);"
        "   border: 2px solid white;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   padding: 8px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(39, 174, 96, 200);"
        "}"
        "QPushButton:disabled {"
        "   background-color: rgba(127, 140, 141, 200);"
        "}"
        );
    m_confirmButton->setEnabled(false);
    connect(m_confirmButton, &QPushButton::clicked, this, &AgentSelectScreen::confirmSelection);
    buttonLayout->addWidget(m_confirmButton);

    mainLayout->addLayout(buttonLayout);
}

void AgentSelectScreen::createAgentCards() {
    m_agentButtonGroup = new QButtonGroup(this);
    m_agentButtonGroup->setExclusive(false);

    int row = 0, col = 0;
    const int maxCols = 4;

    for (int i = 0; i < m_allAgents.size(); ++i) {
        Agent* agent = m_allAgents[i];

        QPushButton* agentButton = new QPushButton();
        agentButton->setCheckable(true);
        agentButton->setFixedSize(150, 200);
        agentButton->setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(50, 50, 50, 200);"
            "   border: 2px solid #555;"
            "   border-radius: 10px;"
            "}"
            "QPushButton:checked {"
            "   border: 3px solid gold;"
            "   background-color: rgba(70, 70, 70, 200);"
            "}"
            "QPushButton:hover {"
            "   border: 2px solid #888;"
            "}"
            );

        QVBoxLayout* buttonLayout = new QVBoxLayout(agentButton);
        buttonLayout->setSpacing(5);
        buttonLayout->setContentsMargins(5, 10, 5, 10);

        QLabel* imageLabel = new QLabel();
        imageLabel->setPixmap(QPixmap(agent->getImagePath()).scaled(100, 100, Qt::KeepAspectRatio));
        imageLabel->setAlignment(Qt::AlignCenter);
        buttonLayout->addWidget(imageLabel);

        QLabel* nameLabel = new QLabel(agent->getName());
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
        buttonLayout->addWidget(nameLabel);

        QLabel* typeLabel = new QLabel(
            agent->getType() == Agent::WATER_WALKING ? "🌊 Water" :
                agent->getType() == Agent::GROUNDED ? "🏔️ Ground" :
                agent->getType() == Agent::FLYING ? "🦅 Flying" : "👻 Floating"
            );
        typeLabel->setAlignment(Qt::AlignCenter);
        typeLabel->setStyleSheet("color: white; font-size: 12px;");
        buttonLayout->addWidget(typeLabel);

        QLabel* statsLabel = new QLabel(
            QString("❤️ %1  🏃 %2\n⚔️ %3  🎯 %4")
                .arg(agent->getHp())
                .arg(agent->getMobility())
                .arg(agent->getDamage())
                .arg(agent->getAttackRange())
            );
        statsLabel->setAlignment(Qt::AlignCenter);
        statsLabel->setStyleSheet("color: white; font-size: 12px;");
        buttonLayout->addWidget(statsLabel);

        m_gridLayout->addWidget(agentButton, row, col);
        m_agentButtonGroup->addButton(agentButton, i);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }

    connect(m_agentButtonGroup, &QButtonGroup::idClicked,
            this, &AgentSelectScreen::handleAgentSelection);
}

void AgentSelectScreen::handleAgentSelection(int id) {
    Agent* selectedAgent = m_allAgents[id];

    if (m_currentPlayerSelection.contains(selectedAgent)) {
        m_currentPlayerSelection.removeOne(selectedAgent);
    } else {
        if (m_currentPlayerSelection.size() < 5) {
            m_currentPlayerSelection.append(selectedAgent);
        } else {
            // Deselect the button if already have 5 selected
            m_agentButtonGroup->button(id)->setChecked(false);
        }
    }

    updateSelectionDisplay();
}

void AgentSelectScreen::updateSelectionDisplay() {
    m_selectionCountLabel->setText(QString("Selected: %1/5").arg(m_currentPlayerSelection.size()));
    m_confirmButton->setEnabled(m_currentPlayerSelection.size() == 5);
}

void AgentSelectScreen::confirmSelection() {
    if (m_currentSelectingPlayer == Agent::PLAYER1) {
        m_player1Agents = m_currentPlayerSelection;
        for (Agent* agent : m_player1Agents) {
            agent->setPlayer(Agent::PLAYER1);
        }
        clearCurrentPlayerSelection();
        switchPlayer();
    } else {
        m_player2Agents = m_currentPlayerSelection;
        for (Agent* agent : m_player2Agents) {
            agent->setPlayer(Agent::PLAYER2);
        }
        emit agentsSelected();
    }
}

void AgentSelectScreen::switchPlayer() {
    m_currentSelectingPlayer = (m_currentSelectingPlayer == Agent::PLAYER1)
    ? Agent::PLAYER2
    : Agent::PLAYER1;

    clearCurrentPlayerSelection();

    QString playerName = (m_currentSelectingPlayer == Agent::PLAYER1)
                             ? m_player1Name
                             : m_player2Name;
    m_playerLabel->setText(playerName + "'s Selection (0/5)");

    m_switchPlayerButton->setText(
        (m_currentSelectingPlayer == Agent::PLAYER1)
            ? "Switch to " + m_player2Name
            : "Switch to " + m_player1Name
        );

    // Reset all buttons
    for (QAbstractButton* button : m_agentButtonGroup->buttons()) {
        button->setChecked(false);
        button->setEnabled(true);
    }

    // Disable already selected agents
    QVector<Agent*> allSelected = m_player1Agents + m_player2Agents;
    for (int i = 0; i < m_allAgents.size(); ++i) {
        if (allSelected.contains(m_allAgents[i])) {
            m_agentButtonGroup->button(i)->setEnabled(false);
        }
    }
}

void AgentSelectScreen::clearCurrentPlayerSelection() {
    m_currentPlayerSelection.clear();
    updateSelectionDisplay();
}
