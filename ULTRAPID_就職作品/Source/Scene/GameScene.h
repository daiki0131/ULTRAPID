#pragma once
#include "SceneBase.h"
#include "Boxs.h"
#include "PlayerController.h"
#include "Items.h"
#include "Slopes.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "Particle.h"
#include "Frustum.h"
#include "ScoreUI.h"
#include "SpeedUI.h"
#include "TimeUI.h"
#include "SpeedGaugeUI.h"
#include "UVAnimation2D.h"
#include "CountNumberUI.h"
#include "Light.h"

//===========================================//
// メインゲームシーンクラス
//===========================================//
class GameScene : public SceneBase
{
public:
    static constexpr int MaxRecordDataSize = 300;  // プレイヤーとカメラの情報を保存する最大のコンテナサイズ
  
public:
    // ゲームシーンの状態
    enum GAME_SCENE_STATE
    {
        NO_PLAY,            // 開始前
        PLAY,               // プレイ中
        TIME_UP,            // タイムアップ処理
        RESULT,             // リザルト表示
        SCENE_END,          // ゲーム終了処理
    };
    // ステージ上のオブジェクトの種類
    enum MAP_OBJECT_TYPE
    {
       GROUND,              // 地面
       WALL,                // 壁
       SLOPE,               // 坂道
       PLAYER         = 10, // プレイヤー
       ITEM_SMALL,          // アイテム小
       ITEM_MIDDLE,         // アイテム中
       ITEM_BIG,            // アイテム大
       ITEM_MEGA_BIG,       // アイテム特大
    };
    // マップオブジェクトの情報
    struct MapObject
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 rotation;
        DirectX::XMFLOAT3 scale;
        MAP_OBJECT_TYPE objType;
    };
    // カメラ保存データ（コースアウト時使用）
    struct CameraRecordData
    {
        DirectX::XMFLOAT3 eye;
        float angle;
    };
    // プレイヤー保存データ（コースアウト時使用）
    struct PlayerRecordData
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 oldPosition;
        float angle;
    };

