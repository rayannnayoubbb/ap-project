#include <QApplication>
#include <QStackedWidget>
#include<QMessageBox>
#include "splashscreen.h"
#include "menuscreen.h"
#include "galleryscreen.h"
#include "playernamescreen.h"
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
    ArenaSelectionScreen *arenaScreen = nullptr;
    GameBoardScreen *gameBoard = nullptr;


    stack.addWidget(splash);
    stack.addWidget(menu);
    stack.addWidget(gallery);
    stack.addWidget(nameScreen);

    QObject::connect(splash, &SplashScreen::continueClicked, [&](){
        stack.setCurrentWidget(menu);
    });

    QObject::connect(menu, &MenuScreen::galleryRequested, [&](){
        stack.setCurrentWidget(gallery);
    });

    QObject::connect(menu, &MenuScreen::gameRequested, [&](){
        nameScreen->resetForm();
        stack.setCurrentWidget(nameScreen);
    });


    QObject::connect(gallery, &GalleryScreen::backRequested, [&](){
        stack.setCurrentWidget(menu);
    });


    QObject::connect(nameScreen, &PlayerNameScreen::namesConfirmed, [&](const QString &p1, const QString &p2){

        if (arenaScreen) {
            arenaScreen->deleteLater();
        }
        if (gameBoard) {
            gameBoard->deleteLater();
        }


        arenaScreen = new ArenaSelectionScreen(p1, p2);
        stack.addWidget(arenaScreen);


        QObject::connect(arenaScreen, &ArenaSelectionScreen::arenaSelected, [&, p1, p2](const QString &filename){

            if (gameBoard) {
                gameBoard->deleteLater();
            }


            gameBoard = new GameBoardScreen(p1, p2);


            if (gameBoard->loadBoardFromFile(filename)) {
                stack.addWidget(gameBoard);


                QObject::connect(gameBoard, &GameBoardScreen::backToMenuRequested, [&](){

                    if (arenaScreen) arenaScreen->deleteLater();
                    if (gameBoard) gameBoard->deleteLater();
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


        QObject::connect(arenaScreen, &ArenaSelectionScreen::backToPlayerNamesRequested, [&](){
            arenaScreen->deleteLater();
            arenaScreen = nullptr;
            stack.setCurrentWidget(nameScreen);
        });

        stack.setCurrentWidget(arenaScreen);
    });


    stack.setWindowTitle("Hexagon Game");
    stack.setFixedSize(800, 450);
    stack.show();

    return a.exec();
}
