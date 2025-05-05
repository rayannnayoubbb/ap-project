#include "arenaselectionscreen.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QSignalMapper>

ArenaSelectionScreen::ArenaSelectionScreen(const QString& player1, const QString& player2, QWidget *parent)
    : QWidget(parent), m_player1Name(player1), m_player2Name(player2)
{
    setupUI();
}

void ArenaSelectionScreen::setupUI()
{
    setFixedSize(800, 450);


    QLabel *background = new QLabel(this);
    background->setGeometry(0, 0, 800, 450);
    background->setStyleSheet("background-color: black;");


    QWidget *container = new QWidget(this);
    container->setGeometry(0, 0, 800, 450);


    QLabel *title = new QLabel("SELECT ARENA", container);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-size: 28px; font-weight: bold;");
    title->setGeometry(0, 20, 800, 60);


    QLabel *playersLabel = new QLabel(QString("%1  vs  %2").arg(m_player1Name).arg(m_player2Name), container);
    playersLabel->setAlignment(Qt::AlignCenter);
    playersLabel->setStyleSheet("color: white; font-size: 20px;");
    playersLabel->setGeometry(0, 80, 800, 30);


    QWidget *buttonsContainer = new QWidget(container);
    buttonsContainer->setGeometry(150, 120, 500, 200);
    QGridLayout *gridLayout = new QGridLayout(buttonsContainer);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(0, 0, 0, 0);


    QSignalMapper *signalMapper = new QSignalMapper(this);
    for (int i = 0; i < 8; i++) {
        QPushButton *button = new QPushButton(QString("ARENA %1").arg(i+1), buttonsContainer);
        button->setFixedSize(110, 60);
        button->setStyleSheet(
            "QPushButton {"
            "   background-color: rgba(33, 178, 62, 200);"
            "   border: 2px solid white;"
            "   border-radius: 10px;"
            "   color: white;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgba(46, 204, 113, 200);"
            "}"
            );

        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, i+1); // Map to arena number 1-8
        m_arenaButtons.append(button);
        gridLayout->addWidget(button, i / 4, i % 4);
    }

    connect(signalMapper, SIGNAL(mappedInt(int)), this, SLOT(onArenaButtonClicked(int)));


    QPushButton *backButton = new QPushButton("BACK", container);
    backButton->setGeometry(350, 380, 100, 40);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(231, 76, 60, 200);"
        "   border: 2px solid white;"
        "   border-radius: 10px;"
        "   color: white;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgba(192, 57, 43, 200);"
        "}"
        );
    connect(backButton, &QPushButton::clicked, this, &ArenaSelectionScreen::backToPlayerNamesRequested);
}

void ArenaSelectionScreen::onArenaButtonClicked(int arenaNumber)
{
    QString filename = QString(":/new/prefix1/New folder/grid%1.txt").arg(arenaNumber);
    qDebug() << "Attempting to load arena:" << filename;

    if (QFile::exists(filename)) {
        emit arenaSelected(filename);
    } else {
        qWarning() << "Arena file not found:" << filename;

    }
}
