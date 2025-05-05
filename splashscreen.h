#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = nullptr);

signals:
    void continueClicked();

private slots:
    void onContinueClicked();

private:
    QLabel *imageLabel;
    QPushButton *continueButton;
};

#endif
