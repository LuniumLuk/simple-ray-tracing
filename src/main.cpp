#include <stdio.h>
#include <stdlib.h>
#include <image.h>
#include <mesh.h>

int main()
{
    Utils::Image test_img("assets/texture/lenna.bmp");

    printf("Image size: (%d, %d, %d)\n", test_img.width(), test_img.height(), test_img.channels());

    return 0;
}