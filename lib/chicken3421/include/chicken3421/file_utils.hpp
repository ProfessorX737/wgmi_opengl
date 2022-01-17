#ifndef CHICKEN3421_FILE_UTILS_HPP
#define CHICKEN3421_FILE_UTILS_HPP

#include <string>

namespace chicken3421 {

    /**
     *
     * Reads the file at "path" and returns its contents.
     *
     * Throws an exception if "path" is not accessible.
     *
     * @param path: The path to the file to read.
     * @return The content of the file.
     */
    std::string read_file(const std::string &path);

    /**
     *
     * A generic image.
     *
     * width is the width of the image in pixels
     * height is the heigh of the image in pixels
     * n_channels is the number of channels each pixel has (by default, rgba)
     * data is the raw image data.
     *
     * Note: data must not be manually freed.
     *
     */
    struct image_t {
        int width;
        int height;
        int n_channels;
        void *data;
    };

    /**
     *
     * Loads an image from the filesystem into RAM.
     * The type of image is inferred from the file's extension
     *
     * @param filename: the path to the file.
     * @return: the image
     */
    image_t load_image(const std::string &filename, bool flip_vertical=true);

    /**
     *
     * Deletes an image and frees its resources.
     * An image must only be deleted once.
     *
     * @param img: The image to delete.
     */
    void delete_image(image_t &img);

}

#endif //CHICKEN3421_FILE_UTILS_HPP
