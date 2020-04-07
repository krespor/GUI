#include "parsebmp.h"

ParseBmp::ParseBmp(const char *path)
{
    file.open(path, ios::binary);

    if (!file)
        cout << "file not open" << endl; //!

    //Заголовок файла
    read(WORD, sizeof (WORD));      //Сигнатура "BM"
    read(DWORD, sizeof (DWORD));    //Размер файла
    read(WORD, sizeof (WORD));      //Зарезервировано
    read(WORD, sizeof (WORD));      //Зарезервировано
    read(DWORD, sizeof (DWORD));    //Смещение изображения от начала файла

    //Информационный заголовок
    read(DWORD, sizeof (DWORD));    //Длина заголовка
    read(width, sizeof (width));    //Ширина изображения
    read(height, sizeof (height));  //Высота изображения
    read(WORD, sizeof (WORD));      //Число плоскостей
    read(WORD, sizeof (WORD));      //Глубина цвета, бит на точку
    read(DWORD, sizeof (DWORD));    //Тип компрессии
    read(DWORD, sizeof (DWORD));    //Размер изображения, байт
    read(DWORD, sizeof (DWORD));    //Горизонтальное разрешение, точки на метр
    read(DWORD, sizeof (DWORD));    //Вертикальное разрешение, точки на метр
    read(DWORD, sizeof (DWORD));    //Число используемых цветов
    read(DWORD, sizeof (DWORD));    //Число основных цветов
}

//возвращает следующую строку по пикселям в формате ([r,g,b] [r,g,b] [r,g,b] [r,g,b] ... [r,g,b])
QByteArray ParseBmp::getLine()
{
    static unsigned int r, g, b;

    if (i < height)
    {
        line = "";
        for (unsigned int j = 0; j < width; j++)
        {
            read(b, 1);
            read(g, 1);
            read(r, 1);

            line += to_string(r).c_str();
            line += " ";
            line += to_string(g).c_str();
            line += " ";
            line += to_string(b).c_str();
            line += " ";
        }
        line.remove(line.size() - 1, 1);
        read(r, width % 4);

        i++;

        return line;
    } else
        return NULL;
}

//возвращает ширину и высоту изображения в формате ([w] [h])
QByteArray ParseBmp::getWH()
{
    QByteArray wh = "";
    wh += to_string(width).c_str();
    wh += " ";
    wh += to_string(height).c_str();

    return wh;
}

void ParseBmp::close()
{
    file.close();
}

template<class T>
void ParseBmp::read(T &buff, int size)
{
    file.read(reinterpret_cast<char*>(&buff), size);
}
