#include "Geometry.h"
#include "DebugConsole.h"

using namespace DirectX;

Geometry::VertexData Geometry::boxData;
Geometry::VertexData Geometry::sphereData;
Geometry::VertexData Geometry::slopeData;
Geometry::VertexData Geometry::spriteCircleData;
Geometry::VertexData Geometry::squareData;
Geometry::VertexData Geometry::torusData;

// ����������
void Geometry::Init()
{
    // �{�b�N�X���b�V���쐬
    CreateBox();

    // �X�t�B�A���b�V���쐬
    CreateSphere();

    // �⓹���b�V���쐬
    CreateSlope();

    // �~���b�V���쐬
    CreateSpriteCircle();

    // ��`�쐬
    CreateSquare();

    // �g�[���X�쐬
    CreateTorus();
}

// �{�b�N�X���b�V���쐬
void Geometry::CreateBox()
{
    // �{�b�N�X�̊e�ӂ̔����̒������`
    const float d = 0.5f;

    Renderer::Vertex vertices[] =
    {
        // �O��
        { XMFLOAT3(d, d, -d),  XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(d, d, d),   XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(d, -d, -d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(d, -d, d),  XMFLOAT2(1.0f, 0.0), XMFLOAT3(1.0f, 0.0f, 0.0f) },

        // �w��
        { XMFLOAT3(-d, d, d),   XMFLOAT2(0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-d, d, -d),  XMFLOAT2(0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-d, -d, d),  XMFLOAT2(1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-d, -d, -d), XMFLOAT2(1.0f, 0.0), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

        // ���
        { XMFLOAT3(-d, d, d),  XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(d, d, d),   XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(-d, d, -d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(d, d, -d),  XMFLOAT2(1.0f, 0.0), XMFLOAT3(0.0f, 1.0f, 0.0f) },

        // ����
        { XMFLOAT3(-d, -d, -d), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f,-1.0f, 0.0f) },
        { XMFLOAT3(d, -d, -d), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f,-1.0f, 0.0f) },
        { XMFLOAT3(-d, -d, d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f,-1.0f, 0.0f) },
        { XMFLOAT3(d, -d, d), XMFLOAT2(1.0f, 0.0), XMFLOAT3(0.0f,-1.0f, 0.0f) },

        // ����
        { XMFLOAT3(-d, d, d), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0) },
        { XMFLOAT3(d, d, d), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0) },
        { XMFLOAT3(-d, -d, d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0) },
        { XMFLOAT3(d, -d, d), XMFLOAT2(1.0f, 0.0), XMFLOAT3(0.0f, 0.0f, 1.0) },

        // �E��
        { XMFLOAT3(-d, -d, -d), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f,-1.0f) },
        { XMFLOAT3(d, -d, -d), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,-1.0f) },
        { XMFLOAT3(-d, d, -d), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f,-1.0f) },
        { XMFLOAT3(d, d, -d), XMFLOAT2(1.0f, 0.0), XMFLOAT3(0.0f, 0.0f,-1.0f) },

    };

    // �C���f�b�N�X����6�ʁ~2�O�p�`�~3���_
    boxData.indexCount = 36;

    // �C���f�b�N�X�z��̒�` (�e�ʂ�2�̎O�p�`�ŕ`��)
    UINT indices[] =
    {
       0, 2, 1,  1, 2, 3,      // �O��
       4, 6, 5,  5, 6, 7,      // �w��
       8, 10, 9,  9, 10, 11,   // ���
       12, 14, 13, 13, 14, 15, // ����
       16, 18, 17, 17, 18, 19, // ����
       20, 22, 21, 21, 22, 23, // �E��
    };

    HRESULT hr;

    // ���_�o�b�t�@�ݒ�
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vsrData = {};
    vsrData.pSysMem = vertices;
    hr = Renderer::GetDevice()->CreateBuffer(&vbDesc, &vsrData, boxData.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("boxData.vertexBuffer �쐬���s�FGeometry::CreateBox���Ŕ���", DebugConsole::ERROR_LOG);
    }
    // �C���f�b�N�X�o�b�t�@�ݒ�
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA isrData = {};
    isrData.pSysMem = indices;
    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &isrData, boxData.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("boxData.indexBuffer �쐬���s�FGeometry::CreateBox���Ŕ���", DebugConsole::ERROR_LOG);
    }
}

