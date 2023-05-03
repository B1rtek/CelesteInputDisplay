#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <Dwmapi.h>
#include <utility>
#include <vector>
#include <fstream>

#pragma comment (lib, "Dwmapi.lib")

class Settings {
private:
    sf::Color pressed, released;
    int alpha;

public:
    Settings(const std::string& path = "settings.txt") {
        struct stat buffer{};
        if (stat (path.c_str(), &buffer) == 0) { // file exists
            std::fstream settingsFile;
            settingsFile.open(path, std::ios::in);
            int r, g, b;
            std::string settingName;
            settingsFile >> settingName >> r >> g >> b;
            released = sf::Color(r, g, b);
            settingsFile >> settingName >> r >> g >> b;
            pressed = sf::Color(r, g, b);
            settingsFile >> settingName >> alpha;
            settingsFile.close();
        } else {
            pressed = sf::Color(120, 120, 120);
            released = sf::Color(200, 200, 200);
            alpha = 128;
            std::fstream settingsFile;
            settingsFile.open(path, std::ios::out);
            settingsFile << "Released: 200 200 200\nPressed: 120 120 120\nOpacity: 128";
            settingsFile.close();
        }
    }

    sf::Color getPressedColor() const noexcept {
        return sf::Color(pressed.r, pressed.g, pressed.b, alpha);
    }

    sf::Color getReleasedColor() const noexcept {
        return sf::Color(released.r, released.g, released.b, alpha);
    }
};

sf::Font buttonFont;

class DisplayButton {
private:
    sf::RectangleShape rect;
    sf::Text text;
    const static int pressed = 120, released = 200, opacity = 128;
    bool buttonPressed = false;
    std::vector<sf::Keyboard::Key> triggers;
    Settings *settings;

public:
    DisplayButton(sf::Vector2i position, sf::Vector2i dimensions, const std::string& text, std::vector<sf::Keyboard::Key> keys, Settings& settings) {
        rect = sf::RectangleShape(sf::Vector2f(dimensions));
        rect.setPosition(position.x, position.y);
        rect.setFillColor(settings.getReleasedColor());
        int characterSize = text.size() == 1 ? dimensions.y / 2 : dimensions.y / 4;
        int magicOffset = text.size() == 1 ? 0 : -20;
        this->text = sf::Text(text, buttonFont, characterSize);
        this->text.setPosition(position.x + (dimensions.x - text.size() / 3 * 2) / 2 + magicOffset,
                               position.y + dimensions.y / 3);
        this->text.setFillColor(sf::Color::Black);
        triggers = std::move(keys);
        this->settings = &settings;
    }

    void checkTriggers() {
        bool triggered = false;
        for(sf::Keyboard::Key trigger: triggers) {
            triggered = triggered | sf::Keyboard::isKeyPressed(trigger);
        }
        if(triggered != buttonPressed) {
            buttonPressed = triggered;
            if(triggered) {
                setPressed();
            } else {
                setReleased();
            }
        }
    }

    void setPressed() noexcept {
        rect.setFillColor(settings->getPressedColor());
        buttonPressed = true;
    }

    void setReleased() noexcept {
        rect.setFillColor(settings->getReleasedColor());
        buttonPressed = false;
    }

    void draw(sf::RenderWindow &window) {
        window.draw(rect);
        window.draw(text);
    }

    bool isPressed() const noexcept {
        return buttonPressed;
    }
};

void makeWindowOnTop(sf::RenderWindow &window) {
    HWND hwnd = window.getSystemHandle();
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

int main() {

    sf::RenderWindow window(sf::VideoMode(300, 300), "Celeste input display");
    window.setFramerateLimit(144);
    window.setKeyRepeatEnabled(false);

    MARGINS margins;
    margins.cxLeftWidth = -1;

    SetWindowLong(window.getSystemHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);
    DwmExtendFrameIntoClientArea(window.getSystemHandle(), &margins);

    makeWindowOnTop(window);

    buttonFont.loadFromFile("arial.ttf");
    Settings settings = Settings();

    DisplayButton upButton = DisplayButton(sf::Vector2i(110, 10), sf::Vector2i(80, 80), "^", {sf::Keyboard::Up}, settings);
    DisplayButton leftButton = DisplayButton(sf::Vector2i(10, 110), sf::Vector2i(80, 80), "<", {sf::Keyboard::Left}, settings);
    DisplayButton downButton = DisplayButton(sf::Vector2i(110, 110), sf::Vector2i(80, 80), "v", {sf::Keyboard::Down}, settings);
    DisplayButton rightButton = DisplayButton(sf::Vector2i(210, 110), sf::Vector2i(80, 80), ">", {sf::Keyboard::Right}, settings);
    DisplayButton climbButton = DisplayButton(sf::Vector2i(10, 210), sf::Vector2i(80, 80), "climb", {sf::Keyboard::LShift, sf::Keyboard::Z, sf::Keyboard::V}, settings);
    DisplayButton dashButton = DisplayButton(sf::Vector2i(110, 210), sf::Vector2i(80, 80), "dash", {sf::Keyboard::X}, settings);
    DisplayButton jumpButton = DisplayButton(sf::Vector2i(210, 210), sf::Vector2i(80, 80), "jump", {sf::Keyboard::C}, settings);
    DisplayButton escButton = DisplayButton(sf::Vector2i(10, 10), sf::Vector2i(80, 80), "Esc", {sf::Keyboard::Escape}, settings);

    std::vector<DisplayButton *> buttons = {&upButton, &leftButton, &downButton, &rightButton, &climbButton,
                                            &dashButton, &jumpButton, &escButton};

    sf::Vector2i grabbedOffset;
    bool grabbedWindow = false;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    grabbedOffset = window.getPosition() - sf::Mouse::getPosition();
                    grabbedWindow = true;
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left)
                    grabbedWindow = false;
            } else if (event.type == sf::Event::MouseMoved) {
                if (grabbedWindow)
                    window.setPosition(sf::Mouse::getPosition() + grabbedOffset);
            }
        }

        for(DisplayButton* button: buttons) {
            button->checkTriggers();
        }

        window.clear(sf::Color::Transparent);
        for (DisplayButton *button: buttons) {
            button->draw(window);
        }
        window.display();
    }
    return 0;
}
