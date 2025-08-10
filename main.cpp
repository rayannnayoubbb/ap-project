#include <QApplication>
#include <QStackedWidget>
#include <QMessageBox>
#include "splashscreen.h"
#include "menuscreen.h"
#include "galleryscreen.h"
#include "playernamescreen.h"
#include "agentselectscreen.h"
#include "arenaselectionscreen.h"
#include "gameboardscreen.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStackedWidget stack;

    // Create screens
    SplashScreen *splash = new SplashScreen();
    MenuScreen *menu = new MenuScreen();
    GalleryScreen *gallery = new GalleryScreen();
    PlayerNameScreen *nameScreen = new PlayerNameScreen();
    AgentSelectScreen *agentScreen = nullptr;
    ArenaSelectionScreen *arenaScreen = nullptr;
    GameBoardScreen *gameBoard = nullptr;

    stack.addWidget(splash);
    stack.addWidget(menu);
    stack.addWidget(gallery);
    stack.addWidget(nameScreen);

    // Splash to Menu
    QObject::connect(splash, &SplashScreen::continueClicked, [&](){
        stack.setCurrentWidget(menu);
    });

    // Menu to Gallery
    QObject::connect(menu, &MenuScreen::galleryRequested, [&](){
        stack.setCurrentWidget(gallery);
    });

    // Menu to Game (via Player Names)
    QObject::connect(menu, &MenuScreen::gameRequested, [&](){
        nameScreen->resetForm();
        stack.setCurrentWidget(nameScreen);
    });

    // Gallery back to Menu
    QObject::connect(gallery, &GalleryScreen::backRequested, [&](){
        stack.setCurrentWidget(menu);
    });

    // Player Names to Agent Selection
    QObject::connect(nameScreen, &PlayerNameScreen::namesConfirmed, [&](const QString &p1, const QString &p2){
        // Clean up previous screens
        if (agentScreen) agentScreen->deleteLater();
        if (arenaScreen) arenaScreen->deleteLater();
        if (gameBoard) gameBoard->deleteLater();

        // Create new agent selection screen
        agentScreen = new AgentSelectScreen(p1, p2);
        stack.addWidget(agentScreen);

        // Agent Selection to Arena Selection
        QObject::connect(agentScreen, &AgentSelectScreen::agentsSelected, [&, p1, p2](){
            arenaScreen = new ArenaSelectionScreen(p1, p2);
            stack.addWidget(arenaScreen);

            // Arena Selection to Game Board
            QObject::connect(arenaScreen, &ArenaSelectionScreen::arenaSelected, [&, p1, p2](const QString &filename){
                if (gameBoard) gameBoard->deleteLater();

                gameBoard = new GameBoardScreen(p1, p2);
                // TODO: Set selected agents on gameBoard here

                if (gameBoard->loadBoardFromFile(filename)) {
                    stack.addWidget(gameBoard);

                    // Game Board back to Menu
                    QObject::connect(gameBoard, &GameBoardScreen::backToMenuRequested, [&](){
                        agentScreen->deleteLater();
                        arenaScreen->deleteLater();
                        gameBoard->deleteLater();
                        agentScreen = nullptr;
                        arenaScreen = nullptr;
                        gameBoard = nullptr;
                        stack.setCurrentWidget(menu);
                    });

                    stack.setCurrentWidget(gameBoard);
                } else {
                    QMessageBox::warning(nullptr, "Error", "Failed to load arena file!");
                    gameBoard->deleteLater();
                    gameBoard = nullptr;
                }
            });

            // Arena Selection back to Agent Selection
            QObject::connect(arenaScreen, &ArenaSelectionScreen::backToPlayerNamesRequested, [&](){
                arenaScreen->deleteLater();
                arenaScreen = nullptr;
                stack.setCurrentWidget(agentScreen);
            });

            stack.setCurrentWidget(arenaScreen);
        });

        // Agent Selection back to Player Names
        QObject::connect(agentScreen, &AgentSelectScreen::backToPlayerNamesRequested, [&](){
            agentScreen->deleteLater();
            agentScreen = nullptr;
            stack.setCurrentWidget(nameScreen);
        });

        stack.setCurrentWidget(agentScreen);
    });

    stack.setWindowTitle("Hexagon Game");
    stack.setFixedSize(800, 450);
    stack.show();

    return a.exec();
}
