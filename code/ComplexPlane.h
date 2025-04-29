
#ifndef COMPLEXPLANE_H
#define COMPLEXPLANE_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


const unsigned int MAX_ITER = 64;
const float BASE_WIDTH = 4.0;
const float BASE_HEIGHT = 4.0;
const float BASE_ZOOM = 0.5;

using namespace std;

enum class State
{
	CALCULATING,
	DISPLAYING
};

class ComplexPlane : public sf::Drawable
{
public:
	ComplexPlane(int pixelWidth, int pixelHeight);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void zoomIn();
	void zoomout();
	void setCenter(sf::Vector2i mousePixel);
	void setMouseLocation(sf::Vector2i mousePixel);
	void loadText(sf::Text& text);
	void updateRender();
private:
	size_t countIterations(sf::Vector2f coord);
	void iterationsToRGB(size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b);
	sf::Vector2f mapPixelToCoords(sf::Vector2i mousePixel);
	sf::VertexArray vArray;
	State state;
	sf::Vector2f mouseLocation;
	sf::Vector2i pixel_size;
	sf::Vector2f plane_center;
	sf::Vector2f plane_size;
	int zoomCount;
	float aspectRatio;

};

#endif
