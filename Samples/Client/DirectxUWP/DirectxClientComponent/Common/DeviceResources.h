#pragma once

#include "CameraResources.h"

namespace DX
{
	class DeviceResources
	{
	public:
		DeviceResources();
		void Present(Windows::Graphics::Holographic::HolographicFrame^ frame);

		// Public methods related to holographic devices.
		void SetHolographicSpace(Windows::Graphics::Holographic::HolographicSpace^ space);
		void EnsureCameraResources(
			Windows::Graphics::Holographic::HolographicFrame^ frame,
			Windows::Graphics::Holographic::HolographicFramePrediction^ prediction);

		void AddHolographicCamera(Windows::Graphics::Holographic::HolographicCamera^ camera);
		void RemoveHolographicCamera(Windows::Graphics::Holographic::HolographicCamera^ camera);

		// Holographic accessors.
		template<typename RetType, typename LCallback>
		RetType						UseHolographicCameraResources(const LCallback& callback);

		// D3D Accessors.
		ID3D11Device4*				GetD3DDevice() const					{ return m_d3dDevice.Get(); }
		ID3D11DeviceContext3*		GetD3DDeviceContext() const				{ return m_d3dContext.Get(); }
		
		// DXGI acessors.
		IDXGIAdapter3*				GetDXGIAdapter() const					{ return m_dxgiAdapter.Get(); }

		// Render target properties.
		Windows::Foundation::Size	GetRenderTargetSize()	const			{ return m_d3dRenderTargetSize; }

	private:
		void CreateDeviceResources();
		void InitializeUsingHolographicSpace();

		// Direct3D objects.
		Microsoft::WRL::ComPtr<ID3D11Device4>						m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext3>				m_d3dContext;
		Microsoft::WRL::ComPtr<IDXGIAdapter3>						m_dxgiAdapter;

		// Direct3D interop objects.
		Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice^	m_d3dInteropDevice;

		// The holographic space provides a preferred DXGI adapter ID.
		Windows::Graphics::Holographic::HolographicSpace^			m_holographicSpace;

		// Back buffer resources, etc. for attached holographic cameras.
		std::map<UINT32, std::unique_ptr<CameraResources>>			m_cameraResources;
		std::mutex													m_cameraResourcesLock;
		Windows::Foundation::Size									m_d3dRenderTargetSize;
	};
}

// Device-based resources for holographic cameras are stored in a std::map.
// Access this list by providing a callback to this function, and the std::map
// will be guarded from add and remove events until the callback returns.
// The callback is processed immediately and must not contain any nested calls
// to UseHolographicCameraResources.
// The callback takes a parameter of type
// std::map<UINT32,std::unique_ptr<DX::CameraResources>>& through which the list
// of cameras will be accessed.
template<typename RetType, typename LCallback>
RetType DX::DeviceResources::UseHolographicCameraResources(const LCallback& callback)
{
	std::lock_guard<std::mutex> guard(m_cameraResourcesLock);
	return callback(m_cameraResources);
}
