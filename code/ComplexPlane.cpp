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
	Vector2f newCenter = mapPixelToCoords(mousePixel); 
	plane_center = newCenter;
	state = State::CALCULATING;

}


void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	Vector2f coord = mapPixelToCoords(mousePixel);
	mouseLocation = coord;

}

void ComplexPlane::loadText(sf::Text& text)
{
	ostringstream oss;
	oss << "MandelbrotSet" << endl;
	oss << "Cursos Position: (" << mouseLocation.x << ", " << mouseLocation.y << ")" << endl;
	oss << "Center: (" << plane_center.x << ", " << plane_center.y << ")" << endl;
	oss << "Left-Click to Zoom in" << endl;
	oss << "Right-Click to Zoom out" << endl;

	text.setString(oss.str());
}
void ComplexPlane::updateRender()
{
	if (state != State::CALCULATING) // checking for the correct state
		return;

	const int threadCount = thread::hardware_concurrency(); // getting avalible cores for max threading power
	vector<thread> threads; // createes a contatiner for all the threads
	auto renderSlice = [this](int startY, int endY)
		{
			for (int y = startY; y < endY; ++y)
			{
				for (int x = 0; x < pixel_size.x; ++x)
				{
					Vector2f coord = mapPixelToCoords(Vector2i(x, y));
					int iterations = countIterations(coord);   

					Uint8 r, g, b;
					iterationsToRGB(iterations, r, g, b);

					size_t index = x + y * pixel_size.x;
					vArray[index].position = Vector2f((float)x, (float)y);
					vArray[index].color = Color(r, g, b);
				}
			}
		};


	int sliceHeight = pixel_size.y / threadCount;
	for (int i = 0; i < threadCount; ++i)
	{
		int startY = i * sliceHeight;
		int endY = (i == threadCount - 1) ? pixel_size.y : (i + 1) * sliceHeight;
		threads.emplace_back(renderSlice, startY, endY);
	}

	for (auto& t : threads)
		t.join();

	state = State::DISPLAYING;
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
	if (count == MAX_ITER) {
		r = g = b = 0; // Inside the set = black
		return;
	}

	float t = (float) count / MAX_ITER;

	
	r = (Uint8) (9 * (1 - t) * t * t * t * 255);
	g = (Uint8)(15 * (1 - t) * (1 - t) * t * t * 255);
	b = (Uint8)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
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
