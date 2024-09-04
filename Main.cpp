//
// FCAI – Structured Programming – 2024 - Assignment 3 - Task 1
//
// File: CS112_A3_T1_20230022_20230240_20230277
// Purpose: Baby photoshop
// GitHub: https://github.com/faresbakhit/baby-photoshop
//
// Authors:
// * Ahmed Shaaban Maghraby Mohammed - S23 - 20230022
// * Ali Ahmed Mohamed Reda - S23 - 20230240
// * Fares Ahmed Bakhit Hussain - S23 - 20230277
//
// [Ahmed Shaaban Maghraby Mohammed]: modymansh05@gmail.com
// [Ali Ahmed Mohamed Reda]: aliahmedreda34@gmail.com
// [Fares Ahmed Bakhit Hussain]: faresa.bakhit@gmail.com
//
// Ahmed Shaaban Maghraby Mohammed:
// * Filter 1: Grayscale Conversion
// * Filter 4: Merge Images
// * Filter 7: Darken and Lighten Image
// * Filter 10: Detect Image Edges
// * Filter 16: Purple Image
//
// Ali Ahmed Mohamed Reda:
// * Filter 2: Black and White
// * Filter 5: Flip Image
// * Filter 8: Crop Images
// * Filter 11: Resizing Images
// * Filter 18: Image Skewing Filter
//
// Fares Ahmed Bakhit Hussain:
// * Filter 3: Invert Image
// * Filter 6: Rotate Image
// * Filter 9: Frame Image
// * Filter 12: Blur Images
// 
//

#include <iostream>
#include <limits>
#include <cmath>
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

class Image {
public:
    unsigned char* raw_image = nullptr;
    int width = 0;
    int height = 0;
    int channels = 3;

    Image() = default;

    Image(int width, int height) : width(width), height(height) {
        raw_image = (unsigned char*)malloc(width * height * this->channels);
    }

    ~Image() {
        if (raw_image != nullptr) {
            stbi_image_free(raw_image);
        }
    }

    void load(const string& filepath) {
        raw_image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb);
        if (raw_image == nullptr) {
            throw invalid_argument(stbi_failure_reason());
        }
    }

    void save(const string& filepath) {
        if (raw_image == nullptr) {
            throw out_of_range("image not initalized");
        }

        const char* extension = strrchr(filepath.c_str(), '.');
        int success;

        if (!extension) {
            throw invalid_argument("missing image file extension");
        }

        if (strcmp(extension, ".png") == 0) {
            success = stbi_write_png(filepath.c_str(), width, height, STBI_rgb, raw_image, 0);
        } else if (strcmp(extension, ".bmp") == 0) {
            success = stbi_write_bmp(filepath.c_str(), width, height, STBI_rgb, raw_image);
        } else if (strcmp(extension, ".tga") == 0) {
            success = stbi_write_tga(filepath.c_str(), width, height, STBI_rgb, raw_image);
        } else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
            success = stbi_write_jpg(filepath.c_str(), width, height, STBI_rgb, raw_image, 90);
        } else {
            throw invalid_argument("unsupported image format");
        }

        if (!success) {
            throw invalid_argument("couldn't save image in path");
        }
    }

    unsigned char& operator()(int x, int y, int c) {
        if (x > width || x < 0) {
            throw out_of_range("row out of bounds");
        }
        if (y > height || y < 0) {
            throw out_of_range("column out of bounds");
        }
        if (c > channels || c < 0) {
            throw out_of_range("channel out of bounds");
        }
        return raw_image[(y * width + x) * channels + c];
    }

    const unsigned char& operator()(int x, int y, int c) const {
        if (x > width || x < 0) {
            throw out_of_range("row out of bounds");
        }
        if (y > height || y < 0) {
            throw out_of_range("column out of bounds");
        }
        if (c > channels || c < 0) {
            throw out_of_range("channel out of bounds");
        }
        return raw_image[(y * width + x) * channels + c];
    }
};

void InvertImage(Image& image) {
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            for (int k = 0; k < 3; ++k) {
                image(i, j, k) = 0xFF - image(i, j, k);
            }
        }
    }
}

