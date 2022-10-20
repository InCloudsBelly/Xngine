
#include "Xpch.h"

#include "Runtime/Platform/DirectX11/RenderStates.h"
#include "Runtime/Platform/DirectX11/DxTrace.h"

namespace X
{
	using namespace Microsoft::WRL;

	ComPtr<ID3D11RasterizerState> RenderStates::RSNoCull = nullptr;
	ComPtr<ID3D11RasterizerState> RenderStates::RSWireframe = nullptr;
	ComPtr<ID3D11RasterizerState> RenderStates::RSCullClockWise = nullptr;
	ComPtr<ID3D11RasterizerState> RenderStates::RSDepth = nullptr;

	ComPtr<ID3D11SamplerState> RenderStates::SSPointClamp = nullptr;
	ComPtr<ID3D11SamplerState> RenderStates::SSAnisotropicWrap = nullptr;
	ComPtr<ID3D11SamplerState> RenderStates::SSLinearWrap = nullptr;
	ComPtr<ID3D11SamplerState> RenderStates::SSShadow = nullptr;

	ComPtr<ID3D11BlendState> RenderStates::BSAlphaToCoverage = nullptr;
	ComPtr<ID3D11BlendState> RenderStates::BSNoColorWrite = nullptr;
	ComPtr<ID3D11BlendState> RenderStates::BSTransparent = nullptr;
	ComPtr<ID3D11BlendState> RenderStates::BSAdditive = nullptr;
	ComPtr<ID3D11BlendState> RenderStates::BSAlphaWeightedAdditive = nullptr;

	ComPtr<ID3D11DepthStencilState> RenderStates::DSSEqual = nullptr;
	ComPtr<ID3D11DepthStencilState> RenderStates::DSSLessEqual = nullptr;
	ComPtr<ID3D11DepthStencilState> RenderStates::DSSWriteStencil = nullptr;
	ComPtr<ID3D11DepthStencilState> RenderStates::DSSDrawWithStencil = nullptr;
	ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDoubleBlend = nullptr;
	ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDepthTest = nullptr;
	ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDepthWrite = nullptr;
	ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDepthTestWithStencil = nullptr;
	ComPtr<ID3D11DepthStencilState> RenderStates::DSSNoDepthWriteWithStencil = nullptr;

	bool RenderStates::IsInit()
	{
		// һ����˵��ʼ������������е�״̬����������
		return RSWireframe != nullptr;
	}

