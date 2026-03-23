#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>

Board::Board(int width, int height)
{
    m_width = width;
    m_height = height;

    for (int x = 0; x < DEFAULT_BOARD_WIDTH; x++)
    {
        for (int y = 0; y < DEFAULT_BOARD_HEIGHT; y++)
        {
            m_storage[x][y] = nullptr;
            m_board[x][y] = nullptr;
        }
    }
}

Board::~Board()
{
    for (int x = 0; x < DEFAULT_BOARD_WIDTH; x++)
    {
        for (int y = 0; y < DEFAULT_BOARD_HEIGHT; y++)
        {
            delete m_storage[x][y];
            m_storage[x][y] = nullptr;
            m_board[x][y] = nullptr;
        }
    }
}


Candy* Board::getCell(int x, int y) const
{
    if (x >= 0 && y >= 0 && x < m_width && y < m_height)
    {
        return m_board[x][y];
    }
    return nullptr;
}

void Board::setCell(Candy* candy, int x, int y)
{
    if (x >= 0 && y >= 0 && x < m_width && y < m_height)
    {
        m_board[x][y] = candy;
    }
}


int Board::getWidth() const
{
    return m_width;
}


int Board::getHeight() const
{
    return m_height;
}

int Board::countInDirection(int x, int y, int dx, int dy) const
{
    CandyType type = m_board[x][y]->getType();
    int count = 1; // contamos la pieza actual

    // avanzar en direccion (dx, dy)
    int nx = x + dx;
    int ny = y + dy;
    while (nx >= 0 && ny >= 0 && nx < m_width && ny < m_height
        && m_board[nx][ny] != nullptr
        && m_board[nx][ny]->getType() == type)
    {
        count++;
        nx += dx;
        ny += dy;
    }

    // avanzar en direccion contraria (-dx, -dy)
    nx = x - dx;
    ny = y - dy;
    while (nx >= 0 && ny >= 0 && nx < m_width && ny < m_height
        && m_board[nx][ny] != nullptr
        && m_board[nx][ny]->getType() == type)
    {
        count++;
        nx -= dx;
        ny -= dy;
    }

    return count;
}

bool Board::shouldExplode(int x, int y) const
{
    if (m_board[x][y] == nullptr)
    {
        return false;
    }

    // horizontal, vertical, diagonal \, diagonal /
    return countInDirection(x, y, 1, 0) >= SHORTEST_EXPLOSION_LINE
        || countInDirection(x, y, 0, 1) >= SHORTEST_EXPLOSION_LINE
        || countInDirection(x, y, 1, 1) >= SHORTEST_EXPLOSION_LINE
        || countInDirection(x, y, 1, -1) >= SHORTEST_EXPLOSION_LINE;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    std::vector<Candy*> result;
    bool explosionHappened = true;

    while (explosionHappened)
    {
        // FASE 1: marcar qué celdas explotan
        bool toExplode[DEFAULT_BOARD_WIDTH][DEFAULT_BOARD_HEIGHT];
        for (int x = 0; x < m_width; x++)
            for (int y = 0; y < m_height; y++)
                toExplode[x][y] = shouldExplode(x, y);

        // FASE 2: borrar las celdas marcadas
        explosionHappened = false;
        for (int x = 0; x < m_width; x++)
        {
            for (int y = 0; y < m_height; y++)
            {
                if (toExplode[x][y])
                {
                    result.push_back(m_board[x][y]);
                    m_board[x][y] = nullptr;
                    explosionHappened = true;
                }
            }
        }

        // FASE 3: hacer caer las piezas por columna
        for (int x = 0; x < m_width; x++)
        {
            int writeY = m_height - 1;
            for (int y = m_height - 1; y >= 0; y--)
            {
                if (m_board[x][y] != nullptr)
                {
                    m_board[x][writeY] = m_board[x][y];
                    if (writeY != y)
                        m_board[x][y] = nullptr;
                    writeY--;
                }
            }
        }
    }

    return result;
}

bool Board::dump(const std::string& output_path) const
{
    std::ofstream file(output_path);
    if (!file.is_open())
    {
        return false;
    }

    file << m_width << " " << m_height << "\n";

    for (int x = 0; x < m_width; x++)
    {
        for (int y = 0; y < m_height; y++)
        {
            if (m_board[x][y] != nullptr)
            {
                file << x << " " << y << " " << static_cast<int>(m_board[x][y]->getType()) << "\n";
            }
        }
    }

    return true;
}

bool Board::load(const std::string& input_path)
{
    std::ifstream file(input_path);
    if (!file.is_open())
    {
        return false;
    }

    // limpiar el tablero actual
    for (int x = 0; x < DEFAULT_BOARD_WIDTH; x++)
    {
        for (int y = 0; y < DEFAULT_BOARD_HEIGHT; y++)
        {
            delete m_storage[x][y];
            m_storage[x][y] = nullptr;
            m_board[x][y] = nullptr;
        }
    }

    int width, height;
    file >> width >> height;
    m_width = width;
    m_height = height;

    int x, y, type;
    while (file >> x >> y >> type)
    {
        m_storage[x][y] = new Candy(static_cast<CandyType>(type));
        m_board[x][y] = m_storage[x][y];
    }

    return true;
}
