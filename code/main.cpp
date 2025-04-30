#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"
#include <iostream>

using namespace sf;
using namespace std;

int main()

{
	int pixelWidth = (VideoMode::getDesktopMode().width);
	int pixelHeight = (VideoMode::getDesktopMode().height);
	VideoMode vm(pixelWidth, pixelHeight);
	RenderWindow window(vm, "Mandelbrot Set", Style::Default);

	Font font;
	Text text;
	font.loadFromFile("font/arial.ttf");
	text.setFont(font);
	text.setCharacterSize(18);
	text.setFillColor(Color::White);
	text.setPosition(5, 0);
	ComplexPlane comPl(pixelWidth, pixelHeight);

	while (window.isOpen())
	{
		//Input handling
		Event event;

		while (window.pollEvent(event)){
			if (event.type == Event::Closed) {
				window.close();
			}
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
				comPl.zoomIn();
				comPl.setCenter(Mouse::getPosition(window));
			}
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Right) {
				comPl.zoomout();
				comPl.setCenter(Mouse::getPosition(window));
			}
			if (event.type == Event::MouseMoved) {
				comPl.setMouseLocation(Mouse::getPosition(window));
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		///Update rendering
		comPl.updateRender();
		comPl.loadText(text);

		///Draw scene

		window.clear();
		window.draw(comPl);
		window.draw(text);
		window.display();

	}




}
