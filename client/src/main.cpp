#include "raylib.h"
#include "ui/screens.hpp"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Estado global accesible desde el loop
static chrono_clash::ui::MainMenuScreen mainMenu;

void GameLoop()
{
    mainMenu.update();

    BeginDrawing();
    mainMenu.draw();
    EndDrawing();
}

int main()
{
    // Inicializar ventana
    InitWindow(1280, 720, "ChronoClash");
    InitAudioDevice();

    SetTargetFPS(60);

    // Inicializar menú principal
    mainMenu.init();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(GameLoop, 0, 1);
#else
    while (!WindowShouldClose()) {
        GameLoop();
    }
#endif

    // Liberar recursos
    mainMenu.unload();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
