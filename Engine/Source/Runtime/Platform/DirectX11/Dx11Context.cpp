#include "Xpch.h"

#include "Runtime/Core/AppFramework/Application.h"
#include "Runtime/Platform/DirectX11/Dx11Context.h"
#include "Runtime/Platform/DirectX11/DXTrace.h"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>

namespace X
{
    Dx11Context::Dx11Context(GLFWwindow* windowHandle)
        : mWindowHandle(windowHandle)
    {
        X_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void Dx11Context::Init()
    {
        HWND hWnd = glfwGetWin32Window(mWindowHandle);

        HRESULT hr = S_OK;

        // ����D3D�豸 �� D3D�豸������
        UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        // ���Եȼ�����
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        UINT numFeatureLevels = ARRAYSIZE(featureLevels);
        D3D_FEATURE_LEVEL featureLevel;
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, pDevice.GetAddressOf(), &featureLevel, pContext.GetAddressOf());

        if (hr == E_INVALIDARG)
        {
            // Direct3D 11.0 ��API������D3D_FEATURE_LEVEL_11_1������������Ҫ�������Եȼ�11.0�Լ����µİ汾
            hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                D3D11_SDK_VERSION, pDevice.GetAddressOf(), &featureLevel, pContext.GetAddressOf());
        }
        if (FAILED(hr))
        {
            MessageBox(0, "D3D11CreateDevice Failed.", 0, 0);
        }
        // ����Ƿ�֧�����Եȼ�11.0��11.1
        if (featureLevel != D3D_FEATURE_LEVEL_11_0 && featureLevel != D3D_FEATURE_LEVEL_11_1)
        {
            MessageBox(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
        }


        // ��������Device�ˣ����ڿ���ȥ��� MSAA֧�ֵ������ȼ�
        pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
        assert(m4xMsaaQuality > 0);

        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 0;
        sd.BufferDesc.RefreshRate.Denominator = 0;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

        if (bEnable4xMsaa)
        {
            sd.SampleDesc.Count = 4;
            sd.SampleDesc.Quality = bEnable4xMsaa - 1;
        }
        else
        {
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
        }

        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;
        sd.OutputWindow = hWnd;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = 0;

        UINT swapCreateFlags = 0u;
#ifndef NDEBUG
        swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        HR(D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            swapCreateFlags,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &sd,
            &pSwap,
            &pDevice,
            nullptr,
            &pContext
        ));

        glfwMakeContextCurrent(mWindowHandle);
    }

    void Dx11Context::SwapBuffers()
    {
        glfwSwapBuffers(mWindowHandle);
    }
}