#pragma once
#include "MathData.h"

//スクリーンプリント
class ScreenPrintf {
public://メンバ変数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns></returns>
	static ScreenPrintf* GetInstance();

	/// <summary>
	/// ベクトルのスクリーンプリント
	/// </summary>
	/// <param name="x">x</param>
	/// <param name="y">y</param>
	/// <param name="vector">ベクトル</param>
	/// <param name="label">ラベル</param>
	void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label);

	/// <summary>
    /// 行列のスクリーンプリント
    /// </summary>
    /// <param name="x">x</param>
    /// <param name="y">y</param>
    /// <param name="matrix">行列</param>
	/// <param name="label">ラベル</param>
	void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();
private://静的メンバ変数
	//インスタンス
	static inline ScreenPrintf* instance = nullptr;
	//解放したかどうか
	static inline bool isFinalize = false;
private://メンバ関数
	//コンストラクタの封印
	ScreenPrintf() = default;
	//デストラクタの封印
	~ScreenPrintf() = default;
	//コピーコンストラクタの封印
	ScreenPrintf(const ScreenPrintf&) = delete;
	//代入演算子の封印
	ScreenPrintf& operator=(const ScreenPrintf&) = delete;
public://定数
	//列の幅
	static inline const int kColumnWidth = 60;
	//行の幅
	static inline const int kRowHeight = 20;
};

