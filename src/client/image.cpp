#include "image.h"
#include <algorithm>
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

void Image::copyFromImage(int x, int y, const Image& img) {
    auto n = std::min(this->channel_, img.channel_);
    for (int i = 0; i < img.width_; i++) {
        for (int j = 0; j < img.height_; j++) {
            auto dx = i + x;
            auto dy = j + y;
            if (!(dx >= 0 && dx < width_ && dy >= 0 && dy < height_)) continue;
            for (int r = 0; r < n; r++) {
                this->get(dx, dy)[r] = img.get(i, j)[r];
            }
        }
    }
}
