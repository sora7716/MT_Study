#include <Novice.h>
#include "MathData.h"
#include "Rendering.h"
#include "Camera.h"
#include "ScreenPrintf.h"
#include <cstdint>
#include <numbers>
#include <cmath>
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

const char kWindowTitle[] = "GSManager";

//球のデータ
struct SphereData {
	Vector3 center; //中心座標
	float radius;   //半径
};

struct Matrix3x3 {
	float m[3][3];
};

/// <summary>
/// グリッドの描画
/// </summary>
/// <param name="viewProjectionMatrix">ビュー射影行列</param>
/// <param name="viewportMatrix">ビューポート行列</param>
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;//グリッドの半分の幅
	const uint32_t kSubdivision = 10;//分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / static_cast<float>(kSubdivision);//1つ分の長さ

	//奥から手前ヘの線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; xIndex++) {
		//ローカル座標を求める
		float z = -kGridHalfWidth + kGridEvery * static_cast<float>(xIndex);
		Vector3 localStartPos = { -kGridHalfWidth, 0.0f, z };
		Vector3 localEndPos = { kGridHalfWidth, 0.0f, z };

		//スクリーン座標に変換
		Vector3 screenStartPos = Rendering::Transform(localStartPos, viewProjectionMatrix);
		screenStartPos = Rendering::Transform(screenStartPos, viewportMatrix);
		Vector3 screenEndPos = Rendering::Transform(localEndPos, viewProjectionMatrix);
		screenEndPos = Rendering::Transform(screenEndPos, viewportMatrix);

		//色の設定
		uint32_t color = 0xAAAAAAFF;
		if (xIndex == kSubdivision / 2) {
			color = BLACK; // 中央の線は黒色にする
		}

		//描画
		Novice::DrawLine(
			static_cast<int32_t>(screenStartPos.x),
			static_cast<int32_t>(screenStartPos.y),
			static_cast<int32_t>(screenEndPos.x),
			static_cast<int32_t>(screenEndPos.y),
			color
		);
	}

	//左から右も同じように順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; zIndex++) {
		//ローカル座標を求める
		float x = -kGridHalfWidth + kGridEvery * static_cast<float>(zIndex);
		Vector3 localStartPos = { x, 0.0f, -kGridHalfWidth };
		Vector3 localEndPos = { x, 0.0f, kGridHalfWidth };

		//スクリーン座標に変換
		Vector3 screenStartPos = Rendering::Transform(localStartPos, viewProjectionMatrix);
		screenStartPos = Rendering::Transform(screenStartPos, viewportMatrix);
		Vector3 screenEndPos = Rendering::Transform(localEndPos, viewProjectionMatrix);
		screenEndPos = Rendering::Transform(screenEndPos, viewportMatrix);

		//色の設定
		uint32_t color = 0xAAAAAAFF;
		if (zIndex == kSubdivision / 2) {
			color = BLACK; // 中央の線は黒色にする
		}

		//描画
		Novice::DrawLine(
			static_cast<int32_t>(screenStartPos.x),
			static_cast<int32_t>(screenStartPos.y),
			static_cast<int32_t>(screenEndPos.x),
			static_cast<int32_t>(screenEndPos.y),
			color
		);
	}

}

