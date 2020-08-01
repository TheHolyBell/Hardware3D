#pragma once

#include "ChiliWin.h"
#include "ChiliException.h"
#include <string>
#include <cassert>
#include <memory>


class Surface
{
public:
	class Color
	{
	public:
		unsigned int dword;
	public:
		constexpr Color() noexcept : dword()
		{}
		constexpr Color(const Color& col) noexcept
			:
			dword(col.dword)
		{}
		constexpr Color(unsigned int dw) noexcept
			:
			dword(dw)
		{}
		constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((x << 24u) | (r << 16u) | (g << 8u) | b)
		{}
		constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((r << 16u) | (g << 8u) | b)
		{}
		constexpr Color(Color col, unsigned char x) noexcept
			:
			Color((x << 24u) | col.dword)
		{}
		Color& operator =(Color color) noexcept
		{
			dword = color.dword;
			return *this;
		}
		constexpr unsigned char GetX() const noexcept
		{
			return dword >> 24u;
		}
		constexpr unsigned char GetA() const noexcept
		{
			return GetX();
		}
		constexpr unsigned char GetR() const noexcept
		{
			return (dword >> 16u) & 0xFFu;
		}
		constexpr unsigned char GetG() const noexcept
		{
			return (dword >> 8u) & 0xFFu;
		}
		constexpr unsigned char GetB() const noexcept
		{
			return dword & 0xFFu;
		}
		void SetX(unsigned char x) noexcept
		{
			dword = (dword & 0xFFFFFFu) | (x << 24u);
		}
		void SetA(unsigned char a) noexcept
		{
			SetX(a);
		}
		void SetR(unsigned char r) noexcept
		{
			dword = (dword & 0xFF00FFFFu) | (r << 16u);
		}
		void SetG(unsigned char g) noexcept
		{
			dword = (dword & 0xFFFF00FFu) | (g << 8u);
		}
		void SetB(unsigned char b) noexcept
		{
			dword = (dword & 0xFFFFFF00u) | b;
		}
	};
public:
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const char* file, const std::string& note) noexcept;

		virtual const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string m_Note;
	};
public:

	static Surface FromFile(const std::string& filename);

	Surface(unsigned int width, unsigned int height) noexcept;
	
	Surface(Surface&& rhs) noexcept;
	Surface& operator=(Surface&& rhs) noexcept;

	Surface(const Surface& rhs) = delete;
	Surface& operator=(const Surface& rhs) = delete;

	~Surface();

	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!IS_DEBUG);
	Color GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG);

	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;

	Color* GetBufferPtr() noexcept;

	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	
	void Save(const std::string& filename) const;
	void Copy(const Surface& src) noexcept(!IS_DEBUG);
private:
	Surface(unsigned int width, unsigned int height, std::unique_ptr<Color[]> pBufferParam) noexcept;
private:
	std::unique_ptr<Color[]> m_pBuffer;
	unsigned int m_Width;
	unsigned int m_Height;
};