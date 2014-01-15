#ifndef __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_
#define __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "BrainOpenGLTextRenderInterface.h"

class QGLWidget;

class QFont;

namespace caret {

    class BrainOpenGLWidgetTextRenderer : public BrainOpenGLTextRenderInterface {
        
    public:
        BrainOpenGLWidgetTextRenderer(QGLWidget* glWidget);
        
        virtual ~BrainOpenGLWidgetTextRenderer();
        
        bool isValid() const;
        
        void drawTextAtWindowCoords(const int viewport[4],
                                    const int windowX,
                                    const int windowY,
                                    const QString& text,
                                    const TextAlignmentX alignmentX,
                                    const TextAlignmentY alignmentY,
                                    const TextStyle textStyle = NORMAL,
                                    const int fontHeight = 14,
                                    const AString& fontName = "");
        
        void drawTextAtModelCoords(const double modelX,
                                   const double modelY,
                                   const double modelZ,
                                   const QString& text,
                                   const TextStyle textStyle = NORMAL,
                                   const int fontHeight = 14,
                                   const AString& fontName = "");
        
    private:
        BrainOpenGLWidgetTextRenderer(const BrainOpenGLWidgetTextRenderer&);

        BrainOpenGLWidgetTextRenderer& operator=(const BrainOpenGLWidgetTextRenderer&);
        
    private:
        class FontData {
        public:
            FontData(QFont* font,
                     const QString fontName,
                     const int fontHeight,
                     const TextStyle textStyle);

            ~FontData();
            
            QFont* m_font;
            QString m_fontName;
            int m_fontHeight;
            TextStyle m_textStyle;
        };
        
        std::vector<FontData*> m_fonts;
        
        QFont* findFont(const QString& fontName,
                        const int fontHeight,
                        const TextStyle textStyle);
        
        QGLWidget* m_glWidget;
        
        QString m_emptyNameFont;
    };
    
#ifdef __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_WIDGET_TEXT_RENDERER__H_
