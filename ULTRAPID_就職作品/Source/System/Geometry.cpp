#include "Geometry.h"
#include "DebugConsole.h"

using namespace DirectX;

Geometry::VertexData Geometry::boxData;
Geometry::VertexData Geometry::sphereData;
Geometry::VertexData Geometry::slopeData;
Geometry::VertexData Geometry::spriteCircleData;
Geometry::VertexData Geometry::squareData;
Geometry::VertexData Geometry::torusData;

// 初期化処理
void Geometry::Init()
{
    // ボックスメッシュ作成
    CreateBox();

    // スフィアメッシュ作成
    CreateSphere();

    // 坂道メッシュ作成
    CreateSlope();

    // 円メッシュ作成
    CreateSpriteCircle();

    // 矩形作成
    CreateSquare();

    // トーラス作成
    CreateTorus();
}

// ボックスメッシュ作成
void Geometry::CreateBox()
{
    // ボックスの各辺の半分の長さを定義
    const float d = 0.5f;

    Renderer::Vertex vertices[] =
    {
        // 前面
        { XMFLOAT3(d, d, -d),  XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(d, d, d),   XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(d, -d, -d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(d, -d, d),  XMFLOAT2(1.0f, 0.0), XMFLOAT3(1.0f, 0.0f, 0.0f) },

        // 背面
        { XMFLOAT3(-d, d, d),   XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-d, d, -d),  XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-d, -d, d),  XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-d, -d, -d), XMFLOAT2(1.0f, 0.0), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

        // 上面
        { XMFLOAT3(-d, d, d),  XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(d, d, d),   XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(-d, d, -d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(d, d, -d),  XMFLOAT2(1.0f, 0.0), XMFLOAT3(0.0f, 1.0f, 0.0f) },

        // 下面
        { XMFLOAT3(-d, -d, -d), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f,-1.0f, 0.0f) },
        { XMFLOAT3(d, -d, -d), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f,-1.0f, 0.0f) },
        { XMFLOAT3(-d, -d, d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f,-1.0f, 0.0f) },
        { XMFLOAT3(d, -d, d), XMFLOAT2(1.0f, 0.0), XMFLOAT3(0.0f,-1.0f, 0.0f) },

        // 左面
        { XMFLOAT3(-d, d, d), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0) },
        { XMFLOAT3(d, d, d), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0) },
        { XMFLOAT3(-d, -d, d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0) },
        { XMFLOAT3(d, -d, d), XMFLOAT2(1.0f, 0.0), XMFLOAT3(0.0f, 0.0f, 1.0) },

        // 右面
        { XMFLOAT3(-d, -d, -d), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f,-1.0f) },
        { XMFLOAT3(d, -d, -d), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,-1.0f) },
        { XMFLOAT3(-d, d, -d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f,-1.0f) },
        { XMFLOAT3(d, d, -d), XMFLOAT2(1.0f, 0.0), XMFLOAT3(0.0f, 0.0f,-1.0f) },

    };

    // インデックス数は6面×2三角形×3頂点
    boxData.indexCount = 36;

    // インデックス配列の定義 (各面を2つの三角形で描画)
    UINT indices[] =
    {
       0, 2, 1,  1, 2, 3,      // 前面
       4, 6, 5,  5, 6, 7,      // 背面
       8, 10, 9,  9, 10, 11,   // 上面
       12, 14, 13, 13, 14, 15, // 下面
       16, 18, 17, 17, 18, 19, // 左面
       20, 22, 21, 21, 22, 23, // 右面
    };

    HRESULT hr;

    // 頂点バッファ設定
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vsrData = {};
    vsrData.pSysMem = vertices;
    hr = Renderer::GetDevice()->CreateBuffer(&vbDesc, &vsrData, boxData.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("boxData.vertexBuffer 作成失敗：Geometry::CreateBox内で発生", DebugConsole::ERROR_LOG);
    }
    // インデックスバッファ設定
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA isrData = {};
    isrData.pSysMem = indices;
    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &isrData, boxData.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("boxData.indexBuffer 作成失敗：Geometry::CreateBox内で発生", DebugConsole::ERROR_LOG);
    }
}

