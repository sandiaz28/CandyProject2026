#include <filesystem>
#include "board.h"
#include "candy.h"
#include "controller.h"
#include "game.h"
#include "util.h"

bool test()
{
    // Test board 2D container
    Candy c(CandyType::TYPE_ORANGE);
    Board b(10, 10);
    b.setCell(&c, 0, 0);
    if (b.getCell(0, 0) != &c)
    {
        return false;
    }

    // Dump and load board
    {
        Board b2(10, 10);
        if (!b.dump(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (!b2.load(getDataDirPath() + "dump_board.txt"))
        {
            return false;
        }
        if (b2.getCell(0, 0)->getType() != c.getType())
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_board.txt");
    }

    // Dump and load game -> lo dejamos comentado porque aun no se 
    // ha implementado
    /*
    {
        Game g;
        Controller cont;
        g.update(cont);
        if (!g.dump(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        Game g2;
        if (!g2.load(getDataDirPath() + "dump_game.txt"))
        {
            return false;
        }
        if (g != g2)
        {
            return false;
        }
        std::filesystem::remove(getDataDirPath() + "dump_game.txt");
    }
    */

    // Mis tests adicionales
    // TEST 1: constructor vacío
    {
        Board b3(10, 10);
        if (b3.getWidth() != 10 || b3.getHeight() != 10)
            return false;
        for (int x = 0; x < 10; x++)
            for (int y = 0; y < 10; y++)
                if (b3.getCell(x, y) != nullptr)
                    return false;
    }

    // TEST 2: getCell fuera de límites devuelve nullptr
    {
        Board b4(10, 10);
        if (b4.getCell(-1, 0) != nullptr) return false;
        if (b4.getCell(0, -1) != nullptr) return false;
        if (b4.getCell(10, 0) != nullptr) return false;
    }

    // TEST 3: shouldExplode línea horizontal
    {
        Board b5(10, 10);
        Candy c1(CandyType::TYPE_RED);
        Candy c2(CandyType::TYPE_RED);
        Candy c3(CandyType::TYPE_RED);
        b5.setCell(&c1, 0, 0);
        b5.setCell(&c2, 1, 0);
        b5.setCell(&c3, 2, 0);
        if (!b5.shouldExplode(1, 0)) return false;
    }

    // TEST 4: shouldExplode no explota con 2 iguales
    {
        Board b6(10, 10);
        Candy c1(CandyType::TYPE_RED);
        Candy c2(CandyType::TYPE_RED);
        b6.setCell(&c1, 0, 0);
        b6.setCell(&c2, 1, 0);
        if (b6.shouldExplode(0, 0)) return false;
    }

    // TEST 5: explodeAndDrop elimina piezas
    {
        Board b7(10, 10);
        Candy c1(CandyType::TYPE_GREEN);
        Candy c2(CandyType::TYPE_GREEN);
        Candy c3(CandyType::TYPE_GREEN);
        b7.setCell(&c1, 0, 0);
        b7.setCell(&c2, 1, 0);
        b7.setCell(&c3, 2, 0);
        std::vector<Candy*> exploded = b7.explodeAndDrop();
        if (exploded.size() != 3) return false;
        if (b7.getCell(0, 0) != nullptr) return false;
    }

    // TEST 6: load fichero inexistente devuelve false
    {
        Board b8(10, 10);
        if (b8.load("fichero_inexistente.txt") != false) return false;
    }

    // TEST 7: shouldExplode línea vertical
    {
        Board b9(10, 10);
        Candy c1(CandyType::TYPE_BLUE);
        Candy c2(CandyType::TYPE_BLUE);
        Candy c3(CandyType::TYPE_BLUE);
        b9.setCell(&c1, 0, 0);
        b9.setCell(&c2, 0, 1);
        b9.setCell(&c3, 0, 2);
        if (!b9.shouldExplode(0, 1)) return false;
    }

    // TEST 8: las piezas caen después de explotar
    {
        Board b10(10, 10);
        Candy c1(CandyType::TYPE_RED);
        Candy c2(CandyType::TYPE_RED);
        Candy c3(CandyType::TYPE_RED);
        Candy c4(CandyType::TYPE_BLUE);
        b10.setCell(&c1, 0, 7);
        b10.setCell(&c2, 1, 7);
        b10.setCell(&c3, 2, 7);
        b10.setCell(&c4, 0, 6);
        b10.explodeAndDrop();
        if (b10.getCell(0, 9) == nullptr) return false;
    }

    // TEST 9: dump con ruta inválida devuelve false
    {
        Board b11(10, 10);
        if (b11.dump("/ruta/que/no/existe/fichero.txt") != false) return false;
    }

    return true;
}
