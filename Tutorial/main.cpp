#define OLC_PGE_APPLICATION
#include <string>
#include "olcPixelGameEngine.h"

constexpr auto screenBlockWidth = 24;
constexpr auto screenBlockHeight = 30;

constexpr auto barHeight = 28;
constexpr auto barInitBlockPos = 2;
constexpr auto barBlockWidth = 4;
constexpr auto barBlockValue = 9;

class BreakOut : public olc::PixelGameEngine
{
public:
	BreakOut()
	{
		sAppName = "TUTORIAL - BreakOut Clone";
	}

private:
	const olc::vi2d vBlockSize = { 16,16 };
	std::unique_ptr<int[]> blocks;

	float fBatPos = vBlockSize.x * barInitBlockPos;
	int iBatBlockPos = barInitBlockPos;
	float fBatWidth = vBlockSize.x * barBlockWidth;
	float fBatSpeed = 250.0f;

	olc::vf2d vBallPos = { 0.0f, 0.0f };
	olc::vf2d vBallDir = { 0.0f, 0.0f };
	float fBallSpeed = 20.0f;
	float fBallRadius = 5.0f;



	std::unique_ptr<olc::Sprite> sprTile;
	std::unique_ptr<olc::Sprite> sprBat;
	std::unique_ptr<olc::Sprite> sprFragment;
	std::unique_ptr<olc::Decal> decFragment;


	struct sFragment
	{
		olc::vf2d pos;
		olc::vf2d vel;
		float fAngle;
		float fTime;
		olc::Pixel colour;
	};

