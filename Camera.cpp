#include "Camera.h"
#include "Rendering.h"

//初期化
void Camera::Initialize(float windowWidth, float windowHeight) {
	windowWidth_ = windowWidth;
	windowHeight_ = windowHeight;
}

//更新
void Camera::Update() {
	MakeViewProjectionMatrix();
	MakeViewportMatrix();
}

//ビュー射影行列のゲッター
Matrix4x4 Camera::GetViewProjectionMatrix() const {
	return viewProjectionMatrix_;
}

//ビューポート行列のゲッター
Matrix4x4 Camera::GetViewportMatrix() const {
	return viewportMatrix_;
}

//回転のセッター
void Camera::SetRotate(const Vector3& rotate) {
	rotate_ = rotate;
}

//平行移動のセッター
void Camera::SetTranslate(const Vector3& translate) {
	translate_ = translate;
}

//ビュープロジェクション行列の作成
void Camera::MakeViewProjectionMatrix() {
	worldMatrix_ = Rendering::MakeSTRMatrix(scale_, rotate_, translate_);
	viewMatrix_ = worldMatrix_.Inverse();
	projectionMatrix_ = Rendering::MakePerspectiveFovMatrix(0.45f, windowWidth_ / windowHeight_, 0.1f, 100.0f);
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

//ビューポートの生成
void Camera::MakeViewportMatrix() {
	viewportMatrix_ = Rendering::MakeViewportMatrix(0, 0, windowWidth_, windowHeight_, 0.0f, 1.0f);
}
