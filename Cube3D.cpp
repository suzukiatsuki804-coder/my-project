#include "Cube3D.h"
#include <DirectXMath.h>
#include "Camera.h"
#include <d3dcompiler.h>
#include "GameObject.h"
#pragma comment(lib, "d3dcompiler.lib")


using namespace DirectX;
namespace Cube
{
    struct Vertex_3D
    {
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT2 texcoord;
    };


    Vertex_3D vertices[] =
    {
        {{-1,-1,-1},{0,0,0},{0,0}},
        {{ 1,-1,-1},{0,0,0},{1,0}},
        {{ 1, 1,-1},{0,0,0},{1,1}},
        {{-1, 1,-1},{0,0,0},{0,1}},
        {{-1,-1, 1},{0,0,0},{0,0}},
        {{ 1,-1, 1},{0,0,0},{1,0}},
        {{ 1, 1, 1},{0,0,0},{1,1}},
        {{-1, 1, 1},{0,0,0},{0,1}},
    };

    uint16_t indices[] =
    {
        0,1,2, 0,2,3,
        1,5,6, 1,6,2,
        5,4,7, 5,7,6,
        4,0,3, 4,3,7,
        3,2,6, 3,6,7,
        4,5,1, 4,1,0
    };

    // 定数バッファ用の構造体
    struct CBTransform
    {
        XMMATRIX wvp;
        XMMATRIX world;
    };

    ID3D11Buffer* g_pVertexBuffer = nullptr;        // 頂点バッファ
    ID3D11Buffer* g_pIndexBuffer = nullptr;         // インデックスバッファ
    ID3D11Buffer* g_pConstantBuffer = nullptr;      // 定数バッファ
    ID3D11ShaderResourceView* g_pTexture = nullptr; // テクスチャ

    ID3D11VertexShader* g_pVertexShader = nullptr;
    ID3D11PixelShader* g_pPixelShader = nullptr;
    ID3D11InputLayout* g_pInputLayout = nullptr;

    // 注意！初期化で外部から設定されるもの。Release不要。
    ID3D11Device* g_pDevice = nullptr;
    ID3D11DeviceContext* g_pContext = nullptr;

    XMFLOAT3 GetPosition()
    {
        return XMFLOAT3(0,0,0);
    }

    void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    {
        g_pDevice = pDevice;
        g_pContext = pContext;

        // 初期化
        for (auto& v : vertices)
            v.normal = XMFLOAT3(0, 0, 0);

        // 面ごと
        for (int i = 0; i < _countof(indices); i += 3)
        {
            auto& v0 = vertices[indices[i + 0]];
            auto& v1 = vertices[indices[i + 1]];
            auto& v2 = vertices[indices[i + 2]];
            // 何か法線がスムースになるおまじない
            XMVECTOR p0 = XMLoadFloat3(&v0.position);
            XMVECTOR p1 = XMLoadFloat3(&v1.position);
            XMVECTOR p2 = XMLoadFloat3(&v2.position);
            
            XMVECTOR n = XMVector3Normalize(
                XMVector3Cross(p2 - p0, p1 - p0)
            );

            XMStoreFloat3(&v0.normal, XMLoadFloat3(&v0.normal) + n);
            XMStoreFloat3(&v1.normal, XMLoadFloat3(&v1.normal) + n);
            XMStoreFloat3(&v2.normal, XMLoadFloat3(&v2.normal) + n);
        }

        // 最後に正規化
        for (auto& v : vertices)
        {
            XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&v.normal));
            XMStoreFloat3(&v.normal, n);
        }



        ID3DBlob* pVSBlob = nullptr;
        ID3DBlob* pErrorBlob = nullptr;
        HRESULT hr;
        hr = D3DCompileFromFile(
            L"shader_vertex_3d.hlsl",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main",          // エントリポイント
            "vs_5_0",        // Vertex Shader 5.0
            0,
            0,
            &pVSBlob,
            &pErrorBlob
        );
        pDevice->CreateVertexShader(
            pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(),
            nullptr,
            &g_pVertexShader
        );
        ID3DBlob* pPSBlob = nullptr;
        hr = D3DCompileFromFile(
            L"shader_pixel_3d.hlsl",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main",
            "ps_5_0",        // Pixel Shader 5.0
            0,
            0,
            &pPSBlob,
            &pErrorBlob);
        // ピクセルシェーダ作成
        pDevice->CreatePixelShader(
            pPSBlob->GetBufferPointer(),
            pPSBlob->GetBufferSize(),
            nullptr,
            &g_pPixelShader
        );
        // 入力レイアウトの作成
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            //XMFLOAT3と同じサイズのフォーマットの指定をする
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0,
              D3D11_INPUT_PER_VERTEX_DATA, 0 },

              //XMFLOAT3と同じサイズのフォーマットの指定をする
              { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
                D3D11_INPUT_PER_VERTEX_DATA, 0 },

                //XMFLOAT2と同じサイズのフォーマットの指定をする
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24,
                  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        pDevice->CreateInputLayout(
            layout,
            _countof(layout),
            pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(),
            &g_pInputLayout);

        pVSBlob->Release();
        pPSBlob->Release();


        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.Usage = D3D11_USAGE_DEFAULT;
        vbDesc.ByteWidth = sizeof(vertices);
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vbData = {};
        vbData.pSysMem = vertices;

        pDevice->CreateBuffer(&vbDesc, &vbData, &g_pVertexBuffer);
        // インデックスバッファの作成
        D3D11_BUFFER_DESC ibDesc = {};
        ibDesc.Usage = D3D11_USAGE_DEFAULT;
        ibDesc.ByteWidth = sizeof(indices);
        ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA ibData = {};
        ibData.pSysMem = indices;

        pDevice->CreateBuffer(&ibDesc, &ibData, &g_pIndexBuffer);
        // 定数バッファの作成
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.ByteWidth = sizeof(CBTransform);
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = 0;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;

        pDevice->CreateBuffer(&cbDesc, nullptr, &g_pConstantBuffer);
    }

    void Finalize(void)
    {
    }

    void Update(void)
    {
    }

    void Draw(void)
    {
        // 行列を作る
        // ワールド行列は単位行列(デフォルトのまま）
        XMMATRIX world = XMMatrixIdentity();

		// 0427:ビュー行列とプロジェクション行列はカメラからもらう
        XMMATRIX view, proj;
		Camera::GetCameraMatrix(view, proj);

        CBTransform cb;
        // Transform情報を行列計算で設定する
        cb.wvp = XMMatrixTranspose(world * view * proj);
        cb.world = world;
        // 定数バッファ更新
        g_pContext->UpdateSubresource(
            g_pConstantBuffer, 0, nullptr, &cb, 0, 0
        );
        // VSの b0 にセット
        g_pContext->VSSetConstantBuffers(
            0, 1, &g_pConstantBuffer
        );

        UINT stride = sizeof(Vertex_3D);
        UINT offset = 0;

        g_pContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
        g_pContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        g_pContext->IASetInputLayout(g_pInputLayout);

        g_pContext->VSSetShader(g_pVertexShader, nullptr, 0);
        g_pContext->PSSetShader(g_pPixelShader, nullptr, 0);
        g_pContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
        // 描画コマンド実行
        g_pContext->DrawIndexed(36, 0, 0);
    }


}