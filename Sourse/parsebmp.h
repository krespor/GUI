#ifndef PARSEBMP_H
#define PARSEBMP_H

#include <QByteArray>
#include <QDebug>

#include <iostream>
#include <fstream>

using namespace std;

class ParseBmp
{
public:
    ParseBmp() {};
    bool open(const char* path);
    QByteArray getLine();
    QByteArray getWH();

    void close();

private:
    ifstream file;
    QByteArray line;

    unsigned int  i = 0;

    unsigned int height;
    unsigned int width;

    unsigned short WORD;
    unsigned int DWORD;

    template <class T>
    void read(T &buff, int size);
};

#endif // PARSEBMP_H
