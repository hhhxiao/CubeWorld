#ifndef IMAGE_H
#define IMAGE_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>

// 8 bit image buffer, compatable with stb image,
/*(0,0)--------------> x
| rgba rgba rgba
|
|
|
|
|
v
y
*/
// **no boundary check*
namespace fs = std::filesystem;
class Image {
   public:
    // for dbg
    // get the data (not reference, auto dctor)
    Image(int width, int height, int channel, uint8_t* data, bool copy);
    Image(int width, int height, int channel, uint8_t* data) : Image(width, height, channel, data, true) {}
    Image(int width, int height, int channel) : Image(width, height, channel, nullptr, true) {}

    ~Image() {
        if (own_data_) delete[] data_;
    }
    void save(const fs::path& path);

    inline uint8_t* data() { return data_; }
    inline const uint8_t* data() const { return data_; }

    inline uint8_t* get(size_t x, size_t y) { return data_ + (width_ * y + x) * channel_; }
    inline const uint8_t* get(size_t x, size_t y) const { return data_ + (width_ * y + x) * channel_; }

    inline int width() const { return width_; }
    inline int height() const { return height_; }
    inline int channel() const { return channel_; }

    // will auto fill
    void buildAtlas(int x, int y, const Image& img, int padding);

   private:
    int width_{0};
    int height_{0};
    int channel_{0};
    bool own_data_{true};
    uint8_t* data_{nullptr};
};

#endif
