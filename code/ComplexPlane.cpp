#include "ComplexPlane.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <complex>

using namespace std;
using namespace sf;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
	pixel_size = Vector2i(pixelWidth, pixelHeight);
	aspectRatio = (float)pixelWidth / (float)pixelHeight; // cast width and height
	plane_center = Vector2f(0, 0);
	plane_size = Vector2f(BASE_WIDTH, BASE_HEIGHT * aspectRatio); //Complex plane size
	zoomCount = 0;
	state = State::CALCULATING; // initializing m_state to 0 the fancy way
	vArray.setPrimitiveType(Points);
	vArray.resize(pixelWidth * pixelHeight);


}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(vArray);
}

void ComplexPlane::zoomIn()
{
	zoomCount++;
	float xSize = BASE_WIDTH * (pow(BASE_ZOOM, zoomCount));
	float ySize = BASE_HEIGHT * aspectRatio * (pow(BASE_ZOOM, zoomCount));
	plane_size = (Vector2f(xSize, ySize));
	state = State::CALCULATING;
}

void ComplexPlane::zoomout()
{
	zoomCount--;
	float xSize = BASE_WIDTH * (pow(BASE_ZOOM, zoomCount));
	float ySize = BASE_HEIGHT * aspectRatio * (pow(BASE_ZOOM, zoomCount));
	plane_size = (Vector2f(xSize, ySize));
	state = State::CALCULATING;
}


void ComplexPlane::setCenter(Vector2i mousePixel)
{
	Vector2f newCenter = mapPixelToCoords(mousePixel); // FINISH
	plane_center = newCenter;
	state = State::CALCULATING;

}


void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	Vector2f coord = mapPixelToCoords(mousePixel);
	mouseLocation = coord;

}
//FINISH THIS FUNCTION
void ComplexPlane::loadText(sf::Text& text)
{
	ostringstream oss;

}

void ComplexPlane::updateRender()
{
	if (state == State::CALCULATING) // checking if 0 using a class
	{
		for (int j = 0; j < pixel_size.x; j++)
		{
			for (int i = 0; i < pixel_size.y; i++)
			{
				// set pos of VertexArray that corresponds to the screen coord j,i
				vArray[j + i * pixel_size.x].position = { (float)j, (float)i }; // note m_pixel_size.x is width
				Vector2f coord = mapPixelToCoords(Vector2i(j, i));
				int Iterations = countIterations(coord);
				Uint8 r = 0, g = 0, b = 0; //start with black and changes color depending on pixel(and iteration)
				iterationsToRGB(Iterations, r, g, b);
				vArray[j + i * pixel_size.x].color = { r, g, b }; // sets the color variable to corospond to the screen coordinate 

			}
		}
		state = State::DISPLAYING;
	}
}

size_t ComplexPlane::countIterations(Vector2f coord)
{
	size_t iteration = 0;
	float current, twoab, a2, b2, iterfunc;
	float cReal_a = coord.x; // following c = a + bi, where a is real and b is imaginary
	float cImag_b = coord.y;
	float a = 0.0f, b = 0.0f; // z = 0 + 0i, we start at 0 therefore we set a and b to 0

	while (iteration < MAX_ITER)
	{
		// z = (a^2 + b^2) +2abi + a_0 + b_0i
		a2 = a * a;
		b2 =b * b;
		if (a2 + b2 >= 4.0f)
		{
			break;
		}
		twoab = 2.0f * a * b;
		float new_a = a2 - b2 + cReal_a;
		float new_b = twoab + cImag_b;
		a = new_a;
		b = new_b;
		iteration++;
	}
	return iteration;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
	if (count == MAX_ITER)
	{
		r = g = b = 0;
	}
	else
	{
		r = g = b = 255;
	}
}

sf::Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
	float x = (float)mousePixel.x / pixel_size.x;
	float y = (float)mousePixel.y / pixel_size.y;

	float xCoord = plane_center.x + (x - 0.5f) * plane_size.x;
	float yCoord = plane_center.y + (y - 0.5f) * plane_size.y;

	Vector2f mappingCoords(xCoord, yCoord);
	return mappingCoords;
}
