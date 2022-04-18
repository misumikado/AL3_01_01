#pragma once

#include "Audio.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

#pragma region モデル部分

	// 3Dモデル
	Model* model_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_[100];
	// ビュープロジェクション
	ViewProjection viewProjection_;

#pragma endregion

#pragma region デバッグテキスト部分

	float translationValX_ = 10.0f;
	float translationValY_ = 10.0f;
	float translationValZ_ = 10.0f;

	float rotationValX_ = 0.785398f;
	float rotationValY_ = 0.785398f;
	float rotationValZ_ = 0.0f;

	float scaleValX_ = 5.0f;
	float scaleValY_ = 5.0f;
	float scaleValZ_ = 5.0f;

#pragma endregion

	/// <summary>
	/// ゲームシーン用
	/// </summary>

//カメラ上方向の角度
	float viewAngle = 0.0f;
};