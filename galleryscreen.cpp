#include "galleryscreen.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>

GalleryScreen::GalleryScreen(QWidget *parent) : QWidget(parent)
{

    setFixedSize(800, 450);


    QWidget *container = new QWidget(this);
    container->setFixedSize(800, 450);

    QLabel *imageLabel = new QLabel(container);
    imageLabel->setGeometry(0, 0, 800, 450);

    QPixmap galleryImage(":/new/prefix1/Picsart_25-04-29_09-09-25-428.png");

    if (!galleryImage.isNull()) {
        qDebug() << "Original gallery image size:" << galleryImage.size();


        QPixmap scaled = galleryImage.scaled(800, 450,
                                             Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation);


        imageLabel->setPixmap(scaled);
        imageLabel->setAlignment(Qt::AlignCenter);


        imageLabel->setStyleSheet("background-color: black;");
    } else {
        qDebug() << "Failed to load gallery image!";
        QPixmap placeholder(800, 450);
        placeholder.fill(Qt::darkGray);
        imageLabel->setPixmap(placeholder);
    }


    QPushButton *backButton = new QPushButton("Back to Menu", container);
    backButton->setFixedSize(150, 50);


    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(100, 255, 100, 150);"
        "   border: 2px solid white;"
        "   border-radius: 10px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   color: white;"
        "}"
        "QPushButton:hover { background-color: rgba(255, 255, 255, 200); }"
        );


    int btnX = (800 - backButton->width()) / 2;
    int btnY = 450 - backButton->height() - 40;
    backButton->move(btnX, btnY);

    connect(backButton, &QPushButton::clicked, this, &GalleryScreen::onBackClicked);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(container);
    mainLayout->setContentsMargins(0, 0, 0, 0);
}

void GalleryScreen::onBackClicked()
{
    emit backRequested();
}
