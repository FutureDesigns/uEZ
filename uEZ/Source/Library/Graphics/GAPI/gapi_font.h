#include <uEZPacked.h>

/* set if raster is RLE encoded */
#define EFNT_RLE 0x0001
/* set if raster is 8bpp */
#define EFNT_8BPP 0x0010
/* set if raster is 1bpp */
#define EFNT_1BPP 0x0020
/* set if raster is 4bpp */
#define EFNT_4BPP 0x0040

PACK_STRUCT_BEGIN
typedef struct
{
        PACK_STRUCT_FIELD(unsigned long char_code);
        PACK_STRUCT_FIELD(signed long raster_offset);
        PACK_STRUCT_FIELD(signed short raster_style);
        PACK_STRUCT_FIELD(signed short width);
        PACK_STRUCT_FIELD(signed short height);
        PACK_STRUCT_FIELD(signed short pitch);
        PACK_STRUCT_FIELD(signed short left);
        PACK_STRUCT_FIELD(signed short bottom);
        PACK_STRUCT_FIELD(signed short advance);
} PACK_STRUCT_STRUCT type_Eglyph;
PACK_STRUCT_END

PACK_STRUCT_BEGIN
typedef struct
{
        PACK_STRUCT_FIELD(signed short xMin);
        PACK_STRUCT_FIELD(signed short yMin);
        PACK_STRUCT_FIELD(signed short xMax);
        PACK_STRUCT_FIELD(signed short yMax);
} PACK_STRUCT_STRUCT type_Ebbox;
PACK_STRUCT_END

PACK_STRUCT_BEGIN
typedef struct
{
        PACK_STRUCT_FIELD(unsigned long signature);
        PACK_STRUCT_FIELD(char family_name[32]);
        PACK_STRUCT_FIELD(char style_name[32]);
        PACK_STRUCT_FIELD(signed long num_glyphs);
        PACK_STRUCT_FIELD(signed short render_style);
        PACK_STRUCT_FIELD(signed short render_size);

        PACK_STRUCT_FIELD(type_Ebbox bbox);

        PACK_STRUCT_FIELD(signed short ascender);
        PACK_STRUCT_FIELD(signed short descender);
        PACK_STRUCT_FIELD(signed short height);
        PACK_STRUCT_FIELD(signed short max_advance_width);
        PACK_STRUCT_FIELD(signed short max_advance_height);
        PACK_STRUCT_FIELD(signed short underline_position);
        PACK_STRUCT_FIELD(signed short underline_thickness);
} PACK_STRUCT_STRUCT type_Efont;
PACK_STRUCT_END

#define RENDER_BOLD 0x0001
#define RENDER_ITALIC 0x0002
#define RENDER_LITTLE_ENDIAN 0x0004
#define RENDER_NO_RLE 0x0008
#define RENDER_FIXED_WIDTH 0x0010
#define RENDER_8BPP 0x0100
#define RENDER_1BPP 0x0200
#define RENDER_4BPP 0x0400

