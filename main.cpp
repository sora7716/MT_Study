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
struct Sphere {
	Vector3 center; //中心座標
	float radius;   //半径
};

//AABB
struct AABB {
	Vector3 min;//最小値
	Vector3 max;//最大値
};

//OBB
struct OBB {
	Vector3 center;//中心点
	Vector3 rotate;//回転
	Vector3 orientations[3];//座法軸。正規化・直行必須
	Vector3 size;//座標軸方向の長さ半分。中心から面までの距離
};

/// <summary>
/// AABB同士の当たり判定
/// </summary>
/// <param name="aabb1">aabb1</param>
/// <param name="aabb2">aabb2</param>
/// <returns>衝突したかのフラグ</returns>
bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	return aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x &&
		aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y &&
		aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z;
}

/// <summary>
/// AABBと球の当たり判定
/// </summary>
/// <param name="aabb">aabb</param>
/// <param name="sphere">球</param>
/// <returns>衝突したかのフラグ</returns>
bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	//最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z),
	};

	//最近接点から球の中心との距離を求める
	float distance = (closestPoint - sphere.center).Length();

	//距離が半径より小さければ衝突
	return distance <= sphere.radius;
}

/// <summary>
/// OBBと球の当たり判定
/// </summary>
/// <param name="obb">obb</param>
/// <param name="sphere">球</param>
/// <returns>衝突したかのフラグ</returns>
bool IsCollision(const OBB& obb, const Sphere& sphere) {
	//OBBのワールド行列の逆行列を作成
	Matrix4x4 obbWorldMatrixInverse = Rendering::MakeOBBWorldMatrix(obb.orientations, obb.center).Inverse();

	//球の中心をOBB空間上に持っていく
	Vector3 centerInOBBLocalSpace = Rendering::Transform(sphere.center, obbWorldMatrixInverse);

	//AABBをOBBのサイズをもとに作成
	AABB aabbOBBLocal = { -obb.size,obb.size };

	//OBB空間上に持って行った中心座標を元に球を作成
	Sphere sphereOBBLocal = { centerInOBBLocalSpace,sphere.radius };

	//AABBと球の当たり判定をする
	return IsCollision(aabbOBBLocal, sphereOBBLocal);
}

