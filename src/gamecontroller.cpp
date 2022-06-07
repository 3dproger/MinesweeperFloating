#include "gamecontroller.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QDateTime>

GameController::GameController(const QSize& fieldSize, const int minesCount_, QObject *parent)
    : QObject{parent}
    , minesCount(minesCount_)
{
    random = QRandomGenerator(QDateTime::currentMSecsSinceEpoch());

    field = QVector<QVector<Cell>>();

    for (int x = 0; x < fieldSize.width(); ++x)
    {
        QVector<Cell> line;

        for (int y = 0; y < fieldSize.height(); ++y)
        {
            Cell cell;
            cell.type = CellType::Empty;

            line.append(cell);
        }

        field.append(line);
    }
}

QVector<QVector<GameController::Cell> > GameController::getField() const
{
    return field;
}

QSize GameController::getFieldSize() const
{
    const int width = field.count();

    if (width == 0)
    {
        return QSize(0, 0);
    }

    return QSize(width, field[0].count());
}

void GameController::clickAtCell(const QPoint &point)
{
    if (point.x() >= field.count())
    {
        qCritical() << "point.x >= field.width";
        return;
    }

    QVector<GameController::Cell>& line = field[point.x()];

    if (point.y() >= line.count())
    {
        qCritical() << "point.y >= field.height";
        return;
    }

    GameController::Cell& cell = line[point.y()];

    switch (cell.type)
    {
    case GameController::Unknown:
        qCritical() << "cell is unknown";
        return;

    case GameController::Empty:
        cell.type = GameController::CellType::Opened;
        break;

    case GameController::Opened:
        qCritical() << "cell already is opened";
        return;

    case GameController::Mine:
        emit gameEnd(ResultGame::Lose);
        return;
    }

    if (!minesPlaced)
    {
        int minesAdded = 0;
        while (minesAdded < minesCount)
        {
            const int x = random.generate() % getFieldSize().width();
            const int y = random.generate() % getFieldSize().height();

            if (field[x][y].type == GameController::CellType::Empty)
            {
                field[x][y].type = GameController::CellType::Mine;
            }
            else
            {
                continue;
            }

            minesAdded++;
        }

        minesPlaced = true;
    }

    recursiveUpdateCell(point);

    floatCells();

    recalculateCells();

    emit filedChanged();

    checkWin();
}

void GameController::nextFloatStep()
{
    floatCells();

    recalculateCells();

    emit filedChanged();

    checkWin();
}

void GameController::recursiveUpdateCell(const QPoint& point)
{
    GameController::Cell* cell;

    cell = getCell(point.x()    , point.y() - 1);
    if (cell && cell->type == GameController::CellType::Empty)
    {
        cell->type = GameController::CellType::Opened;
        recursiveUpdateCell(QPoint(point.x()    , point.y() - 1));
    }

    cell = getCell(point.x() + 1, point.y()    );
    if (cell && cell->type == GameController::CellType::Empty)
    {
        cell->type = GameController::CellType::Opened;
        recursiveUpdateCell(QPoint(point.x() + 1, point.y()    ));
    }

    cell = getCell(point.x()    , point.y() + 1);
    if (cell && cell->type == GameController::CellType::Empty)
    {
        cell->type = GameController::CellType::Opened;
        recursiveUpdateCell(QPoint(point.x()    , point.y() + 1));
    }

    cell = getCell(point.x() - 1, point.y()    );
    if (cell && cell->type == GameController::CellType::Empty)
    {
        cell->type = GameController::CellType::Opened;
        recursiveUpdateCell(QPoint(point.x() - 1, point.y()    ));
    }
}

void GameController::recalculateCells()
{
    for (int x = 0; x < field.count(); ++x)
    {
        QVector<GameController::Cell>& line = field[x];

        for (int y = 0; y < line.count(); ++y)
        {
            GameController::Cell& cell = line[y];

            cell.number = 0;

            GameController::Cell* other;

            other = getCell(x - 1, y - 1);
            cell.number += other && other->type == GameController::CellType::Mine;

            other = getCell(x    , y - 1);
            cell.number += other && other->type == GameController::CellType::Mine;

            other = getCell(x + 1, y - 1);
            cell.number += other && other->type == GameController::CellType::Mine;

            other = getCell(x + 1, y    );
            cell.number += other && other->type == GameController::CellType::Mine;

            other = getCell(x + 1, y + 1);
            cell.number += other && other->type == GameController::CellType::Mine;

            other = getCell(x    , y + 1);
            cell.number += other && other->type == GameController::CellType::Mine;

            other = getCell(x - 1, y + 1);
            cell.number += other && other->type == GameController::CellType::Mine;

            other = getCell(x - 1, y    );
            cell.number += other && other->type == GameController::CellType::Mine;
        }
    }
}

GameController::Cell* GameController::getCell(int x, int y)
{
    if (x >= field.count() || x < 0)
    {
        return nullptr;
    }

    if (y >= field[x].count() || y < 0)
    {
        return nullptr;
    }

    return &field[x][y];
}

void GameController::checkWin()
{
    int opened = 0;
    int fieldsCount = 0;

    for (int x = 0; x < field.count(); ++x)
    {
        const QVector<GameController::Cell>& line = field[x];

        for (int y = 0; y < line.count(); ++y)
        {
            const GameController::Cell& cell = line[y];
            if (cell.type == CellType::Opened)
            {
                opened++;
            }

            fieldsCount++;
        }
    }

    if (opened + minesCount >= fieldsCount)
    {
        emit gameEnd(ResultGame::Win);
    }
}

void GameController::floatCells()
{
    for (int x = 0; x < field.count(); ++x)
    {
        QVector<GameController::Cell>& line = field[x];

        for (int y = 0; y < line.count(); ++y)
        {
            GameController::Cell& cell = line[y];

            for (int i = 0; i < 8; ++i)
            {
                if (cell.type == GameController::CellType::Mine)
                {
                    const int dir = random.generate() % 4;

                    if (dir == 0)
                    {
                        Cell* other = getCell(x, y - 1);
                        if (other && other->type == CellType::Empty)
                        {
                            other->type = CellType::Mine;
                            other->number = 0;
                            cell.type = CellType::Empty;
                            cell.number = 0;
                            break;
                        }
                    }
                    else if (dir == 1)
                    {
                        Cell* other = getCell(x + 1, y);
                        if (other && other->type == CellType::Empty)
                        {
                            other->type = CellType::Mine;
                            other->number = 0;
                            cell.type = CellType::Empty;
                            cell.number = 0;
                            break;
                        }
                    }
                    else if (dir == 2)
                    {
                        Cell* other = getCell(x, y + 1);
                        if (other && other->type == CellType::Empty)
                        {
                            other->type = CellType::Mine;
                            other->number = 0;
                            cell.type = CellType::Empty;
                            cell.number = 0;
                            break;
                        }
                    }
                    else if (dir == 3)
                    {
                        Cell* other = getCell(x - 1, y);
                        if (other && other->type == CellType::Empty)
                        {
                            other->type = CellType::Mine;
                            other->number = 0;
                            cell.type = CellType::Empty;
                            cell.number = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
}