void RotateImage(Image& image, int rotations) {
    for (int n = 0; n < rotations; n++) {
        Image flipped_image(image.height, image.width);
        for (int i = 0; i < image.width; ++i) {
            for (int j = 0; j < image.height; ++j) {
                for (int k = 0; k < 3; ++k) {
                    flipped_image(image.height - j, i, k) = image(i, j, k);
                }
            }
        }
        swap(image.raw_image, flipped_image.raw_image);
        swap(image.width, image.height);
    }
}

enum FrameImageKind {
    FrameImageSimple,
    FrameImageFancy,
    FrameImageVeryFancy,
};

void FrameImageDrawRectangle(Image &image, int x, int y, int width, int height, int thickness, int color[3]) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < thickness; ++j) {
            for (int k = 0; k < 3; ++k) {
                image(x + i, y + j, k) = color[k];
                image(x + i, y + j + height - thickness, k) = color[k];
            }
        }
    }
    for (int i = 0; i < thickness; ++i) {
        for (int j = thickness; j < (height - thickness); ++j) {
            for (int k = 0; k < 3; ++k) {
                image(x + i, y + j, k) = color[k];
                image(x + i + width - thickness, y + j, k) = color[k];
            }
        }
    }
}

void FrameImageDrawFilledRectangle(Image &image, int x, int y, int width, int height, int color[3]) {
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            for (int k = 0; k < 3; ++k) {
                image(x + i, y + j, k) = color[k];
            }
        }
    }
}

void FrameImage(Image &image, FrameImageKind kind, int color[3]) {
    int white[3] = {255, 255, 255};
    int outer_frame_box_width = image.height / 32; // px
    int outer_frame_thickness = image.height / 64; // px
    int inner_frame_thickness = image.height / 96; // px
    int inner_inner_frame_thickness = image.height / 128; // px

    FrameImageDrawRectangle(
        image,
        0, 0, // x, y
        image.width, image.height, // width, height
        outer_frame_thickness, // thickness
        color
    );

    if (kind == FrameImageSimple) {
        return;
    }

    FrameImageDrawRectangle(
        image,
        outer_frame_thickness, outer_frame_thickness, // x, y
        image.width - 2 * outer_frame_thickness, // width
        image.height - 2 * outer_frame_thickness, // height
        inner_frame_thickness, // thickness
        white
    );

    FrameImageDrawRectangle(
        image,
        outer_frame_thickness + 2 * inner_frame_thickness,
        outer_frame_thickness + 2 * inner_frame_thickness, // x, y
        image.width - 2 * outer_frame_thickness - 4 * inner_frame_thickness, // width
        image.height - 2 * outer_frame_thickness - 4 * inner_frame_thickness, // height
        inner_inner_frame_thickness, // thickness
        white
    );

    if (kind == FrameImageFancy) {
        return;
    }

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            FrameImageDrawFilledRectangle(
                image,
                (1 - 2 * i) * (outer_frame_thickness + 2 * inner_frame_thickness) + i * (image.width - outer_frame_box_width), // x
                (1 - 2 * j) * (outer_frame_thickness + 2 * inner_frame_thickness) + j * (image.height - outer_frame_box_width), // y
                outer_frame_box_width, outer_frame_box_width, // width, height
                white
            );
            FrameImageDrawRectangle(
                image,
                (1 - 2 * i) * (outer_frame_thickness + 2 * inner_frame_thickness) + i * (image.width - 2.4 * outer_frame_box_width), // x
                (1 - 2 * j) * (outer_frame_thickness + 2 * inner_frame_thickness) + j * (image.height - 2.4 * outer_frame_box_width), // y
                2.4 * outer_frame_box_width, // width
                2.4 * outer_frame_box_width, // height
                inner_frame_thickness, // thickness
                white
            );
            FrameImageDrawRectangle(
                image,
                (1 - 2 * i) * (outer_frame_thickness + 2 * inner_frame_thickness) + i * (image.width - 1.7 * outer_frame_box_width), // x
                (1 - 2 * j) * (outer_frame_thickness + 2 * inner_frame_thickness) + j * (image.height - 1.7 * outer_frame_box_width), // y
                1.7 * outer_frame_box_width, // width
                1.7 * outer_frame_box_width, // height
                inner_inner_frame_thickness / 2, // thickness
                white
            );
            FrameImageDrawFilledRectangle(
                image,
                i * ((1 - 2 * j) * outer_frame_thickness + j * (image.width - inner_frame_thickness)), // x
                !i * ((1 - 2 * j) * (outer_frame_thickness) + j * (image.height - inner_frame_thickness)), // y
                i * inner_frame_thickness + !i * image.width, // width
                i * image.height + !i * inner_frame_thickness, // height
                white
            );
        }
    }
}

