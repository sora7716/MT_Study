#pragma once
#include <Novice.h>
#include <cstdint>
/// <summary>
/// 入力
/// </summary>
class Input {
public://メンバ関数
	/// <summary>
	/// インスタンスのゲッター
	/// </summary>
	/// <returns></returns>
	static Input* GetInstance();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// Pressキーのゲッター
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>Pressキー</returns>
	bool GetPressKeys(int32_t keyNumber);

	/// <summary>
	/// Triggerきーのゲッター
	/// </summary>
	/// <param name="keyNumber">キー番号</param>
	/// <returns>Triggerキー</returns>
	bool GetTriggerKeys(int32_t keyNumber);
private://静的メンバ変数
	//インスタンス
	static inline Input* instance = nullptr;
	//解放したかどうか
	static inline bool isFinalize = false;
private://メンバ関数
	//コンストラクタの封印
	Input() = default;
	//デストラクタの封印
	~Input() = default;
	//コピーコンストラクタの封印
	Input(const Input&) = delete;
	//代入演算子の封印
	Input& operator=(const Input&) = delete;
private://メンバ変数
	char keys_[256] = {};
	char preKeys_[256] = {};
};

