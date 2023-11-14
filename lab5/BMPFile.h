#pragma once
#define BMPSIGN 0x4D42
#include <fstream>
#include <exception>
#include <math.h>
#include <vector>
#include <algorithm>
#include <cstdint>   // std::int*_t
#include <cstddef>   // std::size_t
#include <string>    // std::string
#include <cstring>   // std::memcmp
#include <exception> // std::exception

typedef unsigned char BITE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int SDWORD;
typedef short SWORD;
typedef char SBITE;

#pragma pack(push, 1)
struct BitmapHeader
{
	WORD magic;              /* Magic number for file always BM which is 0x4D42 */
	DWORD file_size;          /* Size of file */
	WORD reserved1;          /* Reserved */
	WORD reserved2;          /* Reserved */
	DWORD offset_bits;        /* Offset to bitmap data */
	/* Bitmap file info structure */
	DWORD size;               /* Size of info header */
	SDWORD width;               /* Width of image */
	SDWORD height;              /* Height of image */
	WORD planes;             /* Number of color planes */
	WORD bits_per_pixel;     /* Number of bits per pixel */
	DWORD compression;        /* Type of compression to use */
	DWORD size_image;         /* Size of image data */
	SDWORD x_pixels_per_meter;  /* X pixels per meter */
	SDWORD y_pixels_per_meter;  /* Y pixels per meter */
	DWORD clr_used;           /* Number of colors used */
	DWORD clr_important;      /* Number of important colors */
};
struct Pixel
{
	Pixel()
	{
		this->r = 0;
		this->g = 0;
		this->b = 0;
	}
	Pixel(BITE r, BITE g, BITE b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}
	Pixel(BITE c)
	{
		this->r = c;
		this->g = c;
		this->b = c;
	}
	BITE r; /* Blue value */
	BITE g; /* Green value */
	BITE b; /* Red value */
};
#pragma pack(pop)

class BMPF
{
private:
	DWORD GetArrPos(DWORD x, DWORD y)
	{
		return (x + this->bmph.width * y);
	}
	Pixel* pixels = nullptr;
	BitmapHeader bmph;
	size_t len = 0;
public:
	bool in_bounds(const SDWORD x, const SDWORD y) const
	{
		return (x >= 0) && (x < this->bmph.width) && (y >= 0) && (y < this->bmph.height);
	}
	void Set(const SDWORD x, const SDWORD y, const Pixel color)
	{
		if (!in_bounds(x, y))
		{
			throw std::runtime_error("Bitmap::Set(" + std::to_string(x) + ", " + std::to_string(y) + "): x, y out of bounds");
		}
		this->pixels[GetArrPos(x, y)] = color;
	}
	SDWORD Width()
	{
		return this->bmph.width;
	}
	SDWORD Height()
	{
		return this->bmph.height;
	}
	BMPF()
	{
		this->bmph = BitmapHeader();
		this->pixels = nullptr;
		len = 0;
	}

	BMPF(SDWORD width, SDWORD height)
	{
		const SDWORD row_size = width * 3 + height % 4;
		const DWORD bitmap_size = row_size * height;
		this->bmph.magic = BMPSIGN;
		this->bmph.file_size = bitmap_size + sizeof(BitmapHeader);
		this->bmph.reserved1 = 0;
		this->bmph.reserved2 = 0;
		this->bmph.offset_bits = sizeof(BitmapHeader);
		this->bmph.size = 40;
		this->bmph.width = width;
		this->bmph.height = height;
		this->bmph.planes = 1;
		this->bmph.bits_per_pixel = sizeof(Pixel) * 8;
		this->bmph.compression = 0;
		this->bmph.size_image = bitmap_size;
		this->bmph.x_pixels_per_meter = 0;
		this->bmph.y_pixels_per_meter = 0;
		this->bmph.clr_used = 0;
		this->bmph.clr_important = 0;
		if (this->pixels) delete[] this->pixels;
		this->pixels = new Pixel[this->bmph.size_image];
		std::fill_n(this->pixels, this->bmph.size_image, Pixel(255, 255, 255));
	}

