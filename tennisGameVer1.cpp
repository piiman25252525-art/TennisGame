#include "Dxlib.h"
#include <stdlib.h>

#define MOUSE_USE

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const int WIDTH = 960;	// 画面の幅
	const int HEIGHT = 640;	// 画面の高さ
	SetWindowText("テニスゲーム");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);
	SetMouseDispFlag(FALSE);	// マウスカーソルを非表示にする

	int ballX = 40;		// ボールの初期X座標
	int ballY = 80;		// ボールの初期Y座標
	int ballVx = 5;		// ボールのX方向の速度
	int ballVy = 5;		// ボールのY方向の速度
	int ballR = 10;		// ボールの半径

	int racketX = WIDTH/2;		// ラケットの初期X座標
	int racketY = HEIGHT - 50;	// ラケットの初期Y座標
	int racketW = 120;			// ラケットの幅
	int racketH = 12;			// ラケットの高さ

	enum { TITLE, PLAY, OVER };	// ゲームのシーン
	int scene = TITLE;		// ゲームのシーンを管理
	int timer = 0;			// ゲームの時間を計測
	int score = 0;			// スコア
	int higtScore = 1000;	// ハイスコア
	int dx, dy;				// ボールとラケットの距離
	int mouseX, mouseY;		// マウスの座標（Yは使用しない）

	int startSE = LoadSoundMem("SE/gameStart.mp3");	// ゲーム開始のSE
	int overSE = LoadSoundMem("SE/gameOver.mp3");	// ゲームオーバーのSE
	int receiveSE = LoadSoundMem("SE/recieve.mp3");	// ボールを受けるSE
	int reflectSE = LoadSoundMem("SE/reflect.mp3");	// ボールが壁に当たるSE

	int ballColor = 0xffffff;	// ボールの色
	int racketColor = 0xffffff;	// ラケットの色

	while (1)
	{
		ClearDrawScreen();
		timer++;
		switch (scene)	// シーンごとの処理に分岐
		{
		case TITLE:
			SetFontSize(50);
			DrawString(WIDTH / 2 - 50 / 2 * 12 / 2, HEIGHT / 3, "Tennis Game", 0xffffff);	// タイトルの表示
			if (timer % 60 < 30) {
				SetFontSize(30);
				DrawString(WIDTH / 2 - 46 / 2 * 21 / 2, HEIGHT * 2 / 3,		// スタートの案内の表示
					"Press SPACE or CLICK to Start.", 0xffffff);
			}

			if (CheckHitKey(KEY_INPUT_SPACE) == 1 || GetMouseInput() & MOUSE_INPUT_LEFT)
			{
				ballX = 40;
				ballY = 80;
				ballVx = 5;
				ballVy = 5;
				racketX = WIDTH / 2;		// ラケットX位置は画面中央
				racketY = HEIGHT - 50;		// ラケットY位置は画面下部
				score = 0;
				scene = PLAY;
				PlaySoundMem(startSE, DX_PLAYTYPE_BACK);
			}
			break;

		case PLAY:
			ballX = ballX + ballVx;	// ボールのX移動
			if (ballX < ballR && ballVx < 0) {			// 左壁に衝突
				ballVx = -ballVx;
				ballColor = 0xffff00;
				PlaySoundMem(reflectSE, DX_PLAYTYPE_BACK);
			}
			if (ballX > WIDTH - ballR && ballVx > 0) {	// 右壁に衝突
				ballVx = -ballVx;
				ballColor = 0xffff00;
				PlaySoundMem(reflectSE, DX_PLAYTYPE_BACK);
			}

			ballY = ballY + ballVy;	// ボールのY移動
			if (ballY < ballR && ballVy < 0) {			// 上壁に衝突
				ballVy = -ballVy;
				ballColor = 0xffff00;
				PlaySoundMem(reflectSE, DX_PLAYTYPE_BACK);
			}
			if (ballY > HEIGHT)		// 下に落ちた
			{
				scene = OVER;
				timer = 0;
				PlaySoundMem(overSE, DX_PLAYTYPE_BACK);
				break;
			}
			if (ballColor <= 0xffffff) {
				ballColor += 0x000011;
				if (ballColor > 0xffffff) ballColor = 0xffffff;
			}
			DrawCircle(ballX, ballY, ballR, ballColor, FALSE);	// ボールの描画
#ifdef MOUSE_USE	// マウスで操作
			DxLib::GetMousePoint(&mouseX, &mouseY);
			racketX = mouseX;
//		racketY = mouseY;	// Y座標は固定なので使用しない
			if (racketX < racketW / 2)	racketX = racketW / 2;
			if (racketX > WIDTH - racketW / 2)	racketX = WIDTH - racketW / 2;
#else				// キーボードで操作
			if (CheckHitKey(KEY_INPUT_LEFT) == 1) {
				racketX = racketX - 10;
				if (racketX < racketW / 2) racketX = racketW / 2;
			}
			if (CheckHitKey(KEY_INPUT_RIGHT) == 1) {
				racketX = racketX + 10;
				if (racketX > WIDTH - racketW / 2)	racketX = WIDTH - racketW / 2;
			}
#endif
			// ラケットの描画
			if (racketColor <= 0xffffff) {
				racketColor += 0x000011;
				if (racketColor > 0xffffff) racketColor = 0xffffff;
			}
			DxLib::DrawBox(racketX - racketW / 2, racketY - racketH / 2,racketX + racketW / 2, racketY + racketH / 2, racketColor, FALSE);

			dx = ballX - racketX;	// ボールとラケットのX方向距離
			dy = ballY - racketY;	// ボールとラケットのY方向距離
			if (-racketW / 2 - 10 < dx && dx < racketW / 2 + 10 && -20 < dy && dy < 0) {	// ボールがラケットに衝突
				ballVy = -5 - rand() % 5;	// ボールのY速度をランダムに変化させる
				PlaySoundMem(receiveSE, DX_PLAYTYPE_BACK);
				racketColor = 0xffff00; // ラケットの色を黄色に変える
				ballColor = 0xffff00;	// ボールの色を黄色に変える
				score = score + 100;
				if (score > higtScore)	higtScore = score;	// ハイスコアの更新
			}
			break;

		case OVER:
			SetFontSize(40);
			DxLib::DrawString(WIDTH / 2 - 40 / 2 * 9 / 2, HEIGHT / 3, "GAME OVER", 0xffffff);	// ゲームオーバーの表示
			if (timer > 60 * 5)	scene = TITLE;
			break;
		}

		SetFontSize(30);
		DxLib::DrawFormatString(10, 10, 0xffffff, "SCORE %d", score);	// スコアの表示
		DxLib::DrawFormatString(WIDTH - 200, 10, 0xffffff, "HI-SC %d", higtScore);	// ハイスコアの表示

		DxLib::ScreenFlip();
		DxLib::WaitTimer(16);
		if (DxLib::ProcessMessage() == -1) break;
		if (DxLib::CheckKeyInput(KEY_INPUT_ESCAPE) == 1) break;
	}
	DxLib::DxLib_End();
	return 0;
}
