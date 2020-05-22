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

#include "Text.h"
#include "GUI/Text/DistanceFieldGenerator.h"
#include "Core/XML/XMLImporter.h"
#include "Core/Utils/DegateExceptions.h"
#include "Globals.h"

#include <iostream>
#include <QtConcurrent/QtConcurrent>

namespace degate
{
    std::map<QOpenGLContext*, std::shared_ptr<FontContext>> Text::contexts;
    std::vector<std::shared_ptr<FontData>> Text::fonts;

    struct TextVertex2D
    {
        QVector2D pos;
        QVector2D tex_uv;
        QVector3D color;
        float alpha;
        float texture_index;
    };

    FontContext::FontContext(QOpenGLContext* context)
    {
        this->context = context;

        QOpenGLShader* vshader = new QOpenGLShader(QOpenGLShader::Vertex);
        const char* vsrc =
                "#version 130\n"
                "in vec2 pos;\n"
                "in vec2 uv;\n"
                "in vec3 color;\n"
                "in float alpha;\n"
                "in float texture_index;\n"
                "uniform mat4 mvp;\n"
                "out vec2 TexCoords;\n"
                "out vec4 out_color;\n"
                "flat out int texture_layer;\n"
                "void main()\n"
                "{\n"
                "    gl_Position = mvp * vec4(pos, 0.0, 1.0);\n"
                "    TexCoords = uv;\n"
                "	 out_color = vec4(color, alpha);\n"
                "    texture_layer = int(texture_index);\n"
                "}\n";
        vshader->compileSourceCode(vsrc);

        QOpenGLShader* fshader = new QOpenGLShader(QOpenGLShader::Fragment);
        const char* fsrc =
                "#version 130\n"
                "uniform sampler2DArray texture_array;\n"
                "in vec2 TexCoords;\n"
                "in vec4 out_color;\n"
                "flat in int texture_layer;\n"
                "const float width = 0.5;\n"                    // Change this value to change the text style
                "const float edge = 0.03;\n"                    // Change this value to change the text style
                "out vec4 color;\n"
                "void main()\n"
                "{\n"
                "    float distance = 1.0 - texture(texture_array, vec3(TexCoords.xy, texture_layer)).a;\n"
                "    float alpha = 1.0 - smoothstep(width, width + edge, distance);\n"
                "    color = vec4(out_color.rgb, out_color.a * alpha);\n"
                "}\n";
        fshader->compileSourceCode(fsrc);

        program.addShader(vshader);
        program.addShader(fshader);

        program.link();

        delete vshader;
        delete fshader;
    }

    FontContext::~FontContext()
    {
        for(auto& font : fonts)
        {
            if(context->functions()->glIsTexture(font->font_atlas_texture_array) == GL_TRUE)
                context->functions()->glDeleteTextures(1, &font->font_atlas_texture_array);
        }
    }

    std::shared_ptr<FontContextData> FontContext::get_font(const Font& font)
    {
        // Check if the font is already "ready" for this context (created the OpenGL texture).
        for(auto& e : fonts)
        {
            if(e->font_data->font == font)
                return e;
        }

        // If not, search for the FontData of the corresponding font to create the associated OpenGL texture.
        std::shared_ptr<FontData> font_data = Text::search_font(font);

        // Create a new font context data.
        std::shared_ptr<FontContextData> font_context_data = std::make_shared<FontContextData>();
        font_context_data->font_data = font_data;

        // Insert the font context data in the fonts list for this context.
        fonts.push_back(font_context_data);

        reload_font_context(font_context_data, true);

        return font_context_data;
    }

