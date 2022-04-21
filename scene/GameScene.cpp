#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");
	// 3dモデルの生成
	model_ = Model::Create();

	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスター
	std::mt19937_64 engine(seed_gen());
	//乱数範囲（回転角用）
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	//乱数範囲（座標用）
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	//カメラ視点座標を設定
	viewProjection_.eye = {0, 0, -50};
	//カメラ上方向ベクトルを設定（右上45度指定）
	viewProjection_.up = {0,1, 0.0f};

	//カメラ垂直方向視野角を設定
	//viewProjection_.fovAngleY = XMConvertToRadians(10.0f);

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//キャラクターの大元
	worldTransform_[PartId::Root].Initialize();
	//脊椎
	worldTransform_[PartId::Spine].translation_ = {0, 4.5f, 0};
	worldTransform_[PartId::Spine].parent_ = &worldTransform_[PartId::Root];
	worldTransform_[PartId::Spine].Initialize();
	//胸
	worldTransform_[PartId::Chest].translation_ = {0,0, 0};
	worldTransform_[PartId::Chest].parent_ = &worldTransform_[PartId::Spine];
	worldTransform_[PartId::Chest].Initialize();
	//頭
	worldTransform_[PartId::Head].translation_ = {0, 4.5f, 0};
	worldTransform_[PartId::Head].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::Head].Initialize();
	//左腕
	worldTransform_[PartId::ArmL].translation_ = {4.5f, 0, 0};
	worldTransform_[PartId::ArmL].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmL].Initialize();
	//右腕
	worldTransform_[PartId::ArmR].translation_ = {-4.5f, 0, 0};
	worldTransform_[PartId::ArmR].parent_ = &worldTransform_[PartId::Chest];
	worldTransform_[PartId::ArmR].Initialize();
	//尻
	worldTransform_[PartId::Hip].translation_ = {0, -4.5f, 0};
	worldTransform_[PartId::Hip].parent_ = &worldTransform_ [PartId::Spine];
	worldTransform_[PartId::Hip].Initialize();
	//左足
	worldTransform_[PartId::LegL].translation_ = {4.5f,-4.5f, 0};
	worldTransform_[PartId::LegL].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegL].Initialize();
	// 右足
	worldTransform_[PartId::LegR].translation_ = {-4.5f,-4.5f, 0};
	worldTransform_[PartId::LegR].parent_ = &worldTransform_[PartId::Hip];
	worldTransform_[PartId::LegR].Initialize();
}

void GameScene::Update() {
	//視点移動処理
	{
		//視点の移動ベクトル
		XMFLOAT3 move = {0, 0, 0};

		//視点の移動速さ
		const float kEyeSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_W)) {
			move = {0, 0, kEyeSpeed};
		} else if (input_->PushKey(DIK_S)) {
			move = {0, 0, -kEyeSpeed};
		}
		//視点移動（ベクトルの加算）
		viewProjection_.eye.x += move.x;
		viewProjection_.eye.y += move.y;
		viewProjection_.eye.z += move.z;

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		debugText_->SetPos(50, 50);
		debugText_->Printf(
		  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	}


	//注視点移動処理

	{
		//注視点の移動ベクトル
		XMFLOAT3 move = {0, 0, 0};

		//注視点の移動速さ
		const float kTargetSpeed = 0.2f;


		//注視点移動（ベクトルの加算）
		viewProjection_.target.x += move.x;
		viewProjection_.target.y += move.y;
		viewProjection_.target.z += move.z;

		//行列の再計算
		viewProjection_.UpdateMatrix();

		//デバック用表示
		debugText_->SetPos(50, 70);
		debugText_->Printf(
		  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
		  viewProjection_.target.z);
	}

	
	//Fov変更処理
	{
		//上キーで視野角広がる
		if (input_->PushKey(DIK_UP)) {
			viewProjection_.fovAngleY += 0.01f;
			viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
		} else if (input_->PushKey(DIK_DOWN)) {
			viewProjection_.fovAngleY -= 0.01f;
			viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
		}

		//行列の再計算
		viewProjection_.UpdateMatrix();
		//デバッグ用表示
		debugText_->SetPos(50, 110);
		debugText_->Printf("fovAngleY(Degree):%f", XMConvertToDegrees(viewProjection_.fovAngleY));
	}


	//キャラクター移動処理
	{
		//キャラクターの移動ベクトル
		XMFLOAT3 move = {0, 0, 0};

		//キャラクターの移動速度
		const float kCharacterSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move = {-kCharacterSpeed, 0, 0};
		} else if (input_->PushKey(DIK_RIGHT)) {
			move = {kCharacterSpeed, 0, 0};
		}

		//注視点移動(ベクトルの加算)
		worldTransform_[PartId::Root].translation_.x += move.x;
		worldTransform_[PartId::Root].translation_.y += move.y;
		worldTransform_[PartId::Root].translation_.z += move.z;

		//デバッグ用表示
		debugText_->SetPos(50, 150);
		debugText_->Printf(
		  "Root:(%f,%f,%f)", worldTransform_[PartId::Root].translation_.x,
		  worldTransform_[PartId::Root].translation_.y,
		  worldTransform_[PartId::Root].translation_.z);
	}
//大元から順に更新していく
//大元
	worldTransform_[PartId::Spine].UpdateMatrix();
	//脊椎
	worldTransform_[PartId::Spine].UpdateMatrix();
	//胸
	worldTransform_[PartId::Chest].UpdateMatrix();
	//頭
	worldTransform_[PartId::Head].UpdateMatrix();
	//左腕
	worldTransform_[PartId::ArmL].UpdateMatrix();
	//右腕
	worldTransform_[PartId::ArmR].UpdateMatrix();
	//尻
	worldTransform_[PartId::Hip].UpdateMatrix();
	//左足
	worldTransform_[PartId::LegL].UpdateMatrix();
	// 右足
	worldTransform_[PartId::LegR].UpdateMatrix();

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//大元
	//model_->Draw(worldTransform_[PartId::Root], viewProjection_, textureHandle_);
	//脊椎
	//model_->Draw(worldTransform_[PartId::Spine], viewProjection_, textureHandle_);
	//胸
	model_->Draw(worldTransform_[PartId::Chest], viewProjection_, textureHandle_);
	//頭
	model_->Draw(worldTransform_[PartId::Head], viewProjection_, textureHandle_);
	//左腕
	model_->Draw(worldTransform_[PartId::ArmL], viewProjection_, textureHandle_);
	//右腕
	model_->Draw(worldTransform_[PartId::ArmR], viewProjection_, textureHandle_);
	//尻
	model_->Draw(worldTransform_[PartId::Hip], viewProjection_, textureHandle_);
	//左足
	model_->Draw(worldTransform_[PartId::LegL], viewProjection_, textureHandle_);
	//右足
	model_->Draw(worldTransform_[PartId::LegR], viewProjection_, textureHandle_);
	
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}