private:
    // ゲームオブジェクト
    std::unique_ptr<PlayerController> player;           // プレイヤー
    std::unique_ptr<Boxs> ground;                       // 地面
    std::unique_ptr<Boxs> wall;                         // 壁
    std::unique_ptr<Slopes> slope;                      // 坂道
    std::unique_ptr<Items> item;                        // アイテム
    std::vector<std::unique_ptr<ItemEffect>> itemFxs;   // アイテムのエフェクト
    std::unique_ptr<FireFlowerEffect> fireFlowerFx;     // プレイヤーから出る火花エフェクト
    std::unique_ptr<GameSceneCamera> camera3D;          // カメラ3D
    std::unique_ptr<Light> sceneLight;                  // ライト
    std::unique_ptr<FadeManager> holeFade;              // コースアウト時のフェード
    std::unique_ptr<ScoreUI> scoreNumUI;                // スコア数字UI
    std::unique_ptr<SpeedUI> speedNumUI;                // 速度数字UI
    std::unique_ptr<TimeUI> timeNumUI;                  // 制限時間数字UI
    std::unique_ptr<SpeedGaugeUI> speedGaugeUI;         // 速度ゲージUI
    std::unique_ptr<Object2D> scoreUI;                  // Scoreテクスチャ
    std::unique_ptr<Object2D> timeUI;                   // Timeテクスチャ
    std::unique_ptr<UVAnimation2D> speedAnim;           // 風切り演出エフェクト
    std::unique_ptr<CountNumberUI> startCountUI;        // スタート時の数字UI
    std::unique_ptr<Object2D> startUI;                  // Startテクスチャ
    std::unique_ptr<Object2D> timeUpUI;                 // TimeUpテクスチャ
    std::unique_ptr<Object2D> resultBg;                 // リザルト背景
    std::unique_ptr<Object2D> resultUI;                 // Resultテクスチャ
    std::unique_ptr<Object2D> resultScoreUI;            // リザルト「Score」テクスチャ
    std::unique_ptr<ScoreUI> resultScoreNumUI;          // リザルトスコア数字UI
    std::unique_ptr<Object2D> pressEnterKeyUI;          // PressEnterKeyテクスチャ
    std::unique_ptr<Object2D> newBestScoreUI;           // NewBestScoreテクスチャ
    CollisionManager collisionManager;                  // 当たり判定専用オブジェクト
    Frustum frustum;                                    // フラスタムカリング用

    GAME_SCENE_STATE gameState;                         // シーンの状態
    std::vector<PlayerRecordData> playerRecordData;     // プレイヤー保存データ（コースアウト時使用）
    std::vector<CameraRecordData> cameraRecordData;     // カメラ保存データ（コースアウト時使用）
    std::optional<std::reference_wrapper<ItemObject>> refTempItem; // 取得アイテムの参照を保存（ヒットストップ時使用）
    
    bool isCourseOut = false;                           // プレイヤーのコースアウトフラグ
    bool isHitStop = false;                             // ヒットストップ中か

    int itemFxIndex = 0;                                // 使用するアイテムエフェクトのインデックス番号
    int recordDataIndex = 0;                            // プレイヤーとカメラの保存データのインデックス番号
    int restoreLocationIndexCount = 30;                 // プレイヤーとカメラのデータを復元するときに使用
    int recordDataSize = 90;                            // 保存用データのコンテナサイズ

    float holeReturnAddPositionY = 15.0f;               // コースアウトから復帰したときのY座標の調整
    float holeJudgePositionY = -50.0f;                  // コースアウト判定のY座標
    float playerSpeedRatio = 0;                         // プレイヤーの速度の割合
    float baseHitStopTime = 0.15f;                      // 標準ヒットストップ時間
    float hitStopTime = 0.0f;                           // 正確なヒットストップ時間
    float hitStopElapsedTime = 0.0f;                    // ヒットストップ経過時間
    float scoreUpRatio = 0.0f;                          // スコア加算割合
    float baseScoreUpRatio = 1.0f;                      // スコア加算標準割合
    float speedAnimSpeed = 10.0f;                       // 風切りアニメーションのアニメーション速度
    float speedAnimStartRatio = 0.3f;                   // 風切りアニメーションの開始時のプレイヤー速度割合
    float speedAnimMaxAlpha = 0.5f;                     // 風切りアニメーションの最大α値
    const float smoothingAnimFactor = 0.05f;            // フレームごとのアニメーションの補完
    const float fovInterpSpeed = 5.0f;
    float stageTime = 0.0f;                             // ステージの制限時間
    float startCount = 3.0f;                            // ゲーム開始時のカウント数
    float seTimer = 1.0f;                               // SEを再生する間隔時間
    float countDownSeStartTime = 10.0f;                 // 終了数秒前のカウントダウンを開始する秒数
    float resultSeInterval = 0.1f;                      // リザルトスコア加算SEの間隔

    float startUIAlpha = 1.0f;                          // Startテクスチャのα値
    float timeUpUIAlpha = 1.0f;                         // TimeUpテクスチャののα値
    float changeAlphaSpeed = 0.01f;                     // StartとTimeUpのα値を変更させる速度
    float timeUpUIDrawTime = 1.5f;                      // TimeUpテクスチャの描画時間（α値減少前）
    float resultStartWaitTime = 0.5f;                   // リザルトスコア加算開始前の待ち時間
    float resultScoreSpeedRaito = 1.0f;                 // リザルトスコア加算速度割合
    float resultScoreMinSpeedFactor = 0.05f;            // リザルトスコア加算最小速度係数
    float resultDrawScore = 0.0f;                       // 表示するスコア数値
    float resultProgress = 0.0f;                        // 目標値までの経過割合
    bool isResultEnd = false;                           // リザルト終了合図

    float ratioPhase1 = 0.0f;                           // 速度状態割合第1段階
    float ratioPhase2 = 0.3f;                           // 速度状態割合第2段階
    float ratioPhase3 = 0.7f;                           // 速度状態割合第3段階
    float ratioPhase4 = 0.9f;                           // 速度状態割合第4段階

    DirectX::XMFLOAT3 colorPhase1 = { 0.0f,1.0f,1.0f }; // 速度割合の色 第1段階
    DirectX::XMFLOAT3 colorPhase2 = { 0.0f,1.0f,0.0f }; // 速度割合の色 第2段階
    DirectX::XMFLOAT3 colorPhase3 = { 1.0f,0.8f,0.0f }; // 速度割合の色 第3段階
    DirectX::XMFLOAT3 colorPhase4 = { 1.0f,0.0f,0.0f }; // 速度割合の色 第4段階

    std::vector<Collision::ActiveBoundsData> activeBoundsData;      // フラスタムカリングに使用するオブジェクトコンテナ
    std::vector<Collision::ActiveBoundsData> broadPhaseBoundsData;  // ブロードフェーズ時に使用するオブジェクトコンテナ

