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

#ifndef __WORKSPACETEXT_H__
#define __WORKSPACETEXT_H__

#include <QtOpenGL/QtOpenGL>

#define TEXT_SPACE 1
#define FONT_GLYPH_SIZE 64
#define FONT_ATLAS_SIZE 1024.0
#define FONT_GLYPH_PER_LINE 16
#define FONT_DEFAULT_SIZE 40

namespace degate
{
	class WorkspaceText
	{
	public:
		/*
		 * Call this one time to init the font (that will be use for every text).
		 */
		static void init_font();

		/*
		 * Delete the font.
		 */
		static void delete_font();

		/*
		 * Call this to draw a single text (no vbo, less performance than normal use except if you need an update at each frame).
		 * 
		 * @param x : left bottom corner x coordinate of the first letter.
		 * @param y : left bottom corner y coordinate of the first letter.
		 * @param text : text to draw;
		 * @param projection : the projection matrix to apply. 
		 * @param size : size of the text, the texture atlas is bold Arial 40.
		 * @param color : the color of the text, (R,G,B) with values between 0 and 255.
		 * @param alpha : the opacity of the text, between 0 and 1.
		 * @param center : if true, the text will be centered around the (x,y) point.
		 */
		static void draw_single(unsigned x, unsigned y, const char* text, const QMatrix4x4& projection, const unsigned size = FONT_DEFAULT_SIZE, const QVector3D& color = QVector3D(255, 255, 255), const float alpha = 1, const bool center = false);

		WorkspaceText(QWidget* new_parent);
		~WorkspaceText();

		/*
		 * Init OpenGL routine (vbo).
		 */
		void init();

		/*
	     * Update all vbo with a new total size.
	     * 
	     * @param size : the size of total text to draw.
	     */
		void update(unsigned size);

		/*
	     * Add a new text to the vbo (that will be drawn with others).
	     * 
	     * @param offset : offset to the first character of the first string.
	     * @param x : left bottom corner x coordinate of the first letter.
		 * @param y : left bottom corner y coordinate of the first letter.
		 * @param text : text to draw;
		 * @param size : size of the text, the texture atlas is bold Arial 40.
		 * @param color : the color of the text, (R,G,B) with values between 0 and 255.
		 * @param alpha : the opacity of the text, between 0 and 1.
		 * @param center : if true, the text will be centered around the (x,y) point.
	     */
		void add_sub_text(unsigned offset, unsigned x, unsigned y, const char* text, const unsigned size = FONT_DEFAULT_SIZE, const QVector3D& color = QVector3D(255, 255, 255), const float alpha = 1, const bool center = false);

		/*
	     * Draw all text in the vbo.
	     * 
	     * @param projection : the projection matrix to apply. 
	     */
		void draw(const QMatrix4x4& projection);

	private:
		static GLuint font_texture;
		static QOpenGLShaderProgram program;
		static QOpenGLFunctions* context;
		static GLuint temp_vbo;

		QWidget* parent;
		GLuint vbo;
		unsigned total_size;

	};
}

#endif