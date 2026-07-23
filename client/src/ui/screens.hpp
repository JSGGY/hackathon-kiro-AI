#pragma once

#include "raylib.h"

namespace chrono_clash::ui {

/// Estado de un botón del menú
struct MenuButton {
    Rectangle bounds;
    const char* text;
    bool isHovered;
    bool isPressed;
};

/// Pantalla del menú principal
class MainMenuScreen {
public:
    MainMenuScreen();
    ~MainMenuScreen();

    /// Inicializa recursos (fuente, sonidos, música)
    void init();

    /// Libera recursos cargados
    void unload();

    /// Actualiza lógica del menú (hover, clicks, música)
    void update();

    /// Dibuja el menú completo
    void draw();

    /// Indica si se presionó "Buscar Partida"
    bool isFindMatchPressed() const;

    /// Indica si se presionó "Cómo Jugar"
    bool isHowToPlayPressed() const;

private:
    Font font_;
    Music menuMusic_;
    Sound buttonSelectSfx_;
    Sound buttonPressedSfx_;

    MenuButton findMatchBtn_;
    MenuButton howToPlayBtn_;

    bool findMatchTriggered_;
    bool howToPlayTriggered_;

    bool initialized_;

    void updateButton(MenuButton& btn);
    void drawButton(const MenuButton& btn);
};

} // namespace chrono_clash::ui