void BlackAndWhiteImage(Image& image) {
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            int r = image(i, j, 0);
            int g = image(i, j, 1);
            int b = image(i, j, 2);
            int grayScale = (r + g + b) / 3;
            if (grayScale > 126) {
                image(i, j, 0) = 255;
                image(i, j, 1) = 255;
                image(i, j, 2) = 255;
            } else {
                image(i, j, 0) = 0;
                image(i, j, 1) = 0;
                image(i, j, 2) = 0;
            }
        }
    }
}

void FlipHorizontally(Image& image) {
    int width = image.width;
    int middle = width / 2;
    bool isEven = width % 2 == 0;
    int end = isEven ? middle - 2 : middle - 1;
    for (int i = 0; i < end; i++) {
        for (int j = 0; j < image.height; j++) {
            for (int c = 0; c < 3; c++) {
                int temp = image(i, j, c);
                image(i, j, c) = image(width - i - 1, j, c);
                image(width - i - 1, j, c) = temp;
            }
        }
    }
    if (isEven) {
        for (int j = 0; j < image.height; j++) {
            for (int c = 0; c < 3; c++) {
                int temp = image(middle, j, c);
                image(middle, j, c) = image(middle - 1, j, c);
                image(middle - 1, j, c) = temp;
            }
        }
    }
}

void FlipVertically(Image& image) {
    int height = image.height;
    int middle = height / 2;
    bool isEven = height % 2 == 0;
    int end = isEven ? middle - 2 : middle - 1;
    for (int j = 0; j < end; j++) {
        for (int i = 0; i < image.width; ++i) {
            for (int c = 0; c < 3; c++) {
                int temp = image(i, j, c);
                image(i, j, c) = image(i, height - j - 1, c);
                image(i, height - j - 1, c) = temp;
            }
        }
    }
    if (isEven) {
        for (int i = 0; i < image.width; ++i) {
            for (int c = 0; c < 3; c++) {
                int temp = image(i, middle, c);
                image(i, middle, c) = image(i, middle - 1, c);
                image(i, middle - 1, c) = temp;
            }
        }
    }
}

void CropImage(Image& image, int x, int y, int w, int h) {
    Image result(w, h);
    for (int a = x; a < x + w; a++) {
        for (int b = y; b < y + h; b++) {
            for (int c = 0; c < 3; c++) {
                result(a - x, b - y, c) = image(a, b, c);
            }
        }
    }

    swap(image.raw_image, result.raw_image);
    image.width = w;
    image.height = h;
}

void ResizeImage(Image& image, int w, int h) {
    Image result(w, h);
    bool w_bigger = w > image.width;
    bool h_bigger = h > image.height;
    float ratio_w = w_bigger ? (float) image.width / (float) w : (float) w / (float) image.width;
    float ratio_h = h_bigger ? (float) image.height / (float) h : (float) h / (float) image.height;
    float end_w = w_bigger ? w : image.width;
    float end_h = h_bigger ? h : image.height;

    for (float i = 0; i < end_w; i++) {
        for (float y = 0; y < end_h; y++) {
            for (int c = 0; c < 3; c++) {
                result(w_bigger ? i : (int) floor(i * ratio_w), h_bigger ? y : (int) floor(y * ratio_h), c)
                    = image(w_bigger ? (int) floor(i * ratio_w) : i, h_bigger ? (int) floor(y * ratio_h) : y, c);
            }
        }
    }

    swap(image.raw_image, result.raw_image);
    image.width = w;
    image.height = h;
}


