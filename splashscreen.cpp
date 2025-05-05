#include "splashscreen.h"
#include <QVBoxLayout>
#include <QPixmap>
#include <QDebug>

SplashScreen::SplashScreen(QWidget *parent) : QWidget(parent)
{

    setFixedSize(800, 450);


    QWidget *container = new QWidget(this);
    container->setFixedSize(800, 450);


    imageLabel = new QLabel(container);
    imageLabel->setGeometry(0, 0, 800, 450); // Fill container


    QPixmap splashImage(":/new/prefix1/Picsart_25-04-28_01-41-26-837.png");

    if (!splashImage.isNull()) {
        qDebug() << "Original image size:" << splashImage.size();


        QPixmap scaled = splashImage.scaled(800, 450,
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation);


        imageLabel->setPixmap(scaled);
        imageLabel->setAlignment(Qt::AlignCenter);


        imageLabel->setStyleSheet("background-color: black;");
    } else {
        qDebug() << "Failed to load image!";
        QPixmap placeholder(800, 450);
        placeholder.fill(Qt::darkGray);
        imageLabel->setPixmap(placeholder);
    }


    continueButton = new QPushButton("START!", container);
    continueButton->setFixedSize(100, 50);


    continueButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 100, 255, 150);"
        "   border: 2px solid white;"
        "   border-radius: 10px;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   color: white;"
        "}"
        "QPushButton:hover { background-color: rgba(253, 207, 34, 0.8); }"
        );


    int btnX = (800 - continueButton->width()) / 2;
    int btnY = 450 - continueButton->height() - 40;
    continueButton->move(btnX, btnY);

    connect(continueButton, &QPushButton::clicked, this, &SplashScreen::onContinueClicked);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(container);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void SplashScreen::onContinueClicked()
{
    emit continueClicked();
}