	~BMPF()
	{
		if (this->pixels) delete[] this->pixels;
		len = 0;
	}
	BMPF(const char* path)
	{
		this->Read(path);
	}
	void Read(const std::string& path)
	{
		std::ifstream fin = std::ifstream(path, std::ios_base::binary);
		if (!fin.is_open()) throw new std::exception("File couldn't be opened");
		fin.read((char*)&this->bmph, sizeof(this->bmph));
		if (!(this->bmph.magic == BMPSIGN)) throw new std::exception("File signature doesn't match");
		if (this->bmph.bits_per_pixel != 24)
		{
			fin.close();
			throw std::runtime_error("Bitmap::Load(\"" + path + "\"): Only 24 bits per pixel bitmaps supported.");
		}
		if (this->pixels) delete[] this->pixels;
		this->len = static_cast<size_t>(this->bmph.width) * static_cast<size_t>(this->bmph.height);
		const std::int32_t rowSize = this->bmph.width * 3 + this->bmph.width % 4;
		this->pixels = new Pixel[len];
		std::vector<BITE> line(rowSize);
		for (std::int32_t y = this->bmph.height - 1; y >= 0; y--)
		{
			fin.read((char*)line.data(), line.size());
			std::size_t i = 0;
			for (std::int32_t x = 0; x < this->bmph.width; x++)
			{
				Pixel color{};
				color.b = line[i++];
				color.g = line[i++];
				color.r = line[i++];
				pixels[GetArrPos(x, y)] = color;
			}
		}
		fin.close();
	}


	void Alter()
	{
		int x0 = this->bmph.width/2;
		int y0 = this->bmph.height / 2;
		int r = 50;
		if(!this->pixels) throw new std::exception("BMPF is empty");
		for (int i = 0; i < this->bmph.height; i++)
		{
			for (int j = 0; j < this->bmph.width; j++)
			{
				if (std::pow((i-x0), 2) + std::pow((j-y0), 2) < std::pow(r, 2))
				{
					this->pixels[GetArrPos(i, j)] = Pixel(0, 0, 0);
				}
			}
		}
	}

