#include <cmath>
#include <iostream>
#include <unordered_set>
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

bool verifyBitmapFileHeader(BITMAPFILEHEADER *bitmapFileHeader, int fileLength, QString &errorMessage) {
    if (fileLength != (int)bitmapFileHeader->bfSize) {
       errorMessage = "File size mismatch between actual and header information.";
        return false;
    }

    return true;
}

bool verifyBitmapInfoHeader(BITMAPINFOHEADER *bitmapInfoHeader, QString &errorMessage) {
    if (bitmapInfoHeader->biSize != 40) {
        errorMessage = "Bitmap information header is not BITMAPINFOHEADER.";
        return false;
    } else if (bitmapInfoHeader->biBitCount != 8) {
        errorMessage = "Bitmap is not 8-bit.";
        return false;
    } else if (bitmapInfoHeader->biCompression != 0) {
        errorMessage = "Bitmaps with compression are not supported.";
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

bool compareBitmapDimensions(BITMAP *bmp1, BITMAP *bmp2, QString &errorMessage) {
    if (bmp1->bitmapInfoHeader->biWidth != bmp2->bitmapInfoHeader->biWidth
            && bmp1->bitmapInfoHeader->biHeight != bmp2->bitmapInfoHeader->biHeight) {
        errorMessage = "Images do not have the same dimensions.";
        return false;
    }

    return true;}

bool constructNewColorTable(BITMAP *bmp1, BITMAP *bmp2, std::vector<QColor> &bitmapColorTable) {
    std::unordered_set<QRgb> colorTable;

    for (unsigned int i = 0; i < bmp1->bitmapPixelIndices.size(); i++) {
        QRgb qRgb = bmp1->bitmapColorTable[bmp1->bitmapPixelIndices[i]].rgb();
        colorTable.insert(qRgb);
    }

    for (unsigned int i = 0; i < bmp2->bitmapPixelIndices.size(); i++) {
        QRgb qRgb = bmp2->bitmapColorTable[bmp2->bitmapPixelIndices[i]].rgb();
        colorTable.insert(qRgb);
    }

    if (colorTable.size() > 256) {
        std::cout << "more than 256 colors needed to overlay bitmap, its a no go :(" << std::endl;
        return false;
    }

    bitmapColorTable.insert(bitmapColorTable.end(), colorTable.begin(), colorTable.end());
    return true;
}

bool mapPixelIndicesToColorTable(BITMAP *bmp1, BITMAP *bmp2, BITMAP *newBmp, std::vector<QColor> &bitmapColorTable) {

    for (unsigned int i = 0; i < bmp1->bitmapPixelIndices.size(); i++) {
        QColor qColor = bmp1->bitmapColorTable[bmp1->bitmapPixelIndices[i]];

        auto itr = find(bitmapColorTable.begin(), bitmapColorTable.end(), qColor);

        if (itr == bitmapColorTable.end()) {
            std::cout << "color wasnt foud in color table, this shouldnt happen uh oh" << std::endl;
            return false;
        }

        newBmp->bitmapPixelIndices.push_back((unsigned char)std::distance(bitmapColorTable.begin(), itr));
    }

    for (unsigned int i = 0; i < bmp2->bitmapPixelIndices.size(); i++) {
        QColor qColor = bmp2->bitmapColorTable[bmp2->bitmapPixelIndices[i]];

        if (qColor.rgb() == 4294967295) continue;

        auto itr = find(bitmapColorTable.begin(), bitmapColorTable.end(), qColor);

        if (itr == bitmapColorTable.end()) {
            std::cout << "color wasnt foud in color table, this shouldnt happen uh oh" << std::endl;
            return false;
        }

        newBmp->bitmapPixelIndices[i] = (unsigned char)std::distance(bitmapColorTable.begin(), itr);
    }

    return false;
}


void constructBitmapFileHeader(BITMAP *newBmp) {
    newBmp->bitmapFileHeader->bfType = 19778; //'BM' bitmap magic
    newBmp->bitmapFileHeader->bfSize = 14 + newBmp->bitmapInfoHeader->biSize + 4 * newBmp->bitmapColorTable.size() + newBmp->bitmapPixelIndices.size();
    newBmp->bitmapFileHeader->bfReserved1 = 0;
    newBmp->bitmapFileHeader->bfReserved2 = 0;
    newBmp->bitmapFileHeader->bfOffBits = 14 + newBmp->bitmapInfoHeader->biSize + 4 * newBmp->bitmapColorTable.size();
}

void constructBitmapInfoHeader(BITMAP *newBmp, long width, long height) {
    newBmp->bitmapInfoHeader->biSize = 40;
    newBmp->bitmapInfoHeader->biWidth = width;
    newBmp->bitmapInfoHeader->biHeight = height;
    newBmp->bitmapInfoHeader->biPlanes = 1;
    newBmp->bitmapInfoHeader->biBitCount = 8;
    newBmp->bitmapInfoHeader->biCompression = 0;
    newBmp->bitmapInfoHeader->biSizeImage = width * height;
    newBmp->bitmapInfoHeader->biXPelsPerMeter = 0;
    newBmp->bitmapInfoHeader->biYPelsPerMeter = 0;
    newBmp->bitmapInfoHeader->biClrUsed = newBmp->bitmapColorTable.size();
    newBmp->bitmapInfoHeader->biClrImportant = 0;
}

void saveBitmap(BITMAP *bmp, QString filePath)
{
    std::ofstream outfile(filePath.toStdString(), std::ofstream::binary);

    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapFileHeader->bfType), sizeof(bmp->bitmapFileHeader->bfType));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapFileHeader->bfSize), sizeof(bmp->bitmapFileHeader->bfSize));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapFileHeader->bfReserved1), sizeof(bmp->bitmapFileHeader->bfReserved1));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapFileHeader->bfReserved2), sizeof(bmp->bitmapFileHeader->bfReserved2));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapFileHeader->bfOffBits), sizeof(bmp->bitmapFileHeader->bfOffBits));

    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biSize), sizeof(bmp->bitmapInfoHeader->biSize));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biWidth), sizeof(bmp->bitmapInfoHeader->biWidth));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biHeight), sizeof(bmp->bitmapInfoHeader->biHeight));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biPlanes), sizeof(bmp->bitmapInfoHeader->biPlanes));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biBitCount), sizeof(bmp->bitmapInfoHeader->biBitCount));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biCompression), sizeof(bmp->bitmapInfoHeader->biCompression));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biSizeImage), sizeof(bmp->bitmapInfoHeader->biSizeImage));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biXPelsPerMeter), sizeof(bmp->bitmapInfoHeader->biXPelsPerMeter));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biYPelsPerMeter), sizeof(bmp->bitmapInfoHeader->biYPelsPerMeter));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biClrUsed), sizeof(bmp->bitmapInfoHeader->biClrUsed));
    outfile.write(reinterpret_cast<const char *>(&bmp->bitmapInfoHeader->biClrImportant), sizeof(bmp->bitmapInfoHeader->biClrImportant));

    for(QColor qc : bmp->bitmapColorTable)
    {
        unsigned char blue = (unsigned char)qc.blue();
        unsigned char green = (unsigned char)qc.green();
        unsigned char red = (unsigned char)qc.red();
        unsigned char reserved = (unsigned char)0;

        outfile.write(reinterpret_cast<char *>(&blue), sizeof(blue));
        outfile.write(reinterpret_cast<char *>(&green), sizeof(blue));
        outfile.write(reinterpret_cast<char *>(&red), sizeof(blue));
        outfile.write(reinterpret_cast<char *>(&reserved), sizeof(reserved));
    }

    for(unsigned char c : bmp->bitmapPixelIndices)
    {
        outfile.write(reinterpret_cast<char *>(&c), sizeof(c));
    }

    outfile.close();
}
