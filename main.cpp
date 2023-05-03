#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <Dwmapi.h>
#include <utility>
#include <vector>

#pragma comment (lib, "Dwmapi.lib")

sf::Font buttonFont;

class DisplayButton {
private:
    sf::RectangleShape rect;
    sf::Text text;
    const static int pressed = 120, released = 200, opacity = 128;
    bool buttonPressed = false;
    std::vector<sf::Keyboard::Key> triggers;

public:
    DisplayButton(sf::Vector2i position, sf::Vector2i dimensions, const std::string& text, std::vector<sf::Keyboard::Key> keys) {
        rect = sf::RectangleShape(sf::Vector2f(dimensions));
        rect.setPosition(position.x, position.y);
        rect.setFillColor(sf::Color(released, released, released, opacity));
        int characterSize = text.size() == 1 ? dimensions.y / 2 : dimensions.y / 4;
        int magicOffset = text.size() == 1 ? 0 : -20;
        this->text = sf::Text(text, buttonFont, characterSize);
        this->text.setPosition(position.x + (dimensions.x - text.size() / 3 * 2) / 2 + magicOffset,
                               position.y + dimensions.y / 3);
        this->text.setFillColor(sf::Color::Black);
        triggers = std::move(keys);
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
        rect.setFillColor(sf::Color(pressed, pressed, pressed, opacity));
        buttonPressed = true;
    }

    void setReleased() noexcept {
        rect.setFillColor(sf::Color(released, released, released, opacity));
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

    DisplayButton upButton = DisplayButton(sf::Vector2i(110, 10), sf::Vector2i(80, 80), "^", {sf::Keyboard::Up});
    DisplayButton leftButton = DisplayButton(sf::Vector2i(10, 110), sf::Vector2i(80, 80), "<", {sf::Keyboard::Left});
    DisplayButton downButton = DisplayButton(sf::Vector2i(110, 110), sf::Vector2i(80, 80), "v", {sf::Keyboard::Down});
    DisplayButton rightButton = DisplayButton(sf::Vector2i(210, 110), sf::Vector2i(80, 80), ">", {sf::Keyboard::Right});
    DisplayButton climbButton = DisplayButton(sf::Vector2i(10, 210), sf::Vector2i(80, 80), "climb", {sf::Keyboard::LShift, sf::Keyboard::Z, sf::Keyboard::V});
    DisplayButton dashButton = DisplayButton(sf::Vector2i(110, 210), sf::Vector2i(80, 80), "dash", {sf::Keyboard::X});
    DisplayButton jumpButton = DisplayButton(sf::Vector2i(210, 210), sf::Vector2i(80, 80), "jump", {sf::Keyboard::C});

    std::vector<DisplayButton *> buttons = {&upButton, &leftButton, &downButton, &rightButton, &climbButton,
                                            &dashButton, &jumpButton};

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
