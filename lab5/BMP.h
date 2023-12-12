#pragma once
#include <cstring>
#include <exception>
#include <memory>
#include <algorithm>
#include <fstream>
#include <cstddef>
#include <vector>
#include <string>
#include <cstdint>

namespace bmp
{
	static constexpr const std::uint16_t BITMAP_BUFFER_MAGIC = 0x4D42;

#pragma pack(push, 1)
	struct BitmapHeader
	{
		std::uint16_t magic;
		std::uint32_t file_size;
		std::uint16_t reserved1;
		std::uint16_t reserved2;
		std::uint32_t offset_bits;
		std::uint32_t size;
		std::int32_t width;
		std::int32_t height;
		std::uint16_t planes;
		std::uint16_t bits_per_pixel;
		std::uint32_t compression;
		std::uint32_t size_image;
		std::int32_t x_pixels_per_meter;
		std::int32_t y_pixels_per_meter;  
		std::uint32_t clr_used;           
		std::uint32_t clr_important;
	};
	static_assert(sizeof(BitmapHeader) == 54, "Bitmap header size must be 54 bytes");

	struct Pixel
	{
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;

		constexpr Pixel() noexcept : r(0), g(0), b(0) {}
		explicit constexpr Pixel(const std::int32_t rgb) noexcept : r((rgb >> 16) & 0xff), g((rgb >> 8) & 0xff), b((rgb >> 0x0) & 0xff) {}
		constexpr Pixel(std::uint8_t red, std::uint8_t green, std::uint8_t blue) noexcept : r(red), g(green), b(blue) {}

		constexpr bool operator==(const Pixel other) const noexcept { return r == other.r and g == other.g and b == other.b; }
		constexpr bool operator!=(const Pixel other) const noexcept { return not ((*this) == other); }
	};
	static_assert(sizeof(Pixel) == 3, "Bitmap Pixel size must be 3 bytes");
#pragma pack(pop)

	static constexpr const Pixel Black{ std::uint8_t(0), std::uint8_t(0), std::uint8_t(0) };
	static constexpr const Pixel White{ std::uint8_t(255), std::uint8_t(255), std::uint8_t(255) };
	static constexpr const Pixel Red{ std::uint8_t(255), std::uint8_t(0), std::uint8_t(0) };
	static constexpr const Pixel Green{ std::uint8_t(0), std::uint8_t(255), std::uint8_t(0) };
	static constexpr const Pixel Blue{ std::uint8_t(0), std::uint8_t(0), std::uint8_t(255) };

	class Exception : public std::runtime_error
	{
	public:
		explicit Exception(const std::string& message) : std::runtime_error(message) {}
	};

	class BMP
	{
	public:
		BMP() noexcept
			: m_pixels(),
			m_width(0),
			m_height(0)
		{
		}
		explicit BMP(const std::string& filename)
			: m_pixels(),
			m_width(0),
			m_height(0)
		{
			this->load(filename);
		}
		BMP(const std::int32_t width, const std::int32_t height)
			: m_pixels(static_cast<std::size_t>(width)* static_cast<std::size_t>(height)),
			m_width(width),
			m_height(height)
		{
			if (width == 0 || height == 0)
				throw Exception("BMP width and height must be > 0");
		}
		BMP(const BMP& other)
			: m_width(other.m_width),
			m_height(other.m_height),
			m_pixels(other.m_pixels)
		{
		}
		virtual ~BMP() noexcept
		{
			m_pixels.clear();
		}

	public: 
		Pixel& get(const std::int32_t x, const std::int32_t y)
		{
			if (!in_bounds(x, y))
				throw Exception("BMP::Get(" + std::to_string(x) + ", " + std::to_string(y) + "): x,y out of bounds");
			return m_pixels[IX(x, y)];
		}
		const Pixel& get(const std::int32_t x, const std::int32_t y) const
		{
			if (!in_bounds(x, y))
				throw Exception("BMP::Get(" + std::to_string(x) + ", " + std::to_string(y) + "): x,y out of bounds");
			return m_pixels[IX(x, y)];
		}
		std::int32_t width() const noexcept { return m_width; }
		std::int32_t height() const noexcept { return m_height; }
		void clear(const Pixel pixel = Black)
		{
			std::fill(m_pixels.begin(), m_pixels.end(), pixel);
		}

	public:
		const Pixel& operator[](const std::size_t i) const { return m_pixels[i]; }
		Pixel& operator[](const std::size_t i) { return m_pixels[i]; }
		bool operator!() const noexcept { return (m_pixels.size() == 0) || (m_width == 0) || (m_height == 0); }
		operator bool() const noexcept { return !(*this); }
		bool operator==(const BMP& image) const
		{
			if (this != &image)
			{
				return (m_width == image.m_width) &&
					(m_height == image.m_height) &&
					(std::memcmp(m_pixels.data(), image.m_pixels.data(), sizeof(Pixel) * m_pixels.size()) == 0);
			}
			return true;
		}
		bool operator!=(const BMP& image) const { return !(*this == image); }
		BMP& operator=(const BMP& image)
		{
			if (this != &image)
			{
				m_width = image.m_width;
				m_height = image.m_height;
				m_pixels = image.m_pixels;
			}
			return *this;
		}