    void FontContext::reload_font_context(const std::shared_ptr<FontContextData>& font_context_data, const bool full_reload) const
    {
        assert(font_context_data != nullptr);

        static unsigned int atlas_count = font_context_data->font_data->font_atlas.size();

        // If there is no new atlas since last time just add the last glyph (last generated/loaded) to the texture array.
        if(full_reload == false && atlas_count == font_context_data->font_data->font_atlas.size() && context->functions()->glIsTexture(font_context_data->font_atlas_texture_array) == GL_TRUE)
        {
            std::shared_ptr<GlyphData> glyph_data = font_context_data->font_data->glyphs.back();
            auto atlas = font_context_data->font_data->font_atlas.back();

            this->context->functions()->glBindTexture(GL_TEXTURE_2D_ARRAY, font_context_data->font_atlas_texture_array);
            assert(this->context->functions()->glGetError() == GL_NO_ERROR);

            this->context->extraFunctions()->glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                                                             0,
                                                             (glyph_data->atlas_position % font_context_data->font_data->atlas_glyph_per_line) * font_context_data->font_data->glyph_width,
                                                             (glyph_data->atlas_position / font_context_data->font_data->atlas_glyph_per_line) * font_context_data->font_data->glyph_height,
                                                             atlas_count - 1,
                                                             font_context_data->font_data->last_generated_glyph_image->width(), font_context_data->font_data->last_generated_glyph_image->height(),
                                                             1,
                                                             GL_RGBA,
                                                             GL_UNSIGNED_BYTE,
                                                             font_context_data->font_data->last_generated_glyph_image->constBits());

            assert(this->context->functions()->glGetError() == GL_NO_ERROR);

            this->context->functions()->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

            return;
        }

        atlas_count = font_context_data->font_data->font_atlas.size();

        if(context->functions()->glIsTexture(font_context_data->font_atlas_texture_array) == GL_TRUE)
            context->functions()->glDeleteTextures(1, &font_context_data->font_atlas_texture_array);

        GLuint texture_array_id;
        this->context->functions()->glGenTextures(1, &texture_array_id);
        assert(this->context->functions()->glGetError() == GL_NO_ERROR);

