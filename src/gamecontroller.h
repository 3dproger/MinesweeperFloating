#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QSize>
#include <QVector>
#include <QPoint>
#include <QRandomGenerator>

class GameController : public QObject
{
    Q_OBJECT
public:
    enum CellType {
        Unknown,
        Empty,
        Opened,
        Mine
    };

    enum ResultGame {
        Lose,
        Win
    };

    struct Cell {
        CellType type = CellType::Unknown;
        int number = -1;
    };

    explicit GameController(const QSize& fieldSize, const int minesCount, QObject *parent = nullptr);
    QVector<QVector<Cell>> getField() const;
    QSize getFieldSize() const;
    void clickAtCell(const QPoint& point);
    void nextFloatStep();

signals:
    void filedChanged();
    void gameEnd(ResultGame);

private:
    void recursiveUpdateCell(const QPoint& point);
    void recalculateCells();
    Cell* getCell(int x, int y);
    QPoint getCoord(Cell* cell);
    void checkWin();
    void floatCells();

    QVector<QVector<Cell>> field;
    const int minesCount;
    bool minesPlaced = false;
    QRandomGenerator random;
};

#endif // GAMECONTROLLER_H
