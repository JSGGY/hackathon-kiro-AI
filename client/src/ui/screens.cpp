#include "screens.hpp"
#include <cstring>

namespace chrono_clash::ui {

// Colores del tema
static const Color COLOR_BG = {15, 15, 25, 255};
static const Color COLOR_TITLE = {0, 220, 255, 255};
static const Color COLOR_SUBTITLE = {200, 200, 220, 255};
static const Color COLOR_BTN_NORMAL = {30, 30, 50, 255};
static const Color COLOR_BTN_HOVER = {50, 50, 80, 255};
static const Color COLOR_BTN_PRESSED = {20, 20, 40, 255};
static const Color COLOR_BTN_BORDER = {0, 180, 220, 255};
static const Color COLOR_BTN_TEXT = {240, 240, 255, 255};

static constexpr int FONT_SIZE_TITLE = 64;
static constexpr int FONT_SIZE_SUBTITLE = 18;
static constexpr int FONT_SIZE_BUTTON = 28;
static constexpr float BUTTON_WIDTH = 300.0f;
static constexpr float BUTTON_HEIGHT = 60.0f;
static constexpr float BUTTON_SPACING = 20.0f;
static constexpr float MENU_LEFT_MARGIN_RATIO = 0.08f;

MainMenuScreen::MainMenuScreen()
    : font_{}
    , menuMusic_{}
    , buttonSelectSfx_{}
    , buttonPressedSfx_{}
    , findMatchBtn_{}
    , howToPlayBtn_{}
    , findMatchTriggered_(false)
    , howToPlayTriggered_(false)
    , initialized_(false)
{
}

MainMenuScreen::~MainMenuScreen()
{
    if (initialized_) {
        unload();
    }
}

void MainMenuScreen::init()
{
    // Cargar fuente personalizada
    font_ = LoadFontEx("assets/fonts/good-timing-bd.otf", 64, nullptr, 0);
    SetTextureFilter(font_.texture, TEXTURE_FILTER_BILINEAR);

    // Cargar audio
    menuMusic_ = LoadMusicStream("assets/sounds/menu_music.mp3");
    buttonSelectSfx_ = LoadSound("assets/sounds/button_select.wav");
    buttonPressedSfx_ = LoadSound("assets/sounds/button_pressed.wav");

    // Configurar música en loop
    menuMusic_.looping = true;
    PlayMusicStream(menuMusic_);

    // Inicializar botones (posiciones se calculan en update/draw según resolución)
    findMatchBtn_.text = "Buscar Partida";
    findMatchBtn_.isHovered = false;
    findMatchBtn_.isPressed = false;

    howToPlayBtn_.text = "Como Jugar";
    howToPlayBtn_.isHovered = false;
    howToPlayBtn_.isPressed = false;

    findMatchTriggered_ = false;
    howToPlayTriggered_ = false;
    initialized_ = true;
}

void MainMenuScreen::unload()
{
    if (!initialized_) return;

    StopMusicStream(menuMusic_);
    UnloadMusicStream(menuMusic_);
    UnloadSound(buttonSelectSfx_);
    UnloadSound(buttonPressedSfx_);
    UnloadFont(font_);

    initialized_ = false;
}

void MainMenuScreen::update()
{
    if (!initialized_) return;

    // Actualizar stream de música
    UpdateMusicStream(menuMusic_);

    // Resetear triggers cada frame
    findMatchTriggered_ = false;
    howToPlayTriggered_ = false;

    // Calcular posiciones relativas al tamaño de ventana
    float screenW = static_cast<float>(GetScreenWidth());
    float screenH = static_cast<float>(GetScreenHeight());
    float menuX = screenW * MENU_LEFT_MARGIN_RATIO;
    float buttonsY = screenH * 0.55f;

    findMatchBtn_.bounds = {menuX, buttonsY, BUTTON_WIDTH, BUTTON_HEIGHT};
    howToPlayBtn_.bounds = {menuX, buttonsY + BUTTON_HEIGHT + BUTTON_SPACING, BUTTON_WIDTH, BUTTON_HEIGHT};

    // Guardar estado previo de hover para detectar transiciones
    bool prevHoverFind = findMatchBtn_.isHovered;
    bool prevHoverHow = howToPlayBtn_.isHovered;

    updateButton(findMatchBtn_);
    updateButton(howToPlayBtn_);

    // Sonido de selección (hover enter)
    if (findMatchBtn_.isHovered && !prevHoverFind) {
        PlaySound(buttonSelectSfx_);
    }
    if (howToPlayBtn_.isHovered && !prevHoverHow) {
        PlaySound(buttonSelectSfx_);
    }

    // Sonido de presionar y trigger de acción
    if (findMatchBtn_.isPressed) {
        PlaySound(buttonPressedSfx_);
        findMatchTriggered_ = true;
    }
    if (howToPlayBtn_.isPressed) {
        PlaySound(buttonPressedSfx_);
        howToPlayTriggered_ = true;
    }
}

void MainMenuScreen::draw()
{
    if (!initialized_) return;

    float screenW = static_cast<float>(GetScreenWidth());
    float menuX = screenW * MENU_LEFT_MARGIN_RATIO;

    ClearBackground(COLOR_BG);

    // Subtítulo en la parte superior
    const char* subtitle = "Hackathon Kiro by Codigo Facilito y AWS";
    Vector2 subtitleSize = MeasureTextEx(font_, subtitle, FONT_SIZE_SUBTITLE, 1);
    float subtitleX = (screenW - subtitleSize.x) / 2.0f;
    DrawTextEx(font_, subtitle, {subtitleX, 15.0f}, FONT_SIZE_SUBTITLE, 1, COLOR_SUBTITLE);

    // Título del juego
    const char* title = "ChronoClash";
    DrawTextEx(font_, title, {menuX, 100.0f}, FONT_SIZE_TITLE, 2, COLOR_TITLE);

    // Botones
    drawButton(findMatchBtn_);
    drawButton(howToPlayBtn_);
}

bool MainMenuScreen::isFindMatchPressed() const
{
    return findMatchTriggered_;
}

bool MainMenuScreen::isHowToPlayPressed() const
{
    return howToPlayTriggered_;
}

void MainMenuScreen::updateButton(MenuButton& btn)
{
    Vector2 mouse = GetMousePosition();
    btn.isHovered = CheckCollisionPointRec(mouse, btn.bounds);
    btn.isPressed = btn.isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

void MainMenuScreen::drawButton(const MenuButton& btn)
{
    Color bgColor = COLOR_BTN_NORMAL;
    if (btn.isPressed) {
        bgColor = COLOR_BTN_PRESSED;
    } else if (btn.isHovered) {
        bgColor = COLOR_BTN_HOVER;
    }

    // Fondo del botón con bordes redondeados
    DrawRectangleRounded(btn.bounds, 0.3f, 8, bgColor);
    DrawRectangleRoundedLinesEx(btn.bounds, 0.3f, 8, 2.0f, COLOR_BTN_BORDER);

    // Texto centrado en el botón
    Vector2 textSize = MeasureTextEx(font_, btn.text, FONT_SIZE_BUTTON, 1);
    float textX = btn.bounds.x + (btn.bounds.width - textSize.x) / 2.0f;
    float textY = btn.bounds.y + (btn.bounds.height - textSize.y) / 2.0f;
    DrawTextEx(font_, btn.text, {textX, textY}, FONT_SIZE_BUTTON, 1, COLOR_BTN_TEXT);
}

} // namespace chrono_clash::ui