	void Write(const std::string& path)
	{
		const SDWORD row_size = this->bmph.width * 3 + this->bmph.width % 4;
		const DWORD bitmap_size = row_size * this->bmph.height;
		BitmapHeader header{};
		header.magic = BMPSIGN;
		header.file_size = bitmap_size + sizeof(BitmapHeader);
		header.reserved1 = 0;
		header.reserved2 = 0;
		header.offset_bits = sizeof(BitmapHeader);
		/* Bitmap file info structure */
		header.size = 40;
		header.width = this->bmph.width;
		header.height = this->bmph.height;
		header.planes = 1;
		header.bits_per_pixel = sizeof(Pixel) * 8; // 24bpp
		header.compression = 0;
		header.size_image = bitmap_size;
		header.x_pixels_per_meter = 0;
		header.y_pixels_per_meter = 0;
		header.clr_used = 0;
		header.clr_important = 0;
		std::ofstream fout = std::ofstream(path, std::ios::binary);
		if(!fout.is_open()) throw new std::exception("File couldn't be opened");
		fout.write((char*)(&header), sizeof(BitmapHeader));
		std::vector<BITE> line(row_size);
		for (SDWORD y = this->bmph.height - 1; y >= 0; y--)
		{
			std::size_t i = 0;
			for (SDWORD x = 0; x < this->bmph.width; x++)
			{
				const Pixel& color = this->pixels[GetArrPos(x, y)];
				line[i++] = color.b;
				line[i++] = color.g;
				line[i++] = color.r;
			}
			fout.write((char*)(line.data()), line.size());
		}
		fout.close();
	}
};
//
//struct CIEXYZ;
//struct BMP;
//struct CIEXYZTRIPLE;
//struct Header;
//struct InfoHeader;
//struct RGBQ;
//
//struct CIEXYZ
//{
//	CIEXYZ()
//	{
//		DWORD ciexyzX = 0;
//		DWORD ciexyzY = 0;
//		DWORD ciexyzZ = 0;
//	}
//	DWORD ciexyzX = 0;
//	DWORD ciexyzY = 0;
//	DWORD ciexyzZ = 0;
//};
//
//struct CIEXYZTRIPLE
//{
//	CIEXYZTRIPLE()
//	{
//		CIEXYZ ciexyzRed = CIEXYZ();
//		CIEXYZ ciexyzGreen = CIEXYZ();
//		CIEXYZ ciexyzBlue = CIEXYZ();
//	}
//	CIEXYZ ciexyzRed = CIEXYZ();
//	CIEXYZ ciexyzGreen = CIEXYZ();
//	CIEXYZ ciexyzBlue = CIEXYZ();
//};
//
//struct Header
//{
//public:
//	WORD signature = 0;
//	DWORD fileSize = 0;
//	WORD reserved1 = 0;
//	WORD reserved2 = 0;
//	DWORD dataOffset = 0;
//	Header()
//	{
//		WORD signature = 0;
//		DWORD fileSize = 0;
//		WORD reserved1 = 0;
//		WORD reserved2 = 0;
//		DWORD dataOffset = 0;
//	}
//};
//
//struct InfoHeader
//{
//	InfoHeader()
//	{
//		DWORD size = 0;
//		DWORD width = 0;
//		DWORD height = 0;
//		WORD planes = 0;
//		WORD bitCount = 0;
//		DWORD compression = 0;
//		DWORD imageSize = 0;
//		DWORD xPixelsPerM = 0;
//		DWORD yPixelsPerM = 0;
//		DWORD colorsUsed = 0;
//		DWORD colorsImportant = 0;
//
//		DWORD redMask = 0;
//		DWORD greenMask = 0;
//		DWORD blueMask = 0;
//		DWORD alphaMask = 0;
//		DWORD CSType = 0;
//		CIEXYZTRIPLE endPoints = CIEXYZTRIPLE();
//		DWORD gammaRed = 0;
//		DWORD gammaGreen = 0;
//		DWORD gammaBlue = 0;
//
//		DWORD intent = 0;
//		DWORD profileData = 0;
//		DWORD profileSize = 0;
//		DWORD reserved = 0;
//	}
//	DWORD size = 0;
//	DWORD width = 0;
//	DWORD height = 0;
//	WORD planes = 0;
//	WORD bitCount = 0;
//	DWORD compression = 0;
//	DWORD imageSize = 0;
//	DWORD xPixelsPerM = 0;
//	DWORD yPixelsPerM = 0;
//	DWORD colorsUsed = 0;
//	DWORD colorsImportant = 0;
//	
//	DWORD redMask = 0;
//	DWORD greenMask = 0;
//	DWORD blueMask = 0;
//	DWORD alphaMask = 0;
//	DWORD CSType = 0;
//	CIEXYZTRIPLE endPoints = CIEXYZTRIPLE();
//	DWORD gammaRed = 0;
//	DWORD gammaGreen = 0;
//	DWORD gammaBlue = 0;
//
//	DWORD intent = 0;
//	DWORD profileData = 0;
//	DWORD profileSize = 0;
//	DWORD reserved = 0;
//};
//
//struct RGBQ
//{
//	RGBQ()
//	{
//		BITE red = 0;
//		BITE green = 0;
//		BITE blue = 0;
////		BITE reserved = 0;
//	}
//	BITE red = 0;
//	BITE green = 0;
//	BITE blue = 0;
////	BITE reserved = 0;
//};
//
//struct BMP
//{
//	BMP()
//	{
//		header = Header();
//		infoHeader = InfoHeader();
//		//rgbquad = nullptr;
//	}
//	Header header;
//	InfoHeader infoHeader;
//	// RasterData rasterData = RasterData();
//	//RGBQ** rgbquad;
//	std::vector<std::vector<RGBQ>> rgbquad;
//};
//
//class BMPFile
//{
//	BMP* bmp = nullptr;
//private:
//	BITE bitextract(const DWORD byte, const DWORD mask)
//	{
//		if (mask == 0) return 0;
//		DWORD maskBuffer = mask;
//		DWORD maskPadding = 0;
//		while (!(maskBuffer & 1))
//		{
//			maskBuffer >>= 1;
//			maskPadding++;
//		}
//		return (byte & mask) >> maskPadding;
//	}
//public:
//	BMPFile()
//	{
//		bmp = nullptr;
//	}
//	BMPFile(const char* path) : BMPFile()
//	{
//		ReadBMP(path);
//	}
//	void ReadBMP(const char* path)
//	{
//		if (this->bmp) delete this->bmp;
//		this->bmp = new BMP();
//		std::ifstream fin = std::ifstream(path, std::ios_base::binary);
//		if (!fin.is_open()) throw new std::exception("File couldn't be opened");
//		fin.read((char*)&bmp->header.signature, sizeof(bmp->header.signature));
//		if (!(bmp->header.signature == BMPSIGN)) throw new std::exception("File signature doesn't match");
//		fin.read((char*)&bmp->header.fileSize, sizeof(bmp->header.fileSize));
//		fin.read((char*)&bmp->header.reserved1, sizeof(bmp->header.reserved1));
//		fin.read((char*)&bmp->header.reserved2, sizeof(bmp->header.reserved2));
//		fin.read((char*)&bmp->header.dataOffset, sizeof(bmp->header.dataOffset));
//
//		fin.read((char*)&bmp->infoHeader.size, sizeof(bmp->infoHeader.size));
//		fin.read((char*)&bmp->infoHeader.width, sizeof(bmp->infoHeader.width));
//		fin.read((char*)&bmp->infoHeader.height, sizeof(bmp->infoHeader.height));
//		fin.read((char*)&bmp->infoHeader.planes, sizeof(bmp->infoHeader.planes));
//		fin.read((char*)&bmp->infoHeader.bitCount, sizeof(bmp->infoHeader.bitCount));
//
//		int colorsCount = bmp->infoHeader.bitCount >> 3;
//		if (colorsCount < 3) colorsCount = 3;
//
//		int bitsPerColor = bmp->infoHeader.bitCount / colorsCount;
//		int maskValue = (1 << bitsPerColor) - 1;
//
//		if (this->bmp->infoHeader.size >= 40)
//		{
//			fin.read((char*)&bmp->infoHeader.compression, sizeof(bmp->infoHeader.compression));
//			fin.read((char*)&bmp->infoHeader.imageSize, sizeof(bmp->infoHeader.imageSize));
//			fin.read((char*)&bmp->infoHeader.xPixelsPerM, sizeof(bmp->infoHeader.xPixelsPerM));
//			fin.read((char*)&bmp->infoHeader.yPixelsPerM, sizeof(bmp->infoHeader.yPixelsPerM));
//			fin.read((char*)&bmp->infoHeader.colorsUsed, sizeof(bmp->infoHeader.colorsUsed));
//			fin.read((char*)&bmp->infoHeader.colorsImportant, sizeof(bmp->infoHeader.colorsImportant));
//		}
//
//		if (this->bmp->infoHeader.size >= 52)
//		{
//			fin.read((char*)&bmp->infoHeader.redMask, sizeof(bmp->infoHeader.redMask));
//			fin.read((char*)&bmp->infoHeader.greenMask, sizeof(bmp->infoHeader.greenMask));
//			fin.read((char*)&bmp->infoHeader.blueMask, sizeof(bmp->infoHeader.blueMask));
//		}
//
//		if (bmp->infoHeader.redMask == 0 || bmp->infoHeader.greenMask == 0 || bmp->infoHeader.blueMask == 0)
//		{
//			bmp->infoHeader.redMask = maskValue << (bitsPerColor * 2);
//			bmp->infoHeader.greenMask = maskValue << bitsPerColor;
//			bmp->infoHeader.blueMask = maskValue;
//		}
//		if (this->bmp->infoHeader.size >= 56)
//		{
//			fin.read((char*)&bmp->infoHeader.alphaMask, sizeof(bmp->infoHeader.alphaMask));
//		}
//		else
//		{
//			bmp->infoHeader.alphaMask = maskValue << (bitsPerColor * 3);
//		}
//
//		if (this->bmp->infoHeader.size >= 108)
//		{
//			fin.read((char*)&bmp->infoHeader.CSType, sizeof(bmp->infoHeader.CSType));
//			fin.read((char*)&bmp->infoHeader.endPoints, sizeof(bmp->infoHeader.endPoints));
//			fin.read((char*)&bmp->infoHeader.gammaRed, sizeof(bmp->infoHeader.gammaRed));
//			fin.read((char*)&bmp->infoHeader.gammaGreen, sizeof(bmp->infoHeader.gammaGreen));
//			fin.read((char*)&bmp->infoHeader.gammaBlue, sizeof(bmp->infoHeader.gammaBlue));
//		}
//
//		if (this->bmp->infoHeader.size >= 124)
//		{
//			fin.read((char*)&bmp->infoHeader.intent, sizeof(bmp->infoHeader.intent));
//			fin.read((char*)&bmp->infoHeader.profileData, sizeof(bmp->infoHeader.profileData));
//			fin.read((char*)&bmp->infoHeader.profileSize, sizeof(bmp->infoHeader.profileSize));
//			fin.read((char*)&bmp->infoHeader.reserved, sizeof(bmp->infoHeader.reserved));
//		}
//		DWORD buff = 0;
//		/*this->bmp->rgbquad = new RGBQ * [this->bmp->infoHeader.height];
//		for (size_t i = 0; i < this->bmp->infoHeader.height; i++)
//		{
//			this->bmp->rgbquad[i] = new RGBQ[this->bmp->infoHeader.width];
//		}*/
//		for (size_t i = 0; i < this->bmp->infoHeader.height; i++)
//		{
//			this->bmp->rgbquad.push_back(std::vector<RGBQ>());
//		}
//		for (size_t i = 0; i < this->bmp->infoHeader.height; i++)
//		{
//			for (size_t j = 0; j < this->bmp->infoHeader.width; j++)
//			{
//				this->bmp->rgbquad[i].push_back(RGBQ());
//			}
//			//fin.seekg(padding, std::ios_base::cur);
//		}
//		int padding = ((this->bmp->infoHeader.width * (this->bmp->infoHeader.bitCount / 8)) % 4) & 3;
//
//		for (size_t i = 0; i < this->bmp->infoHeader.height; i++)
//		{
//			for (size_t j = 0; j < this->bmp->infoHeader.width; j++)
//			{
//				fin.read((char*)&buff, sizeof(this->bmp->infoHeader.bitCount / 8));
//				this->bmp->rgbquad[i][j].red = bitextract(buff, this->bmp->infoHeader.redMask);
//				this->bmp->rgbquad[i][j].green = bitextract(buff, this->bmp->infoHeader.greenMask);
//				this->bmp->rgbquad[i][j].blue = bitextract(buff, this->bmp->infoHeader.blueMask);
////				this->bmp->rgbquad[i][j].reserved = bitextract(buff, this->bmp->infoHeader.alphaMask);
//			}
//			//fin.seekg(padding, std::ios_base::cur);
//		}
//		
//		fin.close();
//		//int i = 1;
//		//int j = 0;
//		//std::cout << "R: " << (int)this->bmp->rgbquad[i][j].red << " G: " << (int)this->bmp->rgbquad[i][j].green << " B: " << (int)this->bmp->rgbquad[i][j].blue << " A: " << (int)this->bmp->rgbquad[i][j].reserved << std::endl;
//		//for (size_t i = 0; i < 10; i++)
//		//{
//		//	for (size_t j = 0; j < 10; j++)
//		//	{
//		//		//if((int)this->bmp->rgbquad[i][j].red !=0 || (int)this->bmp->rgbquad[i][j].green != 0 || (int)this->bmp->rgbquad[i][j].blue != 0 || (int)this->bmp->rgbquad[i][j].reserved != 0)
//		//		std::cout << "R: " << (int)this->bmp->rgbquad[i][j].red << " G: " << (int)this->bmp->rgbquad[i][j].green << " B: " << (int)this->bmp->rgbquad[i][j].blue << " A: " << (int)this->bmp->rgbquad[i][j].reserved << std::endl;
//		//	}
//		//}
//	}
//
//	void WriteBMP(const char* path)
//	{
//		//AlterBMP();
//		//AlterBMP();
//		if (!this->bmp) throw new std::exception("File already loaded");
//		std::ofstream fout = std::ofstream(path, std::ios_base::binary);
//		if (!fout.is_open()) throw new std::exception("File couldn't be opened");
//		fout.write((char*)&bmp->header.signature, sizeof(bmp->header.signature));
//		//if (!(bmp->header.signature == BMPSIGN)) throw new std::exception("File signature doesn't match");
//		fout.write((char*)&bmp->header.fileSize, sizeof(bmp->header.fileSize));
//		fout.write((char*)&bmp->header.reserved1, sizeof(bmp->header.reserved1));
//		fout.write((char*)&bmp->header.reserved2, sizeof(bmp->header.reserved2));
//		fout.write((char*)&bmp->header.dataOffset, sizeof(bmp->header.dataOffset));
//
//		fout.write((char*)&bmp->infoHeader.size, sizeof(bmp->infoHeader.size));
//		fout.write((char*)&bmp->infoHeader.width, sizeof(bmp->infoHeader.width));
//		fout.write((char*)&bmp->infoHeader.height, sizeof(bmp->infoHeader.height));
//		fout.write((char*)&bmp->infoHeader.planes, sizeof(bmp->infoHeader.planes));
//		fout.write((char*)&bmp->infoHeader.bitCount, sizeof(bmp->infoHeader.bitCount));
//
//		if (this->bmp->infoHeader.size >= 40)
//		{
//			fout.write((char*)&bmp->infoHeader.compression, sizeof(bmp->infoHeader.compression));
//			fout.write((char*)&bmp->infoHeader.imageSize, sizeof(bmp->infoHeader.imageSize));
//			fout.write((char*)&bmp->infoHeader.xPixelsPerM, sizeof(bmp->infoHeader.xPixelsPerM));
//			fout.write((char*)&bmp->infoHeader.yPixelsPerM, sizeof(bmp->infoHeader.yPixelsPerM));
//			fout.write((char*)&bmp->infoHeader.colorsUsed, sizeof(bmp->infoHeader.colorsUsed));
//			fout.write((char*)&bmp->infoHeader.colorsImportant, sizeof(bmp->infoHeader.colorsImportant));
//		}
//
//		if (this->bmp->infoHeader.size >= 52)
//		{
//			fout.write((char*)&bmp->infoHeader.redMask, sizeof(bmp->infoHeader.redMask));
//			fout.write((char*)&bmp->infoHeader.greenMask, sizeof(bmp->infoHeader.greenMask));
//			fout.write((char*)&bmp->infoHeader.blueMask, sizeof(bmp->infoHeader.blueMask));
//		}
//
//		if (this->bmp->infoHeader.size >= 56)
//		{
//			fout.write((char*)&bmp->infoHeader.alphaMask, sizeof(bmp->infoHeader.alphaMask));
//		}
//
//		if (this->bmp->infoHeader.size >= 108)
//		{
//			fout.write((char*)&bmp->infoHeader.CSType, sizeof(bmp->infoHeader.CSType));
//			fout.write((char*)&bmp->infoHeader.endPoints, sizeof(bmp->infoHeader.endPoints));
//			fout.write((char*)&bmp->infoHeader.gammaRed, sizeof(bmp->infoHeader.gammaRed));
//			fout.write((char*)&bmp->infoHeader.gammaGreen, sizeof(bmp->infoHeader.gammaGreen));
//			fout.write((char*)&bmp->infoHeader.gammaBlue, sizeof(bmp->infoHeader.gammaBlue));
//		}
//
//		if (this->bmp->infoHeader.size >= 124)
//		{
//			fout.write((char*)&bmp->infoHeader.intent, sizeof(bmp->infoHeader.intent));
//			fout.write((char*)&bmp->infoHeader.profileData, sizeof(bmp->infoHeader.profileData));
//			fout.write((char*)&bmp->infoHeader.profileSize, sizeof(bmp->infoHeader.profileSize));
//			fout.write((char*)&bmp->infoHeader.reserved, sizeof(bmp->infoHeader.reserved));
//		}
//		//DWORD buff = 0;;
//		/*this->bmp->rgbquad = new RGBQ * [this->bmp->infoHeader.height];
//		for (size_t i = 0; i < this->bmp->infoHeader.height; i++)
//		{
//			this->bmp->rgbquad[i] = new RGBQ[this->bmp->infoHeader.width];
//		}*/
//		size_t i = this->bmp->infoHeader.height - 1, j = 0;
//		int padding = ((this->bmp->infoHeader.width * (this->bmp->infoHeader.bitCount / 8)) % 4) & 3;
//		try
//		{
//			while(i>=0)//for ( i = this->bmp->infoHeader.height - 1; i >= 0; --i)
//			{
//				for ( j = 0; j < this->bmp->infoHeader.width; j++)
//				{
//						fout.put(this->bmp->rgbquad[i][j].blue);
//						fout.put(this->bmp->rgbquad[i][j].green);
//						fout.put(this->bmp->rgbquad[i][j].red);
//						
//					//fout.put(this->bmp->rgbquad[i][j].reserved);
//				}
//				--i;
//			}
//		}
//		catch (...)
//		{
//			std::cout << i << ' ' << j << std::endl;
//		}
//		//for (size_t i = 0; i < this->bmp->infoHeader.height; i++)
//		//{
//		//	for (size_t j = 0; j < this->bmp->infoHeader.width; j++)
//		//	{
//		//		//fout.write((char*)&buff, sizeof(this->bmp->infoHeader.bitCount / 8));
//		//		
//		//		fout.write((char*)&this->bmp->rgbquad[i][j].blue, sizeof(this->bmp->rgbquad[i][j].blue));
//		//		fout.write((char*)&this->bmp->rgbquad[i][j].green, sizeof(this->bmp->rgbquad[i][j].green));
//		//		fout.write((char*)&this->bmp->rgbquad[i][j].red, sizeof(this->bmp->rgbquad[i][j].red));// this->bmp->rgbquad[i][j].red = bitextract(buff, this->bmp->infoHeader.redMask);
//		//		fout.write((char*)&this->bmp->rgbquad[i][j].reserved, sizeof(this->bmp->rgbquad[i][j].reserved));
//		//	}
//		//	//fout.seekp(padding, std::ios_base::cur);
//		//}
//		fout.close();
//	}
//	void AlterBMP()
//	{
//		int radius = 40;
//		int x = 30, y = 40;
//		size_t ctr = 0;
//		int padding = ((this->bmp->infoHeader.width * (this->bmp->infoHeader.bitCount / 8)) % 4) & 3;
//		int w = 0;
//		int h = 1;
//		for (size_t i = 0; i < this->bmp->infoHeader.height; i++)
//		{
//			for (size_t j = 0; j < this->bmp->infoHeader.width; j++)
//			{
//				if (this->bmp->rgbquad[i][j].red == 255 && this->bmp->rgbquad[i][j].green == 255 && this->bmp->rgbquad[i][j].blue == 255)
//				{
//					std::cout << i << " " << j << std::endl;
//				}
//			}
//		}
//		this->bmp->rgbquad[h][w].red = 255; //360 x 480
//		this->bmp->rgbquad[h][w].green = 255;
//		this->bmp->rgbquad[h][w].blue = 255;
//		for (size_t i = 0; i < 1; i++)//this->bmp->infoHeader.height
//		{
//			for (size_t j = 0; j < this->bmp->infoHeader.width; j++)//this->bmp->infoHeader.width-1
//			{
//				
//				//fout.write((char*)&buff, sizeof(this->bmp->infoHeader.bitCount / 8));
//				//if (std::pow((i - x), 2) + std::pow((j - y), 2) <= std::pow(radius, 2))
//				//{
//					/*this->bmp->rgbquad[i][j].red -= 127;
//					//this->bmp->rgbquad[i][j].green -= 127;
//					//this->bmp->rgbquad[i][j].blue -= 127;
//					//if (this->bmp->rgbquad[i][j].red < 0) this->bmp->rgbquad[i][j].red += 255;
//					//if (this->bmp->rgbquad[i][j].green < 0) this->bmp->rgbquad[i][j].red += 255;
//					//if (this->bmp->rgbquad[i][j].blue < 0) this->bmp->rgbquad[i][j].red += 255;
//					//this->bmp->rgbquad[i][j].red %= 255;
//					//this->bmp->rgbquad[i][j].green %= 255;
//					this->bmp->rgbquad[i][j].blue %= 255;*/
//					/*this->bmp->rgbquad[i][j].red = 1234;
//					this->bmp->rgbquad[i][j].green = 0;
//					this->bmp->rgbquad[i][j].blue = 0;*/
//					//this->bmp->rgbquad[i][j].reserved = 0;
//				//ctr++;
//				//std::cout << ctr << " : " << (int)this->bmp->rgbquad[i][j].red << ' ' << (int)this->bmp->rgbquad[i][j].green << ' ' << (int)this->bmp->rgbquad[i][j].blue << ' ' << (int)this->bmp->rgbquad[i][j].reserved << std::endl;
//				//}
//			}
//		}
//	}
//	~BMPFile()
//	{
//		if (this->bmp) delete this->bmp;
//	}
//};
//
