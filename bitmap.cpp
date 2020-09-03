#include <cmath>
#include <iostream>
#include "bitmap.h"

BITMAPFILEHEADER* readBitmapFileHeader(std::ifstream& is) {
    BITMAPFILEHEADER* bitmapFileHeader;

    is.seekg(0, std::ifstream::beg);

    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;

    is.read(reinterpret_cast<char *>(&bfType), sizeof(bfType));
    is.read(reinterpret_cast<char *>(&bfSize), sizeof(bfSize));
    is.read(reinterpret_cast<char *>(&bfReserved1), sizeof(bfReserved1));
    is.read(reinterpret_cast<char *>(&bfReserved2), sizeof(bfReserved2));
    is.read(reinterpret_cast<char *>(&bfOffBits), sizeof(bfOffBits));

    bitmapFileHeader = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
    bitmapFileHeader->bfType = bfType;
    bitmapFileHeader->bfSize = bfSize;
    bitmapFileHeader->bfReserved1 = bfReserved1;
    bitmapFileHeader->bfReserved2 = bfReserved2;
    bitmapFileHeader->bfOffBits = bfOffBits;

    return bitmapFileHeader;
}

BITMAPINFOHEADER* readBitmapInfoHeader(std::ifstream& is) {
    BITMAPINFOHEADER *bitmapInfoHeader;

    is.seekg(14, std::ifstream::beg);

    unsigned int biSize;
    long biWidth;
    long biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    long biXPelsPerMeter;
    long biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;

    is.read(reinterpret_cast<char *>(&biSize), sizeof(biSize));
    is.read(reinterpret_cast<char *>(&biWidth), sizeof(biWidth));
    is.read(reinterpret_cast<char *>(&biHeight), sizeof(biHeight));
    is.read(reinterpret_cast<char *>(&biPlanes), sizeof(biPlanes));
    is.read(reinterpret_cast<char *>(&biBitCount), sizeof(biBitCount));
    is.read(reinterpret_cast<char *>(&biCompression), sizeof(biCompression));
    is.read(reinterpret_cast<char *>(&biSizeImage), sizeof(biSizeImage));
    is.read(reinterpret_cast<char *>(&biXPelsPerMeter), sizeof(biXPelsPerMeter));
    is.read(reinterpret_cast<char *>(&biYPelsPerMeter), sizeof(biYPelsPerMeter));
    is.read(reinterpret_cast<char *>(&biClrUsed), sizeof(biClrUsed));
    is.read(reinterpret_cast<char *>(&biClrImportant), sizeof(biClrImportant));

    bitmapInfoHeader = (BITMAPINFOHEADER *) malloc(sizeof(BITMAPINFOHEADER));
    bitmapInfoHeader->biSize = biSize;
    bitmapInfoHeader->biWidth = biWidth;
    bitmapInfoHeader->biHeight = biHeight;
    bitmapInfoHeader->biPlanes = biPlanes;
    bitmapInfoHeader->biBitCount = biBitCount;
    bitmapInfoHeader->biCompression = biCompression;
    bitmapInfoHeader->biSizeImage = biSizeImage;
    bitmapInfoHeader->biXPelsPerMeter = biXPelsPerMeter;
    bitmapInfoHeader->biYPelsPerMeter = biYPelsPerMeter;
    bitmapInfoHeader->biClrUsed = biClrUsed;
    bitmapInfoHeader->biClrUsed = biClrUsed;

    return bitmapInfoHeader;
}

std::vector<QColor> readBitmapColorTable(std::ifstream& is, BITMAPINFOHEADER *bitmapInfoHeader, int colorTableEntries) {
    std::vector<QColor> BITMAPCOLORTABLE;

    is.seekg(14 + bitmapInfoHeader->biSize, std::ifstream::beg);

    unsigned char rgbBlue;
    unsigned char rgbGreen;
    unsigned char rgbRed;
    unsigned char rgbReserved;

    for (int i = 0; i < colorTableEntries; i++) {
        is.read(reinterpret_cast<char *>(&rgbBlue), sizeof(rgbBlue));
        is.read(reinterpret_cast<char *>(&rgbGreen), sizeof(rgbGreen));
        is.read(reinterpret_cast<char *>(&rgbRed), sizeof(rgbRed));
        is.read(reinterpret_cast<char *>(&rgbReserved), sizeof(rgbReserved));

        QColor qColor(rgbRed, rgbGreen, rgbBlue);
        //rgbQuad->rgbReserved = rgbReserved;

        BITMAPCOLORTABLE.push_back(qColor);
    }

    return BITMAPCOLORTABLE;
}

std::vector<unsigned char> readBitmapPixelIndices(std::ifstream& is, BITMAPFILEHEADER *bitmapFileHeader, BITMAPINFOHEADER* bitmapInfoHeader) {
    is.seekg(bitmapFileHeader->bfOffBits, std::ifstream::beg);

    std::vector<unsigned char> BITMAPPIXELINDICES;

    unsigned char pixelIndex;
    int numPixels = bitmapInfoHeader->biWidth * bitmapInfoHeader->biHeight;

    for (int i = 0; i < numPixels; i++) {
        is.read(reinterpret_cast<char *>(&pixelIndex), sizeof(pixelIndex));
        BITMAPPIXELINDICES.push_back(pixelIndex);
    }

    return BITMAPPIXELINDICES;
}

bool verifyBitmapFileHeader(BITMAPFILEHEADER *bitmapFileHeader, int fileLength) {
    if (fileLength != (int)bitmapFileHeader->bfSize) {
        std::cout << "Error: File size mismatch between actual and header information." << std::endl;
        return false;
    }

    return true;
}

bool verifyBitmapInfoHeader(BITMAPINFOHEADER *bitmapInfoHeader) {
    if (bitmapInfoHeader->biSize != 40) {
        std::cout << "Error: Bitmap information header is not BITMAPINFOHEADER." << std::endl;
        return false;
    } else if (bitmapInfoHeader->biBitCount != 8) {
        std::cout << "Error: Bitmap is not 8-bit." << std::endl;
        return false;
    } else if (bitmapInfoHeader->biCompression != 0) {
        std::cout << "Error: Bitmaps with compression are not supported." << std::endl;
        return false;
    }

    return true;
}

int getColorTableEntries(BITMAPINFOHEADER *bitmapInfoHeader) {
    int colorTableEntries;

    if (bitmapInfoHeader->biClrUsed > 0) {
        colorTableEntries = bitmapInfoHeader->biClrUsed;
    } else {
        colorTableEntries = (int) pow(2, bitmapInfoHeader->biBitCount);
    }

    return colorTableEntries;
}

bool compareBitmapDimensions(BITMAP *bmp1, BITMAP *bmp2) {
    if (bmp1->bitmapInfoHeader->biWidth != bmp2->bitmapInfoHeader->biWidth
            && bmp1->bitmapInfoHeader->biHeight != bmp2->bitmapInfoHeader->biHeight) {
        std::cout << "Error: Images do not have the same dimensions." << std::endl;
        return false;
    }

    return true;
}
