#pragma once
#include "Rendering.h"

/// <summary>
/// カメラ
/// </summary>
class Camera {
public://メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Camera() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Camera() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="windowWidth">画面の幅</param>
	/// <param name="windowHeight">画面の高さ</param>
	void Initialize(float windowWidth, float windowHeight);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ビュー射影行列のゲッター
	/// </summary>
	/// <returns>ビュー射影行列</returns>
	Matrix4x4 GetViewProjectionMatrix() const;

	/// <summary>
	/// ビューポート行列のゲッター
	/// </summary>
	/// <returns>ビューポート行列</returns>
	Matrix4x4 GetViewportMatrix() const;

	/// <summary>
	/// 回転のセッター
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(const Vector3& rotate);

	/// <summary>
	/// 平行移動のセッター
	/// </summary>
	/// <param name="translate">平行移動</param>
	void SetTranslate(const Vector3& translate);
private://メンバ関数
	/// <summary>
	/// ビュープロジェクション行列の作成
	/// </summary>
	void MakeViewProjectionMatrix();

	/// <summary>
	/// ビューポートの生成
	/// </summary>
	void MakeViewportMatrix();
private://メンバ変数
	float windowWidth_ = 0.0f; //画面の幅
	float windowHeight_ = 0.0f; //画面の高さ
	Vector3 scale_ = { 1.0f,1.0f,1.0f };//拡縮
	Vector3 rotate_ = { 0.0f,0.0f,0.0f };//回転
	Vector3 translate_ = { 0.0f,0.0f,-10.0f };//移動
	Matrix4x4 worldMatrix_ = Matrix4x4::Identity4x4();//ワールド行列
	Matrix4x4 viewMatrix_ = Matrix4x4::Identity4x4();//ビュー行列
	Matrix4x4 projectionMatrix_ = Matrix4x4::Identity4x4();//射影行列
	Matrix4x4 viewProjectionMatrix_ = Matrix4x4::Identity4x4();//ビュー射影行列
	Matrix4x4 viewportMatrix_ = Matrix4x4::Identity4x4();//ビューポート行列
};

