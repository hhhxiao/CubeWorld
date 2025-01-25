#include "image.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
Image::Image(int width, int height, int channel, uint8_t* data, bool copy)
    : width_(width), height_(height), channel_(channel) {
    if (!data) {
        this->data_ = new uint8_t[width * height * channel];
        memset(this->data_, 0, width * height * channel);
    } else {
        if (copy) {
            memcpy(this->data_, data, width * height * channel);
        } else {
            this->data_ = data;
            own_data_ = false;
        }
    }
}

void Image::save(const fs::path& path) {
    stbi_write_bmp(path.string().c_str(), width_, height_, channel_, (const void*)this->data_);
}

void Image::buildAtlas(int x, int y, const Image& img, int padding) {
    assert(this->channel_ == 4);
    uint8_t pixel[4]{0, 0, 0, 255};
    for (int i = 0; i < img.width_ + padding * 2; i++) {
        for (int j = 0; j < img.height_ + padding * 2; j++) {
            auto dx = i + x;
            auto dy = j + y;

            auto ix = std::clamp(i - padding, 0, img.width_ - 1);
            auto iy = std::clamp(j - padding, 0, img.height_ - 1);
            // init
            pixel[0] = pixel[1] = pixel[2] = 0;
            pixel[3] = 255;
            //
            for (int n = 0; n < img.channel_; n++) pixel[n] = img.get(ix, iy)[n];
            if (img.channel_ == 1) pixel[2] = pixel[1] = pixel[0];

            for (int n = 0; n < 4; n++) {
                this->get(dx, dy)[n] = pixel[n];
            }
        }
    }
}
