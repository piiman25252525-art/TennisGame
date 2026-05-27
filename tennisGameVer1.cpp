#include "Dxlib.h"
#include <stdlib.h>

#define MOUSE_USE

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const int WIDTH = 960, HEIGHT = 640;
	SetWindowText("テニスゲーム");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	int ballX = 40;
	int ballY = 80;
	int ballVx = 5;
	int ballVy = 5;
	int ballR = 10;

	int racketX = WIDTH/2;
	int racketY = HEIGHT - 50;
	int racketW = 120;
	int racketH = 12;

	enum {TITLE, PLAY, OVER};
	int scene = TITLE;
	int timer = 0;
	int score = 0;
	int higtScore = 1000;
	int dx, dy;
	int mouseX, mouseY;

	while (1)
	{
		ClearDrawScreen();
		timer++;
		switch (scene)
		{
		case TITLE:
			SetFontSize(50);
			DrawString(WIDTH / 2 - 50 / 2 * 12 / 2, HEIGHT / 3, "Tennis Game", 0x00ff00);
			if (timer % 60 < 30) {
				SetFontSize(30);
				DrawString(WIDTH / 2 - 30 / 2 * 21 / 2, HEIGHT * 2 / 3, "Press SPACE to Start.", 0x00ffff);
			}

			if (CheckHitKey(KEY_INPUT_SPACE) == 1)
			{
				ballX = 40;
				ballY = 80;
				ballVx = 5;
				ballVy = 5;
				racketX = WIDTH / 2;
				racketY = HEIGHT - 50;
				score = 0;
				scene = PLAY;
			}
			break;

		case PLAY:
			ballX = ballX + ballVx;
			if (ballX < ballR && ballVx < 0)	ballVx = -ballVx;
			if (ballX > WIDTH - ballR && ballVx > 0) ballVx = -ballVx;
			ballY = ballY + ballVy;
			if (ballY < ballR && ballVy < 0)	ballVy = -ballVy;
			if (ballY > HEIGHT)
			{
				scene = OVER;
				timer = 0;
				break;
			}
			DrawCircle(ballX, ballY, ballR, 0xff0000, TRUE);
#ifdef MOUSE_USE
			DxLib::GetMousePoint(&mouseX, &mouseY);
			racketX = mouseX;
			if (racketX < racketW / 2)	racketX = racketW / 2;
			if (racketX > WIDTH - racketW / 2)	racketX = WIDTH - racketW / 2;
#else
			if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
				racketX = racketX - 10;
				if (racketX < racketW / 2) racketX = racketW / 2;
			}
			if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
				racketX = racketX + 10;
				if (racketX > WIDTH - racketW / 2) racketX = WIDTH - racketW / 2;
			}
#endif
			DxLib::DrawBox(racketX - racketW / 2, racketY - racketH / 2, racketX + racketW / 2, racketY + racketH / 2, 0x0080ff, TRUE);

			dx = ballX - racketX;
			dy = ballY - racketY;
			if (-racketW / 2 - 10 < dx && dx < racketW / 2 + 10 && -20 < dy && dy < 0) {
				ballVy = -5 - rand() % 5;
				score = score + 100;
				if (score > higtScore)	higtScore = score;
			}
			break;

		case OVER:
			SetFontSize(40);
			DxLib::DrawString(WIDTH / 2 - 40 / 2 * 9 / 2, HEIGHT / 3, "GAME OVER", 0xff0000);
			if (timer > 60 * 5)	scene = TITLE;
			break;

		}

		SetFontSize(30);
		DxLib::DrawFormatString(10, 10, 0xffffff, "SCORE %d", score);
		DxLib::DrawFormatString(WIDTH - 200, 10, 0xffff00, "HI-SC %d", higtScore);

		DxLib::ScreenFlip();
		DxLib::WaitTimer(16);
		if (DxLib::ProcessMessage() == -1) break;
		if (DxLib::CheckKeyInput(KEY_INPUT_ESCAPE) == 1) break;
	}
	DxLib::DxLib_End();
	return 0;
}