// �X�t�B�A���b�V���쐬
void Geometry::CreateSphere()
{
    std::vector<Renderer::Vertex> vertices;
    std::vector<DWORD> indices;
     
    // ���ׂ̍����i�������j
    const int detail = 24;

    // ���ʏ�̒��_���v�Z
    for (int i = 0; i <= detail / 2; i++)
    {
        // Y�������̊p�x�i�ܓx�j
        float radY = DirectX::XM_PI / (detail / 2) * i;
        // Y�������̃e�N�X�`�����W�iv�l�j
        float v = static_cast<float>(i) / (detail / 2);

        for (int j = 0; j < detail; j++)
        {
            // XZ���ʏ�̊p�x�i�o�x�j
            float radXZ = DirectX::XM_2PI / detail * j;
            // XZ���ʕ����̃e�N�X�`�����W�iu�l�j
            float u = static_cast<float>(j) / detail;

            // ���ʍ��W���f�J���g���W�ɕϊ�
            float x = sinf(radY) * sinf(radXZ);
            float y = cosf(radY);
            float z = sinf(radY) * cosf(radXZ);

            // �@���x�N�g�����v�Z�����K��
            DirectX::XMFLOAT3 normal(x, y, z);
            DirectX::XMStoreFloat3(&normal, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normal)));

            // ���_�f�[�^���i�[�i�ʒu�AUV�A�@���j
            vertices.push_back
            ({
                DirectX::XMFLOAT3(x * 0.5f, y * 0.5f, z * 0.5f), // ���a0.5f�̋����쐬
                DirectX::XMFLOAT2(u, v),  // �e�N�X�`�����W
                normal  // �@��
            });
        }
    }

    // ���̃C���f�b�N�X�i�ʁj���v�Z
    for (int i = 0; i < detail / 2; i++)
    {
        for (int j = 0; j < detail; j++)
        {
            // ���̃C���f�b�N�X�i�o�x�����j
            int nextJ = (j + 1) % detail;

            // 2�̎O�p�`��1�̎l�p�`���쐬�i�{�g���A�b�v�����j
            indices.push_back(i * detail + nextJ);
            indices.push_back(i * detail + j);
            indices.push_back((i + 1) * detail + j);
            indices.push_back((i + 1) * detail + nextJ);
            indices.push_back(i * detail + nextJ);
            indices.push_back((i + 1) * detail + j);
        }
    }
    // �C���f�b�N�X�̐���ݒ�
    sphereData.indexCount = static_cast<int>(indices.size());

    HRESULT hr;

    // ���_�o�b�t�@�ݒ�
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
        DebugConsole::DebugLog("sphereData.vertexBuffer �쐬���s�FGeometry::CreateSphere���Ŕ���", DebugConsole::ERROR_LOG);
    }

    // �C���f�b�N�X�o�b�t�@�ݒ�
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
        DebugConsole::DebugLog("sphereData.indexBuffer �쐬���s�FGeometry::CreateSphere���Ŕ���", DebugConsole::ERROR_LOG);
    }
}

