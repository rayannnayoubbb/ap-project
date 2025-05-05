#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <QWidget>

class MenuScreen : public QWidget
{
    Q_OBJECT
public:
    explicit MenuScreen(QWidget *parent = nullptr);

signals:
    void galleryRequested();
    void gameRequested();

private slots:
    void onGalleryClicked();
    void onGameClicked();
};
#endif
