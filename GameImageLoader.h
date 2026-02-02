#if !defined(GAMEIMAGELOADER_H)
#define GAMEIMAGELOADER_H
#include <wincodec.h>

#include <malloc.h>
#include <ocidl.h>
#include <Windows.h>
#include <wrl/client.h>
#include <cstdint>
#include <iostream>
#include "GameHelpers.h"

namespace game
{
    class ImageSaver
    {
    public:
        bool Save(const uint32_t *data, const char* filename, const uint32_t width, const uint32_t height, uint32_t extra) const;
    private:
    };

    inline bool ImageSaver::Save(const uint32_t* data, const char* filename, const uint32_t width, const uint32_t height, uint32_t extra) const
    {
		Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
		HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}
		
		// Create a new IWICBitmap object
		Microsoft::WRL::ComPtr<IWICBitmap> pBitmap;
		hr = factory->CreateBitmapFromMemory(width, height, GUID_WICPixelFormat32bppRGBA, width * 4, width * height * 4, (uint8_t*)data, pBitmap.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// Create a new IWICStream object
		Microsoft::WRL::ComPtr<IWICStream> pStream;
		hr = factory->CreateStream(pStream.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// Initialize the IWICStream object
		hr = pStream->InitializeFromFilename(ConvertToWide(filename).c_str(), GENERIC_WRITE);
		if (FAILED(hr))
		{
			return false;
		}

		// Create a new IWICBitmapEncoder object
		Microsoft::WRL::ComPtr<IWICBitmapEncoder> pEncoder;
		hr = factory->CreateEncoder(GUID_ContainerFormatPng, NULL, pEncoder.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// Initialize the IWICBitmapEncoder object
		hr = pEncoder->Initialize(pStream.Get(), WICBitmapEncoderNoCache);
		if (FAILED(hr))
		{
			return false;
		}

		// Create a new IWICBitmapFrameEncode object
		Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> pFrameEncode;
		Microsoft::WRL::ComPtr<IPropertyBag2> pPropertybag;
		hr = pEncoder->CreateNewFrame(pFrameEncode.GetAddressOf(), pPropertybag.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}
		
		// Compression filter test
		//IPropertyBag2* pPropertybag = NULL;
		if (SUCCEEDED(hr))
		{
			// Set the compression quality
			PROPBAG2 option = { 0 };
			VARIANT varValue = {};
			LPCOLESTR s = L"FilterOption";
			option.pstrName = const_cast<wchar_t*>(s); // bad but only way I got it to work.
			varValue.vt = VT_UI1;
			varValue.fltVal = WICPngFilterNone; // no filter, see https://learn.microsoft.com/en-us/windows/win32/api/wincodec/ne-wincodec-wicpngfilteroption
			hr = pPropertybag->Write(1, &option, &varValue);
			if (FAILED(hr)) std::cout << "prperty failed.. \n";
		}

		// Initialize the IWICBitmapFrameEncode object
		hr = pFrameEncode->Initialize(NULL);
		if (FAILED(hr))
		{
			return false;
		}

		// Set the size of the PNG image
		hr = pFrameEncode->SetSize(width, height);
		if (FAILED(hr))
		{
			return false;
		}

		// Write the screenshot data to the PNG image
		//hr = pFrameEncode->WritePixels(height, width * 4, width * height * 4, (BYTE*)data);
		hr = pFrameEncode->WriteSource(pBitmap.Get(), NULL);
		if (FAILED(hr))
		{
			return false;
		}

		// Commit the PNG image
		hr = pFrameEncode->Commit();
		if (FAILED(hr))
		{
			return false;
		}

		// Commit the IWICBitmapEncoder object
		hr = pEncoder->Commit();
		if (FAILED(hr))
		{
			return false;
		}
        return true;
    }

	class ImageLoader
	{
	public :
		ImageLoader();
		void* Load(const char* filename, uint32_t& width, uint32_t& height, uint32_t& componentsPerPixel);
		void UnLoad();
		~ImageLoader();
	private:
		void* _data;
		uint32_t _ARGBToABGR(uint32_t argbColor) const noexcept;
	};

	inline ImageLoader::ImageLoader()
	{
		_data = nullptr;
	}

	inline uint32_t ImageLoader::_ARGBToABGR(uint32_t argbColor) const noexcept
	{
		uint32_t r = (argbColor >> 16) & 0xFF;
		uint32_t b = argbColor & 0xFF;
		return (argbColor & 0xFF00FF00) | (b << 16) | r;
	}

	inline void* ImageLoader::Load(const char* fileName, uint32_t& width, uint32_t& height, uint32_t& componentsPerPixel)
	{
		// Clears data if multiple loads happen
		if (_data != nullptr)
		{
            //delete[] _data;

			_aligned_free(_data);
			_data = nullptr;
		}

        Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.GetAddressOf()));
        if (FAILED(hr)) {
            return nullptr;
        }

        Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
        hr = factory->CreateDecoderFromFilename(ConvertToWide(fileName).c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
        if (FAILED(hr)) {
            return nullptr;
        }

        Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
        hr = decoder->GetFrame(0, frame.GetAddressOf());
        if (FAILED(hr)) {
            return nullptr;
        }

        hr = frame->GetSize(&width, &height);
        if (FAILED(hr)) {
            return nullptr;
        }
        componentsPerPixel = 4;

        _data = (uint8_t*)_aligned_malloc((size_t)width * componentsPerPixel * height * sizeof(uint8_t), 16); //new uint8_t[width * componentsPerPixel * height];

        hr = frame->CopyPixels(nullptr, width * componentsPerPixel, width * componentsPerPixel * height , static_cast<uint8_t*>(_data));

        if (FAILED(hr)) {
            //delete[] _data;
			_aligned_free(_data);
            _data = nullptr;
            return nullptr;
        }

		// Windows loads as ARGB, most use ABGR so convert it
		uint32_t* colorMap = (uint32_t*)_data;
		for (uint32_t pix = 0; pix < width * height; pix++)
		{
			*colorMap = _ARGBToABGR(*colorMap);
			colorMap++;
		}

		return _data;
	}

	inline void ImageLoader::UnLoad()
	{
		if (_data != nullptr)
		{
            //delete[] _data;
			_aligned_free(_data);
			_data = nullptr;
		}
	}

	inline ImageLoader::~ImageLoader()
	{
		if (_data != nullptr)
		{
            //delete[] _data;
			_aligned_free(_data);
			_data = nullptr;
		}
	}
}

#endif