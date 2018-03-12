/* nclua.canvas -- The NCLua Canvas module.
   Copyright (C) 2013-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of NCLua.

NCLua is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

NCLua is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with NCLua.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>
#include <math.h>
#include "aux-glib.h"
#include "aux-lua.h"

#include <cairo.h>
#include "cairo_jpg.h"
#include <pango/pangocairo.h>
#include <string.h>

#if defined WITH_GTK && WITH_GTK
#include <gtk/gtk.h>
#endif

PRAGMA_DIAG_IGNORE (-Wimplicit-fallthrough)

#include "canvas-color-table.h"

#define radians(x) ((x) * G_PI / 180)
#define degrees(x) ((x) * 180 / G_PI)

/* Registry key for the canvas metatable.  */
#define CANVAS "nclua.canvas"

/* Canvas object data.  */
typedef struct _canvas_t
{
  cairo_surface_t *sfc;         /* main surface */
  cairo_surface_t *back_sfc;    /* back surface (for double-buffering) */
  cairo_t *cr;                  /* cairo context */
  int width;                    /* canvas width (in pixels) */
  int height;                   /* canvas height (in pixels) */
  cairo_rectangle_int_t crop;   /* crop attribute */
  cairo_filter_t filter;        /* filter attribute */
  struct
  {                             /* flip attribute */
    cairo_bool_t x;             /* true if horizontal flip is on */
    cairo_bool_t y;             /* true if vertical flip is on */
  } flip;
  PangoFontDescription *font;   /* font attribute */
  unsigned char opacity;        /* opacity attribute (0 is transparent) */
  double rotation;              /* rotation attribute (in radians) */
  struct
  {                             /* scale attribute */
    double x;                   /* horizontal scale factor */
    double y;                   /* vertical scale factor */
  } scale;
} canvas_t;

/* Checks if the object at index INDEX is a canvas.
   If CR is non-NULL, stores the canvas's cairo context into *CR.  */

static inline canvas_t *
canvas_check (lua_State *L, int index, cairo_t **cr)
{
  canvas_t *canvas;
  canvas = (canvas_t *) luaL_checkudata (L, index, CANVAS);
  tryset (cr, canvas->cr);
  return canvas;
}

/* Returns true if canvas CANVAS uses double-buffering.  */
#define canvas_is_double_buffered(canvas)\
  ((canvas)->back_sfc != NULL)

/* Computes the bounding rectangle of canvas CANVAS and stores its
   dimensions into *W and *H.  If CROP is non-null, use CROP instead of
   CANVAS crop region.

   WARNING: This function assumes that CROP is a sub-rectangle of the
   canvas's surface.  */

static void
canvas_get_extents (canvas_t *canvas, int *w, int *h,
                    cairo_rectangle_int_t *crop)
{
  int src_w, src_h;
  double r;

  if (crop == NULL)
    crop = &canvas->crop;

  src_w = crop->width;
  src_h = crop->height;
  r = canvas->rotation;

  *w = (int) lround (src_w * fabs (cos (r)) + src_h * fabs (sin (r)));
  *h = (int) lround (src_w * fabs (sin (r)) + src_h * fabs (cos (r)));
}

/* List of modes supported by most drawing functions.  */
static const char *const fill_or_frame_mode_list[] = {
  "fill", "frame", NULL
};

/* Uses MODE to call the appropriate function over cairo context CR.  */
#define cairox_do_fill_or_frame(cr, mode)       \
  G_STMT_START                                  \
  {                                             \
    switch ((mode))                             \
      {                                         \
      case 0:                                   \
        cairo_fill ((cr));                      \
        break;                                  \
      case 1:                                   \
        cairo_stroke ((cr));                    \
        break;                                  \
      default:                                  \
        g_assert_not_reached ();                \
      }                                         \
  }                                             \
  G_STMT_END

/* Stores the clip region of context CR into *X, *Y, *W, *H.  */
#define cairox_get_clip(cr, x, y, w, h)\
  cairo_clip_extents ((cr), (x), (y), (w), (h))

/* Sets the clip region of context CR to X, Y, W, H.  */
#define cairox_set_clip(cr, x, y, w, h)         \
  G_STMT_START                                  \
  {                                             \
    cairo_reset_clip ((cr));                    \
    cairo_rectangle ((cr), (x), (y), (w), (h)); \
    cairo_clip ((cr));                          \
  }                                             \
  G_STMT_END

/* Stores the source color of context CR into *R, *G, *B, *A.
   The macro assumes that R, G, B, and A are double-precision numbers.  */
#define cairox_get_source_rgba(cr, r, g, b, a)\
  cairo_pattern_get_rgba (cairo_get_source ((cr)), (r), (g), (b), (a))

/* Sets the source color of context CR to R, G, B, A.
   The macro assumes that R, G, B, and A are double-precision numbers.

   NOTE: I don't know why, but the call to cairo_set_source_rgb() is
   necessary.  If we remove it, we get strange errors, such as the
   following.

   canvas:attrColor (255, 0, 0, 0)
   canvas:attrColor () -> 255, 0, 0, 0
   canvas:attrColor (0, 0, 0, 0)
   canvas:attrColor () -> 255, 0, 0, 0    *** wrong ***
*/
#define cairox_set_source_rgba(cr, r, g, b, a)          \
  G_STMT_START                                          \
  {                                                     \
    cairo_set_source_rgb ((cr), 0, 0, 0);               \
    cairo_set_source_rgba ((cr), (r), (g), (b), (a));   \
  }                                                     \
  G_STMT_END

/* Returns true if context CR is valid.  */
#define cairox_is_valid(cr)\
  (cairo_status ((cr)) == CAIRO_STATUS_SUCCESS)

/* Returns a description of the current status of context CR.  */
#define cairox_status_desc(cr)\
  cairo_status_to_string (cairo_status ((cr)))

/* Returns true if pattern PATTERN is valid.  */
#define cairox_pattern_is_valid(pattern)\
  (cairo_pattern_status ((pattern)) == CAIRO_STATUS_SUCCESS)

/* Returns a description of the current status of pattern PATTERN.  */
#define cairox_pattern_status_desc(pattern)\
  cairo_status_to_string (cairo_pattern_status ((pattern)))

/* Returns true if region REGION is valid.  */
#define cairox_region_is_valid(region)\
  (cairo_region_status ((region)) == CAIRO_STATUS_SUCCESS)

/* Returns a description of the current status of region REGION.  */
#define cairox_region_status_desc(region)\
  cairo_status_to_string (cairo_region_status ((region)))

/* Returns a new surface compatible with SFC; W is the width of the new
   surface in pixels and H is its height in pixels.  */
#define cairox_surface_create_similar(sfc, w, h)\
  cairo_surface_create_similar ((sfc), CAIRO_CONTENT_COLOR_ALPHA, (w), (h))

/* Returns true if surface SFC is valid.  */
#define cairox_surface_is_valid(sfc)\
  (cairo_surface_status ((sfc)) == CAIRO_STATUS_SUCCESS)

/* Returns a description of the current status of surface SFC.  */
#define cairox_surface_status_desc(sfc)\
  cairo_status_to_string (cairo_surface_status ((sfc)))

/* Stores the dimensions of surface SFC into *W and *H.  */
#define cairox_surface_get_dimensions(sfc, w, h)        \
  G_STMT_START                                          \
  {                                                     \
    *(w) = cairo_image_surface_get_width ((sfc));       \
    *(h) = cairo_image_surface_get_height ((sfc));      \
  }                                                     \
  G_STMT_END

/* Creates a new surface by loading the image file at path PATH.
   Stores the resulting surface into *DUP and return CAIRO_STATUS_SUCCESS if
   successful, or an error status otherwise.  */

