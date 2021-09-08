#include <iostream>
#include <Windows.h>
#include "Screen.h"
#include <thread>
#include <chrono>
#include <string>
#include "Matrix4.h"
#include "Vec4.h"
#include <map>
#include <vector>

/**
 This application renders a 3D environment using only the command line text output.

 It supports transforming any model defined by a set of coordinates and draws via triangles.

 Two drawing modes are available: 
		Mesh (draws the outlines of each triangle)
        Rasterization (fills in the triangles)

 The engine also supports filtering pixels by their depth in the scene as to not show faces that are behind others.
 This depth filtering is only supported in the rasterization drawing mode.

 * Scenes that change view rapidly will suffer visual glitches as a large number of draw calls will be required in a short amount of time,
   which is quite a slow operation on most systems.	
			Approaches to fixing this will be investigated in the future such as utilizing a pooling technique such that only a certain number
			of pixels are drawn per frame. This would still have a visual side effect but should be much less abrasive to the eye.
 

*/

constexpr int DRAW_MESH = 0;
constexpr int DRAW_RASTERIZE = 1;

typedef struct {
	SHORT x;
	float z;
} xzPair;

int main() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	
	Screen screen(handle);
	screen.updateResolution();

	const TCHAR* lexicon = L"%#-+.*";
	const int size = 6;
	
	int counter = 0;

	int drawStyle = DRAW_RASTERIZE;

	/*
	approach:
		essentially get a vector4 to represent the position of some point in space

		take that vector and multiply it by the transformation matrix in order to get the position
		in screen space.

		constructing the transformation matrix
		components:
			model matrix (matrix defining the position in world space, rotation, etc.)
			view matrix (information about the camera position)
			projection matrix

	*/
	//float model[24] = {
	//	-1.0f, -1.0f, -1.0f, // 0
	//	-1.0f, -1.0f, 1.0f,  // 1
	//	1.0f, -1.0f, -1.0f,  // 2
	//	1.0f, -1.0f, 1.0f,   // 3
	//	-1.0f, 1.0f, -1.0f,  // 4
	//	-1.0f, 1.0f, 1.0f,   // 5
	//	1.0f, 1.0f, -1.0f,   // 6
	//	1.0f, 1.0f, 1.0f,    // 7
	//};
	//unsigned int indices[36] = {
	//	0, 1, 2, 2, 1, 3,
	//	0, 2, 6, 0, 4, 6,
	//	0, 1, 5, 0, 4, 5,
	//	1, 3, 7, 1, 7, 5,
	//	4, 5, 7, 4, 6, 7,
	//	2, 3, 7, 2, 7, 6
	//};
	float model[12] = {
		1.0f, 0.0f, 0.0f,
		std::cosf(2 * 3.14159265f / 3), 0.0f, std::sinf(2 * 3.14159265f / 3),
		std::cosf(4 * 3.14159265f / 3), 0.0f, std::sinf(4 * 3.14159265f / 3),
		0.0f, 1.0f, 0.0f
	};
	unsigned int indices[12] = {
		0, 1, 2,
		0, 1, 3,
		0, 2, 3,
		1, 2, 3
	};
	const int numTriangles = 4;

	const float viewportWidth = 2.0f, viewportHeight = 2.0f;
	const float viewportX = 0.0f, viewportY = 0.0f;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	screen.forceClear();

	float angle = 0.0f;
	float* depthBuffer = new float[MAX_WIDTH * MAX_HEIGHT]; // allocate to heap to conserve stack memory
	Matrix4 perspective;
	perspective.perspective(90.0f, (float)screen.getWidth() / screen.getHeight(), 0.1f, 100.0f);
	float yaw = 0, pitch = 0;
	float rotateSpeed = 10;
	Vec4 camPos = Vec4(2, 0, 0), camUp = Vec4(0, 1, 0), camForward = Vec4(0, 0, -1);
	while (true) {
		
		// calculate vectors using the camera angles
		float yawR = yaw * 3.14159265 / 180;
		float pitchR = pitch * 3.14159265 / 180;
		camForward.x = std::cosf(yawR) * std::cosf(pitchR);
		camForward.y = std::sinf(pitchR);
		camForward.z = std::sinf(yawR) * std::cosf(pitchR);
		camForward.normalize();

		float forwardMovement = 0.0f;
		float rightMovement = 0.0f;
		float speed = 0.15f;
		// handle camera movement
		if (GetKeyState('A') & 0x8000) {
			rightMovement -= speed;
		}
		if (GetKeyState('D') & 0x8000) {
			rightMovement += speed;
		}
		if (GetKeyState('W') & 0x8000) {
			forwardMovement -= speed;
		}
		if (GetKeyState('S') & 0x8000) {
			forwardMovement += speed;
		}
		if (GetKeyState(VK_RIGHT) & 0x8000) {
			yaw -= rotateSpeed;
		}
		if (GetKeyState(VK_LEFT) & 0x8000) {
			yaw += rotateSpeed;
		}

		if (GetKeyState(VK_DOWN) & 0x8000) {
			pitch += rotateSpeed;
		}
		if (GetKeyState(VK_UP) & 0x8000) {
			pitch -= rotateSpeed;
		}

		if (GetKeyState(VK_SPACE) & 0x8000)
			camPos.y += speed;
		if (GetKeyState(VK_LSHIFT) & 0x8000)
			camPos.y -= speed;

		if (pitch > 89) pitch = 89;
		if (pitch < -89) pitch = -89;

		Matrix4 view;
		Vec4 camRight = camForward.cross(camUp);
		camRight.normalize();

		// apply movement
		Vec4 forwardMovementVector = camForward * forwardMovement;
		Vec4 rightMovementVector = camRight * rightMovement;
		forwardMovementVector.y = 0;
		rightMovementVector.y = 0;
		camPos += forwardMovementVector;
		camPos += rightMovementVector;

		view.view(camRight, camUp, camForward, camPos);
		// SET UP TRANSFORMATION
		Matrix4 translation;
		translation.setIdentity();
		
		Matrix4 scale;
		scale.setIdentity();
		scale.scale(0.5f);
		
		Matrix4 rotate;
		rotate.setIdentity();
		rotate.rotate(angle, 0,  1, 0);
		angle += 0.05f;

		Matrix4 transformation = translation.multiply(scale);
		transformation = transformation.multiply(rotate);

		screen.clear(' ');

		// reset the depth buffer to check to only draw the point closest to the screen
		for (int i = 0; i < MAX_WIDTH * MAX_HEIGHT; i++)
			depthBuffer[i] = -9999999.0f;

		float lastZ = 0;
		for (int i = 0; i < numTriangles; i++) {
			SHORT points[6]; // find screen coordinates of the points
			float depth[3];
			for (int j = 0; j < 3; j++) {
				unsigned int index = indices[i * 3 + j] * 3;
				Vec4 point = Vec4(model[index], model[index + 1], model[index + 2]);
				point.multiply(transformation); // apply transformation to get into world space
				point.multiply(view); // apply the camera matrix
				point.multiply(perspective); // finally apply the perspective projection
				// normalize the homogenous coordinate
				point.x /= point.w;
				point.y /= point.w;
				point.z /= point.w;
				// convert point to viewport space
				point.x = (point.x - (viewportX - viewportWidth / 2)) / viewportWidth;
				point.y = (point.y - (viewportY - viewportHeight / 2)) / viewportHeight;
				points[j * 2] = (SHORT)(point.x * screen.getWidth()); 
				points[j * 2 + 1] = (SHORT)(screen.getHeight() - point.y * screen.getHeight());
				depth[j] = -point.z;
				lastZ = point.z;
			}
			
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
					std::vector<xzPair> &vec = rasterMap.at(sy);
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
			if (rasterMap.size() > MAX_HEIGHT) continue;
			// go through each y value in the map
			TCHAR fill = lexicon[i % size];
			for (std::map<SHORT, std::vector<xzPair>>::iterator iter = rasterMap.begin(); iter != rasterMap.end(); ++iter) {
				SHORT y = iter->first;
				std::vector<xzPair> xzVals = iter->second;
				if (xzVals.size() == 1) {
					xzPair pair = xzVals.at(0);
					// check the z value against the depth buffer
					int idx = pair.x + y * screen.getWidth();
					if (idx >= 0 && idx < MAX_WIDTH * MAX_HEIGHT && std::abs(pair.z) <= 1 && pair.z > depthBuffer[idx]) {
						screen.setPixel(pair.x, y, fill);
						depthBuffer[idx] = pair.z;
					}
				}
				else {
					if (drawStyle == DRAW_RASTERIZE) {
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
							int idx = x + y * screen.getWidth();
							if (idx >= 0 && idx < MAX_WIDTH * MAX_HEIGHT && std::abs(z) <= 1 && z >= depthBuffer[idx]) {
								depthBuffer[idx] = z;
								screen.setPixel(x, y, fill);
							}
							z += dz;
						}
					}
					else if (drawStyle == DRAW_MESH) {
						for (std::vector<xzPair>::iterator iter = xzVals.begin(); iter != xzVals.end(); ++iter) {
							screen.setPixel((*iter).x, y, fill);
						}
					}
				}
			}
		}

		screen.render();

		std::wstring yawS = L"yaw: " + std::to_wstring(yaw);
		std::wstring pitchS = L"pitch: " + std::to_wstring(pitch);
		std::wstring camXS = L"X: " + std::to_wstring(camPos.x);
		std::wstring camYS = L"Y: " + std::to_wstring(camPos.y);
		std::wstring camZS = L"Z: " + std::to_wstring(camPos.z);
		std::wstring lastZS = L"LZ: " + std::to_wstring(lastZ);
		screen.drawString(1, 1, yawS.c_str());
		screen.drawString(1, 2, pitchS.c_str());
		screen.drawString(1, 3, camXS.c_str());
		screen.drawString(1, 4, camYS.c_str());
		screen.drawString(1, 5, camZS.c_str());
		screen.drawString(1, 6, lastZS.c_str());
		
		counter++;
		screen.updateResolution();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	delete[] depthBuffer;

	return 0;
}