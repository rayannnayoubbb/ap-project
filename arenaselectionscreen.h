#ifndef ARENASELECTIONSCREEN_H
#define ARENASELECTIONSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QSignalMapper>

class ArenaSelectionScreen : public QWidget
{
    Q_OBJECT
public:
    explicit ArenaSelectionScreen(const QString& player1, const QString& player2, QWidget *parent = nullptr);

signals:
    void arenaSelected(const QString& filePath);
    void backToPlayerNamesRequested();

private slots:
    void onArenaButtonClicked(int arenaNumber);

private:
    void setupUI();
    QString m_player1Name;
    QString m_player2Name;
    QList<QPushButton*> m_arenaButtons;
};

#endif
