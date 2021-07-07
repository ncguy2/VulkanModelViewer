//
// Created by Guy on 06/07/2021.
//

#ifndef GLMODELVIEWER_FONTRENDERER_H
#define GLMODELVIEWER_FONTRENDERER_H

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

class Texture;

class FontRenderer {
public:
    struct Character {
        Texture* texturePtr;  // Reference of the glyph texture
        glm::ivec2 size;      // Size of the glyph
        glm::ivec2 bearing;   // Offset from baseline to the top-left of glyph
        unsigned int advance; // Offset to advance to the next glyph
    };

    void Initialise(std::string fontPath);

protected:
    std::map<char, Character> characters;
    FT_Library ft;
    FT_Face  face;
};

#endif//GLMODELVIEWER_FONTRENDERER_H
