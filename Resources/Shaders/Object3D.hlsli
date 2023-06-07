cbuffer ConstBufferData3D : register(b0)
{
    matrix viewproj;// --> ビュープロジェクション行列
    matrix world;// -----> ワールド行列
    float3 cameraPos;// -> カメラ座標(ワールド座標)
    float4 color;// -----> 色(RGBA)
}

cbuffer MaterialData : register(b1)
{
    float3 m_ambient : packoffset(c0); // --> アンビエント係数
    float3 m_diffuse : packoffset(c1); // --> ディフェーズ係数
    float3 m_specular : packoffset(c2); // -> スペキュラー係数
    float m_alpha : packoffset(c2.w); // ---> アルファ
}

static const int DIRLIGHT_NUM = 3;
static const int POINTLIGHT_NUM = 3;
static const int SPOTLIGHT_NUM = 3;
static const int CIRCLESHADOW_NUM = 1;

struct DirLight
{
    float3 lightv; // -> ライトへの方向の単位ベクトル
    float3 lightcolor; // -> ライトの色(RGB)
    uint active;
};

struct PointLight
{
    float3 lightpos; // -> ライト座標
    float3 lightcolor; // -> ライトの色(RGB)
    float3 lightatten; // -> ライト距離減衰係数
    uint active;
};

struct SpotLight
{
    float3 lightv; // -> ライトの光線方向の逆ベクトル
    float3 lightpos; // -> ライト座標
    float3 lightcolor; // -> ライトの色(RGB)
    float3 lightatten; // -> ライト距離減衰係数
    float2 lightfactoranglecos; // -> ライト減衰角度のコサイン
    uint active;
};

struct CircleShadow
{
    float3 dir; // -> 投影方向逆ベクトル(単位ベクトル)
    float3 casterPos; // -> キャスター座標
    float distanceCasterLight; // -> キャスターとライトの距離
    float3 atten; // -> 距離減衰係数
    float2 factorAngleCos; // -> 減衰角度のコサイン
    uint active;
};

cbuffer cbuff2 : register(b2)
{
    float3 ambientColor;
    DirLight dirLights[DIRLIGHT_NUM];
    PointLight pointLights[POINTLIGHT_NUM];
    SpotLight spotLights[SPOTLIGHT_NUM];
    CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// --頂点シェーダーの出力構造体-- //
// --頂点シェーダーからピクセルシェーダーへのやり取りに使用する-- //
struct VSOutput
{
    float4 svPos : SV_POSITION; // -> システム用頂点座標
    float4 worldPos : POSITION; // -> ワールド座標
    float3 normal : NORMAL; // -----> 法線
    float4 color : COLOR;// --------> 色(RGBA)
    float2 uv : TEXCOORD;// --------> UV値
};