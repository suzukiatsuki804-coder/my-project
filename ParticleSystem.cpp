#include "ParticleSystem.h"
#include <cstdlib>
#include <ctime>
#include <d3dcompiler.h>
#include "Texture.h"

#pragma comment(lib, "d3dcompiler.lib")


// ビルボード行列作成
XMMATRIX MakeBillboardMatrix(const XMFLOAT3& pos, const XMMATRIX& view)
{
    // ビュー行列の逆行列を作る
    XMMATRIX rot = view;
    rot.r[3] = XMVectorSet(0, 0, 0, 1);
    XMMATRIX invView = XMMatrixInverse(nullptr, rot);

    // 移動成分を消す（回転だけにする）
    invView.r[3] = XMVectorSet(0, 0, 0, 1);
    float rd = (rand() % 200 - 100) / 1000.0f;
    XMMATRIX scale = XMMatrixScaling(rd, rd, rd);
    // 平行移動
    XMMATRIX trans = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // ワールド行列
    return scale * invView * trans;
}

void ParticleSystem::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    device = pDevice;
    context = pDeviceContext;

    particles.resize(MAX_PARTICLE);
    for (auto& p : particles)
    {
        p.life = 0.0f; // 初期状態は「死んでる」
    }

    // 頂点バッファ作成（クワッド）
    ParticleVertex vertices[] = {
        { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.5f, 0.5f, 0.0f }, { 1.0f, 0.0f } },
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } }
    };

    D3D11_BUFFER_DESC bufDesc = {};
    bufDesc.Usage = D3D11_USAGE_DEFAULT;
    bufDesc.ByteWidth = sizeof(vertices);
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;

    device->CreateBuffer(&bufDesc, &initData, &vertexBuffer);

    // 定数バッファ作成
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(ConstantBuffer);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = 0;

    device->CreateBuffer(&cbDesc, nullptr, &constBuffer);

    // ブレンドステート作成（加算合成）
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    device->CreateBlendState(&blendDesc, &blendState);

    // サンプラーステート作成
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ID3D11SamplerState* samplerState = nullptr;
    device->CreateSamplerState(&samplerDesc, &samplerState);
    context->PSSetSamplers(0, 1, &samplerState);
    if (samplerState) samplerState->Release();

    // 頂点シェーダーコンパイル
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(
        L"shader_vertex_particle.hlsl",
        nullptr,
        nullptr,
        "main",
        "vs_5_0",
        0,
        0,
        &vsBlob,
        &errorBlob
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return;
    }

    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);

    // 入力レイアウト作成
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }

    };

    device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &inputLayout
    );

    vsBlob->Release();

    // ピクセルシェーダーコンパイル
    ID3DBlob* psBlob = nullptr;
    errorBlob = nullptr;
    hr = D3DCompileFromFile(
        L"shader_pixel_particle.hlsl",
        nullptr,
        nullptr,
        "main",
        "ps_5_0",
        0,
        0,
        &psBlob,
        &errorBlob
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return;
    }

    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();

    // particle02.pngテクスチャ読み込み
    hr = CreateTextureFromFile(device, L"Texture\\particle02.png", &particleTexture);
    if (FAILED(hr))
    {
        OutputDebugStringA("particle02.pngの読み込みに失敗しました\n");
    }

    srand((unsigned int)time(nullptr));
}

void ParticleSystem::Update()
{
    float dt = 0.016f;

    for (auto& p : particles)
    {
        if (p.life <= 0.0f) continue;

        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.pos.z += p.vel.z * dt;

        p.vel.y -= 2.0f * dt;

        p.life -= 0.01f;

        if (p.life <= 0.0f)
        {
            p.life = 0.0f;
        }

        // アルファを寿命に応じてフェードアウト
        p.alpha = (p.life / 1.0f) * 0.3f;  // ← 修正
        if (p.alpha <= 0.0f)
        {
            p.alpha = 0.0f;
        }
    }
    Emit();  //6.24コメントアウト
    if (isLoop)
    {
       
    }
}




void ParticleSystem::Emit(bool loop)
{
    isLoop = loop;
    // 初期テストでは複数生成させる
    for (int i = 0; i < PARTICLE_PER_FLAME; ++i)  // 5個生成
    {
        for (auto& p : particles)
        {
            if (p.life <= 0.0f)
            {
                p.pos = m_basePosition;

                //粒をいじる
                p.vel = {
                    (rand() % 200 - 100) / 100.0f,
                   (rand() % 100) / 40.0f + 1.0f,
                    (rand() % 200 - 100) / 100.0f
                    
                };

                p.life = 1.0f;
                p.alpha = (rand() % 50 + 50) / 100.0f;

                break;
            }
        }
    }
   
}




void ParticleSystem::Draw(const XMMATRIX& view, const XMMATRIX& proj)
{
    // シェーダー設定
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // 入力レイアウト設定
    context->IASetInputLayout(inputLayout);

    // プリミティブトポロジー設定
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // ブレンドステート設定       
    float blendFactor[4] = { 0, 0, 0, 0 };
    context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    // 頂点バッファ設定
    UINT stride = sizeof(ParticleVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    for (auto& p : particles)
    {
        if (p.life <= 0.0f) continue;

        // ビルボード行列
        XMMATRIX world = MakeBillboardMatrix(p.pos, view);

        // 定数バッファに送る
        ConstantBuffer cb = {};
        cb.world = XMMatrixTranspose(world);
        cb.view = XMMatrixTranspose(view);
        cb.proj = XMMatrixTranspose(proj);
        cb.padding[0] = p.alpha;

        context->UpdateSubresource(constBuffer, 0, nullptr, &cb, 0, 0);
        context->VSSetConstantBuffers(0, 1, &constBuffer);

        // テクスチャ設定
        if (particleTexture)
        {
            context->PSSetShaderResources(0, 1, &particleTexture);
        }

        // 描画（quad）
        context->Draw(4, 0);
    }

    // リソースをクリア
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(0, 1, &nullSRV);

    // デフォルトに戻す
    context->OMSetBlendState(nullptr, blendFactor, 0xffffffff);

    // デフォルトの描画設定にリセット
    context->VSSetShader(nullptr, nullptr, 0);
    context->PSSetShader(nullptr, nullptr, 0);
    context->IASetInputLayout(nullptr);
}

void ParticleSystem::Uninit()
{
    if (vertexBuffer) vertexBuffer->Release();
    if (constBuffer) constBuffer->Release();
    if (blendState) blendState->Release();
    if (particleTexture) particleTexture->Release();
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (inputLayout) inputLayout->Release();
}

void ParticleSystem::SetParticleTexture(ID3D11ShaderResourceView* texture)
{
    particleTexture = texture;

    // 既存のブレンドステートをリリース
    if (blendState)
    {
        blendState->Release();
        blendState = nullptr;
    }

    // ブレンドステート作成（アルファブレンド）
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    device->CreateBlendState(&blendDesc, &blendState);
}

void ParticleSystem::SetBasePosition(XMFLOAT3 pos)
{
    m_basePosition = pos;
}
