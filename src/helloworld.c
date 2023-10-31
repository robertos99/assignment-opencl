#include <stdio.h>
#include <stdlib.h>

#include "stb_image.h"
#include "stb_image_write.h"

#include "helloworld.h"

const char *say_hello()
{
    return "Hello, World!";
}

int load_image()
{
    int width, height, channels;
    unsigned char *img = stbi_load("resources/test.png", &width, &height, &channels, 0);
    if (img == NULL)
    {
        fprintf(stderr, "Error in loading the image\n");
        return EXIT_FAILURE;
    }

    // Write the image back to disk
    if (!stbi_write_png("resources/output_image.png", width, height, channels, img, width * channels))
    {
        fprintf(stderr, "Error in saving the image\n");
        stbi_image_free(img);
        return EXIT_FAILURE;
    }

    // Free the image memory
    stbi_image_free(img);

    printf("Image processing complete. Output saved as output_image.png.\n");
    return EXIT_SUCCESS;
}