static cairo_status_t
cairox_surface_create_from_file (const char *path, cairo_surface_t **dup)
{
  cairo_surface_t *sfc;

#if defined WITH_GTK && WITH_GTK

  GdkPixbuf *pixbuf;
  GError *error = NULL;
  cairo_t *cr;
  int w, h;

  g_assert_nonnull (dup);
  pixbuf = gdk_pixbuf_new_from_file (path, &error);
  if (unlikely (pixbuf == NULL))
    {
      cairo_status_t status = (error->domain == G_FILE_ERROR)
        ? CAIRO_STATUS_FILE_NOT_FOUND : CAIRO_STATUS_READ_ERROR;
      g_error_free (error);
      return status;
    }

  w = gdk_pixbuf_get_width (pixbuf);
  h = gdk_pixbuf_get_height (pixbuf);
  sfc = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
  g_assert_nonnull (sfc);
  if (unlikely (!cairox_surface_is_valid (sfc)))
    return cairo_surface_status (sfc);

  cr = cairo_create (sfc);
  g_assert_nonnull (cr);
  if (!cairox_is_valid (cr))
    return cairo_status (cr);

  gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);

  cairo_destroy (cr);
  g_object_unref (pixbuf);

#else

  sfc = cairo_image_surface_create_from_png (path);
  g_assert_nonnull (sfc);
  if (unlikely (!cairox_surface_is_valid (sfc)))
    return cairo_surface_status (sfc);

#endif

  *dup = sfc;
  return CAIRO_STATUS_SUCCESS;
}

/* Copy surface SRC and stores the copy into *DUP.
   If CROP is non-NULL, copies only the area determined by rectangle CROP.
   Returns CAIRO_STATUS_SUCCESS if successful, or an error status otherwise.

   WARNING: This function assumes that CROP is a sub-rectangle of SRC.  */

static cairo_status_t
cairox_surface_duplicate (cairo_surface_t *src, cairo_surface_t **dup,
                          cairo_rectangle_int_t *crop)
{
  cairo_rectangle_int_t rect;
  cairo_surface_t *sfc;
  cairo_t *cr;
  int w, h;

  cairox_surface_get_dimensions (src, &w, &h);
  if (crop == NULL)
    {
      rect.x = 0;
      rect.y = 0;
      rect.width = w;
      rect.height = h;
      crop = &rect;
    }

  sfc = cairox_surface_create_similar (src, crop->width, crop->height);
  g_assert_nonnull (sfc);
  if (unlikely (!cairox_surface_is_valid (sfc)))
    return cairo_surface_status (sfc);

  cr = cairo_create (sfc);
  g_assert_nonnull (cr);
  if (unlikely (!cairox_is_valid (cr)))
    {
      cairo_surface_destroy (sfc);
      return cairo_status (cr);
    }

  cairo_set_source_surface (cr, src, -crop->x, -crop->y);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_rectangle (cr, 0, 0, crop->width, crop->height);
  cairo_fill (cr);
  cairo_destroy (cr);
  *dup = sfc;

  return CAIRO_STATUS_SUCCESS;
}

/* Throws a run-time error.  */
#define error_throw(L, msg)\
  (lua_pushstring ((L), (msg)), lua_error ((L)))

/* Throws an "invalid context" error.  */
#define error_throw_invalid_cr(L, cr)\
  error_throw ((L), cairox_status_desc ((cr)))

/* Throws an "invalid pattern" error.  */
#define error_throw_invalid_pattern(L, pattern)\
  error_throw ((L), cairox_pattern_status_desc ((pattern)))

/* Throws an "invalid region" error.  */
#define error_throw_invalid_region(L, region)\
  error_throw ((L), cairox_region_status_desc ((region)))

/* Throws an "invalid surface" error.  */
#define error_throw_invalid_surface(L, sfc)\
  error_throw ((L), cairox_surface_status_desc ((sfc)))

/*-
 * canvas.new (path:string, [doublebuf:boolean])
 * canvas:new (path:string, [doublebuf:booelan])
 * canvas.new (w, h:number, [doublebuf:boolean])
 * canvas:new (w, h:number, [doublebuf:boolean])
 *       -> canvas:userdata, w:number, h:number; or
 *       -> nil, errmsg:string
 *
 * Creates a new canvas object.
 * 1st and 2nd forms: Returns a canvas initialized with the given image.
 * 3rd and 4th forms: Returns a transparent canvas with the given size.
 *
 * If DOUBLEBUF is true, then uses double-buffering, i.e., delay drawing and
 * composition operations until canvas:flush() is called.
 *
 * All forms return a new canvas object plus its dimensions if successful,
 * otherwise they return nil plus an error message.
 */
static int
l_canvas_new (lua_State *L)
{
  cairo_surface_t *sfc;
  cairo_surface_t *back_sfc;
  cairo_t *cr;
  canvas_t *canvas;

  luax_optudata (L, 1, CANVAS);
  if (lua_type (L, 2) == LUA_TSTRING)
    {
      cairo_status_t status;
      const char *path;
      sfc = NULL;
      path = luaL_checkstring (L, 2);
      status = cairox_surface_create_from_file (path, &sfc);
      if (unlikely (status != CAIRO_STATUS_SUCCESS))
        {
          if (unlikely (status != CAIRO_STATUS_FILE_NOT_FOUND
                        && status != CAIRO_STATUS_READ_ERROR))
            return error_throw (L, cairo_status_to_string (status));

          lua_pushnil (L);
          lua_pushstring (L, cairo_status_to_string (status));
          return 2;
        }
      g_assert_nonnull (sfc);
    }
  else
    {
      int w = luaL_checkint (L, 2);
      int h = luaL_checkint (L, 3);
      sfc = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
      g_assert_nonnull (sfc);
      if (unlikely (!cairox_surface_is_valid (sfc)))
        {
          lua_pushnil (L);
          lua_pushstring (L, cairox_surface_status_desc (sfc));
          return 2;
        }
    }

  cr = cairo_create (sfc);
  g_assert_nonnull (cr);
  if (unlikely (!cairox_is_valid (cr)))
    return error_throw_invalid_cr (L, cr);

  if (lua_toboolean (L, 4))     /* double-buffering */
    {
      cairo_surface_t *dup = NULL;
      cairo_status_t err;

      err = cairox_surface_duplicate (sfc, &dup, NULL);
      if (unlikely (err != CAIRO_STATUS_SUCCESS))
        {
          cairo_surface_destroy (sfc);
          cairo_destroy (cr);
          return error_throw (L, cairo_status_to_string (err));
        }
      g_assert_nonnull (dup);
      back_sfc = sfc;
      sfc = dup;
    }
  else
    {
      back_sfc = NULL;
    }

  canvas = (canvas_t *) lua_newuserdata (L, sizeof (*canvas));
  g_assert_nonnull (canvas);
  canvas->sfc = sfc;
  canvas->back_sfc = back_sfc;
  canvas->cr = cr;
  cairox_surface_get_dimensions (sfc, &canvas->width, &canvas->height);
  canvas->crop.x = 0;
  canvas->crop.y = 0;
  canvas->crop.width = canvas->width;
  canvas->crop.height = canvas->height;
  canvas->filter = CAIRO_FILTER_GOOD;
  canvas->flip.x = FALSE;
  canvas->flip.y = FALSE;
  canvas->font = NULL;
  canvas->opacity = 255;
  canvas->rotation = 0;
  canvas->scale.x = 1.0;
  canvas->scale.y = 1.0;
  luaL_setmetatable (L, CANVAS);
  lua_pushinteger (L, canvas->width);
  lua_pushinteger (L, canvas->height);

  return 3;
}

/*-
 * canvas:__gc ()
 *
 * Destroys the given canvas object.
 */
static int
__l_canvas_gc (lua_State *L)
{
  canvas_t *canvas;

  canvas = canvas_check (L, 1, NULL);
  cairo_destroy (canvas->cr);
  cairo_surface_destroy (canvas->back_sfc);
  pango_font_description_free (canvas->font);
  cairo_surface_destroy (canvas->sfc);

  return 0;
}

/*-
 * canvas:_dump_to_file (path:string) -> status:boolean, errmsg:string
 *
 * Dumps the contents of the given canvas to image PNG file PATH.
 * Returns true if successful, or false plus error message otherwise.
 */