// スフィアメッシュ作成
void Geometry::CreateSphere()
{
    std::vector<Renderer::Vertex> vertices;
    std::vector<DWORD> indices;
     
    // 球の細かさ（分割数）
    const int detail = 24;

    // 球面上の頂点を計算
    for (int i = 0; i <= detail / 2; i++)
    {
        // Y軸方向の角度（緯度）
        float radY = DirectX::XM_PI / (detail / 2) * i;
        // Y軸方向のテクスチャ座標（v値）
        float v = static_cast<float>(i) / (detail / 2);

        for (int j = 0; j < detail; j++)
        {
            // XZ平面上の角度（経度）
            float radXZ = DirectX::XM_2PI / detail * j;
            // XZ平面方向のテクスチャ座標（u値）
            float u = static_cast<float>(j) / detail;

            // 球面座標をデカルト座標に変換
            float x = sinf(radY) * sinf(radXZ);
            float y = cosf(radY);
            float z = sinf(radY) * cosf(radXZ);

            // 法線ベクトルを計算し正規化
            DirectX::XMFLOAT3 normal(x, y, z);
            DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normal)));

            // 頂点データを格納（位置、UV、法線）
            vertices.push_back
            ({
                DirectX::XMFLOAT3(x * 0.5f, y * 0.5f, z * 0.5f), // 半径0.5fの球を作成
                DirectX::XMFLOAT2(u, v),  // テクスチャ座標
                normal  // 法線
            });
        }
    }

    // 球のインデックス（面）を計算
    for (int i = 0; i < detail / 2; i++)
    {
        for (int j = 0; j < detail; j++)
        {
            // 次のインデックス（経度方向）
            int nextJ = (j + 1) % detail;

            // 2つの三角形で1つの四角形を作成（ボトムアップ方式）
            indices.push_back(i * detail + nextJ);
            indices.push_back(i * detail + j);
            indices.push_back((i + 1) * detail + j);
            indices.push_back((i + 1) * detail + nextJ);
            indices.push_back(i * detail + nextJ);
            indices.push_back((i + 1) * detail + j);
        }
    }
    // インデックスの数を設定
    sphereData.indexCount = static_cast<int>(indices.size());

    HRESULT hr;

    // 頂点バッファ設定
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = static_cast<DWORD>(sizeof(Renderer::Vertex) * vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vsrData = {};
    vsrData.pSysMem = vertices.data();

    hr = Renderer::GetDevice()->CreateBuffer(&vbDesc, &vsrData, sphereData.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("sphereData.vertexBuffer 作成失敗：Geometry::CreateSphere内で発生", DebugConsole::ERROR_LOG);
    }

    // インデックスバッファ設定
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = static_cast<DWORD>(sizeof(DWORD) * indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA isrData = {};
    isrData.pSysMem = indices.data();

    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &isrData, sphereData.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("sphereData.indexBuffer 作成失敗：Geometry::CreateSphere内で発生", DebugConsole::ERROR_LOG);
    }
}

