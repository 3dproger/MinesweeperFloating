#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include "gamecontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onClickedCell();
    void onFieldChanged();
    void on_pushButtonNext_clicked();

    void on_actionRestart_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

private:
    void restartGame();
    void clearLayout(QLayout *layout);

    Ui::MainWindow *ui;
    GameController* game = nullptr;
    QMap<QPushButton*, QPoint> cells;
    bool showMines = false;

    const QIcon IconMine;
};
#endif // MAINWINDOW_H