// OBBとAABB/OBB
bool IsCollision(const OBB& obb1, const OBB& obb2) {
	//分割軸の数
	Vector3 separateAxes[15];
	//面の法線
	//OBB1
	separateAxes[0] = obb1.orientations[0];
	separateAxes[1] = obb1.orientations[1];
	separateAxes[2] = obb1.orientations[2];
	//OBB2
	separateAxes[3] = obb2.orientations[0];
	separateAxes[4] = obb2.orientations[1];
	separateAxes[5] = obb2.orientations[2];

	//9つの辺のクロス積
	separateAxes[6] = obb1.orientations[0].Cross(obb2.orientations[0]);
	separateAxes[7] = obb1.orientations[0].Cross(obb2.orientations[1]);
	separateAxes[8] = obb1.orientations[0].Cross(obb2.orientations[2]);
	separateAxes[9] = obb1.orientations[1].Cross(obb2.orientations[0]);
	separateAxes[10] = obb1.orientations[1].Cross(obb2.orientations[1]);
	separateAxes[11] = obb1.orientations[1].Cross(obb2.orientations[2]);
	separateAxes[12] = obb1.orientations[2].Cross(obb2.orientations[0]);
	separateAxes[13] = obb1.orientations[2].Cross(obb2.orientations[1]);
	separateAxes[14] = obb1.orientations[2].Cross(obb2.orientations[2]);

	//半分のベクトル
	//OBB1
	Vector3 obb1HalfVector[3]{
		obb1.orientations[0] * obb1.size.x,
		obb1.orientations[1] * obb1.size.y,
		obb1.orientations[2] * obb1.size.z,
	};
	//OBB2
	Vector3 obb2HalfVector[3]{
		obb2.orientations[0] * obb2.size.x,
		obb2.orientations[1] * obb2.size.y,
		obb2.orientations[2] * obb2.size.z,
	};

	//頂点の数
	const int kCornerNum = 8;

	// 点(頂点)
	Vector3 obb1Corners[kCornerNum] = {
	  obb1.center + obb1HalfVector[0] + obb1HalfVector[1] + obb1HalfVector[2],//背面の右上
	  obb1.center + obb1HalfVector[0] + obb1HalfVector[1] - obb1HalfVector[2],//正面の右上
	  obb1.center + obb1HalfVector[0] - obb1HalfVector[1] + obb1HalfVector[2],//背面の右下
	  obb1.center + obb1HalfVector[0] - obb1HalfVector[1] - obb1HalfVector[2],//正面の右下
	  obb1.center - obb1HalfVector[0] + obb1HalfVector[1] + obb1HalfVector[2],//背面の左上
	  obb1.center - obb1HalfVector[0] + obb1HalfVector[1] - obb1HalfVector[2],//正面の左上
	  obb1.center - obb1HalfVector[0] - obb1HalfVector[1] + obb1HalfVector[2],//背面の左下
	  obb1.center - obb1HalfVector[0] - obb1HalfVector[1] - obb1HalfVector[2],//正面の左下
	};
	Vector3 obb2Corners[kCornerNum] = {
	 obb2.center + obb2HalfVector[0] + obb2HalfVector[1] + obb2HalfVector[2],//背面の右上
	 obb2.center + obb2HalfVector[0] + obb2HalfVector[1] - obb2HalfVector[2],//正面の右上
	 obb2.center + obb2HalfVector[0] - obb2HalfVector[1] + obb2HalfVector[2],//背面の右下
	 obb2.center + obb2HalfVector[0] - obb2HalfVector[1] - obb2HalfVector[2],//正面の右下
	 obb2.center - obb2HalfVector[0] + obb2HalfVector[1] + obb2HalfVector[2],//背面の左上
	 obb2.center - obb2HalfVector[0] + obb2HalfVector[1] - obb2HalfVector[2],//正面の左上
	 obb2.center - obb2HalfVector[0] - obb2HalfVector[1] + obb2HalfVector[2],//背面の左下
	 obb2.center - obb2HalfVector[0] - obb2HalfVector[1] - obb2HalfVector[2],//正面の左下
	};

	// 中心点間のベクトル
	Vector3 centerToCenter = obb1.center - obb2.center;

	//当たり判定の計算
	for (Vector3& separateAxis : separateAxes) {
		separateAxis = separateAxis.Normalize();
		float minOBB1 = (std::numeric_limits<float>::max)();
		float maxOBB1 = (std::numeric_limits<float>::lowest)();
		float minOBB2 = minOBB1;
		float maxOBB2 = maxOBB1;
		for (int32_t cornerIndex = 0; cornerIndex < kCornerNum; cornerIndex++) {
			float obb1Distance = obb1Corners[cornerIndex].Dot(separateAxis);
			minOBB1 = (std::min)(obb1Distance, minOBB1);
			maxOBB1 = (std::max)(obb1Distance, maxOBB1);
			float obb2Distance = obb2Corners[cornerIndex].Dot(separateAxis);
			minOBB2 = (std::min)(obb2Distance, minOBB2);
			maxOBB2 = (std::max)(obb2Distance, maxOBB2);
		}
		//それぞれを射影した範囲長の合計を求める
		float sumSpan = (maxOBB1 - minOBB1) + (maxOBB2 - minOBB2);
		//最大範囲を求める
		float longSpan = (std::max)(maxOBB1, maxOBB2) - (std::min)(minOBB1, minOBB2);
		//分離軸が見つかる判定
		if (sumSpan < longSpan) {
			return false;
		}
	}
	//当たり判定を返す
	return true;
}

//AABBを描画する用のデータ
struct DrawAABBData {
	AABB aabb;
	uint32_t color;
};

//Sphereを描画する用のデータ
struct DrawSphereData {
	Sphere sphere;
	uint32_t color;
};

