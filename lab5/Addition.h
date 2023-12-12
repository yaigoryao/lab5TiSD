#pragma once
#include "C:\Users\Игорь\Desktop\учеба\LabRab\ИУК4-32Б\Типы и структуры данных\lab3\lab3\Menu.h"
#include "C:\Users\Игорь\Desktop\учеба\LabRab\ИУК4-32Б\Типы и структуры данных\lab3\lab3\MenuItem.h"
#include "C:\Users\Игорь\Desktop\учеба\LabRab\ИУК4-32Б\Типы и структуры данных\lab3\lab3\Exceptions.h"

#include <iostream>
#include <fstream>
#include <random>
#include <string>
std::string exc = "exception.txt";
//std::string src = "alph.txt";
std::string dst = "changed.bmp";
std::string src = "figures.bmp";
#include <Windows.h>
#include "myBMP.h"

MyBMP bmp_ = MyBMP();

bool InputError()
{
	std::cin.clear();
	std::cin.ignore(INT_MAX, '\n');
	std::cout << "\nОшибка! Введено некорректное значение!\n Попробуйте снова.\n";
	return false;
}

bool Start(int argc, char* argv[])
{
	std::string message = "Параметры запуска программы:\nsource.txt - обязательный параметр, место хранения графа\ndestination.txt - обязательный параметр, место записи графа\n";
	if (argc <= 1)
	{
		//std::cout << argc << std::endl;
		std::cout << message;
		return false;
	}
	else
	{
		if (argc == 3)
		{
			//std::cout << argc << std::endl;
			src = argv[1];
			dst = argv[2];
			std::cout << dst;
			return true;
		}
		else if (argc > 3)
		{
			//std::cout << argc << std::endl;
			std::cout << message << std::endl;
			return false;
		}
	}
}

void ReadBMP()
{
	try
	{
		bmp_.Load(src.c_str());
		std::cout << "Файл успешно считан!" << std::endl;
	}
	catch (...)
	{
		std::cout << "Ошибка считывания файла" << std::endl;
	}
}

void WriteBMP()
{
	try
	{
		bmp_.Save(dst.c_str());
		std::cout << "Файл успешно записан!" << std::endl;
	}
	catch (...)
	{
		std::cout << "Ошибка записи файла" << std::endl;
	}
}

void DoTask()
{
	try
	{
		bmp_.DoTask();
		
		std::cout << "Цвет успешно изменен!" << std::endl;
	}
	catch (...)
	{
		std::cout << "Ошибка обработки файла" << std::endl;
	}
}
//std::vector<bmp::Pixel> colors;
		//for (int i = 0; i <= 1; i++)
		//{
		//	for (int j = 0; j <= 1; j++)
		//	{
		//		for (int k = 0; k <= 1; k++)
		//		{
		//			bmp::Pixel t = bmp::Pixel(0, 0, 0);
		//			if (i) t.r = 255;
		//			if (j) t.g = 255;
		//			if (k) t.b = 255;
		//			if(!(!i && !j && !k || i && j && k))
		//				colors.push_back(t);
		//		}
		//	}
		//}
		//for (int i = 0; i < bmp_.width(); i++)
		//{
		//	for (int j = 0; j < bmp_.height(); j++)
		//	{
		//		bmp::Pixel cur = bmp_.get(i, j);
		//		for (int i = 0; i<colors.size(); i++)
		//		{
		//			if (cur == colors[i])
		//			{
		//				cur = colors[(i + 1) % colors.size()];
		//				
		//				break;
		//			}
		//		}
		//		bmp_.set(i, j, cur);
		//		//bmp::Pixel new_pixel = bmp::Pixel(255 - cur.r, 255 - cur.g, 255 - cur.b);
		//		//
		//	}
		//}
//void ChangeBMP()
//{
//	try
//	{
//		/*for (int i = 0; i < bmp_.width(); i++)
//		{
//			for (int j = 0; j < bmp_.height(); j++)
//			{
//				bmp::Pixel cur = bmp_.get(i, j);
//				bmp::Pixel new_pixel = bmp::Pixel(255 - cur.r > 127 ? 255 - cur.r: cur.r, 255 - cur.g > 127 ? 255 - cur.g : cur.g, 255 - cur.b > 127 ? 255 - cur.b : cur.b);
//				bmp_.set(i, j, new_pixel);
//			}
//		}
//		std::cout << "Цвет успешно изменен!" << std::endl;*/
//	}
//	catch (...)
//	{
//		std::cout << "Ошибка обработки файла" << std::endl;
//	}
//}

//void GenerateBMP()
//{
//	try
//	{
//		int w = 1280;
//		int h = 720;
//		int wh = w / 2;
//		int hh = 720 / 2;
//		int cols = 1000;
//		int s = w / h;
//		bmp_.clear();
//		bmp_ = bmp::BMP(w, h);
//		for (int i = 0; i < bmp_.width(); i++)
//		{
//			for (int j = 0; j < bmp_.height(); j++)
//			{
//				int index = std::abs(wh - i) + std::abs(hh - j);
//				bmp_.set(i, j, hot_colormap[index]);
//			}
//		}
//		std::cout << "Изображение сгенерировано!" << std::endl;
//	}
//	catch (...)
//	{
//		std::cout << "Ошибка обработки файла" << std::endl;
//	}
//}

int clamp(int v, int l,int h)
{
	return v < l ? l : v > h ? h : v;
}

//void GenerateBMP()
//{
//	try
//	{
//		int w = 1280;
//		int h = 720;
//		int wh = w / 2;
//		int hh = 720 / 2;
//		int radius = 220;
//		bmp_.Clear();
//		bmp_ = MyBMP(w, h);
//		for (int i = 0; i < bmp_.Width(); i++)
//		{
//			for (int j = 0; j < bmp_.Height(); j++)
//			{
//				
//				/*if (std::pow(i - wh, 2) + std::pow(j - hh, 2) <= std::pow(radius, 2))
//				{
//					index = 999;
//				}
//				else
//				{
//					index = static_cast<int>(std::sqrt(std::pow(i - wh, 2) + std::pow(j - hh, 2)));
//					index = clamp(index, 0, bord);
//				}*/
//				int index = 0;
//				int bord = sizeof(hsv_colormap) / sizeof(hsv_colormap[0]) - 1;
//				//index = static_cast<int>(std::sqrt(std::pow(i - wh, 2) + std::pow(j - hh, 2)));
//				index = static_cast<int>(0);
//				index = clamp(index, 0, bord);
//				bmp_.Set(i, j, hsv_colormap[index]);
//			}
//		}
//		std::cout << "Изображение сгенерировано!" << std::endl;
//	}
//	catch (...)
//	{
//		std::cout << "Ошибка обработки файла" << std::endl;
//	}
//	WriteBMP();
//}