static int
_l_canvas_dump_to_file (lua_State *L)
{
  canvas_t *canvas;
  cairo_status_t err;

  canvas = canvas_check (L, 1, NULL);
  err = cairo_surface_write_to_png (canvas->sfc, luaL_checkstring (L, 2));
  if (unlikely (err != CAIRO_STATUS_SUCCESS))
    {
      lua_pushboolean (L, FALSE);
      lua_pushstring (L, cairo_status_to_string (err));
      return 2;
    }
  lua_pushboolean (L, TRUE);

  return 1;
}

/*-
 * canvas:_dump_to_memory (buf:lightuserdata, format:string, w, h, s:number)
 *
 * Dumps the contents of the given canvas into memory buffer BUF.
 *
 * The parameters W and H define the width and height of the image to be
 * stored in buffer; S gives the stride of the image to be stored in buffer,
 * i.e., number of bytes between the start of rows in the buffer as
 * allocated.
 *
 * The following FORMAT strings are supported:
 *
 *   ARGB32 - each pixel is a 32-bit quantity, with alpha in the upper 8
 *            bits, then red, then green, then blue; the 32-bit quantities
 *            are stored native-endian; pre-multiplied alpha is used;
 *
 *   RGB24  - each pixel is a 32-bit quantity, with the upper 8 bits unused;
 *            red, green, and blue are stored in the remaining 24 bits in
 *            that order.
 *
 * The default format string is 'ARGB32'.
 */
static int
_l_canvas_dump_to_memory (lua_State *L)
{
  static const char *format_list[] = {"ARGB32", "RGB24", NULL};
  canvas_t *canvas;
  unsigned char *buf;
  int fmt, w, h, s;

  cairo_t *aux_cr;
  cairo_surface_t *aux_sfc;

  canvas = canvas_check (L, 1, NULL);
  luaL_checktype (L, 2, LUA_TLIGHTUSERDATA);
  buf = (unsigned char *) lua_touserdata (L, 2);
  fmt = luaL_checkoption (L, 3, "ARGB32", format_list);
  w = luaL_checkint (L, 4);
  h = luaL_checkint (L, 5);
  s = luaL_checkint (L, 6);

  aux_sfc = cairo_image_surface_create_for_data (buf, (cairo_format_t) fmt,
                                                 w, h, s);
  g_assert_nonnull (aux_sfc);
  if (unlikely (!cairox_surface_is_valid (aux_sfc)))
    return error_throw_invalid_surface (L, aux_sfc);

  aux_cr = cairo_create (aux_sfc);
  g_assert_nonnull (aux_cr);
  if (unlikely (!cairox_is_valid (aux_cr)))
    {
      cairo_surface_destroy (aux_sfc);
      return error_throw_invalid_cr (L, aux_cr);
    }

  cairo_set_source_surface (aux_cr, canvas->sfc, 0, 0);
  cairo_set_operator (aux_cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (aux_cr);
  cairo_destroy (aux_cr);
  cairo_surface_destroy (aux_sfc);

  return 0;
}

/*-
 * canvas:_resize (w, h:number) -> w, h:number
 *
 * Resizes the surface of the given canvas to W width and H height pixels.
 * Returns the new width and height.
 */
static int l_canvas_flush (lua_State *);        /* forward declaration */
static int
_l_canvas_resize (lua_State *L)
{
  canvas_t *canvas;
  cairo_t *cr;
  cairo_surface_t *sfc;
  cairo_surface_t *back_sfc;
  int w, h;

  double clip_x, clip_y, clip_w, clip_h;
  double r, g, b, a;

  canvas = canvas_check (L, 1, &cr);
  w = luaL_checkint (L, 2);
  h = luaL_checkint (L, 3);

  if (unlikely (w == canvas->width && h == canvas->height))
    {
      lua_pushvalue (L, 2);     /* nothing to do */
      lua_pushvalue (L, 3);
      return 2;
    }

  sfc = cairox_surface_create_similar (canvas->sfc, w, h);
  g_assert_nonnull (sfc);
  if (unlikely (!cairox_surface_is_valid (sfc)))
    return error_throw_invalid_surface (L, sfc);

  cr = cairo_create (sfc);
  g_assert_nonnull (cr);
  if (unlikely (!cairox_is_valid (cr)))
    return error_throw_invalid_cr (L, cr);

  if (canvas_is_double_buffered (canvas))
    {
      cairo_surface_t *dup = NULL;
      cairo_status_t err;

      /* Flush canvas.  */
      lua_pushcfunction (L, l_canvas_flush);
      lua_pushvalue (L, 1);
      lua_call (L, 1, 0);

      err = cairox_surface_duplicate (sfc, &dup, NULL);
      if (unlikely (err != CAIRO_STATUS_SUCCESS))
        {
          cairo_surface_destroy (sfc);
          cairo_destroy (cr);
          return error_throw (L, cairo_status_to_string (err));
        }
      g_assert_nonnull (dup);
      back_sfc = sfc;
      sfc = dup;
    }
  else
    {
      back_sfc = NULL;
    }

  /* Copy canvas contents.  */
  cairo_save (cr);
  cairo_set_source_surface (cr, canvas->sfc, 0, 0);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);
  cairo_restore (cr);

  /* Copy attributes that are stored directly in cairo context.  */
  cairox_get_source_rgba (canvas->cr, &r, &g, &b, &a);
  cairox_set_source_rgba (cr, r, g, b, a);
  cairo_set_line_width (cr, cairo_get_line_width (canvas->cr));

  /* Update clip region if it is set to the whole canvas.  */
  cairox_surface_get_dimensions (sfc, &w, &h);
  cairox_get_clip (canvas->cr, &clip_x, &clip_y, &clip_w, &clip_h);
  if (lround (clip_x) == 0
      && lround (clip_y) == 0
      && lround (clip_w) == canvas->width
      && lround (clip_h) == canvas->height)
    {
      cairox_set_clip (cr, 0, 0, w, h);
    }
  else
    {
      cairox_set_clip (cr, clip_x, clip_y, clip_w, clip_h);
    }

  /* Update crop region if it is set to the whole canvas.  */
  if (canvas->crop.x == 0
      && canvas->crop.y == 0
      && canvas->crop.width == canvas->width
      && canvas->crop.height == canvas->height)
    {
      canvas->crop.width = w;
      canvas->crop.height = h;
    }

  /* Update the remaining stuff.  */
  cairo_surface_destroy (canvas->sfc);
  cairo_surface_destroy (canvas->back_sfc);
  cairo_destroy (canvas->cr);

  canvas->sfc = sfc;
  canvas->back_sfc = back_sfc;
  canvas->cr = cr;
  canvas->width = w;
  canvas->height = h;

  /* Flush canvas.  */
  if (canvas_is_double_buffered (canvas))
    {
      lua_pushcfunction (L, l_canvas_flush);
      lua_pushvalue (L, 1);
      lua_call (L, 1, 0);
    }

  lua_pushinteger (L, w);
  lua_pushinteger (L, h);

  return 2;
}

/*-
 * canvas:_surface () -> surface:lightuserdata
 *
 * Returns the canvas surface.
 */
static int
_l_canvas_surface (lua_State *L)
{
  lua_pushlightuserdata (L, (void *) (canvas_check (L, 1, NULL))->sfc);
  return 1;
}

/*-
 * canvas:attrAntiAlias () -> mode:string
 * canvas:attrAntiAlias (mode:string)
 *
 * Gets or sets the anti-alias attribute of the given canvas.  The
 * anti-alias attribute defines the anti-alias mode used in drawing
 * operations over the given canvas.
 *
 * The following MODE strings are supported:
 *   default  - use the default mode;
 *   none     - disable anti-aliasing;
 *   gray     - single color anti-aliasing;
 *   subpixel - use LCD subpixel;
 *   fast     - prefer speed over quality;
 *   good     - balance speed against quality;
 *   best     - prefer quality over speed.
 *
 *  The default anti-alias mode is 'default'.
 */