	//public:
	//	std::vector<Pixel>::iterator begin() noexcept { return m_pixels.begin(); }
	//	std::vector<Pixel>::iterator end() noexcept { return m_pixels.end(); }
	//	std::vector<Pixel>::const_iterator cbegin() const noexcept { return m_pixels.cbegin(); }
	//	std::vector<Pixel>::const_iterator cend() const noexcept { return m_pixels.cend(); }
	//	std::vector<Pixel>::reverse_iterator rbegin() noexcept { return m_pixels.rbegin(); }
	//	std::vector<Pixel>::reverse_iterator rend() noexcept { return m_pixels.rend(); }
	//	std::vector<Pixel>::const_reverse_iterator crbegin() const noexcept { return m_pixels.crbegin(); }
	//	std::vector<Pixel>::const_reverse_iterator crend() const noexcept { return m_pixels.crend(); }

	public:
		void set(const std::int32_t x, const std::int32_t y, const Pixel color)
		{
			if (!in_bounds(x, y))
			{
				throw Exception("BMP::Set(" + std::to_string(x) + ", " + std::to_string(y) + "): x,y out of bounds");
			}
			m_pixels[IX(x, y)] = color;
		}
		void save(const std::string& filename)
		{
			const std::int32_t row_size = m_width * 3 + m_width % 4;
			const std::uint32_t bitmap_size = row_size * m_height;
			BitmapHeader header{};
			header.magic = BITMAP_BUFFER_MAGIC;
			header.file_size = bitmap_size + sizeof(BitmapHeader);
			header.reserved1 = 0;
			header.reserved2 = 0;
			header.offset_bits = sizeof(BitmapHeader);
			header.size = 40;
			header.width = m_width;
			header.height = m_height;
			header.planes = 1;
			header.bits_per_pixel = sizeof(Pixel) * 8;
			header.compression = 0;
			header.size_image = bitmap_size;
			header.x_pixels_per_meter = 0;
			header.y_pixels_per_meter = 0;
			header.clr_used = 0;
			header.clr_important = 0;
			if (std::ofstream ofs{ filename, std::ios::binary })
			{
				ofs.write(reinterpret_cast<const char*>(&header), sizeof(BitmapHeader));
				std::vector<std::uint8_t> line(row_size);
				for (std::int32_t y = m_height - 1; y >= 0; --y)
				{
					std::size_t i = 0;
					for (std::int32_t x = 0; x < m_width; ++x)
					{
						const Pixel& color = m_pixels[IX(x, y)];
						line[i++] = color.b;
						line[i++] = color.g;
						line[i++] = color.r;
					}
					ofs.write(reinterpret_cast<const char*>(line.data()), line.size());
				}
				ofs.close();
			}
			else
				throw Exception("BMP::Save(\"" + filename + "\"): Failed to save pixels to file.");
		}
		void load(const std::string& filename)
		{
			m_pixels.clear();

			if (std::ifstream ifs{ filename, std::ios::binary })
			{
				std::unique_ptr<BitmapHeader> header(new BitmapHeader());
				ifs.read(reinterpret_cast<char*>(header.get()), sizeof(BitmapHeader));
				if (header->magic != BITMAP_BUFFER_MAGIC)
				{
					ifs.close();
					throw Exception("BMP::Load(\"" + filename + "\"): Unrecognized file format.");
				}
				if (header->bits_per_pixel != 24)
				{
					ifs.close();
					throw Exception("BMP::Load(\"" + filename + "\"): Only 24 bits per pixel bitmaps supported.");
				}
				m_width = header->width;
				m_height = header->height;
				m_pixels.resize(static_cast<std::size_t>(m_width) * static_cast<std::size_t>(m_height), Black);
				const std::int32_t row_size = m_width * 3 + m_width % 4;
				std::vector<std::uint8_t> line(row_size);
				for (std::int32_t y = m_height - 1; y >= 0; --y)
				{
					ifs.read(reinterpret_cast<char*>(line.data()), line.size());
					std::size_t i = 0;
					for (std::int32_t x = 0; x < m_width; ++x)
					{
						Pixel color{};
						color.b = line[i++];
						color.g = line[i++];
						color.r = line[i++];
						m_pixels[IX(x, y)] = color;
					}
				}
				ifs.close();
			}
			else
				throw Exception("BMP::Load(\"" + filename + "\"): Failed to load bitmap pixels from file.");
		}

	private:
		constexpr std::size_t IX(const std::int32_t x, const std::int32_t y) const noexcept
		{
			return static_cast<std::size_t>(x) + static_cast<std::size_t>(m_width) * static_cast<std::size_t>(y);
		}
		constexpr bool in_bounds(const std::int32_t x, const std::int32_t y) const noexcept
		{
			return (x >= 0) && (x < m_width) && (y >= 0) && (y < m_height);
		}

	private:
		std::vector<Pixel> m_pixels;
		std::int32_t m_width;
		std::int32_t m_height;
	};
}