/// <summary>
/// スフィアの描画
/// </summary>
/// <param name="viewProjection">ビュー射影行列</param>
/// <param name="viewportMatrix">ビューポート行列</param>
void DrawSphere(const SphereData& sphereData, const Matrix4x4& viewProjection, const Matrix4x4& viewportMatrix) {
	const uint32_t kSubdivision = 10;//分割数
	const float kPi = std::numbers::pi_v<float>;//円周率
	const float kLonEvery = 2.0f * kPi / static_cast<float>(kSubdivision);//経度分割1つ分の長さ
	const float kLatEvery = kPi / static_cast<float>(kSubdivision);//緯度分割1つ分の長さ

	//緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++) {
		float lat = -kPi / 2.0f + kLatEvery * static_cast<float>(latIndex);

		//経度方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++) {
			float lon = static_cast<float>(lonIndex) * kLonEvery;

			// 球面座標からワールド座標に変換
			Vector3 a = {
				std::cos(lat) * std::cos(lon),
				std::sin(lat),
				std::cos(lat) * std::sin(lon)
			};

			Vector3 b = {
				std::cos(lat + kLatEvery) * std::cos(lon),
				std::sin(lat + kLatEvery),
				std::cos(lat + kLatEvery) * std::sin(lon)
			};

			Vector3 c = {
				std::cos(lat) * std::cos(lon + kLonEvery),
				std::sin(lat),
				std::cos(lat) * std::sin(lon + kLonEvery)
			};

			//球の半径と中心を考慮して座標を調整
			a = a * sphereData.radius + sphereData.center;
			b = b * sphereData.radius + sphereData.center;
			c = c * sphereData.radius + sphereData.center;

			//スクリーン座標に変換
			Vector3 screenA = Rendering::Transform(a, viewProjection);
			screenA = Rendering::Transform(screenA, viewportMatrix);

			Vector3 screenB = Rendering::Transform(b, viewProjection);
			screenB = Rendering::Transform(screenB, viewportMatrix);

			Vector3 screenC = Rendering::Transform(c, viewProjection);
			screenC = Rendering::Transform(screenC, viewportMatrix);

			// 経度線
			Novice::DrawLine(
				static_cast<int32_t>(screenA.x),
				static_cast<int32_t>(screenA.y),
				static_cast<int32_t>(screenB.x),
				static_cast<int32_t>(screenB.y),
				BLACK
			);

			// 緯度線
			Novice::DrawLine(
				static_cast<int32_t>(screenA.x),
				static_cast<int32_t>(screenA.y),
				static_cast<int32_t>(screenC.x),
				static_cast<int32_t>(screenC.y),
				BLACK
			);
		}
	}
}

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
	Matrix4x4 result = Matrix4x4::Identity4x4();
	Vector3 u = from.Normalize();
	Vector3 v = to.Normalize();
	Vector3 n = (u.Cross(v)).Normalize();
	float cosTheta = u.Dot(v);
	float sinTheta = u.Cross(v).Length();

	if (cosTheta <= -1.0f) {
		if (u.x != 0.0f || u.y != 0.0f) {
			n = Vector3(u.y, -u.x, 0.0f).Normalize();
		} else if (u.x != 0.0f || u.z != 0.0f) {
			n = Vector3(u.z, 0.0f, -u.x).Normalize();
		}
	}

	result.m[0][0] = std::pow(n.x, 2.0f) * (1.0f - cosTheta) + cosTheta;
	result.m[0][1] = n.x * n.y * (1.0f - cosTheta) + n.z * sinTheta;
	result.m[0][2] = n.x * n.z * (1.0f - cosTheta) - n.y * sinTheta;

	result.m[1][0] = n.x * n.y * (1.0f - cosTheta) - n.z * sinTheta;
	result.m[1][1] = std::pow(n.y, 2.0f) * (1.0f - cosTheta) + cosTheta;
	result.m[1][2] = n.y * n.z * (1.0f - cosTheta) + n.x * sinTheta;

	result.m[2][0] = n.x * n.z * (1.0f - cosTheta) + n.y * sinTheta;
	result.m[2][1] = n.y * n.z * (1.0f - cosTheta) - n.x * sinTheta;
	result.m[2][2] = std::pow(n.z, 2.0f) * (1.0f - cosTheta) + cosTheta;
	return result;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//カメラの生成と初期化
	Camera* camera = new Camera();
	camera->Initialize(1280.0f, 720.0f);
	Vector3 cameraRotate = { 0.26f,0.0f,0.0f };
	Vector3 cameraTranslate = { 0.0f,1.9f,-6.49f };

	//球
	SphereData sphereData = { .center{},.radius = 1.0f };

	Vector3 from0 = Vector3(1.0f, 0.7f, 0.5f).Normalize();
	Vector3 to0 = -from0;
	Vector3 from1 = Vector3(-0.6f, 0.9f, 0.2f);
	Vector3 to1 = Vector3(0.4f, 0.7f, -0.5f);
	Matrix4x4 rotateMatrix0 = DirectionToDirection(Vector3(1.0f, 0.0f, 0.0f).Normalize(), Vector3(-1.0f, 0.0f, 0.0f).Normalize());
	Matrix4x4 rotateMatrix1 = DirectionToDirection(from0, to0);
	Matrix4x4 rotateMatrix2 = DirectionToDirection(from1, to1);

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		//カメラの更新 
		camera->Update();
		camera->SetRotate(cameraRotate);
		camera->SetTranslate(cameraTranslate);

#ifdef USE_IMGUI
		ImGui::DragFloat3("camera.rotate", &cameraRotate.x, 0.1f);
		ImGui::DragFloat3("camera.translate", &cameraTranslate.x, 0.1f);
		ImGui::Separator();
		ImGui::DragFloat3("sphere.translate", &sphereData.center.x, 0.1f);
		ImGui::DragFloat("sphere.radius", &sphereData.radius, 0.1f, 0.0f, 10.0f);
#endif // USE_IMGUI

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//グリッドの描画
		DrawGrid(camera->GetViewProjectionMatrix(), camera->GetViewportMatrix());

		//球の描画
		DrawSphere(sphereData, camera->GetViewProjectionMatrix(), camera->GetViewportMatrix());

		const int kRowHeight = 20;
		ScreenPrintf::GetInstance()->MatrixScreenPrintf(0, 0, rotateMatrix0, "rotateMatrix0");
		ScreenPrintf::GetInstance()->MatrixScreenPrintf(0, kRowHeight * 5, rotateMatrix1, "rotateMatrix1");
		ScreenPrintf::GetInstance()->MatrixScreenPrintf(0, kRowHeight * 10, rotateMatrix2, "rotateMatrix2");
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();

	delete camera;
	return 0;
}
