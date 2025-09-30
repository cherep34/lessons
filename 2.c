#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
    short bfType;
    int   bfSize;
    short bfReserved1;
    short bfReserved2;
    int   bfOffBits;
} BMPHeader;

typedef struct {
    int   biSize;
    int   biWidth;
    int   biHeight;
    short biPlanes;
    short biBitCount;
    int   biCompression;
    int   biSizeImage;
    int   biXPelsPerMeter;
    int   biYPelsPerMeter;
    int   biClrUsed;
    int   biClrImportant;
} BMPInfoHeader;
#pragma pack(pop)

void draw_line(unsigned char* pixel_data, int width, int height, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1) {
        if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
            int bmp_y = height - 1 - y1;
            int idx = (bmp_y * width + x1) * 3;
            pixel_data[idx] = b;
            pixel_data[idx + 1] = g;
            pixel_data[idx + 2] = r;
        }
        if (x1 == x2 && y1 == y2) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x1 += sx; }
        if (e2 < dy)  { err += dx; y1 += sy; }
    }
}

int get_row_size(int width) {
    return ((width * 3 + 3) / 4) * 4;
}

int main(int argc, char* argv[]) {
    FILE* file = fopen(argv[1], "rb+");
    BMPHeader header;
    BMPInfoHeader infoheader;
    fread(&header, sizeof(header), 1, file);
    fread(&infoheader, sizeof(infoheader), 1, file);

    int w = infoheader.biWidth;
    int h = infoheader.biHeight;
    int row_size = get_row_size(w);
    char* data = malloc(row_size * h);
    fseek(file, header.bfOffBits, SEEK_SET);
    fread(data, 1, row_size * h, file);

    draw_line(data, w, h, 0, 0, w - 1, h - 1, 255, 0, 0);
    draw_line(data, w, h, 0, h - 1, w - 1, 0, 255, 0, 0);

    fseek(file, header.bfOffBits, SEEK_SET);
    fwrite(data, 1, row_size * h, file);

    free(data);
    fclose(file);
    return 0;
}