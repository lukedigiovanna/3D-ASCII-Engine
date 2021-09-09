#include "Screen.h"
#include <map>
#include <vector>
#include <cmath>


void Screen::updateResolution()
{
	CONSOLE_SCREEN_BUFFER_INFO inf;
	GetConsoleScreenBufferInfo(handle, &inf);
	
	SHORT w = (inf.srWindow.Right - inf.srWindow.Left) / 2;
	SHORT h = inf.srWindow.Bottom - inf.srWindow.Top;

	if (w != this->width || h != this->height)
		this->forceClear();
	this->width = w;
	this->height = h;
}

void Screen::setPixel(SHORT x, SHORT y, TCHAR val)
{
	if (x >= 0 && x < this->width && y >= 0 && y < this->height)
		this->drawBuffer[y * width + x] = val;
}

void Screen::rasterize(const SHORT points[6], const float depth[3], const TCHAR fill)
{
	// iterate between each adjacent point
			// use a set to ensure that duplicate x values are not inserted
	std::map<SHORT, std::vector<xzPair>> rasterMap;
	for (int j = 0; j < 3; j++) {
		int n = (j + 1) % 3;
		float dx = points[n * 2] - points[j * 2];
		float dy = points[n * 2 + 1] - points[j * 2 + 1];
		float mag = std::sqrt(dx * dx + dy * dy);
		dx /= mag;
		dy /= mag;
		float dz = (depth[n] - depth[j]) / mag;
		float x = points[j * 2], y = points[j * 2 + 1], z = depth[j];
		for (int k = 0; k < mag; k++) {
			SHORT sx = (SHORT)x, sy = (SHORT)y;

			if (rasterMap.find(sy) == rasterMap.end())
				rasterMap.insert({ sy, std::vector<xzPair>() });
			std::vector<xzPair>& vec = rasterMap.at(sy);
			bool ended = false;
			if (vec.size() > 0) {
				for (int l = 0; l < vec.size(); l++)
					if (sx < vec.at(l).x) {
						vec.insert(vec.begin() + l, { sx, z });
						ended = true;
						break;
					}
			}
			if (!ended) {
				vec.push_back({ sx, z });
			}

			x += dx;
			y += dy;
			z += dz;
		}
	}
	if (rasterMap.size() < MAX_HEIGHT) {
		// go through each y value in the map
		for (std::map<SHORT, std::vector<xzPair>>::iterator iter = rasterMap.begin(); iter != rasterMap.end(); ++iter) {
			SHORT y = iter->first;
			std::vector<xzPair> xzVals = iter->second;
			if (xzVals.size() == 1) {
				xzPair pair = xzVals.at(0);
				// check the z value against the depth buffer
				int idx = pair.x + y * this->width;
				if (idx >= 0 && idx < MAX_WIDTH * MAX_HEIGHT && std::abs(pair.z) <= 1 && pair.z > depthBuffer[idx]) {
					this->setPixel(pair.x, y, fill);
					depthBuffer[idx] = pair.z;
				}
			}
			else {
				//if (drawStyle == DRAW_RASTERIZE) {
					xzPair x0 = xzVals.at(0);
					xzPair x1 = xzVals.at(xzVals.size() - 1);
					if (x0.x > x1.x) {
						xzPair t = x0;
						x0 = x1;
						x1 = t;
					}
					if (x1.x - x0.x > MAX_WIDTH) continue;
					float z = x0.z;
					float dz = (x1.z - x0.z) / (x1.x - x0.x);
					for (SHORT x = x0.x; x <= x1.x; x++) {
						// check against the depth buffer
						int idx = x + y * this->width;
						if (idx >= 0 && idx < MAX_WIDTH * MAX_HEIGHT && std::abs(z) <= 1 && z >= depthBuffer[idx]) {
							depthBuffer[idx] = z;
							this->setPixel(x, y, fill);
						}
						z += dz;
					}
				//}
				//else if (drawStyle == DRAW_MESH) {
				//	for (std::vector<xzPair>::iterator iter = xzVals.begin(); iter != xzVals.end(); ++iter) {
				//		this->setPixel((*iter).x, y, fill);
				//	}
				//}
			}
		}
	}
}

void Screen::drawString(SHORT x, SHORT y, LPCWSTR string)
{
	WriteConsoleOutputCharacter(this->handle, string, wcslen(string), { x * 2, y }, &dw);
}

void Screen::clear(TCHAR val)
{
	for (SHORT x = 0; x < this->getWidth(); x++) {
		for (SHORT y = 0; y < this->getHeight(); y++) {
			this->setPixel(x, y, val);
		}
	}
}

void Screen::forceClear()
{
	TCHAR val = ' ';
	for (SHORT x = 0; x < MAX_WIDTH * 2; x++) {
		for (SHORT y = 0; y < MAX_HEIGHT; y++) {
			WriteConsoleOutputCharacter(this->handle, &val, 1, { x, y }, &this->dw);
			if (x % 2 == 0) cache[(x / 2) + y * this->getWidth()] = ' ';
		}
	}
}

void Screen::render()
{
	for (SHORT x = 0; x < this->getWidth(); x++) {
		for (SHORT y = 0; y < this->getHeight(); y++) {
			int idx = x + y * this->getWidth();
			TCHAR val = this->drawBuffer[idx];
			if (val != cache[idx]) {
				WriteConsoleOutputCharacter(this->handle, &val, 1, { x * 2, y }, &this->dw);
				cache[idx] = val;
			}
		}
	}
	// reset the depth buffer to check to only draw the point closest to the screen
	for (int i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++)
		depthBuffer[i] = -9999999.0f;
}
