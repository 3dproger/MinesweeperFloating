#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , IconMine(QIcon(":/images/mine.png"))
{
    ui->setupUi(this);

    restartGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onClickedCell()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button)
    {
        qCritical() << "!button";
        return;
    }

    if (!cells.contains(button))
    {
        qCritical() << "button not contains cells";
        return;
    }

    const QPoint point = cells[button];
    game->clickAtCell(point);
}

void MainWindow::onFieldChanged()
{
    const QVector<QVector<GameController::Cell>> field = game->getField();

    for (int x = 0; x < field.count(); ++x)
    {
        const QVector<GameController::Cell> line = field[x];

        for (int y = 0; y < line.count(); ++y)
        {
            const GameController::Cell cell = line[y];
            const QPoint point(x, y);

            QColor color = QColor(0, 0, 0);

            QPushButton* button = cells.key(point, nullptr);
            if (!button)
            {
                qCritical() << "!button";
                continue;
            }

            button->setIcon(QIcon());

            switch (cell.type)
            {
            case GameController::Unknown:
                button->setText("?");
                button->setChecked(false);
                color = QColor(0, 0, 0);
                break;

            case GameController::Empty:
                button->setText("");
                button->setChecked(false);
                color = QColor(0, 0, 0);
                break;

            case GameController::Opened:
                button->setText(QString("%1").arg(cell.number));
                color = QColor(0, 0, 0);

                if (cell.number == 0)
                {
                    color = QColor(160, 160, 160);
                }
                else if (cell.number == 1)
                {
                    color = QColor(0, 0, 255);
                }
                else if (cell.number == 2)
                {
                    color = QColor(0, 255, 0);
                }
                else if (cell.number == 3)
                {
                    color = QColor(255, 0, 0);
                }
                else if (cell.number == 4)
                {
                    color = QColor(255, 0, 255);
                }
                else if (cell.number == 5)
                {
                    color = QColor(127, 0, 0);
                }
                else if (cell.number == 6)
                {
                    color = QColor(0, 127, 0);
                }
                else if (cell.number == 7)
                {
                    color = QColor(0, 0, 127);
                }
                else if (cell.number == 8)
                {
                    color = QColor(127, 0, 127);
                }

                button->setChecked(true);
                break;

            case GameController::Mine:
                if (showMines)
                {
                    button->setIcon(IconMine);
                }
                else
                {
                    button->setIcon(QIcon());
                }

                button->setText("");
                button->setChecked(false);
                color = QColor(0, 0, 0);
                break;
            }

            button->setStyleSheet(
                        QString(
                            "border-style: outset;"
                            "border-radius: 4px;"
                            "border-width: 2px;"
                            "border-color: silver;"
                            "background-color: %1;"
                            "font: bold;"
                            "color: %2;")
                        .arg(cell.type == GameController::Opened ? "#ffffff" : "#42a5f5")
                        .arg(color.name()));
        }
    }
}

void MainWindow::restartGame()
{
    showMines = false;
    if (game)
    {
        game->deleteLater();
    }

    game = new GameController(QSize(8, 8), 30);

    connect(game, &GameController::filedChanged, this, &MainWindow::onFieldChanged);
    connect(game, QOverload<GameController::ResultGame>::of(&GameController::gameEnd), this, [this](GameController::ResultGame result)
    {
        showMines = true;
        onFieldChanged();

        switch (result)
        {
        case GameController::Lose:
            QMessageBox::information(this, tr("Game end"), tr("You lose"));
            break;

        case GameController::Win:
            QMessageBox::information(this, tr("Game end"), tr("You win"));
            break;
        }

        restartGame();
    });

    cells.clear();
    clearLayout(ui->gameLayout);

    const QVector<QVector<GameController::Cell>> field = game->getField();

    for (int x = 0; x < field.count(); ++x)
    {
        const QVector<GameController::Cell> line = field[x];

        for (int y = 0; y < line.count(); ++y)
        {
            const GameController::Cell cell = line[y];

            QPushButton* button = new QPushButton(this);

            connect(button, &QPushButton::clicked, this, &MainWindow::onClickedCell);

            button->setCheckable(true);
            button->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
            button->setMinimumSize(40, 40);

            cells.insert(button, QPoint(x, y));

            ui->gameLayout->addWidget(button, x, y);
        }
    }

    onFieldChanged();
}

void MainWindow::clearLayout(QLayout *layout)
{
    if (!layout)
    {
        return;
    }

    QLayoutItem *item;
    while ((item = layout->takeAt(0)))
    {
        if (item->layout())
        {
            clearLayout(item->layout());
            delete item->layout();
        }

        if (item->widget())
        {
           delete item->widget();
        }

        delete item;
    }
}

void MainWindow::on_pushButtonNext_clicked()
{
    game->nextFloatStep();
}

void MainWindow::on_actionRestart_triggered()
{
    restartGame();
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("About"), tr("Author: https://github.com/3dproger"));
}