	void RenderStates::InitAll(ID3D11Device* device)
	{
		// ��ǰ��ʼ�����Ļ���û��Ҫ������
		if (IsInit())
			return;

		// ******************
		// ��ʼ����դ����״̬
		//
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

		// �߿�ģʽ
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthClipEnable = true;
		HR(device->CreateRasterizerState(&rasterizerDesc, RSWireframe.GetAddressOf()));
		
		// �ޱ����޳�ģʽ
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthClipEnable = true;
		HR(device->CreateRasterizerState(&rasterizerDesc, RSNoCull.GetAddressOf()));

		// ˳ʱ���޳�ģʽ
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.DepthClipEnable = true;
		HR(device->CreateRasterizerState(&rasterizerDesc, RSCullClockWise.GetAddressOf()));

		// ���ƫ��ģʽ
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.DepthBias = 100000;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.SlopeScaledDepthBias = 1.0f;
		HR(device->CreateRasterizerState(&rasterizerDesc, RSDepth.GetAddressOf()));

		// ******************
		// ��ʼ��������״̬
		//
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));

		// �������Clampģʽ
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HR(device->CreateSamplerState(&sampDesc, SSPointClamp.GetAddressOf()));

		// ���Թ�����Wrapģʽ
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HR(device->CreateSamplerState(&sampDesc, SSLinearWrap.GetAddressOf()));

		// �������Թ�����Wrapģʽ
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MaxAnisotropy = 4;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HR(device->CreateSamplerState(&sampDesc, SSAnisotropicWrap.GetAddressOf()));

		// ������״̬����ȱȽ���Borderģʽ
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		sampDesc.BorderColor[0] = { 1.0f };
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HR(device->CreateSamplerState(&sampDesc, SSShadow.GetAddressOf()));

		// ******************
		// ��ʼ�����״̬
		//
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		auto& rtDesc = blendDesc.RenderTarget[0];
		// Alpha-To-Coverageģʽ
		blendDesc.AlphaToCoverageEnable = true;
		blendDesc.IndependentBlendEnable = false;
		rtDesc.BlendEnable = false;
		rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HR(device->CreateBlendState(&blendDesc, BSAlphaToCoverage.GetAddressOf()));

		// ͸�����ģʽ
		// Color = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor 
		// Alpha = SrcAlpha
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		rtDesc.BlendEnable = true;
		rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

		HR(device->CreateBlendState(&blendDesc, BSTransparent.GetAddressOf()));

		// �ӷ����ģʽ
		// Color = SrcColor + DestColor
		// Alpha = SrcAlpha
		rtDesc.SrcBlend = D3D11_BLEND_ONE;
		rtDesc.DestBlend = D3D11_BLEND_ONE;
		rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

		HR(device->CreateBlendState(&blendDesc, BSAdditive.GetAddressOf()));

		// ��AlphaȨ�صļӷ����ģʽ
		// Color = SrcAlpha * SrcColor + DestColor
		// Alpha = SrcAlpha
		rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtDesc.DestBlend = D3D11_BLEND_ONE;
		rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

		HR(device->CreateBlendState(&blendDesc, BSAlphaWeightedAdditive.GetAddressOf()));

		// ����ɫд����ģʽ
		// Color = DestColor
		// Alpha = DestAlpha
		rtDesc.BlendEnable = false;
		rtDesc.SrcBlend = D3D11_BLEND_ZERO;
		rtDesc.DestBlend = D3D11_BLEND_ONE;
		rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
		rtDesc.DestBlendAlpha = D3D11_BLEND_ONE;
		rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtDesc.RenderTargetWriteMask = 0;
		HR(device->CreateBlendState(&blendDesc, BSNoColorWrite.GetAddressOf()));

		// ******************
		// ��ʼ�����/ģ��״̬
		//
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof dsDesc);
		// ���������ֵһ�µ����ؽ���д������/ģ��״̬
		// û��Ҫд�����
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_EQUAL;

		HR(device->CreateDepthStencilState(&dsDesc, DSSEqual.GetAddressOf()));

		// ����ʹ�����ֵһ�µ����ؽ����滻�����/ģ��״̬
		// ��״̬���ڻ�����պУ���Ϊ���ֵΪ1.0ʱĬ���޷�ͨ����Ȳ���
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		HR(device->CreateDepthStencilState(&dsDesc, DSSLessEqual.GetAddressOf()));

		// д��ģ��ֵ�����/ģ��״̬
		// ���ﲻд�������Ϣ
		// ���������滹�Ǳ��棬ԭ��ָ���������ģ��ֵ���ᱻд��StencilRef
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// ���ڱ���ļ����������ǲ�������Ⱦ�ģ���������������޹ؽ�Ҫ
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		HR(device->CreateDepthStencilState(&dsDesc, DSSWriteStencil.GetAddressOf()));

		// ��ָ��ģ��ֵ���л��Ƶ����/ģ��״̬
		// ������ģ��ֵ����������Ž��л��ƣ����������
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		// ���ڱ���ļ����������ǲ�������Ⱦ�ģ���������������޹ؽ�Ҫ
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		HR(device->CreateDepthStencilState(&dsDesc, DSSDrawWithStencil.GetAddressOf()));

		// �޶��λ�����/ģ��״̬
		// ����Ĭ����Ȳ���
		// ͨ���Ե���ʹ��ԭ��StencilRef��ֵֻ��ʹ��һ�Σ�ʵ�ֽ�һ�λ��
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		// ���ڱ���ļ����������ǲ�������Ⱦ�ģ���������������޹ؽ�Ҫ
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		HR(device->CreateDepthStencilState(&dsDesc, DSSNoDoubleBlend.GetAddressOf()));

		// �ر���Ȳ��Ե����/ģ��״̬
		// �����Ʒ�͸�����壬����ϸ��ջ���˳��
		// ����͸����������Ҫ���Ļ���˳��
		// ��Ĭ�������ģ����Ծ��ǹرյ�
		dsDesc.DepthEnable = false;
		dsDesc.StencilEnable = false;

		HR(device->CreateDepthStencilState(&dsDesc, DSSNoDepthTest.GetAddressOf()));


		// �ر���Ȳ���
		// �����Ʒ�͸�����壬����ϸ��ջ���˳��
		// ����͸����������Ҫ���Ļ���˳��
		// ������ģ��ֵ����������Ž��л���
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		// ���ڱ���ļ����������ǲ�������Ⱦ�ģ���������������޹ؽ�Ҫ
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		HR(device->CreateDepthStencilState(&dsDesc, DSSNoDepthTestWithStencil.GetAddressOf()));

		// ������Ȳ��ԣ�����д�����ֵ��״̬
		// �����Ʒ�͸������ʱ��Ӧʹ��Ĭ��״̬
		// ����͸������ʱ��ʹ�ø�״̬������Чȷ�����״̬�Ľ���
		// ����ȷ����ǰ�ķ�͸����������赲�Ϻ��һ������
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = false;

		HR(device->CreateDepthStencilState(&dsDesc, DSSNoDepthWrite.GetAddressOf()));


		// ������Ȳ��ԣ�����д�����ֵ��״̬
		// �����Ʒ�͸������ʱ��Ӧʹ��Ĭ��״̬
		// ����͸������ʱ��ʹ�ø�״̬������Чȷ�����״̬�Ľ���
		// ����ȷ����ǰ�ķ�͸����������赲�Ϻ��һ������
		// ������ģ��ֵ����������Ž��л���
		dsDesc.StencilEnable = true;
		dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		// ���ڱ���ļ����������ǲ�������Ⱦ�ģ���������������޹ؽ�Ҫ
		dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		HR(device->CreateDepthStencilState(&dsDesc, DSSNoDepthWriteWithStencil.GetAddressOf()));
	}
}