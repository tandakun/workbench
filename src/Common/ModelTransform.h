#ifndef __MODEL_TRANSFORM_H__
#define __MODEL_TRANSFORM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/


#include "CaretObject.h"

namespace caret {

    class ModelTransform : public CaretObject {
        
    public:
        ModelTransform();
        
        virtual ~ModelTransform();
        
        ModelTransform(const ModelTransform&);
        
        ModelTransform& operator=(const ModelTransform&);
        
        AString getName() const;
        
        void getTranslation(float translation[3]) const;
        
        void getRotation(float rotation[4][4]) const;
        
        void getObliqueRotation(float obliqueRotation[4][4]) const;
        
        float getScaling() const;
        
        void setName(const AString& name);
        
        AString getComment() const;
        
        void setComment(const AString& comment);
        
        void setTranslation(const float translation[3]);
        
        void setTranslation(const float translationX,
                            const float translationY,
                            const float translationZ);
        
        void setRotation(const float rotation[4][4]);
        
        void setObliqueRotation(const float obliqueRotation[4][4]);
        
        void setScaling(const float scaling);
        
        void setPanningRotationMatrixAndZoom(const float panX,
                                             const float panY,
                                             const float panZ,
                                             const float rotationMatrix[4][4],
                                             const float obliqueRotationMatrix[4][4],
                                             const float zoom);
        
        void getPanningRotationMatrixAndZoom(float& panX,
                                             float& panY,
                                             float& panZ,
                                             float rotationMatrix[4][4],
                                             float obliqueRotationMatrix[4][4],
                                             float& zoom) const;
        
        AString getAsString() const;
        
        bool setFromString(const AString& s);
        
        void setToIdentity();
        
        bool operator<(const ModelTransform& view) const;
        
        bool operator==(const ModelTransform& view) const;
        
    public:
        virtual AString toString() const;
        
    private:
        void copyHelper(const ModelTransform& ModelTransform);
        
        AString name;
        
        AString comment;
        
        float translation[3];
        
        float rotation[4][4];
        
        float obliqueRotation[4][4];
        
        float scaling;
        
        static const QString s_separatorInPreferences;
    };
    
#ifdef __MODEL_TRANSFORM_DECLARE__
    const QString ModelTransform::s_separatorInPreferences = "::::";
#endif // __MODEL_TRANSFORM_DECLARE__

} // namespace
#endif  //__MODEL_TRANSFORM_H__
