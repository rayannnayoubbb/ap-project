#ifndef PLAYERNAMESCREEN_H
#define PLAYERNAMESCREEN_H

#include <QWidget>
#include <QLineEdit>

class PlayerNameScreen : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerNameScreen(QWidget *parent = nullptr);
    void resetForm();

signals:
    void namesConfirmed(const QString& player1, const QString& player2);

private slots:
    void onSubmitClicked();

private:
    QLineEdit *player1Input;
    QLineEdit *player2Input;
};

#endif