// �⓹���b�V���쐬
void Geometry::CreateSlope()
{
    const float w = 1.0f;  // ��
    const float h = 1.0f;  // ����
    const float d = 1.0f;  // ���s��

    float centerX = 0.0f;     // ���S���WX
    float centerY = h / 2.0f; // ���S���WY
    float centerZ = 0.0f;     // ���S���WZ

    // ���_�f�[�^���`
    Renderer::Vertex vertices[] = {
        // ���
        { { -w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },  // ����
        { { w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },   // �E��
        { { -w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 0.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },   // ����
        { { w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 1.0f, 1.0f }, { 0.0f, -1.0f, 0.0f } },    // �E��

        // ������
        { { -w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },   // ����
        { { -w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },    // ����
        { { -w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f } },   // ����

        // �E����
        { { w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },  // �E��
        { { w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 1.0f, 0.0f }, { -1.0f, 0.0f, 0.0f } },   // �E��
        { { w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 1.0f, 1.0f }, { -1.0f, 0.0f, 0.0f } },  // �E��

        // �w��
        { { -w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },  // ����
        { { w / 2 - centerX, 0 - centerY, -d / 2 - centerZ }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },   // �E��
        { { -w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },  // ����
        { { w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },   // �E��

        // ��ʁi�Ζʁj
        { { -w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.5f } },  // ����
        { { w / 2 - centerX, h - centerY, -d / 2 - centerZ }, { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.5f } },   // �E��
        { { -w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.5f } },   // ����
        { { w / 2 - centerX, 0 - centerY, d / 2 - centerZ }, { 1.0f, 1.0f }, { 0.0f, 1.0f, 0.5f } }     // �E��
    };

    // ���_�f�[�^��slopeData�ɒǉ�
    for (int i = 0; i < 18; i++)
    {
        slopeData.vertices.push_back(vertices[i]);
    }

    // �C���f�b�N�X�f�[�^�i�ʂ̒�`�j
    slopeData.indexCount = 24;  // �C���f�b�N�X��

    UINT indices[] = {
        // ��ʁi�l�p�`�j
        0, 2, 1,  // �����A����A�E��
        1, 2, 3,  // �E���A����A�E��

        // ������
        4, 6, 5,  // �����ʎO�p�`

        // �E����
        7, 8, 9,  // �E���ʎO�p�`

        // �w��
        10, 12, 11,  // �w�ʎO�p�`
        11, 12, 13,

        // ��ʁi�Ζʁj
        14, 16, 15,  // �����̎O�p�`
        15, 16, 17   // �E���̎O�p�`
    };

    // �C���f�b�N�X�f�[�^��slopeData�ɒǉ�
    for (int i = 0; i < 24; i++)
    {
        slopeData.indices.push_back(indices[i]);
    }

    HRESULT hr;

    // ���_�o�b�t�@�ݒ�
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vsrData = {};
    vsrData.pSysMem = vertices;
    hr = Renderer::GetDevice()->CreateBuffer(&vbDesc, &vsrData, slopeData.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("slopeData.vertexBuffer �쐬���s�FGeometry::CreateSlope���Ŕ���", DebugConsole::ERROR_LOG);
    }

    // �C���f�b�N�X�o�b�t�@�ݒ�
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA isrData = {};
    isrData.pSysMem = indices;
    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &isrData, slopeData.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("slopeData.indexBuffer �쐬���s�FGeometry::CreateSlope���Ŕ���", DebugConsole::ERROR_LOG);
    }
}

// �~���b�V���쐬
void Geometry::CreateSpriteCircle()
{
    std::vector<Renderer::Vertex> vertices;
    std::vector<DWORD> indices;

    // �~�ׂ̍����i���_�̐��j
    const int detail = 16;

    // ���S�_�̒��_��ǉ��i�~�̒��S�j
    vertices.push_back(
    {
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),  // ���_���W
        DirectX::XMFLOAT2(0.5f, 0.5f),        // �e�N�X�`�����W
        DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)   // �@���x�N�g��
    });

    // �~����̒��_���v�Z���Ēǉ�
    for (int i = 0; i <= detail; ++i)
    {
        // ���݂̊p�x���v�Z�i�~����̈ʒu�j
        float angle = (float)i / detail * DirectX::XM_2PI;
        float x = 0.5f * cosf(angle);  // X���W�i���a0.5f�̉~�j
        float y = 0.5f * sinf(angle);  // Y���W

        // �v�Z�������_��ǉ�
        vertices.push_back(
        {
            DirectX::XMFLOAT3(x, y, 0.0f),          // ���_���W
            DirectX::XMFLOAT2((x / 0.5f + 1.0f) * 0.5f, (y / 0.5f + 1.0f) * 0.5f), // �e�N�X�`�����W
            DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f)     // �@���x�N�g�� (+Z����)
            
        });

        // �C���f�b�N�X��ǉ��i�~�̎O�p�`���\���j
        if (i < detail)
        {
            indices.push_back(0);      // ���S�_
            indices.push_back(i + 2);  // ���݂̉~����̒��_
            indices.push_back(i + 1);  // ���̉~����̒��_
        }
    }

    // �C���f�b�N�X����ݒ�
    spriteCircleData.indexCount = static_cast<int>(indices.size());

    HRESULT hr;

    // ���_�o�b�t�@�ݒ�
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
        DebugConsole::DebugLog("spriteCircleData.vertexBuffer �쐬���s�FGeometry::CreateSpriteCircle���Ŕ���", DebugConsole::ERROR_LOG);
    }

    // �C���f�b�N�X�o�b�t�@�ݒ�
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
        DebugConsole::DebugLog("spriteCircleData.indexBuffer �쐬���s�FGeometry::CreateSpriteCircle���Ŕ���", DebugConsole::ERROR_LOG);
    }

}

// ��`���b�V���쐬
void Geometry::CreateSquare()
{

    // ���_�f�[�^
    std::vector<Renderer::Vertex> vertices =
    {
        // �l�p�`�̒��_�f�[�^�i4���_�j
        { DirectX::XMFLOAT3(-0.5f,  0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },  // ����
        { DirectX::XMFLOAT3(0.5f,  0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },  // �E��
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },  // �E��
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },  // ����
    };

    // �C���f�b�N�X�f�[�^�i2�̎O�p�`�Ŏl�p�`��`���j
    std::vector<UINT> indices =
    {
        0, 1, 2,  // �ŏ��̎O�p�`
        0, 2, 3   // 2�Ԗڂ̎O�p�`
    };

    // �C���f�b�N�X����ݒ�
    squareData.indexCount = static_cast<int>(indices.size());

    HRESULT hr;

    // ���_�o�b�t�@�ݒ�
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
        DebugConsole::DebugLog("squareData.vertexBuffer �쐬���s�FGeometry::CreateSquare���Ŕ���", DebugConsole::ERROR_LOG);
    }

    // �C���f�b�N�X�o�b�t�@�ݒ�
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
        DebugConsole::DebugLog("squareData.indexBuffer �쐬���s�FGeometry::CreateSquare���Ŕ���", DebugConsole::ERROR_LOG);
    }
}

void Geometry::CreateTorus()
{
    // ���_�f�[�^�ƃC���f�b�N�X�f�[�^���i�[����R���e�i
    std::vector<Renderer::Vertex> vertices;
    std::vector<UINT> indices;

    int ringSegments = 64;
    int tubeSegments = 32;
    float innerRadius = 0.01f;
    float outerRadius = 0.5f;

    for (int i = 0; i <= ringSegments; ++i)
    {
        float theta = (float)i / ringSegments * XM_2PI; // �O���p�x
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        for (int j = 0; j <= tubeSegments; ++j)
        {
            float phi = (float)j / tubeSegments * XM_2PI; // �����p�x
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);

            // ���_�̈ʒu
            float x = (outerRadius + innerRadius * cosPhi) * cosTheta;
            float y = innerRadius * sinPhi;
            float z = (outerRadius + innerRadius * cosPhi) * sinTheta;

            // �@���x�N�g��
            DirectX::XMFLOAT3 normal(cosPhi * cosTheta, sinPhi, cosPhi * sinTheta);

            // UV���W
            DirectX::XMFLOAT2 uv((float)i / ringSegments, (float)j / tubeSegments);

            vertices.emplace_back(Renderer::Vertex{ {x, y, z}, uv, normal });
        }
    }

    // �g�[���X�̃C���f�b�N�X����
    for (int i = 0; i < ringSegments; ++i)
    {
        for (int j = 0; j < tubeSegments; ++j) 
        {
            int current = i * (tubeSegments + 1) + j;
            int next = current + tubeSegments + 1;

            // �O�p�`1
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // �O�p�`2
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    torusData.indexCount = 6 * ringSegments * tubeSegments;


    HRESULT hr;

    // ���_�o�b�t�@�ݒ�
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
        DebugConsole::DebugLog("torusData.vertexBuffer �쐬���s�FGeometry::CreateTorus���Ŕ���", DebugConsole::ERROR_LOG);
    }

    // �C���f�b�N�X�o�b�t�@�ݒ�
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
        DebugConsole::DebugLog(" torusData.indexBuffer �쐬���s�FGeometry::CreateTorus���Ŕ���", DebugConsole::ERROR_LOG);
    }
}

