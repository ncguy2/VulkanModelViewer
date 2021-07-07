//
// Created by Guy on 06/07/2021.
//

#include <scene2d/FontRenderer.h>
#include <stdexcept>

void FontRenderer::Initialise(std::string fontPath) {
    if(FT_Init_FreeType(&ft))
        throw std::runtime_error("ERROR::FREETYPE: Could not init FreeType Library");

    if(FT_New_Face(ft, fontPath.c_str(), 0, &face))
        throw std::runtime_error("ERROR::FREETYPE: Failed to load font from " + fontPath);

    FT_Set_Pixel_Sizes(face, 0, 48);

    if(FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        throw std::runtime_error("ERROR::FREETYPE: Failed to load Glyph");



}
