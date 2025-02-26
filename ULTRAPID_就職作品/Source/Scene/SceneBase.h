#pragma once
class SceneManager;
class GameManager;
class Camera;
class Camera2D;

//===========================================//
// シーンの基底クラス
//===========================================//
class SceneBase
{
public:
    virtual ~SceneBase() = default;
    virtual void Init() = 0;
    virtual void Update(float _deltaTime, SceneManager& sceneManager, GameManager& gm) = 0;
    virtual void Draw(Camera2D& _camera2D) = 0;
    virtual void DrawShadowMap() = 0;
    virtual void UnInit() = 0;
    virtual void SetIMGUI() = 0;
    virtual void SaveParameters() = 0;
    virtual void LoadParameters() = 0;
};