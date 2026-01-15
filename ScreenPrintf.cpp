#include "ScreenPrintf.h"
#include <Novice.h>
#include <cassert>

//インスタンスのゲッター
ScreenPrintf* ScreenPrintf::GetInstance() {
	assert(!isFinalize && "GetInstance() called after Finalize()");
	if (instance == nullptr) {
		instance = new ScreenPrintf();
	}
	return instance;
}

//ベクトルのスクリーンプリント
void ScreenPrintf::VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

//行列のスクリーンプリント
void ScreenPrintf::MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label){
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			Novice::ScreenPrintf(x + col * kColumnWidth, (y + row * kRowHeight) + 20, "%6.02f", matrix.m[row][col]);
		}
	}
}

//終了
void ScreenPrintf::Finalize() {
	delete instance;
	instance = nullptr;
	isFinalize = true;
}