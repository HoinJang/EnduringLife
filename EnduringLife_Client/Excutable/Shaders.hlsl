cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint			gnMaterial : packoffset(c4);
};

cbuffer cbTextureObjectInfo : register(b5)
{
    matrix gTexTransform : packoffset(c0);
}
//캐릭터 애니메이션
cbuffer cbSkinned : register(b6)
{
	// 한 캐릭터 당 최대 뼈대 개수
    matrix gBoneTransform[128] : packoffset(c0);
};
//HP정보
cbuffer cbHPInfo : register(b7)
{
    float gHPratio : packoffset(c0);
}

#include "Light.hlsl"

struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);

SamplerState gWrapSamplerState : register(s0);
SamplerState  gClampSamplerState : register(s1);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);

	return(cColor);
}

//#define _WITH_VERTEX_LIGHTING

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
	return(cIllumination);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#endif
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.uv);
    

#ifdef _WITH_VERTEX_LIGHTING
    float4 cIllumination = input.color;
#else
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
#endif
    return (lerp(cColor, cIllumination, 0.1f));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------
// Transforms a normal map sample to world space.
//---------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMapSample - 1.0f;

	// Build orthonormal basis.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}



//---------------------------------------------------------------------------------------


Texture2D gtxtModelTexture : register(t9);
Texture2D gtxtNormalMapTexture : register(t10);

struct VS_TEXTURED_NORMALMAPPED_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct VS_TEXTURED_NORMALMAPPED_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
	//	nointerpolation float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#endif
};

VS_TEXTURED_NORMALMAPPED_LIGHTING_OUTPUT VSTexturedNormalMappedLighting(VS_TEXTURED_NORMALMAPPED_LIGHTING_INPUT input)
{
    VS_TEXTURED_NORMALMAPPED_LIGHTING_OUTPUT output;

    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
    output.uv = input.uv;
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}

float4 PSTexturedNormalMappedLighting(VS_TEXTURED_NORMALMAPPED_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifndef _WITH_VERTEX_LIGHTING
   // 노말 매핑 끄면 요거 on
   //  float4 cIllumination = input.color;
    float4 normalMapSample = gtxtNormalMapTexture.Sample(gWrapSamplerState, input.uv);
    input.normalW = normalize(input.normalW);

    float3 bumpedNormal = NormalSampleToWorldSpace(normalMapSample.rgb, input.normalW, input.tangentW);

    float4 cIllumination = Lighting(input.positionW, bumpedNormal);
#endif

     float4 cColor = gtxtModelTexture.Sample(gWrapSamplerState, input.uv);
     return (lerp(cColor, cIllumination, 0.5f));
    //return cIllumination;
    //return cColor;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

Texture2D gtxtTerrainBaseTexture : register(t6);
Texture2D gtxtTerrainDetailTexture : register(t7); //서술자 테이블
//Texture2D gtxNormalMap; register(t3);
//SamplerState  gNormalSamplerState : register(s2);



struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
    //float3 normal : NORMAL;
	float2 uv0 : TEXCOORD;
	float2 uv1 : DETAIL;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
   // float3 normalW : NORMAL;
	float2 uv0 : TEXCOORD;
    float2 uv1 : DETAIL;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

#ifdef _WITH_CONSTANT_BUFFER_SYNTAX
	output.position = mul(mul(mul(float4(input.position, 1.0f), gcbGameObjectInfo.mtxWorld), gcbCameraInfo.mtxView), gcbCameraInfo.mtxProjection);
#else
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
#endif
	output.color = input.color;
    output.uv0 = input.uv0; //basetext
    output.uv1 = input.uv1; //detailtxt
//
    //output.normalW = mul(input.normal, (float3x3) gmtxGameObject);

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gWrapSamplerState, input.uv0);
	float4 cDetailTexColor = gtxtTerrainDetailTexture.Sample(gWrapSamplerState, input.uv1);
	float4 cColor = input.color * saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));
    
    return (cColor);

//    float4 cIllumination = Lighting(input.positionW, input.normalW);

  //  return (lerp(cColor, cIllumination, 0.5f));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtSkyBox : register(t8);

float4 PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyBox.Sample(gClampSamplerState, input.uv);

	return(cColor);
}

///////////////////////////////////////////////////////////////////
Texture2D gtxtBillboard : register(t1);


VS_TEXTURED_OUTPUT VSBillboardTextured(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;
    return (output);
}

float4 PSBillboardTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtBillboard.Sample(gWrapSamplerState, input.uv);
    if ((cColor.r<0.2) && (cColor.g <0.2 ) && (cColor.b <0.2))  discard;
    else  if ((cColor.r > 0.9) && (cColor.g > 0.9) && (cColor.b > 0.9)) discard;

    return (cColor);
}
/////////////////////////////////////////////////////////////////////////
Texture2D gtxtWaterBaseTexture : register(t2);
Texture2D gtxtWaterDetailTexture : register(t3);

SamplerState gMirrorSamplerState : register(s2);

struct VS_WATER_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct VS_WATER_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

VS_WATER_TEXTURED_OUTPUT VSWaterTextured(VS_WATER_TEXTURED_INPUT input)
{
    VS_WATER_TEXTURED_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
    //output.uv0 =mul(input.uv0, 1.f);

    output.uv0 = mul(mul(float4(input.uv0, 0.0f, 1.0f), gTexTransform).xy, 5.0f);

    output.uv1 = mul(input.uv1, 5.0f);

    return (output);
}

float4 PSWaterTextured(VS_WATER_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtWaterBaseTexture.Sample(gWrapSamplerState, input.uv0);
    float4 cDetailTexColor = gtxtWaterDetailTexture.Sample(gWrapSamplerState, input.uv1);
    float4 cColor = saturate((cBaseTexColor * 0.5f) + (cDetailTexColor * 0.5f));

    return (cColor);
}


////////////////////////////////////////////////////////////////////
//
struct VS_SKINNED_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float4 weights : WEIGHTS;
    float4 boneindices : BONEINDICES;
};

struct VS_SKINNED_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 texCoord : TEXCOORD0;
};

VS_SKINNED_OUTPUT VSSkinned(VS_SKINNED_INPUT input)
{
    VS_SKINNED_OUTPUT output;

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.weights.x;
    weights[1] = input.weights.y;
    weights[2] = input.weights.z;
    weights[3] = 1 - weights[0] - weights[1] - weights[2];

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; i++)
    {
        posL += weights[i] * mul(float4(input.position, 1.0f), gBoneTransform[input.boneindices[i]]).xyz;
        normalL += weights[i] * mul(input.normal, (float3x3) gBoneTransform[input.boneindices[i]]);
    }
	
    output.positionW = mul(float4(posL, 1.0f), gmtxGameObject).xyz;
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.normalW = mul(normalL, (float3x3) gmtxGameObject);
    output.texCoord = input.texCoord;
    
    return output;
}


float4 PSSkinned(VS_SKINNED_OUTPUT input) : SV_Target
{
    input.normalW = normalize(input.normalW);
    float4 cIllumination = Lighting(input.positionW, input.normalW);
    float4 cColor = gtxtTexture.Sample(gWrapSamplerState, input.texCoord);// * cIllumination;

    return (lerp(cColor, cIllumination, 0.1f));
   // return (cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

VS_TEXTURED_OUTPUT VSHPBar(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = float4(input.position.x * gHPratio + (0.5f * gHPratio) - 1.f, input.position.y - 0.7f, input.position.z, 1.0f);
    
    output.uv = input.uv;
    return (output);
}

float4 PSHPBar(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtBillboard.Sample(gWrapSamplerState, input.uv);
	

    return (cColor);
}