static int
l_canvas_attrAntiAlias (lua_State *L)
{
  static const char *mode_list[] = {
    "default",
    "none",
    "gray",
    "subpixel",
#if CAIRO_VERSION >= CAIRO_VERSION_ENCODE (1,12,0)
    "fast",
    "good",
    "best",
#endif
    NULL
  };
  cairo_t *cr;

  canvas_check (L, 1, &cr);
  if (lua_gettop (L) == 1)
    {
      cairo_antialias_t mode;
      mode = cairo_get_antialias (cr);
      g_assert (mode < nelementsof (mode_list) - 1);
      lua_pushstring (L, mode_list[mode]);
      return 1;
    }
  else
    {
      int mode;
      mode = luaL_checkoption (L, 2, NULL, mode_list);
      cairo_set_antialias (cr, (cairo_antialias_t) mode);
      return 0;
    }
}

/*-
 * canvas:attrClip () -> w, y, w, h:number
 * canvas:attrClip (x, y, w, h:number)
 *
 * Gets or sets the clipping region of the given canvas.  The clipping
 * region is the region of the canvas affected by drawing and composition
 * operations.
 *
 * The default clipping region is the whole canvas.
 */
static int
l_canvas_attrClip (lua_State *L)
{
  cairo_t *cr;
  double x, y, w, h;

  canvas_check (L, 1, &cr);
  if (lua_gettop (L) == 1)
    {
      cairox_get_clip (cr, &x, &y, &w, &h);
      lua_pushnumber (L, x);
      lua_pushnumber (L, y);
      lua_pushnumber (L, w - x);
      lua_pushnumber (L, h - y);
      return 4;
    }
  else
    {
      x = luaL_checknumber (L, 2);
      y = luaL_checknumber (L, 3);
      w = luaL_checknumber (L, 4);
      h = luaL_checknumber (L, 5);
      cairox_set_clip (cr, x, y, w, h);
      return 0;
    }
}

/*-
 * canvas:attrColor () -> r, g, b, a:number
 * canvas:attrColor (r, g, b:number, [a:number])
 * canvas:attrColor (name:string) -> status:boolean, errmsg:string
 *
 * Gets or sets the color attribute of the given canvas.  The color
 * attribute defines the color used in drawing operations.
 *
 * 1st form: Returns the current canvas color.
 * 2nd form: Sets the canvas color to the given color.
 * 3rd form: Sets the canvas color to the color denoted by NAME; returns
 *           true if successful, or false plus error message otherwise.
 *
 * The default canvas color is opaque black (i.e., R=0, G=0, B=0, A=255).
 */
static int
l_canvas_attrColor (lua_State *L)
{
  cairo_t *cr;
  double r, g, b, a;

  canvas_check (L, 1, &cr);
  if (lua_gettop (L) == 1)
    {
      cairo_status_t err;

      err = cairox_get_source_rgba (cr, &r, &g, &b, &a);
      if (unlikely (err != CAIRO_STATUS_SUCCESS))
        return error_throw (L, cairo_status_to_string (err));

      lua_pushnumber (L, r * 255);
      lua_pushnumber (L, g * 255);
      lua_pushnumber (L, b * 255);
      lua_pushnumber (L, a * 255);
      return 4;
    }

  if (lua_type (L, 2) == LUA_TSTRING)
    {
      const char *name = luaL_checkstring (L, 2);
      if (!canvas_color_table_index (name, &r, &g, &b))
        {
          lua_pushboolean (L, FALSE);
          lua_pushfstring (L, "unknown color '%s'", name);
          return 2;
        }
      a = 255;
    }
  else
    {
      r = luaL_checknumber (L, 2);
      g = luaL_checknumber (L, 3);
      b = luaL_checknumber (L, 4);
      a = luaL_optnumber (L, 5, 255);
    }

  cairox_set_source_rgba (cr, r / 255, g / 255, b / 255, a / 255);
  lua_pushboolean (L, TRUE);

  return 1;
}

/*-
 * canvas:attrCrop () -> w, y, w, h:number
 * canvas:attrCrop (x, y, w, h:number)
 *
 * Gets or sets the crop region of the given canvas.  The crop region is the
 * region that gets composed when the canvas is used as source of a
 * composition operation.
 *
 * The default crop region is the whole canvas.
 */
static int
l_canvas_attrCrop (lua_State *L)
{
  canvas_t *canvas;

  canvas = canvas_check (L, 1, NULL);
  if (lua_gettop (L) == 1)
    {
      lua_pushinteger (L, canvas->crop.x);
      lua_pushinteger (L, canvas->crop.y);
      lua_pushinteger (L, canvas->crop.width);
      lua_pushinteger (L, canvas->crop.height);
      return 4;
    }
  else
    {
      cairo_rectangle_int_t crop;
      cairo_rectangle_int_t rect;
      cairo_region_t *region;
      cairo_status_t err;

      crop.x = luaL_checkint (L, 2);
      crop.y = luaL_checkint (L, 3);
      crop.width = luaL_checkint (L, 4);
      crop.height = luaL_checkint (L, 5);

      rect.x = 0;
      rect.y = 0;
      rect.width = canvas->width;
      rect.height = canvas->height;
      region = cairo_region_create_rectangle (&rect);
      g_assert_nonnull (region);
      if (unlikely (!cairox_region_is_valid (region)))
        return error_throw_invalid_region (L, region);

      /* Computes the intersection of the given region and canvas.  */
      err = cairo_region_intersect_rectangle (region, &crop);
      if (unlikely (err != CAIRO_STATUS_SUCCESS))
        {
          cairo_region_destroy (region);
          return error_throw (L, cairo_status_to_string (err));
        }
      cairo_region_get_extents (region, &crop);
      cairo_region_destroy (region);

      canvas->crop.x = crop.x;
      canvas->crop.y = crop.y;
      canvas->crop.width = crop.width;
      canvas->crop.height = crop.height;
      return 0;
    }
}

/*-
 * canvas:attrFilter () -> filter:string
 * canvas:attrFilter (filter:string)
 *
 * Gets or sets the filter attribute of the given canvas.  The filter
 * attribute defines what filtering should used when reading the pixels of
 * the given canvas in composition operations.
 *
 * The following FILTER strings are supported:
 *   fast     - high performance, poor quality;
 *   good     - reasonable performance and quality;
 *   best     - poor performance, high quality;
 *   nearest  - nearest neighbor filter;
 *   bilinear - linear interpolation in two dimensions.
 *
 * The default filter is 'good'.
 */
static int
l_canvas_attrFilter (lua_State *L)
{
  static const char *mode_list[] = {
    "fast",
    "good",
    "best",
    "nearest",
    "bilinear",
    NULL
  };
  canvas_t *canvas;

  canvas = canvas_check (L, 1, NULL);
  if (lua_gettop (L) == 1)
    {
      cairo_filter_t mode;
      mode = canvas->filter;
      g_assert (mode < nelementsof (mode_list) - 1);
      lua_pushstring (L, mode_list[mode]);
      return 1;
    }
  else
    {
      int mode;
      mode = luaL_checkoption (L, 2, NULL, mode_list);
      canvas->filter = (cairo_filter_t) mode;
      return 0;
    }
}

/*-
 * canvas:attrFlip () -> fx, fy:boolean
 * canvas:attrFlip (fx, fy:boolean)
 *
 * Gets or sets the flip attributes of the given canvas.  The flip
 * attributes FX and FY define if the canvas should be flipped horizontally
 * (FX=true) or vertically (FY=true) in composition operations.
 *
 * The default flip is FX=false and FY=false.
 */
static int
l_canvas_attrFlip (lua_State *L)
{
  canvas_t *canvas;

  canvas = canvas_check (L, 1, NULL);
  if (lua_gettop (L) == 1)
    {
      lua_pushboolean (L, canvas->flip.x);
      lua_pushboolean (L, canvas->flip.y);
      return 2;
    }
  else
    {
      canvas->flip.x = lua_toboolean (L, 2);
      canvas->flip.y = lua_toboolean (L, 3);
      return 0;
    }
}

/*-
 * canvas:attrFont () -> family:string, size:number, style:string
 * canvas:attrFont (family:string, size:number, [style:string])
 *
 * Gets or sets the font attribute of the given canvas.  The font attribute
 * defines the font family, size (in points), and style used
 * by text drawing operations.
 *
 * The STYLE parameter is a string of the form "WEIGHT-SLANT", where WEIGHT
 * is one of the strings 'thin', 'ultralight', 'light', 'book', 'normal',
 * 'medium', 'semibold', 'bold', 'ultrabold', 'heavy', or 'ultraheavy', and
 * SLANT is either 'normal', 'oblique', or 'italic'.
 *
 * The default font style is 'normal-normal'.
 */
