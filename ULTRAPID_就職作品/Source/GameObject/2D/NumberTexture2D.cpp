#include "NumberTexture2D.h"

using namespace DirectX;

// 描画する数字のUVを計算
void NumberTexture2D::SetUV(const int _num)
{
	// number.pngを10分割
	// 0～9
	switch (_num)
	{
	case 0: // 数字0
		uv.x = 0.0f;
		break;
	case 1: // 数字1
		uv.x = 0.1f;
		break;
	case 2: // 数字2
		uv.x = 0.2f;
		break;
	case 3: // 数字3
		uv.x = 0.3f;
		break;
	case 4: // 数字4
		uv.x = 0.4f;
		break;
	case 5: // 数字5
		uv.x = 0.5f;
		break;
	case 6: // 数字6
		uv.x = 0.6f;
		break;
	case 7: // 数字7
		uv.x = 0.7f;
		break;
	case 8: // 数字8
		uv.x = 0.8f;
		break;
	case 9: // 数字9
		uv.x = 0.9f;
		break;
	case 10:// 数字以外（ドットなど）
		uv.x = 1.0f;
		break;
	default:
		break;
	}
}

// 複数の数字の位置・スケールを一括操作する関数
void NumberTexture2D::SetWorldMatrix(std::vector<std::unique_ptr<Object2D>>& _objects)
{
	// 親オブジェクトの変換行列
	XMMATRIX parentTransform =
		XMMatrixScaling(parentScale.x, parentScale.y, 1.0f) *
		XMMatrixRotationZ(0) *
		XMMatrixTranslation(parentPosition.x, parentPosition.y, parentPosition.z);

	for (int i = 0; i < _objects.size(); i++)
	{
		XMFLOAT3 pos = _objects[i]->GetPosition();
		XMFLOAT3 scale = _objects[i]->GetScale();

		// 各オブジェクトのローカル変換行列を作成
		XMMATRIX localTransform =
			XMMatrixScaling(scale.x, scale.y, 1.0f) *
			XMMatrixRotationZ(0) *
			XMMatrixTranslation(pos.x, pos.y, pos.z);

		// ローカル座標を親の座標系に変換する
		XMMATRIX finalTransform = localTransform * parentTransform;

		// 各オブジェクトのワールド行列を設定
		_objects[i]->SetMatrixWorld(finalTransform);
	}
}