#ifndef GALLERYSCREEN_H
#define GALLERYSCREEN_H

#include <QWidget>

class GalleryScreen : public QWidget
{
    Q_OBJECT
public:
    explicit GalleryScreen(QWidget *parent = nullptr);

signals:
    void backRequested();  // Correct signal name

private slots:
    void onBackClicked();
};

#endif // GALLERYSCREEN_H
