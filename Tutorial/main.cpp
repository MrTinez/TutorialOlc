#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class BreakOut : public olc::PixelGameEngine
{
public:
	BreakOut()
	{
		sAppName = "TUTORIAL - BreakOut Clone";
	}

private:
	float fBatPos = 20.0f;
	float fBatWidth = 40.0f;

	olc::vf2d vBall = { 200.0f, 200.0f };
	olc::vf2d vBallVel = { 200.0f, -100.0f };
	float fBatSpeed = 250.0f;
	float fBallRadius = 5.0f;

public:
	bool OnUserCreate() override
	{
		srand(100);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Handle User Input
		if (GetKey(olc::Key::LEFT).bHeld) fBatPos -= fBatSpeed * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld) fBatPos += fBatSpeed * fElapsedTime;

		if (fBatPos < 11.0f) fBatPos = 11.0f;
		if (fBatPos + fBatWidth > float(ScreenWidth()) - 10.0f) fBatPos = float(ScreenWidth()) - 10.0f - fBatWidth;

		// Update Ball
		vBall += vBallVel * fElapsedTime;

		// Really crude arena detection - this approach sucks
		if (vBall.y <= 10.0f) vBallVel.y *= -1.0f;
		if (vBall.x <= 10.0f) vBallVel.x *= -1.0f;
		if (vBall.x >= float(ScreenWidth()) - 10.0f) vBallVel.x *= -1.0f;

		// Check Bat
		if (vBall.y >= (float(ScreenHeight()) - 20.0f) && (vBall.x > fBatPos) && (vBall.x < fBatPos + fBatWidth))
			vBallVel.y *= -1.0f;


		// Check if ball has gone off screen
		if (vBall.y > ScreenHeight())
		{
			// Reset ball location
			vBall = { 200.0f, 200.0f };
			// Choose Random direction
			float fAngle = (float(rand()) / float(RAND_MAX)) * 2.0f * 3.14159f;
			vBallVel = { 300.0f * cos(fAngle), 300.0f * sin(fAngle) };
		}

		// Erase previous frame
		Clear(olc::DARK_BLUE);

		// Draw Boundary
		DrawLine(10, 10, ScreenWidth() - 10, 10, olc::YELLOW);
		DrawLine(10, 10, 10, ScreenHeight() - 10, olc::YELLOW);
		DrawLine(ScreenWidth() - 10, 10, ScreenWidth() - 10, ScreenHeight() - 10, olc::YELLOW);

		// Draw Bat
		FillRect(int(fBatPos), ScreenHeight() - 20, int(fBatWidth), 10, olc::GREEN);

		// Draw Ball
		FillCircle(vBall, int(fBallRadius), olc::CYAN);
		return true;
	}
};

int main()
{
	BreakOut demo;
	if (demo.Construct(512, 480, 2, 2))
		demo.Start();
	return 0;
}