        this->context->functions()->glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id);
        assert(this->context->functions()->glGetError() == GL_NO_ERROR);

        // Create storage for all font atlas
        this->context->extraFunctions()->glTexImage3D(GL_TEXTURE_2D_ARRAY,
                                                      0,
                                                      GL_RGBA,
                                                      font_context_data->font_data->atlas_width, font_context_data->font_data->atlas_height,
                                                      font_context_data->font_data->font_atlas.size(),
                                                      0,
                                                      GL_RGBA,
                                                      GL_UNSIGNED_BYTE,
                                                      nullptr);

        assert(this->context->functions()->glGetError() == GL_NO_ERROR);

        // Fill the storage with all font atlas
        for(unsigned int i = 0; i < font_context_data->font_data->font_atlas.size(); i++)
        {
            this->context->extraFunctions()->glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                                                             0,
                                                             0, 0, i,
                                                             font_context_data->font_data->atlas_width, font_context_data->font_data->atlas_height,
                                                             1,
                                                             GL_RGBA,
                                                             GL_UNSIGNED_BYTE,
                                                             font_context_data->font_data->font_atlas.at(i)->constBits());

            assert(this->context->functions()->glGetError() == GL_NO_ERROR);
        }

        this->context->functions()->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_TRUE);
        assert(this->context->functions()->glGetError() == GL_NO_ERROR);

        this->context->functions()->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        assert(this->context->functions()->glGetError() == GL_NO_ERROR);

        this->context->functions()->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        assert(this->context->functions()->glGetError() == GL_NO_ERROR);

        this->context->functions()->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        assert(this->context->functions()->glGetError() == GL_NO_ERROR);

        this->context->functions()->glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        assert(this->context->functions()->glGetError() == GL_NO_ERROR);

        this->context->functions()->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        font_context_data->font_atlas_texture_array = texture_array_id;
    }

    void Text::init_context(QOpenGLContext *context)
    {
        if(context == nullptr)
        {
            std::cout << "Can't initialize font context without valid QOpenGLContext." << std::endl;
            return;
        }

        // Context was already initialised
        if(contexts.find(context) != contexts.end())
            return;

        std::shared_ptr<FontContext> font_context = std::make_shared<FontContext>(context);

        contexts.insert({context, font_context});
    }

    void Text::delete_context(QOpenGLContext* context)
    {
        if(context == nullptr)
        {
            std::cout << "Can't delete the font's context without valid QOpenGLContext." << std::endl;
            return;
        }

        auto it = contexts.find(context);

        // Context wasn't initialised
        if(it == contexts.end())
            return;

        // Delete context
        it->second.reset();
        contexts.erase(it);
    }

    std::shared_ptr<FontContext> Text::get_font_context(QOpenGLContext *context)
    {
        if(context == nullptr)
        {
            std::cout << "Can't get the font's context without valid QOpenGLContext." << std::endl;
            return nullptr;
        }

        auto it = contexts.find(context);

        // Context wasn't initialised
        if(it == contexts.end())
            return nullptr;

        return it->second;
    }

    std::shared_ptr<GlyphData> Text::generate_glyph(const std::shared_ptr<FontData>& font_data, const Glyph& glyph)
    {
        // Font
        QFont qt_font(QString::fromStdString(font_data->font.font_family_name), font_data->font.font_size);
        qt_font.setStyleStrategy(QFont::NoAntialias);

        QFontMetricsF font_metrics(qt_font);

        auto glyph_data = std::make_shared<GlyphData>();
        font_data->glyphs.push_back(glyph_data);

        glyph_data->glyph = glyph;
        glyph_data->atlas_position = (font_data->glyphs.size() - 1) % font_data->glyph_per_atlas;
        glyph_data->atlas_index = (font_data->glyphs.size() - 1) / font_data->glyph_per_atlas;
        glyph_data->char_advance = font_metrics.horizontalAdvance(glyph);
        glyph_data->char_width = font_metrics.boundingRect(glyph).width();

        // Font image
        std::shared_ptr<QImage> font_image;

        if(font_data->font_atlas.size() <= glyph_data->atlas_index)
        {
            font_image = std::make_shared<QImage>(font_data->atlas_width, font_data->atlas_height, QImage::Format_ARGB32);
            font_data->font_atlas.push_back(font_image);
            QPainter temp_painter(font_image.get());
            temp_painter.setBackgroundMode(Qt::TransparentMode);
            temp_painter.setCompositionMode (QPainter::CompositionMode_Source);
            temp_painter.setPen(qRgba(0, 0, 0, 0));
            temp_painter.fillRect(0, 0, font_image->width(), font_image->height(), Qt::transparent);
        }
        else
        {
            font_image = font_data->font_atlas.at(glyph_data->atlas_index);
        }

        glyph_data->atlas = font_image;

        // Painter
        QPainter painter(font_image.get());
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setCompositionMode (QPainter::CompositionMode_SourceOver);
        painter.setPen(qRgba(255, 255, 255, 255));
        painter.setFont(qt_font);

        // Distance field generator
        DistanceFieldGenerator dfg(font_data->spread * font_data->scale, font_data->scale);

        // Scaled font
        QFont scaled_font(QString::fromStdString(font_data->font.font_family_name), font_data->font.font_size * font_data->scale);
        scaled_font.setStyleStrategy(QFont::NoAntialias);
        QFontMetricsF scaled_font_metric(scaled_font);

        // Glyph image
        QImage temp_glyph_image(std::ceil(scaled_font_metric.maxWidth() + font_data->padding * 2.0 * font_data->scale), std::ceil(scaled_font_metric.height() + font_data->padding * 2.0 * font_data->scale), QImage::Format_ARGB32);

        // Glyph painter
        QPainter glyph_painter(&temp_glyph_image);
        glyph_painter.setBackgroundMode(Qt::TransparentMode);
        glyph_painter.setCompositionMode (QPainter::CompositionMode_Source);
        glyph_painter.setPen(qRgba(0, 0, 0, 0));
        glyph_painter.fillRect(0, 0, temp_glyph_image.width(), temp_glyph_image.height(), Qt::transparent);
        glyph_painter.setCompositionMode (QPainter::CompositionMode_SourceOver);
        glyph_painter.setPen(qRgba(255, 255, 255, 255));
        glyph_painter.setFont(scaled_font);

        // Glyph draw
        glyph_painter.drawText(QPointF(font_data->padding * font_data->scale, scaled_font_metric.ascent() + font_data->padding * font_data->scale), glyph);

        font_data->last_generated_glyph_image = dfg.generate_distance_field(temp_glyph_image);

        // Distance field conversion
        painter.drawImage(QPointF((glyph_data->atlas_position % font_data->atlas_glyph_per_line) * (font_data->glyph_width), (glyph_data->atlas_position / font_data->atlas_glyph_per_line) * (font_data->glyph_height)), *font_data->last_generated_glyph_image);

        font_data->as_changed = true;

        return glyph_data;
    }

    std::shared_ptr<GlyphData> Text::get_glyph(const Glyph& glyph)
    {
        if(font_context_data.lock() == nullptr || font_context_data.lock()->font_data == nullptr)
            return nullptr;

        for(auto& g : font_context_data.lock()->font_data->glyphs)
        {
            if(g->glyph == glyph)
                return g;
        }

        std::shared_ptr<GlyphData> res = generate_glyph(font_context_data.lock()->font_data, glyph);

        font_context->reload_font_context(font_context_data.lock());

        return res;
    }

    std::shared_ptr<FontData> Text::search_font(const Font& font)
    {
        if(font.font_size == 0 || font.font_family_name.empty())
            return nullptr;


        // Try to get the fond from memory.

        for(auto& e : fonts)
        {
            if(font == e->font)
                return e;
        }

        std::shared_ptr<FontData> res;


        // Try to load the font from the cache.

        res = load_font(font);

        if(res != nullptr)
        {
            fonts.push_back(res);

            return res;
        }


        // Create the font if we can't find it.

        // Font
        QFont qt_font(QString::fromStdString(font.font_family_name), font.font_size);
        qt_font.setStyleStrategy(QFont::PreferAntialias);

        QFontMetricsF font_metrics(qt_font);

        // Font data
        res = std::make_shared<FontData>();
        res->font = font;
        res->default_glyph_height = font_metrics.height();
        res->spread = FONT_DFG_SPREAD;
        res->scale = FONT_DFG_SCALE;
        res->padding = res->spread;
        res->glyph_width = font_metrics.maxWidth() + res->padding * 2.0f;
        res->glyph_height = font_metrics.height() + res->padding * 2.0f;
        res->atlas_height = FONT_ATLAS_SIZE;
        res->atlas_width = FONT_ATLAS_SIZE;
        res->atlas_glyph_per_line = std::floor(res->atlas_width / res->glyph_width);
        res->atlas_glyph_per_column = std::floor(res->atlas_height / res->glyph_height);
        res->glyph_per_atlas = res->atlas_glyph_per_line + res->atlas_glyph_per_column;

        // Create a blank atlas
        std::shared_ptr<QImage> font_image = std::make_shared<QImage>(res->atlas_width, res->atlas_height, QImage::Format_ARGB32);
        res->font_atlas.push_back(font_image);
        QPainter temp_painter(font_image.get());
        temp_painter.setBackgroundMode(Qt::TransparentMode);
        temp_painter.setCompositionMode (QPainter::CompositionMode_Source);
        temp_painter.setPen(qRgba(0, 0, 0, 0));
        temp_painter.fillRect(0, 0, font_image->width(), font_image->height(), Qt::transparent);

        fonts.push_back(res);

        return res;
    }

    void Text::save_fonts_to_cache()
    {
        if(fonts.empty())
            return;

        for(auto& font_data : fonts)
        {
            if(font_data->as_changed == true)
                save_font(font_data);
        }
    }

    std::shared_ptr<FontData> Text::load_font(const Font& font)
    {
        if(font.font_size == 0 || font.font_family_name.empty())
            return nullptr;

        // Fonts config file

        if(!QFile::exists(QString::fromStdString(DEGATE_IN_CONFIGURATION(FONTS_CONFIG_FILE_NAME))))
        {
            return nullptr;
        }

        QFile config_file(QString::fromStdString(DEGATE_IN_CONFIGURATION(FONTS_CONFIG_FILE_NAME)));
        if(!config_file.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            return nullptr;
        }

        QDomDocument doc;
        doc.setContent(&config_file);
        QDomElement root = doc.firstChildElement("config");
        QDomElement font_elem;

        // If file version differs clear all fonts in the cache and reset the font configuration file.
        if(root.attribute("file_version").toInt() != FONT_FILE_VERSION)
        {
            QDomNodeList list = root.elementsByTagName("font");
            for (int x = 0; x < list.length(); x++)
            {
                QDomElement node = list.at(x).toElement();

                QString font_config_file_path = node.attribute("font_config_file_path");
                QString font_atlas_file_path = node.attribute("font_atlas_file_path");
                unsigned int font_count = node.attribute("font_atlas_count").toUInt();

                if(!font_config_file_path.isEmpty())
                    QFile::remove(font_config_file_path);

                if(!font_atlas_file_path.isEmpty() && font_count != 0)
                {
                    for(unsigned int i = 0; i < font_count; i++)
                    QFile::remove(font_atlas_file_path + QString::number(i + 1) + FONT_ATLAS_EXTENSION);
                }
            }

            // Clear the file
            config_file.resize(0);

            config_file.close();

            return nullptr;
        }

        // Search the desired font in the file.
        QDomNodeList list = root.elementsByTagName("font");
        for (int x = 0; x < list.count(); x++)
        {
            QDomElement node = list.at(x).toElement();

            if(node.attribute("font_size").toUInt() == font.font_size && node.attribute("font_family_name") == QString::fromStdString(font.font_family_name))
            {
                font_elem = node;
                break;
            }
        }

        if(font_elem.isNull())
        {
            config_file.close();
            return nullptr;
        }

        QString font_config_file_path = font_elem.attribute("font_config_file_path");
        QString font_atlas_file_path = font_elem.attribute("font_atlas_file_path");
        unsigned int font_count = font_elem.attribute("font_atlas_count").toUInt();

        if(font_config_file_path.isEmpty() || font_atlas_file_path.isEmpty() || font_count == 0)
        {
            root.removeChild(font_elem);
            config_file.close();
            return nullptr;
        }


        // Font config file

        QFile font_config_file(font_config_file_path);
        if(!font_config_file.open(QIODevice::ReadOnly | QIODevice::Text))
            return nullptr;

        std::shared_ptr<FontData> font_data = std::make_shared<FontData>();
        font_data->font = font;

        QTextStream font_config_file_stream(&font_config_file);
        font_config_file_stream.setCodec("UTF-8");

        if(font_config_file_stream.readLine().toUInt() != font_data->font.font_size || font_config_file_stream.readLine().toStdString() != font_data->font.font_family_name)
        {
            config_file.close();
            font_config_file.close();
            return nullptr;
        }

        font_data->default_glyph_height = font_config_file_stream.readLine().toFloat();
        font_data->glyph_width = font_config_file_stream.readLine().toFloat();
        font_data->glyph_height = font_config_file_stream.readLine().toFloat();
        font_data->atlas_width = font_config_file_stream.readLine().toUInt();
        font_data->atlas_height = font_config_file_stream.readLine().toUInt();
        font_data->atlas_glyph_per_line = font_config_file_stream.readLine().toUInt();
        font_data->atlas_glyph_per_column = font_config_file_stream.readLine().toUInt();
        font_data->glyph_per_atlas = font_config_file_stream.readLine().toUInt();
        font_data->padding = font_config_file_stream.readLine().toFloat();
        font_data->spread = font_config_file_stream.readLine().toFloat();
        font_data->scale = font_config_file_stream.readLine().toFloat();
        font_data->as_changed = false;
        unsigned int glyph_count = font_config_file_stream.readLine().toUInt();

        // Font atlas file
        for(unsigned int i = 0; i < font_count; i++)
        {
            font_data->font_atlas.push_back(std::make_shared<QImage>(font_atlas_file_path + QString::number(i + 1) + FONT_ATLAS_EXTENSION));

            if(font_data->font_atlas.at(i)->isNull())
            {
                root.removeChild(font_elem);
                config_file.close();
                font_config_file.close();
                return nullptr;
            }
        }

        // Load glyphs
        for(unsigned int i = 0; i < glyph_count; i++)
        {
            auto glyph = std::make_shared<GlyphData>();
            glyph->glyph = font_config_file_stream.readLine().at(0);
            glyph->char_width = font_config_file_stream.readLine().toFloat();
            glyph->char_advance = font_config_file_stream.readLine().toFloat();
            glyph->atlas_position = font_config_file_stream.readLine().toUInt();
            glyph->atlas_index = font_config_file_stream.readLine().toUInt();
            glyph->atlas = font_data->font_atlas.at(glyph->atlas_index);
            font_data->glyphs.push_back(glyph);
        }

        font_config_file.close();
        config_file.close();

        return font_data;
    }

    void Text::save_font(const std::shared_ptr<FontData>& font_data)
    {
        if(font_data == nullptr)
            return;

        CHECK_PATH(DEGATE_CONFIGURATION_PATH)

        // Fonts config file

        QFile config_file(QString::fromStdString(DEGATE_IN_CONFIGURATION(FONTS_CONFIG_FILE_NAME)));
        QDomDocument doc;
        QDomElement root;
        QDomElement font_elem;

        // Check if the config file exists
        if(!QFile::exists(QString::fromStdString(DEGATE_IN_CONFIGURATION(FONTS_CONFIG_FILE_NAME))))
        {
            if(!config_file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
            {
                throw FileSystemException("Can't create the font config file.");
            }

            root = doc.createElement("config");
            root.setAttribute("file_version", FONT_FILE_VERSION);
            doc.appendChild(root);
            font_elem = doc.createElement("font");
        }
        else
        {
            if(!config_file.open(QIODevice::ReadWrite | QIODevice::Text))
            {
                throw FileSystemException("Can't create the font config file.");
            }

            doc.setContent(&config_file);
            root = doc.firstChildElement("config");

            if(root.isNull())
            {
                root = doc.createElement("config");
                root.setAttribute("file_version", FONT_FILE_VERSION);
                doc.appendChild(root);
                font_elem = doc.createElement("font");
            }
            else
            {
                // Search the desired font in the file.
                QDomNodeList list = root.elementsByTagName("font");
                for (int x = 0; x < list.count(); x++)
                {
                    QDomElement node = list.at(x).toElement();

                    if(node.attribute("font_size").toUInt() == font_data->font.font_size && node.attribute("font_family_name") == QString::fromStdString(font_data->font.font_family_name))
                    {
                        font_elem = node;
                        break;
                    }
                }

                if(font_elem.isNull())
                {
                    font_elem = doc.createElement("font");
                }
            }
        }

        CHECK_PATH(DEGATE_CACHE_PATH)

        std::string font_config_file_path = DEGATE_IN_CACHE(font_data->font.font_family_name + "_" + std::to_string(font_data->font.font_size) + ".fnt");
        std::string font_atlas_file_path = DEGATE_IN_CACHE(font_data->font.font_family_name + "_" + std::to_string(font_data->font.font_size) + "_");

        font_elem.setAttribute("font_size", font_data->font.font_size);
        font_elem.setAttribute("font_family_name", QString::fromStdString(font_data->font.font_family_name));
        font_elem.setAttribute("font_config_file_path", QString::fromStdString(font_config_file_path));
        font_elem.setAttribute("font_atlas_file_path", QString::fromStdString(font_atlas_file_path));
        font_elem.setAttribute("font_atlas_count", QString::number(font_data->font_atlas.size()));

        root.appendChild(font_elem);

        // Clear the file
        config_file.resize(0);

        QTextStream config_file_stream(&config_file);
        config_file_stream << doc.toString();

        config_file.close();


        // Font config file

        QFile font_config_file(QString::fromStdString(font_config_file_path));
        if(!font_config_file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            throw FileSystemException("Can't create the font config file.");
        }

        QTextStream font_config_file_stream(&font_config_file);
        font_config_file_stream.setCodec("UTF-8");

        font_config_file_stream << font_data->font.font_size << endl;
        font_config_file_stream << QString::fromStdString(font_data->font.font_family_name) << endl;
        font_config_file_stream << font_data->default_glyph_height << endl;
        font_config_file_stream << font_data->glyph_width << endl;
        font_config_file_stream << font_data->glyph_height << endl;
        font_config_file_stream << font_data->atlas_width << endl;
        font_config_file_stream << font_data->atlas_height << endl;
        font_config_file_stream << font_data->atlas_glyph_per_line << endl;
        font_config_file_stream << font_data->atlas_glyph_per_column << endl;
        font_config_file_stream << font_data->glyph_per_atlas << endl;
        font_config_file_stream << font_data->padding << endl;
        font_config_file_stream << font_data->spread << endl;
        font_config_file_stream << font_data->scale << endl;
        font_config_file_stream << font_data->glyphs.size() << endl;

        for(auto& glyph_data : font_data->glyphs)
        {
            font_config_file_stream << glyph_data->glyph << endl;
            font_config_file_stream << glyph_data->char_width << endl;
            font_config_file_stream << glyph_data->char_advance << endl;
            font_config_file_stream << glyph_data->atlas_position << endl;
            font_config_file_stream << glyph_data->atlas_index << endl;
        }

        font_config_file.close();


        // Save font atlas

        for(unsigned int i = 0; i < font_data->font_atlas.size(); i++)
            font_data->font_atlas.at(i)->save(QString::fromStdString(font_atlas_file_path) + QString::number(i + 1) + FONT_ATLAS_EXTENSION);

    }

    Text::Text(QWidget* parent, const std::string& font_family_name, const unsigned font_size) : parent(parent), font(Font{font_size, font_family_name})
    {

    }

    Text::~Text()
    {
        if(font_context->context->functions()->glIsBuffer(vbo) == GL_TRUE)
            font_context->context->functions()->glDeleteBuffers(1, &vbo);
    }

    void Text::init()
    {
        font_context = get_font_context();
        font_context_data = font_context->get_font(font);

        font_context->context->functions()->glGenBuffers(1, &vbo);
    }

    void Text::update(unsigned int total_size)
    {
        font_context->context->functions()->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        font_context->context->functions()->glBufferData(GL_ARRAY_BUFFER, total_size * 6 * sizeof(TextVertex2D), 0, GL_DYNAMIC_DRAW);
        font_context->context->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);

        this->total_size = total_size;
    }

    void Text::add_sub_text(unsigned int offset, float x, float y, const std::string& text, const unsigned int text_size, const QVector3D &color, const float alpha, const bool center_x, const bool center_y, float max_width)
    {
        QString string = QString::fromUtf8(text.data(), text.size());
        std::shared_ptr<GlyphData> glyph;
        float size_downscale_factor = 1;
        float size_factor = static_cast<float>(text_size) / static_cast<float>(font.font_size);

        // Max width
        if(max_width > 0)
        {
            float text_width = 0;
            for(Glyph& g : string)
            {
                glyph = get_glyph(g);
                text_width += static_cast<float>(glyph->char_advance) * size_factor;
            }

            if(text_width >= max_width)
                size_downscale_factor = static_cast<float>(max_width) / static_cast<float>(text_width);
        }

        // Set final size factor
        size_factor *= size_downscale_factor;
        QVector3D final_color = color / 255.0;
        float padding = font_context_data.lock()->font_data->padding;

        // Padding adaptation
        x -= padding * size_factor;

        // Center x
        if(center_x == true)
        {
            float x_offset = 0.f;
            for(Glyph& g : string)
            {
                glyph = get_glyph(g);
                x_offset += static_cast<float>(glyph->char_advance) * size_factor;
            }

            x -= x_offset / 2.0;
        }

        // Center y
        if(center_y == true)
            y -= ((font_context_data.lock()->font_data->default_glyph_height + padding) * size_factor) / 2.0;
        else
            y -= padding * 2.0f * size_factor;


        // Fill vbo

        font_context->context->functions()->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        TextVertex2D temp;
        temp.color = final_color;
        temp.alpha = alpha;

        float atlas_width = static_cast<float>(font_context_data.lock()->font_data->atlas_width);
        float atlas_height = static_cast<float>(font_context_data.lock()->font_data->atlas_height);
        unsigned int glyph_per_line = font_context_data.lock()->font_data->atlas_glyph_per_line;

        float pixel_size = 0;
        for(unsigned int i = 0; i < string.size(); i++)
        {
            glyph = get_glyph(string[i]);

            temp.texture_index = glyph->atlas_index;

            auto char_width = glyph->char_advance;

            QVector2D pos_start(x + pixel_size, y);
            QVector2D pos_end(pos_start.x() + (char_width + padding * 2.0) * size_factor, pos_start.y() + (static_cast<float>(font_context_data.lock()->font_data->default_glyph_height) + padding * 2.0) * size_factor);

            QVector2D uv_start((glyph->atlas_position % glyph_per_line) * (font_context_data.lock()->font_data->glyph_width) / atlas_width, (static_cast<float>(glyph->atlas_position / glyph_per_line) * (font_context_data.lock()->font_data->glyph_height)) / atlas_height);
            QVector2D uv_end(uv_start.x() + (char_width + padding * 2.0) / atlas_width, uv_start.y() + (font_context_data.lock()->font_data->glyph_height) / atlas_height);

            temp.pos = QVector2D(pos_start.x(), pos_start.y());
            temp.tex_uv = QVector2D(uv_start.x(), uv_start.y());
            font_context->context->functions()->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(TextVertex2D) + i * 6 * sizeof(TextVertex2D) + 0 * sizeof(TextVertex2D), sizeof(TextVertex2D), &temp);

            temp.pos = QVector2D(pos_end.x(), pos_start.y());
            temp.tex_uv = QVector2D(uv_end.x(), uv_start.y());
            font_context->context->functions()->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(TextVertex2D) + i * 6 * sizeof(TextVertex2D) + 1 * sizeof(TextVertex2D), sizeof(TextVertex2D), &temp);

            temp.pos = QVector2D(pos_start.x(), pos_end.y());
            temp.tex_uv = QVector2D(uv_start.x(), uv_end.y());
            font_context->context->functions()->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(TextVertex2D) + i * 6 * sizeof(TextVertex2D) + 2 * sizeof(TextVertex2D), sizeof(TextVertex2D), &temp);

            temp.pos = QVector2D(pos_start.x(), pos_end.y());
            temp.tex_uv = QVector2D(uv_start.x(), uv_end.y());
            font_context->context->functions()->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(TextVertex2D) + i * 6 * sizeof(TextVertex2D) + 3 * sizeof(TextVertex2D), sizeof(TextVertex2D), &temp);

            temp.pos = QVector2D(pos_end.x(), pos_start.y());
            temp.tex_uv = QVector2D(uv_end.x(), uv_start.y());
            font_context->context->functions()->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(TextVertex2D) + i * 6 * sizeof(TextVertex2D) + 4 * sizeof(TextVertex2D), sizeof(TextVertex2D), &temp);

            temp.pos = QVector2D(pos_end.x(), pos_end.y());
            temp.tex_uv = QVector2D(uv_end.x(), uv_end.y());
            font_context->context->functions()->glBufferSubData(GL_ARRAY_BUFFER, offset * 6 * sizeof(TextVertex2D) + i * 6 * sizeof(TextVertex2D) + 5 * sizeof(TextVertex2D), sizeof(TextVertex2D), &temp);

            pixel_size += char_width * size_factor;
        }

        font_context->context->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Text::draw(const QMatrix4x4 &projection)
    {
        font_context->program.bind();
        font_context->program.setUniformValue("mvp", projection);

        font_context->context->functions()->glBindBuffer(GL_ARRAY_BUFFER, vbo);

        font_context->program.enableAttributeArray("pos");
        font_context->program.setAttributeBuffer("pos", GL_FLOAT, 0, 2, sizeof(TextVertex2D));

        font_context->program.enableAttributeArray("uv");
        font_context->program.setAttributeBuffer("uv", GL_FLOAT, 2 * sizeof(float), 2, sizeof(TextVertex2D));

        font_context->program.enableAttributeArray("color");
        font_context->program.setAttributeBuffer("color", GL_FLOAT, 4 * sizeof(float), 3, sizeof(TextVertex2D));

        font_context->program.enableAttributeArray("alpha");
        font_context->program.setAttributeBuffer("alpha", GL_FLOAT, 7 * sizeof(float), 1, sizeof(TextVertex2D));

        font_context->program.enableAttributeArray("texture_index");
        font_context->program.setAttributeBuffer("texture_index", GL_FLOAT, 8 * sizeof(float), 1, sizeof(TextVertex2D));

        font_context->context->functions()->glBindTexture(GL_TEXTURE_2D_ARRAY, font_context_data.lock()->font_atlas_texture_array);

        font_context->context->functions()->glDrawArrays(GL_TRIANGLES, 0, total_size * 6);

        font_context->context->functions()->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        font_context->context->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
        font_context->program.release();
    }
}