// 坂道メッシュ作成
void Geometry::CreateSlope()
{
    const float w = 1.0f;  // 幅
    const float h = 1.0f;  // 高さ
    const float d = 1.0f;  // 奥行き

    float centerX = 0.0f;     // 中心座標X
    float centerY = h / 2.0f; // 中心座標Y
    float centerZ = 0.0f;     // 中心座標Z

    // 頂点データを定義
    Renderer::Vertex vertices[] = {
        // 底面
        { { -w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },  // 左下
        { { w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },   // 右下
        { { -w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },   // 左上
        { { w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },    // 右上

        // 左側面
        { { -w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },   // 左上
        { { -w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },    // 左下
        { { -w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },   // 左後

        // 右側面
        { { w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },  // 右上
        { { w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },   // 右下
        { { w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },  // 右後

        // 背面
        { { -w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },  // 左下
        { { w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },   // 右下
        { { -w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },  // 左上
        { { w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },   // 右上

        // 上面（斜面）
        { { -w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.5f } },  // 左上
        { { w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.5f } },   // 右上
        { { -w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.5f } },   // 左下
        { { w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.5f } }     // 右下
    };

    // 頂点データをslopeDataに追加
    for (int i = 0; i < 18; i++)
    {
        slopeData.vertices.push_back(vertices[i]);
    }

    // インデックスデータ（面の定義）
    slopeData.indexCount = 24;  // インデックス数

    UINT indices[] = {
        // 底面（四角形）
        0, 2, 1,  // 左下、左上、右下
        1, 2, 3,  // 右下、左上、右上

        // 左側面
        4, 6, 5,  // 左側面三角形

        // 右側面
        7, 8, 9,  // 右側面三角形

        // 背面
        10, 12, 11,  // 背面三角形
        11, 12, 13,

        // 上面（斜面）
        14, 16, 15,  // 左側の三角形
        15, 16, 17   // 右側の三角形
    };

    // インデックスデータをslopeDataに追加
    for (int i = 0; i < 24; i++)
    {
        slopeData.indices.push_back(indices[i]);
    }

    HRESULT hr;

    // 頂点バッファ設定
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vsrData = {};
    vsrData.pSysMem = vertices;
    hr = Renderer::GetDevice()->CreateBuffer(&vbDesc, &vsrData, slopeData.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("slopeData.vertexBuffer 作成失敗：Geometry::CreateSlope内で発生", DebugConsole::ERROR_LOG);
    }

    // インデックスバッファ設定
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA isrData = {};
    isrData.pSysMem = indices;
    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &isrData, slopeData.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("slopeData.indexBuffer 作成失敗：Geometry::CreateSlope内で発生", DebugConsole::ERROR_LOG);
    }
}

// 円メッシュ作成
void Geometry::CreateSpriteCircle()
{
    std::vector<Renderer::Vertex> vertices;
    std::vector<DWORD> indices;

    // 円の細かさ（頂点の数）
    const int detail = 16;

    // 中心点の頂点を追加（円の中心）
    vertices.push_back(
    {
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),  // 頂点座標
        DirectX::XMFLOAT2(0.5f, 0.5f),        // テクスチャ座標
        DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)   // 法線ベクトル
    });

    // 円周上の頂点を計算して追加
    for (int i = 0; i <= detail; ++i)
    {
        // 現在の角度を計算（円周上の位置）
        float angle = (float)i / detail * DirectX::XM_2PI;
        float x = 0.5f * cosf(angle);  // X座標（半径0.5fの円）
        float y = 0.5f * sinf(angle);  // Y座標

        // 計算した頂点を追加
        vertices.push_back(
        {
            DirectX::XMFLOAT3(x, y, 0.0f),          // 頂点座標
            DirectX::XMFLOAT2((x / 0.5f + 1.0f) * 0.5f, (y / 0.5f + 1.0f) * 0.5f), // テクスチャ座標
            DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)     // 法線ベクトル (+Z方向)
            
        });

        // インデックスを追加（円の三角形を構成）
        if (i < detail)
        {
            indices.push_back(0);      // 中心点
            indices.push_back(i + 2);  // 現在の円周上の頂点
            indices.push_back(i + 1);  // 次の円周上の頂点
        }
    }

    // インデックス数を設定
    spriteCircleData.indexCount = static_cast<int>(indices.size());

    HRESULT hr;

    // 頂点バッファ設定
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = static_cast<DWORD>(sizeof(Renderer::Vertex) * vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vsrData = {};
    vsrData.pSysMem = vertices.data();

    hr = Renderer::GetDevice()->CreateBuffer(&vbDesc, &vsrData, spriteCircleData.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("spriteCircleData.vertexBuffer 作成失敗：Geometry::CreateSpriteCircle内で発生", DebugConsole::ERROR_LOG);
    }

    // インデックスバッファ設定
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = static_cast<DWORD>(sizeof(DWORD) * indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA isrData = {};
    isrData.pSysMem = indices.data();

    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &isrData, spriteCircleData.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("spriteCircleData.indexBuffer 作成失敗：Geometry::CreateSpriteCircle内で発生", DebugConsole::ERROR_LOG);
    }

}

// 矩形メッシュ作成
void Geometry::CreateSquare()
{

    // 頂点データ
    std::vector<Renderer::Vertex> vertices =
    {
        // 四角形の頂点データ（4頂点）
        { DirectX::XMFLOAT3(-0.5f,  0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },  // 左上
        { DirectX::XMFLOAT3(0.5f,  0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },  // 右上
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },  // 右下
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },  // 左下
    };

    // インデックスデータ（2つの三角形で四角形を描く）
    std::vector<UINT> indices =
    {
        0, 1, 2,  // 最初の三角形
        0, 2, 3   // 2番目の三角形
    };

    // インデックス数を設定
    squareData.indexCount = static_cast<int>(indices.size());

    HRESULT hr;

    // 頂点バッファ設定
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(Renderer::Vertex) * vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vsrData = {};
    vsrData.pSysMem = vertices.data();

    hr = Renderer::GetDevice()->CreateBuffer(&vbDesc, &vsrData, squareData.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("squareData.vertexBuffer 作成失敗：Geometry::CreateSquare内で発生", DebugConsole::ERROR_LOG);
    }

    // インデックスバッファ設定
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA isrData = {};
    isrData.pSysMem = indices.data();

    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &isrData, squareData.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("squareData.indexBuffer 作成失敗：Geometry::CreateSquare内で発生", DebugConsole::ERROR_LOG);
    }
}

void Geometry::CreateTorus()
{
    // 頂点データとインデックスデータを格納するコンテナ
    std::vector<Renderer::Vertex> vertices;
    std::vector<UINT> indices;

    int ringSegments = 64;
    int tubeSegments = 32;
    float innerRadius = 0.01f;
    float outerRadius = 0.5f;

    for (int i = 0; i <= ringSegments; ++i)
    {
        float theta = (float)i / ringSegments * XM_2PI; // 外周角度
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        for (int j = 0; j <= tubeSegments; ++j)
        {
            float phi = (float)j / tubeSegments * XM_2PI; // 内周角度
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);

            // 頂点の位置
            float x = (outerRadius + innerRadius * cosPhi) * cosTheta;
            float y = innerRadius * sinPhi;
            float z = (outerRadius + innerRadius * cosPhi) * sinTheta;

            // 法線ベクトル
            DirectX::XMFLOAT3 normal(cosPhi * cosTheta, sinPhi, cosPhi * sinTheta);

            // UV座標
            DirectX::XMFLOAT2 uv((float)i / ringSegments, (float)j / tubeSegments);

            vertices.emplace_back(Renderer::Vertex{ {x, y, z}, uv, normal });
        }
    }

    // トーラスのインデックス生成
    for (int i = 0; i < ringSegments; ++i)
    {
        for (int j = 0; j < tubeSegments; ++j) 
        {
            int current = i * (tubeSegments + 1) + j;
            int next = current + tubeSegments + 1;

            // 三角形1
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // 三角形2
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    torusData.indexCount = 6 * ringSegments * tubeSegments;


    HRESULT hr;

    // 頂点バッファ設定
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(Renderer::Vertex) * vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vsrData = {};
    vsrData.pSysMem = vertices.data();

    hr = Renderer::GetDevice()->CreateBuffer(&vbDesc, &vsrData, torusData.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("torusData.vertexBuffer 作成失敗：Geometry::CreateTorus内で発生", DebugConsole::ERROR_LOG);
    }

    // インデックスバッファ設定
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA isrData = {};
    isrData.pSysMem = indices.data();

    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &isrData, torusData.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog(" torusData.indexBuffer 作成失敗：Geometry::CreateTorus内で発生", DebugConsole::ERROR_LOG);
    }
}

// 矩形メッシュ（引数付き、戻り値Model）
Renderer::Model Geometry::CreateSquare(DirectX::XMFLOAT2 center, DirectX::XMFLOAT2 size, DirectX::XMFLOAT2 uv)
{
    float l = center.x - size.x / 2.0f;
    float r = l + size.x;
    float t = center.y + size.y / 2.0f;
    float b = t - size.y;

    Renderer::Vertex vertexList[] =
    {
        // 頂点が時計回りに三角形を形成する側がポリゴンの表になる
        { {l, t, 0.0f}, {0.00f, 0.00f} },    // 0番目の頂点座標　{ x, y, z, u, v }
        { {r, b, 0.0f}, {uv.x, uv.y} },      // 1番目の頂点座標
        { {l, b, 0.0f}, {0.00f, uv.y} },     // 2番目の頂点座標

        { {l, t, 0.0f}, {0.00f, 0.00f} },    // 3番目の頂点
        { {r, t, 0.0f}, {uv.x, 0.00f} },     // 4番目の頂点
        { {r, b, 0.0f}, {uv.x, uv.y} },      // 5番目の頂点
    };

    HRESULT hr;

    // 頂点バッファを作成する
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(vertexList);       // 確保するバッファサイズを指定
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 頂点バッファ作成を指定
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResourceData;
    subResourceData.pSysMem = vertexList;           // VRAMに送るデータを指定
    subResourceData.SysMemPitch = 0;
    subResourceData.SysMemSlicePitch = 0;

    Renderer::Model model = {};
    // 頂点バッファ作成して、それを変数（第３引数）に保存
    hr = Renderer::GetDevice()->CreateBuffer(&bufferDesc, &subResourceData, model.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("model.vertexBuffer 作成失敗：Geometry::CreateSquare内で発生", DebugConsole::ERROR_LOG);
    }

    UINT indices[] = { 0,1,2,3,4,5 };
    D3D11_BUFFER_DESC ibDesc;
    ibDesc.ByteWidth = sizeof(indices);            //2バイト×個数
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;
    ibDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA irData;
    irData.pSysMem = indices;                     //配列の先頭アドレス
    irData.SysMemPitch = 0;
    irData.SysMemSlicePitch = 0;

    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &irData, model.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("model.indexBuffer 作成失敗：Geometry::CreateSquare内で発生", DebugConsole::ERROR_LOG);
    }

    // インデックス数を保存
    model.numIndex = sizeof(indices) / sizeof(indices[0]);
    
    return model;
}
void Geometry::Release()
{
}