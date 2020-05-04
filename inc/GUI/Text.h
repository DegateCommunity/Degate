/* -*-c++-*-

  This file is part of the IC reverse engineering tool degate.

  Copyright 2008, 2009, 2010 by Martin Schobert

  Degate is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  any later version.

  Degate is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with degate. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __TEXT_H__
#define __TEXT_H__

#include <QtOpenGL/QtOpenGL>
#include <map>

#define FONT_DFG_SPREAD 4.0
#define FONT_DFG_SCALE 8.0

#define FONT_ATLAS_SIZE 2048

#define FONT_DEFAULT_FAMILY "arial"
#define FONT_DEFAULT_SIZE 20

#define FONTS_CONFIG_FILE_NAME "font.config"
#define FONT_ATLAS_EXTENSION ".png"

#define FONT_FILE_VERSION 2

namespace degate
{
    /**
     * Describe a font (from it font family name and font size).
     */
     struct Font
     {
         inline bool operator==(const Font& font) const
         {
             return font.font_size == font_size && font.font_family_name == font_family_name;
         }

         inline bool operator<(const Font& font) const
         {
             return font.font_size < font_size && font.font_family_name < font_family_name;
         }

         unsigned int font_size;        /**< The font size. */
         std::string font_family_name;  /**< The font family name. */
     };

     /**
      * A glyph is a simple QChar (to support UTF-8).
      */
     typedef QChar Glyph;

    /**
     * @struct GlyphData
     * @brief Describe a generated/loaded glyph.
     *
     * It stores the glyph information, the atlas and the glyph position in it.
     */
     struct GlyphData
     {
         Glyph glyph;                   /**< The glyph. */
         float char_width;              /**< The width of the bounding box for each glyph (@see https://doc.qt.io/qt-5/qfontmetrics.html#boundingRect)*/
         float char_advance;            /**< The advance of each glyph (@see https://doc.qt.io/qt-5/qfontmetrics.html#horizontalAdvance) */
         std::weak_ptr<QImage> atlas;   /**< The glyph atlas reference. */
         unsigned int atlas_position;   /**< The glyph position in the atlas. */
         unsigned int atlas_index;      /**< The index of the atlas where the glyph belongs to. */
     };

    /**
     * @struct FontData
     * @brief Describe a generated/loaded font.
     *
     * It stores all font atlas (QImage), all generated glyphs, and all needed information to use them.
     */
    struct FontData
    {
        Font font;                                          /**< Store font identification information (size and font family name). */
        float default_glyph_height;                         /**< The glyph height without padding added (the height is the same for all glyphs). */
        float glyph_width;                                  /**< The width of one glyph in the atlas font (with padding added). */
        float glyph_height;                                 /**< The height of one glyph in the atlas font (with padding added). */
        unsigned atlas_width;                               /**< The width of a font atlas. */
        unsigned atlas_height;                              /**< The height of a font atlas. */
        unsigned atlas_glyph_per_line;                      /**< The number of glyphs per line. */
        unsigned atlas_glyph_per_column;                    /**< The number of glyphs per column. */
        unsigned glyph_per_atlas;                           /**< The number of glyphs per atlas. */
        float padding;                                      /**< The padding (= the spread of the Distance Field method). */
        float spread;                                       /**< The spread of the Distance Field method). */
        float scale;                                        /**< The scale of the Distance Field method). */
        std::vector<std::shared_ptr<QImage>> font_atlas;    /**< The font atlas. */
        std::vector<std::shared_ptr<GlyphData>> glyphs;     /**< The list of glyphs. */
        bool as_changed = true;                             /**< Tell if the font needs to be saved again (or not, if it didn't change) */
    };

    /**
     * Store the FontData of a font and the associated OpenGL font atlas.
     */
     struct FontContextData
     {
         std::shared_ptr<FontData> font_data;   /**< The font data of the font */
         GLuint font_atlas_texture_array;       /**< The associated font atlas OpenGL texture array */
     };

    /**
     * @struct FontContext
     * @brief Store fonts in a specific context.
     */
    struct FontContext
    {
        /**
         * Create a font context, it will create the font's texture and the vbo.
         * There is no check here of the presence of the context in the contexts list (@see Text::init_context function).
         *
         * @param context : the (opengl) context.
         */
        explicit FontContext(QOpenGLContext* context);
        ~FontContext();

        /**
         * Get the corresponding font for font_size and font_family_name for the current context, create it if needed.
         *
         * @param font: the font.
         *
         * @return Return the FontData associated to the font and the corresponding OpenGL texture.
         */
        std::shared_ptr<FontContextData> get_font(const Font& font);

        /**
         * Synchronize the font atlas from memory with opengl texture array.
         *
         * @param font_context_data : the font context data to reload.
         */
        void reload_font_context(const std::shared_ptr<FontContextData>& font_context_data);

        // The vector that store fonts (described by his font size and font family name) and FontData with the corresponding OpenGL texture for this context.
        std::vector<std::shared_ptr<FontContextData>> fonts;

        // The shader program.
        QOpenGLShaderProgram program;

        // The context.
        QOpenGLContext* context;
    };

    class Text
    {
    public:
        /**
		 * Call this once to init the font for a specific context (that will be used for every text).
         *
         * @param context : the (opengl) context.
		 */
        static void init_context(QOpenGLContext* context = QOpenGLContext::currentContext());

        /**
		 * Delete the font for a specific context.
         *
         * @param context : the (opengl) context.
		 */
        static void delete_context(QOpenGLContext* context = QOpenGLContext::currentContext());

        /**
         * Search associated FontData for a specific font (defined by font family name and his font size).
         * If no FontData already loaded for the font, it plus search in the cache.
         * If there is no corresponding FontData in the cache it will generate it.
         *
         * @param font : the font.
         *
         * @return Return the associated FontData.
         */
        static std::shared_ptr<FontData> search_font(const Font& font);

        /**
         * Save all new/updated font to the cache (so they will not be regenerated next time).
         */
        static void save_fonts_to_cache();

        /**
         * Create a text.
         *
         * The font size is different from the final text size. The font size will be used to generate a font atlas.
         * And, later, it will be upscaled or downscaled regarding the text size.
         * Theoretically it is useful to increase it if you need to draw big text, it will generate high resolution glyphs to avoid pixelation.
         * But, with the technology used (Distance Field Text) even with a small font size you can draw big text without pixelation.
         * So, to avoid performance and memory issued, let it around 20pt and keep it always the same.
         *
         * @param parent : the parent of the text.
         * @param font_family_name : the font family name.
         * @param font_size : the size of the font (in pt) that will be loaded (different from the text size).
         */
        explicit Text(QWidget* parent, const std::string& font_family_name = FONT_DEFAULT_FAMILY, unsigned font_size = FONT_DEFAULT_SIZE);
        ~Text();

        /**
         * Init OpenGL routine (vbo).
         */
        void init();

        /**
         * Update all vbo with a new total size.
         *
         * @param total_size : the size of total text to draw.
         */
        void update(unsigned total_size);

        /**
         * Add a new text to the vbo (that will be drawn with others).
         *
         * @param offset : offset to the first character of the first string.
         * @param x : left bottom corner x coordinate of the first letter.
         * @param y : left bottom corner y coordinate of the first letter.
         * @param text : text to draw;
         * @param text_size : size of the text (in pt).
         * @param color : the color of the text, (R,G,B) with values between 0 and 255.
         * @param alpha : the opacity of the text, between 0 and 1.
         * @param center_x : if true, the text will be centered on the x axis.
         * @param center_y : if true, the text will be centered on the y axis.
         * @param max_width : downscale the text until the text width is less than max width.
         */
        void add_sub_text(unsigned offset, float x, float y, const std::string& text, unsigned int text_size, const QVector3D& color = QVector3D(255, 255, 255), float alpha = 1, bool center_x = false, bool center_y = false, float max_width = 0);

        /**
         * Draw all text in the vbo.
         *
         * @param projection : the projection matrix to apply.
         */
        void draw(const QMatrix4x4& projection);

    protected:
        /**
         * Get the font context from an opengl context.
         *
         * @param context : the (opengl) context.
         *
         * @return Return the shared pointer to the font context.
         */
        static std::shared_ptr<FontContext> get_font_context(QOpenGLContext* context = QOpenGLContext::currentContext());

        /**
         * Generate a glyph for a specific font with the Distance Field method.
         *
         * @param font : the font.
         * @param glyph : the glyph.
         *
         * @return Return the new generated glyph data.
         */
        static std::shared_ptr<GlyphData> generate_glyph(const std::shared_ptr<FontData>& font_data, const Glyph& glyph);

        /**
         * Load a font from the cache.
         *
         * @param font : the font.
         *
         * @return Return the loaded font.
         */
        static std::shared_ptr<FontData> load_font(const Font& font);

        /**
         * Save a font in the cache.
         *
         * @param font_data : the FontData associated to the font.
         */
        static void save_font(const std::shared_ptr<FontData>& font_data);

        /**
         * Get a glyph for a specific font.
         *
         * @param glyph : the glyph.
         *
         * @return Return the new generated glyph data.
         */
        std::shared_ptr<GlyphData> get_glyph(const Glyph& glyph);

    private:
        // Static map that stores all FontContext (corresponding to OpenGL context).
        static std::map<QOpenGLContext*, std::shared_ptr<FontContext>> contexts;

        // Hold all loaded/generated fonts (a font is described by his font size and font family name).
        static std::vector<std::shared_ptr<FontData>> fonts;

        std::shared_ptr<FontContext> font_context = nullptr;
        QWidget* parent = nullptr;
        GLuint vbo = 0;
        unsigned total_size = 0;
        Font font;
        std::weak_ptr<FontContextData> font_context_data;
    };
}

#endif //__TEXT_H__
