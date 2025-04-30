#include "ComplexPlane.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <complex>
#include <thread>

using namespace std;
using namespace sf;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
	pixel_size = Vector2i(pixelWidth, pixelHeight);
	aspectRatio = (float)pixelHeight / (float)pixelWidth; // cast width and height
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
	Vector2f newCenter = mapPixelToCoords(mousePixel); //sets a new center point for the view based on where the user clicked
	plane_center = newCenter;
	state = State::CALCULATING; // changing state

}


void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	Vector2f coord = mapPixelToCoords(mousePixel); // converts the mouse location to complex coords to store it for real time curor feedback
	mouseLocation = coord;

}

void ComplexPlane::loadText(sf::Text& text)
{
	ostringstream oss;
	oss << "MandelbrotSet" << endl;
	oss << "Cursor's Position: (" << mouseLocation.x << ", " << mouseLocation.y << ")" << endl;
	oss << "Center: (" << plane_center.x << ", " << plane_center.y << ")" << endl; // displays text window
	oss << "Left-Click to Zoom in" << endl;
	oss << "Right-Click to Zoom out" << endl;

	text.setString(oss.str()); // sets the text from the stringstream
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
	float twoab, a2, b2;
	float cReal_a = coord.x; // following c = a + bi, where a is real and b is imaginary
	float cImag_b = coord.y;
	float a = 0.0f, b = 0.0f; // z = 0 + 0i, we start at 0 therefore we set a and b to 0

	while (iteration < MAX_ITER)
	{
		// z = (a^2 + b^2) +2abi + a_0 + b_0i, a^2 + b^2 is a, 2abi is b and the rest is c, expanded z = z^2 + c
		// can also be z = (a^2 + b^2) + cReal_a + 2abi + cImag_b
		//paired the real and imaginary together
		// calculate each part of the equation above
		a2 = a * a;  
		b2 =b * b;
		if (a2 + b2 >= 4.0f) // z^2 = sqrt( a2 + b2) >= 2.0, sqaured both sides to avoud sqrt 
		{
			break;
		}
		twoab = 2.0f * a * b;   
		float new_a = a2 - b2 + cReal_a;  
		float new_b = twoab + cImag_b;
		a = new_a;  // overrides the a and b values for next iteration
		b = new_b;
		iteration++;
	}
	return iteration;
}


void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
	//splits the iter amounts into groups of 16 and creates a color difference per number iter
	if (count == MAX_ITER)
	{
		r = g = b = 0;
	}
	if (count < MAX_ITER) {
		r = 0;
		g = ((MAX_ITER - count) * 16) - 1;
		b = 255;
	}
	if (count < 48) {
		r = 0;
		g = 255;
		b = 255-((48-count)*16)-1;
	}
	if (count < 32) {
		r = ((32-count)*16)-1;
		g = 255;
		b = 0;
	}
	if (count < 16) {
		r = 255;
		g = 255-((16-count)*16-1);
		b = 0;
	}
}

sf::Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel) // maps pixel coordinate to a coord in the complex plane
{
	float x = (float)mousePixel.x / pixel_size.x;    // makes it so that the mouse positions range from 0 to 1 for simplicity
	float y = (float)mousePixel.y / pixel_size.y;

	float xCoord = plane_center.x + (x - 0.5f) * plane_size.x;  //new range becomes [-0.5, 0.5] so 0.5 is to offset it and make sure its on the correct side of the origin
	float yCoord = plane_center.y + (y - 0.5f) * plane_size.y;  //multiply by plane_size to make it complex coord and shift if by plane center

	Vector2f mappingCoords(xCoord, yCoord); // creates vector2f to map and store the coords
	return mappingCoords;
}
