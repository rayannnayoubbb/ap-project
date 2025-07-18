#ifndef GALLERYSCREEN_H
#define GALLERYSCREEN_H

#include <QWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

class GalleryScreen : public QWidget
{
    Q_OBJECT
public:
    explicit GalleryScreen(QWidget *parent = nullptr);

signals:
    void backRequested();

private:
    void createAgentCards();
    void addAgentCard(QGridLayout *grid,
                      const QString &name,
                      const QString &classType,
                      const QString &imagePath,
                      int hp,
                      int mobility,
                      int damage,
                      int range,
                      int row,
                      int col);

    QScrollArea *m_scrollArea;
    QWidget *m_container;
};

#endif // GALLERYSCREEN_H
