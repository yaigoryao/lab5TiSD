#include <iostream>
#include "Addition.h"
using namespace std;
int main(int argc, char** argv)
{
    setlocale(LC_ALL, "ru");

    if (!Start(argc, argv))
    {
        Menu menu = Menu("Петроченков И. А. ИУК4-32Б Вар. 18");
        menu.AddItem("Считать BMP файл", ReadBMP);
        menu.AddItem("Сохранить BMP файл", WriteBMP);
        menu.AddItem("Выполнить задание", DoTask);
        //menu.AddItem("Генерация изображения", GenerateBMP);
        //menu.AddItem("Изменить изображение", ChangeBMP);
        menu.RunMenu();
    }
}
    //BMPF image(1280, 920);
    //double cr, ci;
    //double nextr, nexti;
    //double prevr, previ;
    //constexpr const std::uint16_t max_iterations = 3000;

    //for (std::int32_t y = 0; y < image.Height(); ++y)
    //{
    //    for (std::int32_t x = 0; x < image.Width(); ++x)
    //    {
    //        cr = 1.5 * (2.0 * x / image.Width() - 1.0) - 0.5;
    //        ci = (2.0 * y / image.Height() - 1.0);

    //        nextr = nexti = 0;
    //        prevr = previ = 0;

    //        for (std::uint16_t i = 0; i < max_iterations; ++i)
    //        {
    //            prevr = nextr;
    //            previ = nexti;

    //            nextr = prevr * prevr - previ * previ + cr;
    //            nexti = 2 * prevr * previ + ci;

    //            if (((nextr * nextr) + (nexti * nexti)) > 4)
    //            {
    //                const double z = sqrt(1.5*nextr * nextr + nexti * nexti - 0.6);

    //                // https://en.wikipedia.org/wiki/Mandelbrot_set#Continuous_.28smooth.29_coloring
    //                const std::uint32_t index = static_cast<std::uint32_t>(1000.0 * log2(1.75 + i - log2(log2(std::abs(z))) / log2(max_iterations)));

    //                image.Set(x, y, hot_colormap[index]);

    //                break;
    //            }
    //        }
    //    }
    //}

    //image.Write("mandelbrot.bmp");
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