void GrayscaleImage(Image& image){
    for (int i = 0; i < image.width; i++){
        for (int j = 0; j < image.height; j++){
            int red = image(i, j, 0);
            int green = image(i, j, 1);
            int blue = image(i, j, 2);
            int grayscale_value = (int)(0.21 * red + 0.72 * green + 0.07 * blue);
            for (int k = 0; k < 3; k++){
                image(i, j, k) = grayscale_value;
            }
        }
    }
}

void MergeImage(Image& image1, Image& image2){
    if (image2.width != image1.width || image2.height != image1.height)
        ResizeImage(image2, image1.width, image1.height);

    for (int i = 0; i < image1.width; i++) {
        for (int j = 0; j < image1.height; j++) {
            int red1 = image1(i,j,0);
            int green1 = image1(i,j,1);
            int blue1 = image1(i,j,2);

            int red2 = image2(i,j,0);
            int green2 = image2(i,j,1);
            int blue2 = image2(i,j,2);
            int avg_red = (int)((red1 + red2) / 2);
            int avg_green = (int)((green1 + green2) / 2);
            int avg_blue = (int)((blue1 + blue2) / 2);

            image1(i, j, 0) = avg_red;
            image1(i, j, 1) = avg_green;
            image1(i, j, 2) = avg_blue;
        }
    }
}

void DarkenImage(Image& image) {
    for (int i = 0; i < image.width; i++)
        for (int j = 0; j < image.height; j++)
            for (int k = 0;  k < 3; k++)
                image(i,j,k) = image(i,j,k)/2;
}

void LightenImage(Image& image) {
    for (int i = 0; i < image.width; i++)
        for (int j = 0; j < image.height; j++)
            for (int k = 0;  k < 3; k++)
                image(i, j, k) = min(255, image(i, j, k) + 50);
}

void DetectImageEdges(Image& image) {
    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };
    Image edges(image.width, image.height);
    for (int y = 1; y < image.height - 1; y++) {
        for (int x = 1; x < image.width - 1; x++) {
            int x_gradient = 0, y_gradient = 0;
            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    x_gradient += sobel_x[j + 1][i + 1] * image(x + i, y + j, 0);
                    y_gradient += sobel_y[j + 1][i + 1] * image(x + i, y + j, 0);
                }
            }

            int magnitude = static_cast<int>(sqrt(x_gradient * x_gradient + y_gradient * y_gradient));
            magnitude = max(0, min(255, magnitude));
            for (int c = 0; c < 3; ++c) {
                edges(x, y, c) = magnitude;
            }
        }
    }
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            for (int c = 0; c < 3; c++) {
                image(x, y, c) = edges(x, y, c);
            }
        }
    }
}

void BlurImage(Image& image, int level) {
    Image blurred_image(image.width, image.height);
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            if (i - level < 0 || j - level < 0 || i + level > image.width || j + level > image.height) {
                for (int k = 0; k < 3; ++k) {
                    blurred_image(i, j, k) = image(i, j, k);
                }
                continue;
            }
            for (int k = 0; k < 3; ++k) {
                int sum = 0;
                for (int m = -level; m <= level; ++m) {
                    for (int n = -level; n <= level; ++n) {
                        sum += image(i + m, j + n, k);
                    }
                }
                blurred_image(i, j, k) = sum / pow(2*level + 1, 2);
            }
        }
    }
    swap(image.raw_image, blurred_image.raw_image);
}

void SkewImage(Image& image, int degree) {
    bool isNegative = degree < 0;
    if (isNegative) degree = -degree;

    float sinAlpha = sin((float) degree * 0.01745);
    Image result((int) ((float) image.width + (float) image.height * sinAlpha), image.height);
    for (int y = 0; y < image.height; ++y) {
        int ySinAlpha = isNegative ? (int) ((float) y * sinAlpha) : (int) ((float) (image.height - y) * sinAlpha);
        for (int x = 0; x < image.width; ++x) {
            for (int k = 0; k < 3; ++k) {
                result(x + ySinAlpha, y, k) = image(x, y, k);
            }
        }
    }

    swap(image.raw_image, result.raw_image);
    image.width = result.width;
    image.height = result.height;
}