#ifdef _DEBUG
    std::unique_ptr<DebugCamera> debugCamera3D;        // デバッグ専用カメラ
    bool isDebugCameraChange = false;                  // デバッグカメラと通常カメラの切り替え
    bool isDebugDrawUI = true;                         // UIの表示・非表示
    bool isDebugPause = false;                         // デバッグ時の一時停止
    bool isDebugFrustum = true;                        // デバッグ時のフラスタムカリングの可否
    bool isDebugStepFrame = false;                     // 一時停止中に1フレーム進める
    bool isDebugCollisionSphereDraw = false;
    bool isDebugObjectSingleDraw = false;
    int debugDrawObjectCount = 0;                      // 描画中のオブジェクトの数
#endif

    // モデル情報
    typedef std::unordered_map<std::wstring, Renderer::Model> ModelHash;
    std::unique_ptr<ModelHash> modelManager;
    Renderer::Model model;

public:
    void Init() override;
    void UnInit() override;
    void Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm) override;
    // スタート時の更新処理
    void StartGameUpdate(float _deltaTime);
    // ゲームプレイ中の更新処理
    void PlayGameUpdate(float _deltaTime, GameManager& gm);
    // タイムアップ時の更新処理
    void TimeUpUpdate(float _deltaTime);
    // リザルト時の更新処理
    void ResultUpdate(float _deltaTime, SceneManager& sceneManager, GameManager& gm);
    void Draw(Camera2D& _camera2D) override;
    void DrawShadowMap() override;
    // Set関数
    void SetIMGUI() override;
    void SetRecordDataSize(const int _size);
    // Get関数
    bool GetIsHitStop() { return isHitStop; }
    PlayerController& GetPlayer() { return *player; }
    // パラメータ保存
    void SaveParameters() override;
    // パラメータ読み込み
    void LoadParameters()  override;
    // ヒットストップ開始
    void StartHitStop();
    // ヒットストップ更新
    void UpdateHitStop(float _deltaTime);
    // 当たり判定実行関数（アイテム）
    void CheckItemObjectCollision(GameManager& gm);
    // 当たり判定実行関数（地面、壁、坂道）
    void CheckOtherObjectCollision();
    // アイテムエフェクトの再生
    void ItemEffectPlay(DirectX::XMFLOAT3 _playPos);
    // アイテム処理を実行
    void ProcessItem();
    // ゲーム内の速度の割合を反映
    void UpdateStageByRatio(float _deltaTime);
    // プレイヤーとカメラの情報を記録（コースアウト時使用）
    void UpdateRecordData();
    // プレイヤーとカメラ情報を復元（コースアウト時使用）
    void ApplyRecordData();
    // プレイヤーのコースアウトチェック
    void PlayerHoleCheck();
    // フラスタムカリングによる描画チェック
    void DrawFrustumCheck();

};