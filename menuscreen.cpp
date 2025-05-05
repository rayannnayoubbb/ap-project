#include "menuscreen.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

MenuScreen::MenuScreen(QWidget *parent) : QWidget(parent)
{
    setFixedSize(800, 450);

    QLabel *imageLabel = new QLabel(this);
    QPixmap menuImage(":/new/prefix1/Picsart_25-04-29_09-07-05-740.png");


        setFixedSize(800, 450);


        QWidget *container = new QWidget(this);
        container->setFixedSize(800, 450);


        imageLabel = new QLabel(container);
        imageLabel->setGeometry(0, 0, 800, 450);

        if (!menuImage.isNull()) {
            qDebug() << "Original image size:" << menuImage.size();


            QPixmap scaled = menuImage.scaled(800, 450,
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

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 50, 0, 100);

    QPushButton *galleryButton = new QPushButton("AGENT GALLERY", this);
    galleryButton->setFixedSize(300, 60);
    galleryButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 100, 255, 150);"
        "   border: 2px solid white;"
        "   border-radius: 10px;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   color: white;"
        "}"
        "QPushButton:hover { background-color:rgba(253, 207, 34, 0.8); }"
        );

    QPushButton *gameButton = new QPushButton("Start an Async PVP Game", this);
    gameButton->setFixedSize(300, 60);
    gameButton->setStyleSheet(galleryButton->styleSheet());

    layout->addWidget(galleryButton, 0, Qt::AlignCenter);
    layout->addWidget(gameButton, 0, Qt::AlignCenter);

    connect(galleryButton, &QPushButton::clicked, this, &MenuScreen::onGalleryClicked);
    connect(gameButton, &QPushButton::clicked, this, &MenuScreen::onGameClicked);
}

void MenuScreen::onGalleryClicked()
{
    emit galleryRequested();
}

void MenuScreen::onGameClicked()
{
    emit gameRequested();
}
