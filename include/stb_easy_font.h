// stb_easy_font.h - public domain single-header font rendering
// https://github.com/nothings/stb/blob/master/stb_easy_font.h
#ifndef STB_EASY_FONT_H
#define STB_EASY_FONT_H

/*
   stb_easy_font.h - public domain single-header file for easy bitmap font rendering.
   This provides a simple function to generate quads for text drawing.
   Usage: call stb_easy_font_print(x, y, "text", nullptr, buffer, sizeof(buffer));
   It returns the number of quads (each quad = 4 vertices) written into buffer.
*/

#ifdef __cplusplus
extern "C" {
#endif

int stb_easy_font_print(float x, float y, const char *text, const unsigned char *color, void *vertex_buffer, int vbuf_size);

#ifdef __cplusplus
}
#endif

#endif // STB_EASY_FONT_H
