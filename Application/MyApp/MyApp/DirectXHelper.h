#pragma once

//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include <ppltasks.h>

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw Platform::Exception::CreateException(hr);
		}
	}

	// Function that reads from a binary file asynchronously.
	inline Concurrency::task<std::vector<byte>> ReadDataAsync(const std::wstring& filename)
	{
		using namespace Windows::Storage;
		using namespace Concurrency;

		auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

		return create_task(folder->GetFileAsync(Platform::StringReference(filename.c_str()))).then([](StorageFile^ file)
		{
			return FileIO::ReadBufferAsync(file);
		}).then([](Streams::IBuffer^ fileBuffer) -> std::vector<byte>
		{
			std::vector<byte> returnBuffer;
			returnBuffer.resize(fileBuffer->Length);
			Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(Platform::ArrayReference<byte>(returnBuffer.data(), fileBuffer->Length));
			return returnBuffer;
		});
	}

	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	inline float ConvertDipsToPixels(float dips, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(dips * dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}

	// Converts a length in physical pixels to a length in device-independent pixels (DIPs).
	inline float ConvertPixelsToDips(float pixels)
	{
		static const float dipsPerInch = 96.0f;
		return pixels * dipsPerInch / Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->LogicalDpi; // Do not round.
	}

	// Converts a length in physical pixels to a length in device-independent pixels (DIPs) using the provided DPI. This removes the need
	// to call this function from a thread associated with a CoreWindow, which is required by DisplayInformation::GetForCurrentView().
	inline float ConvertPixelsToDips(float pixels, float dpi)
	{
		static const float dipsPerInch = 96.0f;
		return pixels * dipsPerInch / dpi; // Do not round.
	}
}