void PurpleImage(Image& image) {
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            int red = image(i, j, 0);
            int green = image(i, j, 1);
            int blue = image(i, j, 2);
            red += 50;
            blue += 50;
            green -= 20;
            red = min(255, max(0, red));
            green = min(255, max(0, green));
            blue = min(255, max(0, blue));
            image(i, j, 0) = red;
            image(i, j, 1) = green;
            image(i, j, 2) = blue;
        }
    }
}

#define OILPAINT_RADIUS 5
#define OILPAINT_INTENSITY 20

void OilPaintImage(Image& image) {
    int intensityCount[OILPAINT_INTENSITY] {},
        averageR[OILPAINT_INTENSITY] {},
        averageG[OILPAINT_INTENSITY] {},
        averageB[OILPAINT_INTENSITY] {};
    Image oil_image(image.width, image.height);
    for (int i = OILPAINT_RADIUS; i < image.width - OILPAINT_RADIUS; ++i) {
        for (int j = OILPAINT_RADIUS; j < image.height - OILPAINT_RADIUS; ++j) {
            for (int m = -OILPAINT_RADIUS; m <= OILPAINT_RADIUS; ++m) {
                for (int n = -OILPAINT_RADIUS; n <= OILPAINT_RADIUS; ++n) {
                    int r = image(i + m, j + n, 0),
                        g = image(i + m, j + n, 1),
                        b = image(i + m, j + n, 2);
            		int curIntensity = (int)((double)((r + g + b)/3)*OILPAINT_INTENSITY)/255.0f;
                    intensityCount[curIntensity]++;
            		averageR[curIntensity] += r;
            		averageG[curIntensity] += g;
            		averageB[curIntensity] += b;
                }
            }
            int curMax = 0;
            int maxIndex = 0;
            for(int t = 0; t < OILPAINT_INTENSITY; t++) {
            	if (intensityCount[t] > curMax) {
                    curMax = intensityCount[t];
            		maxIndex = t;
                }
            }
            oil_image(i, j, 0) = averageR[maxIndex] / curMax;
            oil_image(i, j, 1) = averageG[maxIndex] / curMax;
            oil_image(i, j, 2) = averageB[maxIndex] / curMax;
            std::memset(intensityCount, 0, sizeof(intensityCount));
            std::memset(averageR, 0, sizeof(averageR));
            std::memset(averageG, 0, sizeof(averageG));
            std::memset(averageB, 0, sizeof(averageB));
        }
    }
    std::swap(image.raw_image, oil_image.raw_image);
}

int iinteger(const char *p) {
    int i;
    while (true) {
        cout << p;
        cin >> i;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "> error: input not an integer." << endl;
        } else {
            cin.ignore();
            return i;
        }
    }
}

int irange(const char *p, int min, int max) {
    int i;
    while (true) {
        i = iinteger(p);
        if (i < min || i > max) {
            cout << "> error: option " << i << " not in range [" << min << '-' << max << "]." << endl;
        } else {
            return i;
        }
    }
}

void iimage(const char *p, Image& image) {
    string filepath;
    while (true) {
        cout << p;
        getline(cin, filepath);
        try {
            image.load(filepath);
            return;
        } catch(const exception& e) {
            cout << "> error: " << e.what() << endl;
            continue;
        }
    }
}

int icolorhextoint(char r1, char r2) {
    r1 = tolower(r1);
    r2 = tolower(r2);

    if (isdigit(r1)) r1 -= '0';
    else if (r1 >= 'a' && r1 <=  'f') r1 -= 'a' - 10;
    else throw invalid_argument("invalid hexadecimal color");

    if (isdigit(r2)) r2 -= '0';
    else if (r2 >= 'a' && r2 <=  'f') r2 -= 'a' - 10;
    else throw invalid_argument("invalid hexadecimal color");

    return r2 + 16*r1;
}

void icolor(const char *p, int color[3]) {
    string s;
    while (true) {
        cout << p;
        getline(cin, s);
        if (s.size() != 7 || s[0] != '#') {
            cout << "> error: invalid hexadecimal color" << endl;
            continue;
        }
        try {
            color[0] = icolorhextoint(s[1], s[2]);
            color[1] = icolorhextoint(s[3], s[4]);
            color[2] = icolorhextoint(s[5], s[6]);
            break;
        } catch (const exception& e) {
            cout << "> error: " << e.what() << endl;
        }
    }
}