static int
l_canvas_attrFont (lua_State *L)
{
  static int weight_map[] = {
    PANGO_WEIGHT_THIN,
    PANGO_WEIGHT_ULTRALIGHT,
    PANGO_WEIGHT_LIGHT,
    PANGO_WEIGHT_BOOK,
    PANGO_WEIGHT_NORMAL,
    PANGO_WEIGHT_MEDIUM,
    PANGO_WEIGHT_SEMIBOLD,
    PANGO_WEIGHT_BOLD,
    PANGO_WEIGHT_ULTRABOLD,
    PANGO_WEIGHT_HEAVY,
    PANGO_WEIGHT_ULTRAHEAVY,
  };
  static const char *weight_list[] = {
    "thin",
    "ultralight",
    "light",
    "book",
    "normal",
    "medium",
    "semibold",
    "bold",
    "ultrabold",
    "heavy",
    "ultraheavy",
    NULL
  };
  static const char *slant_list[] = {
    "normal",
    "oblique",
    "italic",
    NULL
  };
  canvas_t *canvas;
  const char *family;

  canvas = canvas_check (L, 1, NULL);
  if (lua_gettop (L) == 1)
    {
      int size;
      PangoWeight weight;
      PangoStyle slant;
      size_t i;
      int found;

      if (canvas->font == NULL)
        return 0;

      family = pango_font_description_get_family (canvas->font);
      size = pango_font_description_get_size (canvas->font);

      weight = pango_font_description_get_weight (canvas->font);
      found = 0;
      for (i = 0; i < nelementsof (weight_map); i++)
        {
          if ((int) weight == weight_map[i])
            {
              found = 1;
              break;
            }
        }
      g_assert (found);

      slant = pango_font_description_get_style (canvas->font);
      g_assert (slant < nelementsof (slant_list) - 1);

      lua_pushstring (L, family);
      lua_pushnumber (L, ((double) size / PANGO_SCALE));
      lua_pushfstring (L, "%s-%s", weight_list[i], slant_list[slant]);
      return 3;
    }
  else
    {
      PangoFontDescription *font;
      double size;
      int weight;
      int slant;
      int i;

      family = luaL_checkstring (L, 2);
      size = fabs (luaL_checknumber (L, 3));

      if (lua_isnoneornil (L, 4))       /* default style */
        {
          weight = weight_map[4];
          slant = PANGO_STYLE_NORMAL;
        }
      else                      /* parse style */
        {
          luaL_checktype (L, 4, LUA_TSTRING);
          luaL_getmetafield (L, 4, "__index");
          lua_getfield (L, -1, "match");
          lua_pushvalue (L, 4);
          lua_pushstring (L, "^(%w*)%-*(%w*)$");
          lua_call (L, 2, 2);
          for (i = 0; i < 2; i++)
            {
              if (luaL_len (L, -1) == 0)
                {
                  lua_pop (L, 1);
                  lua_pushnil (L);
                }
              lua_insert (L, 4);
            }
          lua_settop (L, 5);
          weight = luaL_checkoption (L, 4, "normal", weight_list);
          g_assert (weight >= 0);
          g_assert ((PangoWeight) weight < nelementsof (weight_list) - 1);
          weight = weight_map[weight];

          lua_insert (L, 4);
          slant = luaL_checkoption (L, 4, "normal", slant_list);
          lua_insert (L, 4);
        }

      font = pango_font_description_new ();
      g_assert_nonnull (font);
      pango_font_description_set_family (font, family);
      pango_font_description_set_size (font, (int) (size * PANGO_SCALE));
      pango_font_description_set_weight (font, (PangoWeight) weight);
      pango_font_description_set_style (font, (PangoStyle) slant);
      pango_font_description_free (canvas->font);
      canvas->font = font;
      return 0;
    }
}

/*-
 * canvas:attrLineWidth () -> w:number
 * canvas:attrLineWidth (w:number)
 *
 * Gets or sets the line width attribute of the given canvas.  The line
 * width attribute defines the broadness of the brush used in drawing
 * operations.
 *
 * The default line width is 2.
 */
static int
l_canvas_attrLineWidth (lua_State *L)
{
  cairo_t *cr;

  canvas_check (L, 1, &cr);
  if (lua_gettop (L) == 1)
    {
      lua_pushnumber (L, cairo_get_line_width (cr));
      return 1;
    }
  else
    {
      cairo_set_line_width (cr, luaL_checknumber (L, 2));
      return 0;
    }
}

/*-
 * canvas:attrOpacity () -> a:number
 * canvas:attrOpacity (a:number)
 *
 * Gets or sets the opacity attribute of the given canvas.  The opacity
 * attribute defines a opacity value to used in composition operations.
 *
 * The default opacity is 255 (opaque).
 */
static int
l_canvas_attrOpacity (lua_State *L)
{
  canvas_t *canvas;

  canvas = canvas_check (L, 1, NULL);
  if (lua_gettop (L) == 1)
    {
      lua_pushinteger (L, canvas->opacity);
      return 1;
    }
  else
    {
      int opacity;
      opacity = luaL_checkint (L, 2);
      canvas->opacity = (unsigned char) (CLAMP (opacity, 0, 255));
      return 0;
    }
}

/*-
 * canvas:attrRotation () -> ang:number
 * canvas:attrRotation (ang:number)
 *
 * Gets or sets the rotation attribute of the given canvas.  The rotation
 * angle ANG defines the angle of rotation applied to the canvas before
 * composition operations.
 *
 * The angle of rotation is given in degrees.  The direction of rotation is
 * defined such that a positive angles rotate in a clockwise direction, and
 * negative angles rotate in a counter-clockwise direction.
 *
 * The default rotation is 0 degrees.
 */
static int
l_canvas_attrRotation (lua_State *L)
{
  canvas_t *canvas;

  canvas = canvas_check (L, 1, NULL);
  if (lua_gettop (L) == 1)
    {
      lua_pushnumber (L, degrees (canvas->rotation));
      return 1;
    }
  else
    {
      canvas->rotation = radians (luaL_checknumber (L, 2));
      return 0;
    }
}

/*-
 * canvas:attrScale () -> sx, sy:number
 * canvas:attrScale (sx, sy:number)
 *
 * Gets or sets the scale attributes of the given canvas.  The scale factors
 * SX and SY define scale factors applied to coordinates X and Y of the
 * canvas before composition operations.
 *
 * The default scale factors are SX=1 and SY=1.
 */
static int
l_canvas_attrScale (lua_State *L)
{
  canvas_t *canvas;

  canvas = canvas_check (L, 1, NULL);
  if (lua_gettop (L) == 1)
    {
      lua_pushnumber (L, canvas->scale.x);
      lua_pushnumber (L, canvas->scale.y);
      return 2;
    }
  else
    {
      canvas->scale.x = MAX (luaL_checknumber (L, 2), 0);
      canvas->scale.y = MAX (luaL_checknumber (L, 3), 0);
      return 0;
    }
}

/*-
 * canvas:attrSize () -> w, h, sw, sh:number
 *
 * Gets the dimensions of the given canvas (in pixels).
 *
 * Returns the original dimensions of canvas (W and H) and its dimensions
 * when we take into account its crop, scale, and rotation attributes (SW
 * and SH).
 */
static int
l_canvas_attrSize (lua_State *L)
{
  canvas_t *canvas;
  int w, h;

  canvas = canvas_check (L, 1, NULL);
  canvas_get_extents (canvas, &w, &h, NULL);
  lua_pushinteger (L, canvas->width);
  lua_pushinteger (L, canvas->height);
  lua_pushinteger (L, lround (w * canvas->scale.x));
  lua_pushinteger (L, lround (h * canvas->scale.y));

  return 4;
}

