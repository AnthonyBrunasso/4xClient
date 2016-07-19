#include "texloader.h"

#include <sstream>
#include <string>

#include "stb_image.h"
#include "log.h"

namespace imloader {
#define IMAGE_LOG "image.log"
#define IMAGE_DIR "../../data/images"
}

unsigned char* texloader::read(const char* filename, int& width, int& height) {
  int x, y, n;
  int force_channels = 4;
  std::string file = IMAGE_DIR;
  file += "/";
  file += filename;
  unsigned char* img_data = stbi_load(file.c_str(), &x, &y, &n, force_channels);
  std::ostringstream ss;
  ss << "Loading image: " << filename << std::endl;
  if (!img_data) {
    ss << " ERROR: could not load file" << std::endl;
    logging::write(IMAGE_LOG, ss.str());
    return nullptr;
  }

  // Check that image is a power of 2, warn if it is not.
  if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
    ss << " WARNING: image is not a power of 2" << std::endl;
  }

  // Flip the image. OpenGL expects 0 on the Y-axis at the bottom
  // of the texture. Images usually have Y-axis 0 at the top.
  int bytes_width = x * 4;
  unsigned char* top = nullptr;
  unsigned char* bot = nullptr;
  unsigned char t = 0;
  int half_width = y / 2;

  for (int row = 0; row < half_width; ++row) {
    top = img_data + row * bytes_width;
    bot = img_data + (y - row - 1) * bytes_width;
    for (int col = 0; col < bytes_width; ++col) {
      t = *top;
      *top = *bot;
      *bot = t;
      ++top;
      ++bot;
    }
  }

  width = x;
  height = y;

  ss << " Successfully loaded image." << std::endl;
  logging::write(IMAGE_LOG, ss.str());

  return img_data;
}

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

void texloader::load(const char* filename, GLenum slot, GLuint& tex) {
  int width, height;
  // TODO: Memory leaks here
  unsigned char* imdata = texloader::read(filename, width, height);
  if (!imdata) return;

  glGenTextures(1, &tex);
  glActiveTexture(slot);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 
    0, 
    GL_RGBA, 
    width, 
    height, 
    0, 
    GL_RGBA, 
    GL_UNSIGNED_BYTE, 
    imdata);

  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  GLfloat max_ansio = 0.0f;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_ansio);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_ansio);

  delete[] imdata;
}