// ��`���b�V���i�����t���A�߂�lModel�j
Renderer::Model Geometry::CreateSquare(DirectX::XMFLOAT2 center, DirectX::XMFLOAT2 size, DirectX::XMFLOAT2 uv)
{
    float l = center.x - size.x / 2.0f;
    float r = l + size.x;
    float t = center.y + size.y / 2.0f;
    float b = t - size.y;

    Renderer::Vertex vertexList[] =
    {
        // ���_�����v���ɎO�p�`���`�����鑤���|���S���̕\�ɂȂ�
        { {l, t, 0.0f}, {0.00f, 0.00f} },    // 0�Ԗڂ̒��_���W�@{ x, y, z, u, v }
        { {r, b, 0.0f}, {uv.x, uv.y} },      // 1�Ԗڂ̒��_���W
        { {l, b, 0.0f}, {0.00f, uv.y} },     // 2�Ԗڂ̒��_���W

        { {l, t, 0.0f}, {0.00f, 0.00f} },    // 3�Ԗڂ̒��_
        { {r, t, 0.0f}, {uv.x, 0.00f} },     // 4�Ԗڂ̒��_
        { {r, b, 0.0f}, {uv.x, uv.y} },      // 5�Ԗڂ̒��_
    };

    HRESULT hr;

    // ���_�o�b�t�@���쐬����
    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(vertexList);       // �m�ۂ���o�b�t�@�T�C�Y���w��
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���_�o�b�t�@�쐬���w��
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subResourceData;
    subResourceData.pSysMem = vertexList;           // VRAM�ɑ���f�[�^���w��
    subResourceData.SysMemPitch = 0;
    subResourceData.SysMemSlicePitch = 0;

    Renderer::Model model = {};
    // ���_�o�b�t�@�쐬���āA�����ϐ��i��R�����j�ɕۑ�
    hr = Renderer::GetDevice()->CreateBuffer(&bufferDesc, &subResourceData, model.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("model.vertexBuffer �쐬���s�FGeometry::CreateSquare���Ŕ���", DebugConsole::ERROR_LOG);
    }

    UINT indices[] = { 0,1,2,3,4,5 };
    D3D11_BUFFER_DESC ibDesc;
    ibDesc.ByteWidth = sizeof(indices);            //2�o�C�g�~��
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;
    ibDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA irData;
    irData.pSysMem = indices;                     //�z��̐擪�A�h���X
    irData.SysMemPitch = 0;
    irData.SysMemSlicePitch = 0;

    hr = Renderer::GetDevice()->CreateBuffer(&ibDesc, &irData, model.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        DebugConsole::DebugLog("model.indexBuffer �쐬���s�FGeometry::CreateSquare���Ŕ���", DebugConsole::ERROR_LOG);
    }

    // �C���f�b�N�X����ۑ�
    model.numIndex = sizeof(indices) / sizeof(indices[0]);
    
    return model;
}
void Geometry::Release()
{
}