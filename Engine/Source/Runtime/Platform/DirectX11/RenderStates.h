// �ṩһЩ��Ⱦ״̬
// Provide some render states.
#pragma once

#include <wrl/client.h>
#include <d3d11_1.h>

namespace X
{
	class RenderStates
	{
	public:
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		static bool IsInit();

		static void InitAll(ID3D11Device* device);
		// ʹ��ComPtr�����ֹ��ͷ�

	public:
		static ComPtr<ID3D11RasterizerState> RSWireframe;					// ��դ����״̬���߿�ģʽ
		static ComPtr<ID3D11RasterizerState> RSNoCull;						// ��դ����״̬���ޱ���ü�ģʽ
		static ComPtr<ID3D11RasterizerState> RSCullClockWise;	            // ��դ����״̬��˳ʱ��ü�ģʽ
		static ComPtr<ID3D11RasterizerState> RSDepth;						// ��դ����״̬�����ƫ��ģʽ

		static ComPtr<ID3D11SamplerState> SSPointClamp;						// ������״̬���������Clampģʽ
		static ComPtr<ID3D11SamplerState> SSLinearWrap;						// ������״̬�����Թ�����Wrapģʽ
		static ComPtr<ID3D11SamplerState> SSAnisotropicWrap;		        // ������״̬���������Թ�����Wrapģʽ
		static ComPtr<ID3D11SamplerState> SSShadow;							// ������״̬����ȱȽ���Borderģʽ

		static ComPtr<ID3D11BlendState> BSNoColorWrite;		                // ���״̬����д����ɫ
		static ComPtr<ID3D11BlendState> BSTransparent;		                // ���״̬��͸�����
		static ComPtr<ID3D11BlendState> BSAlphaToCoverage;	                // ���״̬��Alpha-To-Coverage
		static ComPtr<ID3D11BlendState> BSAdditive;			                // ���״̬���ӷ����
		static ComPtr<ID3D11BlendState> BSAlphaWeightedAdditive;			// ���״̬����AlphaΪȨ�صļӷ����

		static ComPtr<ID3D11DepthStencilState> DSSEqual;					// ���/ģ��״̬��������������ֵ��ȵ�����
		static ComPtr<ID3D11DepthStencilState> DSSLessEqual;		        // ���/ģ��״̬����Ĭ����Ȳ��ԵĻ����϶�������������ֵ��ȵ�����
		static ComPtr<ID3D11DepthStencilState> DSSWriteStencil;		        // ���/ģ��״̬��д��ģ��ֵ
		static ComPtr<ID3D11DepthStencilState> DSSDrawWithStencil;	        // ���/ģ��״̬����ָ��ģ��ֵ��������л���
		static ComPtr<ID3D11DepthStencilState> DSSNoDoubleBlend;	        // ���/ģ��״̬���޶��λ������
		static ComPtr<ID3D11DepthStencilState> DSSNoDepthTest;		        // ���/ģ��״̬���ر���Ȳ���
		static ComPtr<ID3D11DepthStencilState> DSSNoDepthWrite;		        // ���/ģ��״̬������Ȳ��ԣ���д�����ֵ
		static ComPtr<ID3D11DepthStencilState> DSSNoDepthTestWithStencil;	// ���/ģ��״̬���ر���Ȳ��ԣ���ָ��ģ��ֵ��������л���
		static ComPtr<ID3D11DepthStencilState> DSSNoDepthWriteWithStencil;	// ���/ģ��״̬������Ȳ��ԣ���д�����ֵ����ָ��ģ��ֵ��������л���
	}; 
}