	std::list<sFragment> listFragments;

public:
	void startBall()
	{
		// Start Ball
		float fAngle = float(rand()) / float(RAND_MAX) * 2.0f * 3.14159f;
		fAngle = -0.4f;
		vBallDir = { cos(fAngle), sin(fAngle) };
		vBallPos = { 12.5f, 15.5f };
	}
	bool OnUserCreate() override
	{
		blocks = std::make_unique<int[]>(screenBlockWidth * screenBlockHeight);
		for (int y = 0; y < screenBlockHeight; y++)
		{
			for (int x = 0; x < screenBlockWidth; x++)
			{
				if (x == 0 || y == 0 || x == screenBlockWidth - 1)
					blocks[y * screenBlockWidth + x] = 10;
				else
					blocks[y * screenBlockWidth + x] = 0;

				/*Barrita*/
				if (y == barHeight && (x >= barInitBlockPos && x < (barInitBlockPos + barBlockWidth)) )
				{
					blocks[y * screenBlockWidth + x] = barBlockValue;
				}

				if (x > 2 && x <= 20 && y > 3 && y <= 5)
					blocks[y * screenBlockWidth + x] = 1;
				if (x > 2 && x <= 20 && y > 5 && y <= 7)
					blocks[y * screenBlockWidth + x] = 2;
				if (x > 2 && x <= 20 && y > 7 && y <= 9)
					blocks[y * screenBlockWidth + x] = 3;
			}
		}

		// Load the sprite
		sprTile = std::make_unique<olc::Sprite>("tut_tiles.png");

		// Load bat sprite
		sprBat = std::make_unique<olc::Sprite>("tut_bat.png");

		// Load Fragment Sprite
		sprFragment = std::make_unique<olc::Sprite>("tut_fragment.png");

		// Create decal of fragment
		decFragment = std::make_unique<olc::Decal>(sprFragment.get());

		startBall();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{


		// Handle User Input
		if (GetKey(olc::Key::LEFT).bHeld)
			fBatPos -= fBatSpeed * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld) 
			fBatPos += fBatSpeed * fElapsedTime;

		// Check Bat boundaries
		if (fBatPos < vBlockSize.x) fBatPos = vBlockSize.x;
		if (fBatPos + fBatWidth > (screenBlockWidth - 1) * vBlockSize.x) fBatPos = ((screenBlockWidth - 1) * vBlockSize.x) - fBatWidth;

		//Update Blocks position. TODO: Optimize this, there must be a better way to do this.
		for (int i = iBatBlockPos; i < (iBatBlockPos + barBlockWidth); i++)
		{
			blocks[barHeight * screenBlockWidth + i] = 0;
		}

		iBatBlockPos = fBatPos / vBlockSize.x;

		for (int i = iBatBlockPos; i < (iBatBlockPos + barBlockWidth); i++)
		{
			blocks[barHeight * screenBlockWidth + i] = 9;
		}



		// A better collision detection
		// Calculate where ball should be, if no collision
		olc::vf2d vPotentialBallPos = vBallPos + vBallDir * fBallSpeed * fElapsedTime;

		// Test for hits 4 points around ball
		olc::vf2d vTileBallRadialDims = { fBallRadius / vBlockSize.x, fBallRadius / vBlockSize.y };

		auto TestResolveCollisionPoint = [&](const olc::vf2d& point, olc::vf2d& hitpos, int& id)
		{
			olc::vi2d vTestPoint = vPotentialBallPos + vTileBallRadialDims * point;

			auto& tile = blocks[vTestPoint.y * 24 + vTestPoint.x];
			if (tile == 0)
			{
				// Do Nothing, no collision
				return false;
			}
			else
			{
				// Ball has collided with a tile
				bool bTileHit = tile < 9;
				if (bTileHit)
				{
					id = tile;
					hitpos = { float(vTestPoint.x), float(vTestPoint.y) };
					tile--;
				}

				// Collision response
				if (point.x == 0.0f) vBallDir.y *= -1.0f;
				if (point.y == 0.0f) vBallDir.x *= -1.0f;
				return bTileHit;
			}
		};

		bool bHasHitTile = false;
		olc::vf2d hitpos;
		int hitid = 0;
		bHasHitTile |= TestResolveCollisionPoint(olc::vf2d(0, -1), hitpos, hitid);
		bHasHitTile |= TestResolveCollisionPoint(olc::vf2d(0, +1), hitpos, hitid);
		bHasHitTile |= TestResolveCollisionPoint(olc::vf2d(-1, 0), hitpos, hitid);
		bHasHitTile |= TestResolveCollisionPoint(olc::vf2d(+1, 0), hitpos, hitid);

		if (bHasHitTile)
		{
			for (int i = 0; i < 100; i++)
			{
				sFragment f;
				f.pos = { hitpos.x + 0.5f, hitpos.y + 0.5f };
				float fAngle = float(rand()) / float(RAND_MAX) * 2.0f * 3.14159f;
				float fVelocity = float(rand()) / float(RAND_MAX) * 10.0f;
				f.vel = { fVelocity * cos(fAngle), fVelocity * sin(fAngle) };
				f.fAngle = fAngle;
				f.fTime = 3.0f;
				if (hitid == 1)	f.colour = olc::RED;
				if (hitid == 2)	f.colour = olc::GREEN;
				if (hitid == 3)	f.colour = olc::YELLOW;
				listFragments.push_back(f);
			}
		}



		// Actually update ball position with modified direction
		vBallPos += vBallDir * fBallSpeed * fElapsedTime;

		// Reset Ball
		if (vBallPos.y > 29)
			startBall();

		// Update fragments
		for (auto& f : listFragments)
		{
			f.vel += olc::vf2d(0.0f, 20.0f) * fElapsedTime;
			f.pos += f.vel * fElapsedTime;
			f.fAngle += 5.0f * fElapsedTime;
			f.fTime -= fElapsedTime;
			f.colour.a = (f.fTime / 3.0f) * 255;
		}

		// Remove dead fragments
		listFragments.erase(
			std::remove_if(listFragments.begin(), listFragments.end(), [](const sFragment& f) { return f.fTime < 0.0f; }),
			listFragments.end());

		// Draw Screen
		Clear(olc::DARK_BLUE);
		SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
		for (int y = 0; y < screenBlockHeight; y++)
		{
			for (int x = 0; x < screenBlockWidth; x++)
			{
				switch (blocks[y * screenBlockWidth + x])
				{
				case 0: // Do nothing
					break;
				case 10: // Draw Boundary
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTile.get(), olc::vi2d(0, 0) * vBlockSize, vBlockSize);
					break;
				case 1: // Draw Red Block
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTile.get(), olc::vi2d(1, 0) * vBlockSize, vBlockSize);
					break;
				case 2: // Draw Green Block
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTile.get(), olc::vi2d(2, 0) * vBlockSize, vBlockSize);
					break;
				case 3: // Draw Yellow Block
					DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTile.get(), olc::vi2d(3, 0) * vBlockSize, vBlockSize);
					break;
				}
				//DrawString(x * 16, y * 16, std::to_string(blocks[y * screenBlockWidth + x]), olc::WHITE, 1);
			}
		}
		SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

		// Draw Ball
		FillCircle(vBallPos * vBlockSize, int(fBallRadius), olc::CYAN);

		// Draw Fragments
		for (auto& f : listFragments)
			DrawRotatedDecal(f.pos * vBlockSize, decFragment.get(), f.fAngle, { 4, 4 }, { 1, 1 }, f.colour);

		// Draw Bat
		DrawSprite(int(fBatPos), ScreenHeight() - 32, sprBat.get(), 1);

		return true;
	}
};

int main()
{
	BreakOut demo;
	if (demo.Construct(512, 480, 1, 1))
		demo.Start();
	return 0;
}