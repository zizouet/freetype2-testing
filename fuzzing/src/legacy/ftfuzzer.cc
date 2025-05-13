#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_SVG_H
#include FT_GLYPH_H
#include FT_IMAGE_H



extern "C" {

// Dummy hooks
static FT_Error dummy_init_svg(void*) { return 0; }
static void dummy_free_svg(void*) {}
static FT_Error dummy_preset_slot(FT_GlyphSlot slot, FT_Bool, void*) {
  slot->bitmap.rows = 1;
  slot->bitmap.width = 1;
  slot->bitmap.pitch = 1;
  slot->bitmap.pixel_mode = FT_PIXEL_MODE_GRAY;
  slot->bitmap.buffer = (unsigned char*)malloc(1);
  slot->bitmap.buffer[0] = 0xAB;
  return 0;
}
static FT_Error dummy_render_svg(FT_GlyphSlot slot, void*) {
  if (slot->bitmap.buffer) slot->bitmap.buffer[0] ^= 0xFF;
  return 0;
}

} // extern "C"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 4) return 0;

  FT_Library library;
  if (FT_Init_FreeType(&library)) return 0;

  // Register SVG hooks
  SVG_RendererHooks hooks = {
    .init_svg = dummy_init_svg,
    .free_svg = dummy_free_svg,
    .preset_slot = dummy_preset_slot,
    .render_svg = dummy_render_svg
  };

  FT_Error err = FT_Property_Set(library, "ot-svg", "svg-hooks", &hooks);
  // FT_Property_Set may fail if ot-svg module isn't loaded; ignore failure

  FT_Face face = nullptr;
  err = FT_New_Memory_Face(library, data, size, 0, &face);
  if (err || !face) {
    FT_Done_FreeType(library);
    return 0;
  }

  FT_Set_Pixel_Sizes(face, 0, 16);
  FT_Load_Glyph(face, 0, FT_LOAD_DEFAULT);

  if (face->glyph && face->glyph->format == FT_GLYPH_FORMAT_SVG) {
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    FT_Renderer renderer = FT_Get_Renderer(library, FT_GLYPH_FORMAT_SVG);
    if (renderer && renderer->clazz && renderer->clazz->transform_glyph) {
      FT_Matrix mtx = { 0x10000, 0, 0, 0x10000 };
      FT_Vector vec = { 0, 0 };
      renderer->clazz->transform_glyph(renderer, face->glyph, &mtx, &vec);
    }
  }

  // Try Property_Get
  SVG_RendererHooks out_hooks;
  FT_Property_Get(library, "ot-svg", "svg-hooks", &out_hooks);

  FT_Done_Face(face);
  FT_Done_FreeType(library);
  return 0;
}
