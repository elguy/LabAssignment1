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

std::vector<unsigned char> readBitmapPixelIndices(std::ifstream& is, BITMAPFILEHEADER *bitmapFileHeader, BITMAPINFOHEADER* bitmapInfoHeader, std::unordered_set<unsigned char> &colorSet) {
    is.seekg(bitmapFileHeader->bfOffBits, std::ifstream::beg);

    std::vector<unsigned char> BITMAPPIXELINDICES;

    unsigned char pixelIndex;
    int numPixels = bitmapInfoHeader->biWidth * bitmapInfoHeader->biHeight;

    for (int i = 0; i < numPixels; i++) {
        is.read(reinterpret_cast<char *>(&pixelIndex), sizeof(pixelIndex));

        colorSet.insert(pixelIndex);

        BITMAPPIXELINDICES.push_back(pixelIndex);
    }

    std::cout << colorSet.size() << std::endl;

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

    return true;
}

bool compareColorTables(BITMAP *bmp1, BITMAP *bmp2, QString &errorMessage) {
    std::unordered_set<unsigned char> missingColors;

    for (const auto &color: bmp2->colorSet) {
        if (bmp1->colorSet.find(color) == bmp1->colorSet.end()) {
            missingColors.insert(color);
        }
    }

    if (missingColors.size() > 0) {
        int availableColorSpace = 256 - bmp1->colorSet.size();

        if (availableColorSpace + missingColors.size() > 256) {
            std::cout << availableColorSpace << " " << missingColors.size();
            std::cout << "no room in color table to overlay" << std::endl;
            //error: too many colors to overlay
        } else {
            std::cout << "room in color table to insert overlays colors" << std::endl;
            //we can put all of the overlay's colors into the original's color table
        }
    } else {
        std::cout << "color tables contain all mutual colors" << std::endl;
        //all the colors used in the overlay bitmap are already available in the original bitmap
        //map them out and paint the pixels onto the original
    }

    return true;
}
/*
typedef struct tagBITMAPFILEHEADER {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    unsigned int biSize;
    long biWidth;
    long biHeight;
    unsigned short biPlanes;
    unsigned  short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    long biXPelsPerMeter;
    long biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;

std::vector<QColor> bitmapColorTable;
std::vector<unsigned char> bitmapPixelIndices;
*/
void saveBMP(BITMAP *bmp, std::string filename)
{
    //I just cast everything to char but I know that isn't what needs to be done
    //didn't work without casting but I suspect I did the colortable wrong

    std::ofstream outfile(filename, std::ofstream::binary);

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
