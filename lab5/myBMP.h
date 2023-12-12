#include "C:\Users\Игорь\Desktop\учеба\LabRab\ИУК4-32Б\Типы и структуры данных\lab3\lab3\Menu.h"
#include "C:\Users\Игорь\Desktop\учеба\LabRab\ИУК4-32Б\Типы и структуры данных\lab3\lab3\MenuItem.h"
#include "C:\Users\Игорь\Desktop\учеба\LabRab\ИУК4-32Б\Типы и структуры данных\lab3\lab3\Exceptions.h"
#include <C:\Users\Игорь\Desktop\учеба\LabRab\ЛР ВП 2 Сем\1 ЛР\LR1\LR1\myException.h>
#include "C:\Users\Игорь\Desktop\учеба\LabRab\ЛР ВП 2 Сем\1 ЛР\LR1\LR1\myVector.h"
#include <fstream>
typedef int SDWORD;
// typedef unsigned int DWORD;

typedef short SWORD;
typedef unsigned short WORD;
typedef char SBYTE;
typedef unsigned char BYTE;

#pragma pack(push, 1)
struct BMPHeader
{
	WORD type = 0;
	DWORD size = 0;
	WORD reserved1 = 0;
	WORD reserved2 = 0;
	DWORD offset = 0;

	DWORD headerSize = 0;
	SDWORD width = 0;
	SDWORD height = 0;
	WORD colorPlanes = 0;
	WORD bpp = 0;
	DWORD compression = 0;
	DWORD imgSize = 0;
	DWORD hrez = 0;
	DWORD vres = 0;
	DWORD palette = 0;
	DWORD imporant = 0;
};

struct Pixel
{
public:
	BYTE r = 255;
	BYTE g = 255;
	BYTE b = 255;
	Pixel() :r{ 255 }, g{ 255 }, b{ 255 } {};
	Pixel(BYTE rc, BYTE gc, BYTE bc) :r{ rc }, g{ gc }, b{ bc } {};
	bool operator ==(const Pixel& other) const { return this->r == other.r && this->g == other.g && this->b == other.b; };
	bool operator !=(const Pixel& other) const { return (*this) != other; };
	Pixel(const Pixel& other) : r{ other.r }, g{ other.g }, b{ other.b } {};
	Pixel(Pixel&& other) : r{ other.r }, g{ other.g }, b{ other.b } {};
	const Pixel& operator=(const Pixel& other)
	{
		if (this != &other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
		}
		return *this;
	}
	Pixel& operator=(Pixel&& other)
	{
		if (this != &other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
		}
		return *this;
	}
};
#pragma pack(pop)

class MyBMP
{
private:
	BMPHeader bmph{};
	unsigned int width = 0;
	unsigned int height = 0;
	PIA::myVector<Pixel> pixels{};
public:
	MyBMP() : bmph{}, width{ 0 }, height{ 0 }, pixels{} {};
	MyBMP(const char* path)
	{
		Load(path);
	}
	MyBMP(int width, int height)
	{
		const SDWORD row_size = width * 3 + height % 4;
		const DWORD bitmap_size = row_size * height;
		this->bmph.type = 0x4D42;
		this->bmph.size = bitmap_size + sizeof(BMPHeader);
		this->bmph.reserved1 = 0;
		this->bmph.reserved2 = 0;
		this->bmph.offset = sizeof(BMPHeader);
		this->bmph.size = 40;
		this->bmph.width = width;
		this->bmph.height = height;
		this->bmph.colorPlanes = 1;
		this->bmph.bpp = sizeof(Pixel) * 8;
		this->bmph.compression = 0;
		this->bmph.imgSize = bitmap_size;
		this->bmph.hrez = 0;
		this->bmph.vres = 0;
		this->bmph.palette = 0;
		this->bmph.imporant = 0;
		if (!this->pixels.empty()) this->pixels.clear();
		this->pixels = PIA::myVector<Pixel>{ this->bmph.imgSize };
	}

	void Clear()
	{
		this->pixels.clear();
	}
	void DoTask()
	{
		try
		{
			PIA::myVector<Pixel> colors;
			for (int i = 0; i <= 1; i++)
			{
				for (int j = 0; j <= 1; j++)
				{
					for (int k = 0; k <= 1; k++)
					{
						Pixel t = Pixel{0, 0, 0};
						if (i) t.r = 255;
						if (j) t.g = 255;
						if (k) t.b = 255;
						if (!(!i && !j && !k || i && j && k))
							colors.pushBack(t);
					}
				}
			}
			auto ctr = 0;
			for (int i = 0; i < this->width; i++)
			{
				for (int j = 0; j < this->height; j++)
				{
					Pixel cur = this->pixels[IX(i, j)];
					for (int i = 0; i < colors.getSize(); i++)
					{
						if (cur == colors[i])
						{
							cur = colors[(i + 1) % colors.getSize()];
							ctr++;
							break;
							
						}
					}
					this->Set(i, j, cur);
				}
			}
			std::cout << "Цвет успешно изменен! Обработано " << ctr << " пикселей!" << std::endl;
		}
		catch (...)
		{
			std::cout << "Ошибка обработки файла" << std::endl;
		}
	}