//OBBを描画する用のデータ
struct DrawOBBData {
	OBB obb;
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
/// 球の描画
/// </summary>
/// <param name="sphereData">球</param>
/// <param name="color">色</param>
/// <param name="viewProjection">ビュー射影行列</param>
/// <param name="viewportMatrix">ビューポート行列</param>
void DrawSphere(const Sphere& sphereData, uint32_t color, const Matrix4x4& viewProjection, const Matrix4x4& viewportMatrix) {
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
				color
			);

			// 緯度線
			Novice::DrawLine(
				static_cast<int32_t>(screenA.x),
				static_cast<int32_t>(screenA.y),
				static_cast<int32_t>(screenC.x),
				static_cast<int32_t>(screenC.y),
				color
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

//OBBの描画
void DrawOBB(OBB& obb, uint32_t color, const Matrix4x4& viewProjection, const Matrix4x4& viewport) {
	//obbの回転行列を作成
	Rendering::MakeOBBRotateMatrix(obb.orientations, obb.rotate);
	//obbの回転行列からobbの中心点を加えてワールド行列を作成
	Matrix4x4 worldMatrix = Rendering::MakeOBBWorldMatrix(obb.orientations, obb.center);
	//AABBにobbを適応
	AABB aabb{
		.min = -obb.size,
		.max = obb.size
	};
	Matrix4x4 worldViewProjection = worldMatrix * viewProjection;
	//描画
	DrawAABB(aabb, color, worldViewProjection, viewport);
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
	DrawSphereData drawSphere = { {{0.0f,0.0f,0.0f},1.0f},BLACK };

	//AABB
	DrawAABBData drawAABB;
	drawAABB = { {{ 0.2f,0.2f,0.2f },{1.0f,1.0f,1.0f}},BLACK };

	//OBB
	std::array<DrawOBBData, 2> drawOBBs;
	drawOBBs[0] = {
		{
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f},
		},
		{0.83f,0.26f,0.24f}
		},
		BLACK
	};
	drawOBBs[1] = {
		{
		{0.9f,0.66f,0.78f},
		{-0.05f,-2.49f,0.15f},
		{
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f},
		},
		{0.5f,0.37f,0.5f}
		},
		BLACK
	};

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
		if (IsCollision(drawOBBs[0].obb, drawOBBs[1].obb)) {
			drawOBBs[0].color = RED;
			drawOBBs[1].color = RED;
		} else {
			drawOBBs[0].color = BLACK;
			drawOBBs[1].color = BLACK;
		}

#ifdef USE_IMGUI
		ImGui::DragFloat3("camera.rotate", &cameraRotate.x, 0.1f);
		ImGui::DragFloat3("camera.translate", &cameraTranslate.x, 0.1f);
		ImGui::Separator();
		ImGui::DragFloat3("sphere.translate", &drawSphere.sphere.center.x, 0.1f);
		ImGui::DragFloat("sphere.radius", &drawSphere.sphere.radius, 0.1f, 0.0f, 10.0f);
		ImGui::Separator();
		ImGui::DragFloat3("aabb.min", &drawAABB.aabb.min.x, 0.1f);
		ImGui::DragFloat3("aabb.max", &drawAABB.aabb.max.x, 0.1f);
		for (int32_t i = 0; i < 2; i++) {
			ImGui::Separator();
			ImGui::PushID(i);
			ImGui::DragFloat3("obb.center", &drawOBBs[i].obb.center.x, 0.1f);
			ImGui::DragFloat3("obb.rotate", &drawOBBs[i].obb.rotate.x, 0.1f);
			ImGui::DragFloat3("obb.size", &drawOBBs[i].obb.size.x, 0.1f);
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
		//DrawSphere(drawSphere.sphere, drawSphere.color, camera->GetViewProjectionMatrix(), camera->GetViewportMatrix());

		//AABBの描画
		//DrawAABB(drawAABB.aabb, drawAABB.color, camera->GetViewProjectionMatrix(), camera->GetViewportMatrix());

		//OBBの描画
		for (DrawOBBData drawOBB : drawOBBs) {
			DrawOBB(drawOBB.obb, drawOBB.color, camera->GetViewProjectionMatrix(), camera->GetViewportMatrix());
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