/*-
 * canvas:clear ([x, y, w, h:number])
 *
 * Clears the given canvas with the current color attribute.  If X, Y, W,
 * and H are given, then clears only the area delimited by this rectangle.
 *
 * WARNING: This function ignores the clip attribute.
 */
static int
l_canvas_clear (lua_State *L)
{
  canvas_t *canvas;
  cairo_t *cr;
  double x, y, w, h;

  canvas = canvas_check (L, 1, &cr);
  x = luaL_optnumber (L, 2, 0);
  y = luaL_optnumber (L, 3, 0);
  w = luaL_optnumber (L, 4, canvas->width);
  h = luaL_optnumber (L, 5, canvas->height);

  cairo_save (cr);
  cairo_reset_clip (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_rectangle (cr, x, y, w, h);
  cairo_fill (cr);
  cairo_restore (cr);

  return 0;
}

/*-
 * canvas:compose (x, y:number, src:canvas,
 *                 [src_x, src_y, src_w, src_h:number])
 *
 * Composes the crop region of canvas SRC onto the given canvas at
 * coordinate (X,Y).
 *
 * If SRC_X, SRC_Y, SRC_W, and SRC_H are given, then ignores the crop region
 * of SRC and composes only the area delimited by the this rectangle.
 */
static int
l_canvas_compose (lua_State *L)
{
  canvas_t *src;
  cairo_t *cr;
  double x, y;
  cairo_rectangle_int_t crop;

  cairo_region_t *region;
  cairo_rectangle_int_t rect;
  cairo_region_overlap_t overlap;

  cairo_surface_t *sfc;
  cairo_pattern_t *pattern;
  cairo_matrix_t matrix;
  int w, h;
  int bw, bh;
  int fx, fy;
  double sx, sy;

  canvas_check (L, 1, &cr);
  x = luaL_optnumber (L, 2, 0);
  y = luaL_optnumber (L, 3, 0);

  src = canvas_check (L, 4, NULL);
  crop.x = CLAMP (luaL_optint (L, 5, src->crop.x), 0, src->width);
  crop.y = CLAMP (luaL_optint (L, 6, src->crop.y), 0, src->height);
  crop.width = CLAMP (luaL_optint (L, 7, src->crop.width), 0, src->width);
  crop.height = CLAMP (luaL_optint (L, 8, src->crop.height), 0,
                       src->height);

  rect.x = 0;
  rect.y = 0;
  rect.width = src->width;
  rect.height = src->height;
  region = cairo_region_create_rectangle (&rect);
  g_assert_nonnull (region);
  if (unlikely (!cairox_region_is_valid (region)))
    return error_throw_invalid_region (L, region);

  overlap = cairo_region_contains_rectangle (region, &crop);
  cairo_region_destroy (region);
  if (unlikely (overlap == CAIRO_REGION_OVERLAP_OUT))
    return 0;                   /* nothing to do */

  if (unlikely (src->scale.x <= 0.0 || src->scale.y <= 0.0))
    return 0;                   /* nothing to do */

  /* Crop source, if necessary.  */
  if (crop.x > 0 || crop.y > 0
      || crop.width < src->width || crop.height < src->height)
    {
      /* FIXME: I've tried the following to avoid allocating a new surface
         explicitly, but the performance was terrible, specially when
         running examples/luarocks.lua.  So I'm sticking to the current
         code.

         cairo_push_group (cr);
         cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
         cairo_set_source_surface (cr, src->sfc, 0, 0);
         cairo_rectangle (cr, crop.x, crop.y, crop.width, crop.height);
         cairo_fill (cr);
         cairo_pop_group_to_source (cr);
         pattern = cairo_get_source (cr);
         cairo_pattern_reference (pattern);  */

      cairo_status_t err;

      sfc = NULL;
      err = cairox_surface_duplicate (src->sfc, &sfc, &crop);
      if (unlikely (err != CAIRO_STATUS_SUCCESS))
        return error_throw (L, cairo_status_to_string (err));
      g_assert_nonnull (sfc);
    }
  else
    {
      sfc = src->sfc;
    }

  /* Create source pattern.  */
  pattern = cairo_pattern_create_for_surface (sfc);
  g_assert_nonnull (pattern);
  if (unlikely (!cairox_pattern_is_valid (pattern)))
    return error_throw_invalid_pattern (L, pattern);

  if (sfc != src->sfc)
    cairo_surface_destroy (sfc);

  w = crop.width;
  h = crop.height;
  canvas_get_extents (src, &bw, &bh, &crop);
  fx = src->flip.x;
  fy = src->flip.y;
  sx = src->scale.x;
  sy = src->scale.y;

  cairo_matrix_init (&matrix, 1, 0, 0, 1, 0, 0);

  /* Rotate.  */
  if (src->rotation < 0 || src->rotation > 0)
    {
      cairo_matrix_translate (&matrix, w / 2, h / 2);
      cairo_matrix_rotate (&matrix, 2 * G_PI - src->rotation);
      cairo_matrix_translate (&matrix, -bw / 2, -bh / 2);
    }

  /* Flip.  */
  if (fx || fy)
    {
      cairo_matrix_translate (&matrix, (fx) ? bw : 0, (fy) ? bh : 0);
      cairo_matrix_scale (&matrix, (fx) ? -1 : 1, (fy) ? -1 : 1);
    }

  /* Scale.  */
  if (sx < 1 || sx > 1 || sy < 1 || sy > 1)
    {
      cairo_matrix_scale (&matrix, 1 / sx, 1 / sy);
    }

  /* Move to (x,y).  */
  if (x < 0 || x > 0 || y < 0 || y > 0)
    {
      cairo_matrix_translate (&matrix, -x, -y);
    }

  /* Paint pattern onto destination with the source filter and alpha.  */
  cairo_pattern_set_matrix (pattern, &matrix);
  cairo_pattern_set_filter (pattern, src->filter);
  cairo_save (cr);
  cairo_set_source (cr, pattern);
  cairo_paint_with_alpha (cr, ((double) src->opacity) / 255);
  cairo_restore (cr);
  cairo_pattern_destroy (pattern);

  return 0;
}

/*-
 * canvas:drawEllipse (mode:string, xc, yc, w, h,
 *                     [ang_start, ang_end:number])
 *
 * Draws an ellipse with the given dimensions centered at (XC,YC).
 *
 * The following MODE strings are supported:
 *   fill  - fills the ellipse;
 *   frame - draws only the borders of the ellipse.
 *
 * If ANG_START and ANG_END are given, then the ellipse begins at ANG_START
 * and proceeds in the direction of increasing angles to end at ANG_END.
 * Angles are measured in degrees and increase in a clockwise direction.
 */
static int
l_canvas_drawEllipse (lua_State *L)
{
  cairo_t *cr;
  int mode;
  double xc, yc, w, h, ang_start, ang_end;

  canvas_check (L, 1, &cr);
  mode = luaL_checkoption (L, 2, NULL, fill_or_frame_mode_list);
  xc = luaL_checknumber (L, 3);
  yc = luaL_checknumber (L, 4);
  w = luaL_checknumber (L, 5);
  h = luaL_checknumber (L, 6);
  ang_start = luaL_optnumber (L, 7, 0);
  ang_end = luaL_optnumber (L, 8, 360);

  cairo_save (cr);
  cairo_save (cr);
  cairo_translate (cr, xc, yc);
  cairo_scale (cr, w / 2, h / 2);
  cairo_arc (cr, 0, 0, 1, radians (ang_start), radians (ang_end));
  cairo_restore (cr);
  cairox_do_fill_or_frame (cr, mode);
  cairo_restore (cr);

  return 0;
}

/*-
 * canvas:drawLine (x1, y1, x2, y2:number)
 *
 * Draws a line from coordinate (X1,Y1) to (X2,Y2).
 */
static int
l_canvas_drawLine (lua_State *L)
{
  cairo_t *cr;
  double x1, y1, x2, y2;

  canvas_check (L, 1, &cr);
  x1 = luaL_checknumber (L, 2);
  y1 = luaL_checknumber (L, 3);
  x2 = luaL_checknumber (L, 4);
  y2 = luaL_checknumber (L, 5);

  cairo_save (cr);
  cairo_move_to (cr, x1, y1);
  cairo_line_to (cr, x2, y2);
  cairo_stroke (cr);
  cairo_restore (cr);

  return 0;
}

/*-
 *  (filepath [, format [, quality]]) -> dumped: boolean; err_msg: string
 *
 *   Salva o estado atual do canvas em um arquivo definido por filepath.
 *   Retorna true se o dump for realizado com sucesso e false caso contrário,
 *   mais uma mensagem de erro.
 */
static int
l_canvas_dump (lua_State *L)
{
  canvas_t *canvas;
  cairo_status_t err;
  const char *text, *format;
  canvas = canvas_check (L, 1, NULL);
 // luax_dump_stack(L);
  if( lua_gettop (L) >= 3 ){
  text=luaL_checkstring (L, 3);
  
    if (strcmp(text, "jpeg")==0)
    {
      
        int quality=75;
        if( lua_gettop (L) >=4 )
        {
          quality= luaL_checknumber (L, 4);
          if(quality>100)
            quality=100;
          if(quality<0)
            quality=0;
        }
          format=luaL_checkstring (L, 2);
          err= cairo_image_surface_write_to_jpeg (canvas->sfc, strcat(format, ".jpeg"),quality);
          if (unlikely (err != CAIRO_STATUS_SUCCESS))
          {
            lua_pushboolean (L, FALSE);
            lua_pushstring (L, cairo_status_to_string (err));

            return 2;
          }
          lua_pushboolean (L, TRUE);
        
          return 1;
        
    }
    else if (strcmp(text, "png")==0)
    {

      format=luaL_checkstring (L, 2);
      err = cairo_surface_write_to_png (canvas->sfc, strcat(format, ".png"));

      if (unlikely (err != CAIRO_STATUS_SUCCESS))
      {
        lua_pushboolean (L, FALSE);
        lua_pushstring (L, cairo_status_to_string (err));
        return 2;
      }
      lua_pushboolean (L, TRUE);

      return 1;
      
    }
    else
    {

      lua_pushboolean (L, FALSE);
      lua_pushstring (L, "format invalid");

      return 2;

    }
  }
  else
  {
    
    format=luaL_checkstring (L, 2);
    err= cairo_image_surface_write_to_jpeg (canvas->sfc, strcat(format, ".jpeg"),75);
    if (unlikely (err != CAIRO_STATUS_SUCCESS))
    {
      lua_pushboolean (L, FALSE);
      lua_pushstring (L, cairo_status_to_string (err));
      return 2;
    }
    lua_pushboolean (L, TRUE);
        
    return 1;
  }
}

/*-
 * canvas:drawPolygon (mode:string)
 *
 * Returns an anonymous binary drawer function that receives the coordinate
 * of the next vertex and returns itself as the result.  If the drawer
 * function receives nil as input, it completes the vertex collection and
 * draws the resulting polygon.
 *
 * This recurrent procedure allows for the idiom:
 *
 *     canvas:drawPolygon ('open')(1,1)(10,1)(10,10)(1,10)()
 *
 * The following MODE strings are supported:
 *
 *   fill  - links the last point to the first and fills the
 *           resulting area;
 *
 *   close - links the last point to the first and draws only
 *           the borders of the resulting area;
 *
 *   open  - don't link the last point to the first and draws
 *           the resulting open path.
 */
static int
l_canvas_drawPolygon_drawer (lua_State *L)
{
  cairo_t *cr;
  double x, y;

  canvas_check (L, lua_upvalueindex (1), &cr);

  if (lua_isnoneornil (L, 1))   /* last call */
    {
      switch (luaL_checkint (L, lua_upvalueindex (2)))
        {
        case 0:
          cairo_close_path (cr);
          cairo_fill (cr);
          break;
        case 1:
          cairo_close_path (cr);
          /* fall-through */
        case 2:
          cairo_stroke (cr);
          break;
        default:
          g_assert_not_reached ();
        }
      cairo_restore (cr);
      return 0;
    }

  x = luaL_checknumber (L, 1);
  y = luaL_checknumber (L, 2);

  if (lua_isboolean (L, lua_upvalueindex (3)))  /* first call */
    {
      cairo_save (cr);
      cairo_move_to (cr, x, y);
    }
  else
    {
      cairo_line_to (cr, x, y);
    }

  lua_pushvalue (L, lua_upvalueindex (1));
  lua_pushvalue (L, lua_upvalueindex (2));
  lua_pushcclosure (L, l_canvas_drawPolygon_drawer, 2);

  return 1;
}

static int
l_canvas_drawPolygon (lua_State *L)
{
  static const char *const mode_list[] = {"fill", "close", "open", NULL};

  canvas_check (L, 1, NULL);
  lua_pushinteger (L, luaL_checkoption (L, 2, NULL, mode_list));
  lua_replace (L, 2);
  lua_settop (L, 2);
  lua_pushboolean (L, TRUE);
  lua_pushcclosure (L, l_canvas_drawPolygon_drawer, 3);

  return 1;
}

/*-
 * canvas:drawRect (mode:string, x, y, w, h:number)
 *
 * Draws a rectangle with the given dimensions at coordinate (X,Y).
 *
 * The following MODE strings are supported:
 *   fill  - fills the rectangle;
 *   frame - draws only the borders of the rectangle.
 */
static int
l_canvas_drawRect (lua_State *L)
{
  cairo_t *cr;
  int mode;
  double x, y, w, h;

  canvas_check (L, 1, &cr);
  mode = luaL_checkoption (L, 2, NULL, fill_or_frame_mode_list);
  x = luaL_checknumber (L, 3);
  y = luaL_checknumber (L, 4);
  w = luaL_checknumber (L, 5);
  h = luaL_checknumber (L, 6);

  cairo_save (cr);
  cairo_rectangle (cr, x, y, w, h);
  cairox_do_fill_or_frame (cr, mode);
  cairo_restore (cr);

  return 0;
}

/*-
 * canvas:drawRoundRect (mode:string, x, y, w, h, r:number)
 *
 * Draws a rounded rectangle with the given dimensions at coordinate (X,Y).
 *
 * The following MODE strings are supported:
 *   fill  - fills the rounded rectangle;
 *   frame - draws only the borders of the rounded rectangle.
 *
 * The R parameter defines the radius of the round corners of the rectangle.
 */
static int
l_canvas_drawRoundRect (lua_State *L)
{
  cairo_t *cr;
  int mode;
  double x, y, w, h, r;

  canvas_check (L, 1, &cr);
  mode = luaL_checkoption (L, 2, NULL, fill_or_frame_mode_list);
  x = luaL_checknumber (L, 3);
  y = luaL_checknumber (L, 4);
  w = luaL_checknumber (L, 5);
  h = luaL_checknumber (L, 6);
  r = CLAMP (luaL_checknumber (L, 7), 0, MIN (w, h) / 2);

  cairo_save (cr);
  cairo_arc (cr, x + r, y + r, r, G_PI, 1.5 * G_PI);
  cairo_arc (cr, x + w - r, y + r, r, 1.5 * G_PI, 2 * G_PI);
  cairo_arc (cr, x + w - r, y + h - r, r, 0, G_PI / 2);
  cairo_arc (cr, x + r, y + h - r, r, G_PI / 2, G_PI);
  cairo_close_path (cr);
  cairox_do_fill_or_frame (cr, mode);
  cairo_restore (cr);

  return 0;
}

/*-
 * canvas:drawText ([mode:string], x, y:number, text:string)
 *
 * Renders UTF-8 text TEXT with the current font attribute and draws the
 * result at coordinate (X,Y).
 *
 * The following MODE strings are supported:
 *   fill  - fills each glyph of the text (default);
 *   frame - draws the borders each glyph of the text.
 */
static int
l_canvas_drawText (lua_State *L)
{
  canvas_t *canvas;
  cairo_t *cr;
  double x, y;
  const char *text;
  int mode;
  PangoLayout *layout;

  canvas = canvas_check (L, 1, &cr);
  if (lua_type (L, 2) == LUA_TNUMBER)
    {
      lua_pushliteral (L, "fill");
      lua_insert (L, 2);
    }
  mode = luaL_checkoption (L, 2, NULL, fill_or_frame_mode_list);
  x = luaL_checknumber (L, 3);
  y = luaL_checknumber (L, 4);
  text = luaL_checkstring (L, 5);

  layout = pango_cairo_create_layout (cr);
  g_assert_nonnull (layout);
  pango_layout_set_text (layout, text, -1);
  pango_layout_set_font_description (layout, canvas->font);

  cairo_save (cr);
  cairo_move_to (cr, x, y);
  pango_cairo_layout_path (cr, layout);
  cairox_do_fill_or_frame (cr, mode);
  cairo_restore (cr);
  g_object_unref (layout);

  return 0;
}

/*-
 * canvas:flush ()
 *
 * Flushes the pending operations on the given canvas, i.e.,
 * paints canvas->back_sfc onto canvas->sfc.
 */
static int
l_canvas_flush (lua_State *L)
{
  canvas_t *canvas;
  cairo_t *aux_cr;

  canvas = canvas_check (L, 1, NULL);
  if (!canvas_is_double_buffered (canvas))
    return 0;                   /* nothing to do */

  aux_cr = cairo_create (canvas->sfc);
  g_assert_nonnull (aux_cr);
  if (unlikely (!cairox_is_valid (aux_cr)))
    return error_throw_invalid_cr (L, aux_cr);

  cairo_set_source_surface (aux_cr, canvas->back_sfc, 0, 0);
  cairo_set_operator (aux_cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (aux_cr);
  cairo_destroy (aux_cr);

  return 0;
}

/*-
 * canvas:measureText (text:string) -> w, h:number
 *
 * Returns the dimensions of the rectangle that encloses the "inked" portion
 * of text TEXT, as it would be drawn by canvas:drawText(TEXT).
 */
static int
l_canvas_measureText (lua_State *L)
{
  canvas_t *canvas;
  cairo_t *cr;
  const char *text;
  PangoLayout *layout;
  int w, h;

  canvas = canvas_check (L, 1, &cr);
  text = luaL_checkstring (L, 2);

  layout = pango_cairo_create_layout (cr);
  g_assert_nonnull (layout);
  pango_layout_set_text (layout, text, -1);
  pango_layout_set_font_description (layout, canvas->font);
  pango_layout_get_size (layout, &w, &h);
  g_object_unref (layout);

  lua_pushnumber (L, (double) w / PANGO_SCALE);
  lua_pushnumber (L, (double) h / PANGO_SCALE);

  return 2;
}

/*-
 * canvas:pixel (x, y) -> r, g, b, a:number
 * canvas:pixel (x, y, r, g, b, a)
 *
 * Gets or sets the color of the given pixel.  The first pixel is (0,0) and
 * the last one is (w-1,h-1), where w and h are the width and height of the
 * given canvas.
 *
 * WARNING: This function returns an approximation of the original pixel
 * value, not the original value.  E.g.,
 *
 *   c:pixel (0, 0, 100, 0, 0, 0, 15)
 *   c:pixel (0, 0) -> 85, 0, 0, 15    *** approx. ***
 *
 * This occurs because, currently, the color components of the pixel are
 * stored with pre-multiplied alpha.  If the alpha value is too low then,
 * due to loss in arithmetic precision, the original color cannot be
 * recovered.  In particular, if the alpha component is zero, the original
 * color is completely lost.  E.g.,
 *
 *   c:pixel (0, 0, 100, 0, 0, 0, 0)
 *   c:pixel (0, 0) -> 0, 0, 0, 0
 */
static int
l_canvas_pixel (lua_State *L)
{
  canvas_t *canvas;
  int x, y;

  canvas = canvas_check (L, 1, NULL);
  x = luaL_checkint (L, 2);
  y = luaL_checkint (L, 3);

  if (lua_gettop (L) <= 3)
    {
      unsigned char r, g, b, a;

      if (unlikely (x < 0 || y < 0
                    || x >= canvas->width || y >= canvas->height))
        {
          r = 0;
          g = 0;
          b = 0;
          a = 0;
        }
      else
        {
          unsigned char *data;
          cairo_format_t format;
          guint32 pixel;
          int stride;

          cairo_surface_flush (canvas->sfc);
          data = cairo_image_surface_get_data (canvas->sfc);
          g_assert_nonnull (data);

          format = cairo_image_surface_get_format (canvas->sfc);
          g_assert (format == CAIRO_FORMAT_ARGB32);

          stride = cairo_image_surface_get_stride (canvas->sfc);
          g_assert (stride > 0);

          pixel = *((guint32 *) ((void *) (data + y * stride + 4 * x)));
          a = (unsigned char) (pixel >> 24);
          r = (unsigned char) (pixel >> 16 & 0xff);
          g = (unsigned char) (pixel >> 8 & 0xff);
          b = (unsigned char) (pixel & 0xff);

          if (a > 0 && a < 255)
            {
              if (r > 0 && r < 255)
                r = (unsigned char) ((double) r / (double) a * 255);

              if (g > 0 && g < 255)
                g = (unsigned char) ((double) g / (double) a * 255);

              if (b > 0 && b < 255)
                b = (unsigned char) ((double) b / (double) a * 255);
            }
        }
      lua_pushinteger (L, r);
      lua_pushinteger (L, g);
      lua_pushinteger (L, b);
      lua_pushinteger (L, a);
      return 4;
    }
  else
    {
      cairo_t *aux_cr;
      double r, g, b, a;

      r = luaL_checknumber (L, 4);
      g = luaL_checknumber (L, 5);
      b = luaL_checknumber (L, 6);
      a = luaL_optnumber (L, 7, 255);

      aux_cr = cairo_create (canvas->sfc);
      g_assert_nonnull (aux_cr);
      if (unlikely (!cairox_is_valid (aux_cr)))
        return error_throw_invalid_cr (L, aux_cr);

      cairo_set_source_rgba (aux_cr, r / 255, g / 255, b / 255, a / 255);
      cairo_set_operator (aux_cr, CAIRO_OPERATOR_SOURCE);
      cairo_rectangle (aux_cr, x, y, 1, 1);
      cairo_fill (aux_cr);
      cairo_destroy (aux_cr);
      return 0;
    }
}

/* Canvas object methods.  */
static const struct luaL_Reg funcs[] = {
  {"new", l_canvas_new},
  {"__gc", __l_canvas_gc},
  {"_dump_to_file", _l_canvas_dump_to_file},
  {"_dump_to_memory", _l_canvas_dump_to_memory},
  {"_resize", _l_canvas_resize},
  {"_surface", _l_canvas_surface},
  {"attrAntiAlias", l_canvas_attrAntiAlias},
  {"attrClip", l_canvas_attrClip},
  {"attrColor", l_canvas_attrColor},
  {"attrCrop", l_canvas_attrCrop},
  {"attrFilter", l_canvas_attrFilter},
  {"attrFlip", l_canvas_attrFlip},
  {"attrFont", l_canvas_attrFont},
  {"attrLineWidth", l_canvas_attrLineWidth},
  {"attrOpacity", l_canvas_attrOpacity},
  {"attrRotation", l_canvas_attrRotation},
  {"attrScale", l_canvas_attrScale},
  {"attrSize", l_canvas_attrSize},
  {"clear", l_canvas_clear},
  {"compose", l_canvas_compose},
  {"drawEllipse", l_canvas_drawEllipse},
  {"drawLine", l_canvas_drawLine},
  {"dump", l_canvas_dump},
  {"drawPolygon", l_canvas_drawPolygon},
  {"drawRect", l_canvas_drawRect},
  {"drawRoundRect", l_canvas_drawRoundRect},
  {"drawText", l_canvas_drawText},
  {"flush", l_canvas_flush},
  {"measureText", l_canvas_measureText},
  {"pixel", l_canvas_pixel},
  {NULL, NULL},
};

int luaopen_nclua_canvas (lua_State *L);

int
luaopen_nclua_canvas (lua_State *L)
{
  G_TYPE_INIT_WRAPPER ();
  luax_newmetatable (L, CANVAS);
  luaL_setfuncs (L, funcs, 0);
  return 1;
}
