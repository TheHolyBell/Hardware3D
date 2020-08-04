#define FULL_WINTARD
#include "Surface.h"
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>
#include <sstream>
#include "ScriptCommander.h"

#pragma comment(lib, "gdiplus.lib")


Surface Surface::FromFile(const std::string& filename)
{
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Color[]> pBuffer;

	bool alphaLoaded = false;
	{
		// convert filenam to wide string (for Gdiplus)
		wchar_t wideName[512];
		mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

		Gdiplus::Bitmap bitmap(wideName);
		if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
		{
			std::stringstream ss;
			ss << "Loading image [" << filename << "]: failed to load.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[]>(width * height);

		for (unsigned int y = 0; y < height; y++)
		{
			for (unsigned int x = 0; x < width; x++)
			{
				Gdiplus::Color c;
				bitmap.GetPixel(x, y, &c);
				pBuffer[y * width + x] = c.GetValue();
				if (c.GetAlpha() != 255)
				{
					alphaLoaded = true;
				}
			}
		}
	}

	return Surface(width, height, std::move(pBuffer), alphaLoaded);
}

Surface::Surface(unsigned int width, unsigned int height) noexcept
	: m_pBuffer(std::make_unique<Color[]>(width * height)),
	m_Width(m_Width), m_Height(height)
{
}

Surface::Surface(Surface&& rhs) noexcept
	: m_pBuffer(std::move(rhs.m_pBuffer)),
	m_Width(rhs.m_Width), m_Height(rhs.m_Height)
{
}

Surface& Surface::operator=(Surface&& rhs) noexcept
{
	m_Width = rhs.m_Width;
	m_Height = rhs.m_Height;
	m_pBuffer = std::move(rhs.m_pBuffer);
	rhs.m_pBuffer = nullptr;
	return *this;
}

Surface::~Surface()
{
}

void Surface::Clear(Color fillValue) noexcept
{
	memset(m_pBuffer.get(), fillValue.dword, m_Width * m_Height * sizeof(Color));
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < m_Width);
	assert(y < m_Height);
	m_pBuffer[y * m_Width + x] = c;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < m_Width);
	assert(y < m_Height);
	return m_pBuffer[y * m_Width + x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return m_Width;
}

unsigned int Surface::GetHeight() const noexcept
{
	return m_Height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return m_pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return m_pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return m_pBuffer.get();
}


void Surface::Save(const std::string& filename) const
{
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; size == 0.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == nullptr)
		{
			std::stringstream ss;
			ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to allocate memory.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return;
			}
		}

		free(pImageCodecInfo);
		std::stringstream ss;
		ss << "Saving surface to [" << filename <<
			"]: failed to get encoder; failed to find matching encoder.";
		throw Exception(__LINE__, __FILE__, ss.str());
	};

	CLSID bmpID;
	GetEncoderClsid(L"image/bmp", &bmpID);


	// convert filenam to wide string (for Gdiplus)
	wchar_t wideName[512];
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(m_Width, m_Height, m_Width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)m_pBuffer.get());
	if (bitmap.Save(wideName, &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
}

void Surface::Copy(const Surface& src) noexcept(!IS_DEBUG)
{
	assert(m_Width == src.m_Width);
	assert(m_Height == src.m_Height);
	memcpy(m_pBuffer.get(), src.m_pBuffer.get(), m_Width * m_Height * sizeof(Color));
}

bool Surface::AlphaLoaded() const noexcept
{
	return m_bAlphaLoaded;
}

Surface::Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam, bool alphaLoaded) noexcept
	: m_Width(width), m_Height(height), m_pBuffer(std::move(pBufferParam)),
	m_bAlphaLoaded(alphaLoaded)
{
}

Surface::Exception::Exception(int line, const char* file, const std::string& note) noexcept
	: ChiliException(line, file),
	m_Note(std::move(note))
{
}


const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl
		<< "[Note] " << GetNote();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

const std::string& Surface::Exception::GetNote() const noexcept
{
	return m_Note;
}