	void Save(const char* filename)
	{
		try 
		{
			const int row_size = this->width * 3 + this->width % 4;
			const unsigned int bitmap_size = row_size * this->height;
			if (std::ofstream ofs{ filename, std::ios::binary })
			{
				ofs.write(reinterpret_cast<const char*>(&this->bmph), sizeof(BMPHeader));
				PIA::myVector<BYTE> line(row_size);
				for (int y = this->height - 1; y >= 0; --y)
				{
					int i = 0;
					for (int x = 0; x < this->width; ++x)
					{
						const Pixel& color = this->pixels[IX(x, y)];
						line[i++] = color.b;
						line[i++] = color.g;
						line[i++] = color.r;
					}
					ofs.write(reinterpret_cast<const char*>(line.data()), line.getSize());
				}
				ofs.close();
			}
			std::cout << "Изображение сохранено!" << std::endl;
		}
		catch (...)
		{
			std::cout << "Ошибка сохранения файла!" << std::endl;
		}
	}
	void Load(const char* filename)
	{
		try
		{
			this->pixels.clear();
			if (std::ifstream ifs{ filename, std::ios::binary })
			{
				ifs.read(reinterpret_cast<char*>(&this->bmph), sizeof(BMPHeader));
				if (this->bmph.type != 0x4D42)
				{
					ifs.close();
					throw BaseE("Wrong file type");
				}
				this->width = bmph.width;
				this->height = bmph.height;
				this->pixels.resize(static_cast<std::size_t>(this->width) * static_cast<std::size_t>(this->width), Pixel{255, 255, 255});
				const int row_size = this->width * 3 + this->width % 4;
				PIA::myVector<BYTE> line(row_size);
				for (int y = this->height - 1; y >= 0; --y)
				{
					ifs.read(reinterpret_cast<char*>(line.data()), line.getSize());
					int i = 0;
					for (int x = 0; x < this->width; ++x)
					{
						Pixel color{};
						color.b = line[i++];
						color.g = line[i++];
						color.r = line[i++];
						this->pixels[IX(x, y)] = color;
					}
				}
				ifs.close();
			}
			std::cout << "Изображение было загружено!" << std::endl;
		}
		catch (...)
		{
			std::cout << "Ошибка загрузки файла!" << std::endl;
		}
	}
	size_t Width() const { return this->width; }
	size_t Height() const { return this->height; }
	void Set(const unsigned int x, const unsigned int y, const Pixel& pixel) 
	{ 
		if (InBounds(x, y)) this->pixels[IX(x, y)] = pixel;
		else throw OutOfRangeE("Pixel coordinated out of range");
	}
	Pixel& Get(const unsigned int x, const unsigned int y)
	{
		if (!InBounds(x, y)) throw OutOfRangeE("Pixel coordinated out of range");
		return this->pixels[IX(x, y)];
	}
private:
	unsigned int IX(const unsigned int x, const unsigned int y) { return this->width * y + x; };
	bool InBounds(unsigned int x, unsigned int y) const { return x >= 0 && x < this->width && y >= 0 && y < this->height; }
};

/*void Load(const char* path)
	{
		try
		{
			pixels.clear();
			std::ifstream fin = std::ifstream(path, std::ios_base::binary);
			if (!fin.is_open()) throw InputErrorE("Couldn't open file");
			fin.read(reinterpret_cast<char*>(&this->bmph), sizeof(BMPHeader));
			if (this->bmph.type != 0x4D42) throw BaseE("File extension doesn't match");
			this->width = bmph.width;
			this->height = bmph.height;
			this->pixels.resize(width * height, Pixel(255, 255, 255));
			PIA::myVector<DWORD> row{ width * 3 + width % 4 };
			for (int i = this->height - 1; i >= 0; i--)
			{
				fin.read(reinterpret_cast<char*>(row.data()), row.getSize());
				int c = 0;
				for (int j = 0; j < this->width; j++)
				{
					Pixel p{};
					p.b = row[c++];
					p.g = row[c++];
					p.r = row[c++];
					pixels[IX(j, i)] = p;
				}
			}
			fin.close();
		}
		catch (...){}
	}

	void Save(const char* path)
	{
		try
		{
			std::ofstream fout = std::ofstream(path, std::ios_base::binary);
			if (!fout.is_open()) throw InputErrorE("Couldn't open file");
			fout.write(reinterpret_cast<const char*>(&(this->bmph)), sizeof(BMPHeader));
			const unsigned int rlen = this->width * 3 + this->width % 4;
			PIA::myVector<BYTE> row{ rlen };
			for (int i = this->height - 1; i >= 0; i--)
			{
				int k = 0;
				for (int j = 0; j < this->width; j++)
				{
					const Pixel& c = this->pixels[IX(j, i)];
					row[k++] = c.b;
					row[k++] = c.g;
					row[k++] = c.r;
				}
				fout.write(reinterpret_cast<const char*>(row.data()), row.getSize());
			}
			fout.close();
		}
		catch (...) {}
	}*/