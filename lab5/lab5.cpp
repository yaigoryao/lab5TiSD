// lab5.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>

#include "BMPFile.h"
#include "BMP.h"

#include <complex>
#include ".\color_maps.inl"

using namespace std;
int main()
{
    BMPF image(1280, 920);
    double cr, ci;
    double nextr, nexti;
    double prevr, previ;
    constexpr const std::uint16_t max_iterations = 3000;

    for (std::int32_t y = 0; y < image.Height(); ++y)
    {
        for (std::int32_t x = 0; x < image.Width(); ++x)
        {
            cr = 1.5 * (2.0 * x / image.Width() - 1.0) - 0.5;
            ci = (2.0 * y / image.Height() - 1.0);

            nextr = nexti = 0;
            prevr = previ = 0;

            for (std::uint16_t i = 0; i < max_iterations; ++i)
            {
                prevr = nextr;
                previ = nexti;

                nextr = prevr * prevr - previ * previ + cr;
                nexti = 2 * prevr * previ + ci;

                if (((nextr * nextr) + (nexti * nexti)) > 4)
                {
                    const double z = sqrt(1.5*nextr * nextr + nexti * nexti - 0.6);

                    // https://en.wikipedia.org/wiki/Mandelbrot_set#Continuous_.28smooth.29_coloring
                    const std::uint32_t index = static_cast<std::uint32_t>(1000.0 * log2(1.75 + i - log2(log2(std::abs(z))) / log2(max_iterations)));

                    image.Set(x, y, hot_colormap[index]);

                    break;
                }
            }
        }
    }

    image.Write("mandelbrot.bmp");
    //constexpr const std::uint16_t max_iterations = 300;

    //constexpr const double cr = -0.70000;
    //constexpr const double ci = 0.27015;

    //double prevr, previ;

    //double prevx = 0, prevy = 0;
    //const int x0 = 1;
    //const int y0 = 1;
    //for (std::int32_t y = 0; y < image.Height(); ++y)
    //{
    //    for (std::int32_t x = 0; x < image.Width(); ++x)
    //    {
    //        const Pixel color = hot_colormap[static_cast<std::size_t>((1000.0 * prevx) / max_iterations)];//hsv_colormap[static_cast<std::size_t>((1000.0 * i) / max_iterations)];
    //        double nextr = 1.5 * (2.0 * x / image.Width() - 1.0);
    //        double nexti = (2.0 * y / image.Height() - 1.0);

    //        for (std::uint16_t i = 0; i < max_iterations; ++i)
    //        {
    //            prevr = nextr;
    //            previ = nexti;

    //            nextr = prevr * prevr - previ * previ + cr;
    //            nexti = 2 * prevr * previ + ci;

    //            if (((nextr * nextr) + (nexti * nexti)) > 4)
    //            {
    //                const Pixel color = hot_colormap[static_cast<std::size_t>((1000.0 * i) / max_iterations)];//hsv_colormap[static_cast<std::size_t>((1000.0 * i) / max_iterations)];
    //                image.Set(x, y, color);
    //                break;
    //            }
    //        }

    //    }
    //}

    //image.Write("julia.bmp");
	//BMPFile test = BMPFile("3sample.bmp");
	//test.WriteBMP("new2.bmp");
	//BMPF test = BMPF(500, 400);
	//test.Alter();
	//test.Write("nnew.bmp");
	/*bmp::Bitmap img = bmp::Bitmap();
	img.load("sample.bmp");
	img.save("new3.bmp");*/
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
