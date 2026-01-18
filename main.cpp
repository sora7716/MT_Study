#define NOMINMAX
#include <Novice.h>
#include "MathData.h"
#include "Rendering.h"
#include "Camera.h"
#include "ScreenPrintf.h"
#include <cstdint>
#include <numbers>
#include <cmath>
#include <vector>
#include <array>
#include <algorithm>
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

const char kWindowTitle[] = "GSManager";

//球のデータ
struct SphereData {
	Vector3 center; //中心座標
	float radius;   //半径
};

//AABB
struct AABB {
	Vector3 min;//最小値
	Vector3 max;//最大値

	/// <summary>
	/// AABB同士の当たり判定
	/// </summary>
	/// <param name="aabb">aabb</param>
	/// <returns>衝突したかのフラグ</returns>
	bool IsCollision(const AABB& aabb) {
		return min.x <= aabb.max.x && max.x >= aabb.min.x &&
			min.y <= aabb.max.y && max.y >= aabb.min.y &&
			min.z <= aabb.max.z && max.z >= aabb.min.z;
	}
};

//AABBを描画する用のデータ
struct Drawaabb {
	AABB aabb;
	uint32_t color;
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

/// <summary>
/// AABBの描画
/// </summary>
/// <param name="aabb">aabb</param>
/// <param name="color">カラー</param>
/// <param name="viewProjection">ビュープロジェクション</param>
/// <param name="viewportMatrix">ビューポート</param>
void DrawAABB(AABB& aabb, uint32_t color, const Matrix4x4& viewProjection, const Matrix4x4& viewportMatrix) {
	//矩形の頂点数
	const int32_t kRectVertexCount = 4;

	//ローカル座標
	std::vector<Vector3>localVertices;
	localVertices.resize(kRectVertexCount * 2);

	//minがmaxを超えないようにする
	aabb.min.x = std::min(aabb.min.x, aabb.max.x);
	aabb.max.x = std::max(aabb.min.x, aabb.max.x);
	aabb.min.y = std::min(aabb.min.y, aabb.max.y);
	aabb.max.y = std::max(aabb.min.y, aabb.max.y);
	aabb.min.z = std::min(aabb.min.z, aabb.max.z);
	aabb.max.z = std::max(aabb.min.z, aabb.max.z);

	//正面
	localVertices[0] = { aabb.min.x,aabb.max.y,aabb.min.z };//左上
	localVertices[1] = { aabb.max.x,aabb.max.y,aabb.min.z };//右上
	localVertices[2] = { aabb.max.x,aabb.min.y,aabb.min.z };//右下
	localVertices[3] = { aabb.min.x,aabb.min.y,aabb.min.z };//左下

	//背面
	localVertices[kRectVertexCount] = { aabb.min.x,aabb.max.y,aabb.max.z };//左上
	localVertices[kRectVertexCount + 1] = { aabb.max.x,aabb.max.y,aabb.max.z };//右上
	localVertices[kRectVertexCount + 2] = { aabb.max.x,aabb.min.y,aabb.max.z };//右下
	localVertices[kRectVertexCount + 3] = { aabb.min.x,aabb.min.y,aabb.max.z };//左下

	//スクリーン座標
	std::vector<Vector3>screenVertices;
	screenVertices.resize(kRectVertexCount * 2);

	//座標変換
	for (int32_t i = 0; i < kRectVertexCount * 2; i++) {
		screenVertices[i] = Rendering::Transform(localVertices[i], viewProjection);
		screenVertices[i] = Rendering::Transform(screenVertices[i], viewportMatrix);
	}

	//描画
	const int32_t kAABBEdgeCount = 12;//辺の数

	//12本の辺をIndexでペアにする
	std::array<std::pair<int32_t, int32_t>, kAABBEdgeCount>aabbEadges = {
		{{0,1},{1,2},{2,3},{3,0} ,
		 {0,4},{1,5},{2,6},{3,7},
		 {4,5},{5,6},{6,7},{ 7,4 }}
	};

	//描画
	for (auto [begin, end] : aabbEadges) {
		Novice::DrawLine(
			static_cast<int32_t>(screenVertices[begin].x), static_cast<int32_t>(screenVertices[begin].y),
			static_cast<int32_t>(screenVertices[end].x), static_cast<int32_t>(screenVertices[end].y),
			color
		);
	}

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
	Vector3 cameraTranslate = { 0.0f,0.0f,-6.49f };

	//球
	SphereData sphereData = { .center{},.radius = 1.0f };

	//AABB
	std::array<Drawaabb, 2>drawAABBs;
	drawAABBs[0] = { {{-0.5f,-0.5f,-0.5f},{0.0f,0.0f,0.0f}},BLACK };
	drawAABBs[1] = { {{ 0.2f,0.2f,0.2f },{1.0f,1.0f,1.0f}},BLACK };

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

		//衝突判定
		if (drawAABBs[0].aabb.IsCollision(drawAABBs[1].aabb)) {
			drawAABBs[0].color = RED;
			drawAABBs[1].color = RED;
		} else {
			drawAABBs[0].color = BLACK;
			drawAABBs[1].color = BLACK;
		}

#ifdef USE_IMGUI
		ImGui::DragFloat3("camera.rotate", &cameraRotate.x, 0.1f);
		ImGui::DragFloat3("camera.translate", &cameraTranslate.x, 0.1f);
		ImGui::Separator();
		ImGui::DragFloat3("sphere.translate", &sphereData.center.x, 0.1f);
		ImGui::DragFloat("sphere.radius", &sphereData.radius, 0.1f, 0.0f, 10.0f);
		for (int32_t i = 0; i < drawAABBs.size(); i++) {
			ImGui::Separator();
			ImGui::PushID(i);
			ImGui::DragFloat3("aabb.min", &drawAABBs[i].aabb.min.x, 0.1f);
			ImGui::DragFloat3("aabb.max", &drawAABBs[i].aabb.max.x, 0.1f);
			ImGui::PopID();
		}
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
		//DrawSphere(sphereData, camera->GetViewProjectionMatrix(), camera->GetViewportMatrix());

		for (Drawaabb& drawAABB : drawAABBs) {
			DrawAABB(drawAABB.aabb, drawAABB.color, camera->GetViewProjectionMatrix(), camera->GetViewportMatrix());
		}
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
