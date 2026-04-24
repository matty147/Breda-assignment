#pragma once

#include <vector>

namespace Tmpl8 {

class Surface;

struct Bucket
{
    std::vector<int> entityIDs;
};

class Game
{
public:
	static int currentLevelID;
	static bool updateLevel;

	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();
	void Tick( float deltaTime );
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { /* implement if you want to detect mouse movement */ }
	void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown( int key ) { /* implement if you want to handle keys */ }

	void SpatialHashing(Surface* gameScreen);
    void CheckEntityCollision(int bucketYSize, int bucketXSize);

  private:
	Surface* screen;

	bool isOverlapping(int box1X, int box1Y, int box1Width, int box1Height, int box2X, int box2Y, int box2Width, int box2Height);

	std::vector<std::vector<Bucket>> grid;
};
}; // namespace Tmpl8