int main() {
    while (true) {
        cout << "> 1. Open new image" << endl
             << "> 2. Exit" << endl;

        int action = irange(">> ", 1, 2);
        if (action == 2) {
            break;
        }

        Image image;
        iimage(">> Enter image path: ", image);

        while (true) {
            cout
                << "> 1. Grayscale" << endl
                << "> 2. Black and White" << endl
                << "> 3. Invert" << endl
                << "> 4. Merge Images" << endl
                << "> 5. Flip" << endl
                << "> 6. Rotate" << endl
                << "> 7. Darken or Lighten" << endl
                << "> 8. Crop Image" << endl
                << "> 9. Frame" << endl
                << "> 10. Detect Edges" << endl
                << "> 11. Resize" << endl
                << "> 12. Blur" << endl
                // << "> 13. Sunlight Filter" << endl
                << "> 14. Oil Paint Filter" << endl
                // << "> 15. Old TV Noise Filter" << endl
                << "> 16. Purple Filter" << endl
                // << "> 17. infrared Filter" << endl
                << "> 18. Skew Image" << endl
                << "> 19. Save" << endl;

            int filter = irange(">> ", 1, 19);
            if (filter == 19) {
                break;
            }

            switch (filter) {
            case 1:
                GrayscaleImage(image);
                break;
            case 2:
                BlackAndWhiteImage(image);
                break;
            case 3:
                InvertImage(image);
                break;
            case 4: {
                Image image2;
                iimage(">> Enter second image path: ", image2);
                MergeImage(image, image2);
                break;
            }
            case 5:
                cout << "> 1. Flip Horizontally" << endl
                     << "> 2. Flip Vertically" << endl;
                if (irange(">> ", 1, 2) == 1) {
                    FlipHorizontally(image);
                } else {
                    FlipVertically(image);
                }
                break;
            case 6:
                cout << "> 1. Rotate by 90 degrees" << endl
                     << "> 2. Rotate by 180 degrees" << endl
                     << "> 3. Rotate by 270 degrees" << endl;
                RotateImage(image, irange(">> ", 1, 3));
                break;
            case 7:
                cout << "> 1. Darker by 50%" << endl
                     << "> 2. Lighter by 50%" << endl;
                if (irange(">> ", 1, 2) == 1) {
                    DarkenImage(image);
                } else {
                    LightenImage(image);
                }
                break;
            case 8: {
                int x = irange(">> the x value of starting point: ", 0, image.width);
                int y = irange(">> the y value of starting point: ", 0, image.height);
                int w = irange(">> Enter the width of area to crop: ", 0, image.width - x);
                int h = irange(">> Enter the height of area to crop: ", 0, image.height - y);
                CropImage(image, x, y, w, h);
                break;
            }
            case 9: {
                int color[3];
                icolor(">> Enter hexadecimal color: ", color);
                cout << "> 1. Simple frame" << endl
                     << "> 2. Fancy frame" << endl
                     << "> 3. Very fancy frame" << endl;
                FrameImage(image, FrameImageKind(irange(">> ", 1, 3) - 1), color);
                break;
            }
            case 10:
                DetectImageEdges(image);
                break;
            case 11: {
                int w = irange(">> Enter the width of new image: ", 0, INT_MAX);
                int h = irange(">> Enter the height of new image: ", 0, INT_MAX);
                ResizeImage(image, w, h);
                break;
            }
            case 12:
                BlurImage(image, irange(">> Enter bluring level: ", 1, INT_MAX));
                break;
            case 14:
                OilPaintImage(image);
                break;
            case 16:
                PurpleImage(image);
                break;
            case 18:
                SkewImage(image, irange(">> Enter degree to skew (from -89 to 89 degrees, positive towards the right, negative towards the left): ", -89, 89));
                break;
            }
        }

        while (true) {
            string filepath;
            cout << ">> Enter new image path: ";
            getline(cin, filepath);
            try {
                image.save(filepath);
                break;
            } catch(const exception& e) {
                cout << "> error: " << e.what() << endl;
            }
        }
    }
}
