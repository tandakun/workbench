
#ifndef __VOLUME_FILE_H__
#define __VOLUME_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#include "VolumeBase.h"
#include "CaretMappableDataFile.h"
#include "CaretMutex.h"
#include "CaretVolumeExtension.h"
#include "StructureEnum.h"
#include "GiftiMetaData.h"
#include "BoundingBox.h"
#include "DescriptiveStatistics.h"
#include "PaletteFile.h"
#include "VolumeFileVoxelColorizer.h"

namespace caret {
    
    class VolumeFileVoxelColorizer;
    class VolumeSpline;
    
    class VolumeFile : public VolumeBase, public CaretMappableDataFile
    {
        CaretVolumeExtension m_caretVolExt;
        
        void parseExtensions();//called after reading a file, in order to populate m_caretVolExt with best guesses
        
        void validateMembers();//called to ensure extension agrees with number of subvolumes
        
        void updateCaretExtension();//called before writing a file, erases all existing caret extensions from m_extensions, and rebuilds one from m_caretVolExt
        
        void checkStatisticsValid();
        
        struct BrickAttributes//for storing ONLY stuff that doesn't get saved to the caret extension
        {//TODO: prune this once statistics gets straightened out
            CaretPointer<DescriptiveStatistics> m_statistics;
            CaretPointer<FastStatistics> m_fastStatistics;
            CaretPointer<DescriptiveStatistics> m_statisticsLimitedValues;
            CaretPointer<Histogram> m_histogram;
            CaretPointer<Histogram> m_histogramLimitedValues;
            CaretPointer<GiftiMetaData> m_metadata;//NOTE: does not get saved currently!
        };
        
        mutable std::vector<BrickAttributes> m_brickAttributes;//because statistics and metadata construct lazily
        
        bool m_brickStatisticsValid;//so that setModified() doesn't do something slow
        
        /** Performs coloring of voxels.  Will be NULL if coloring is disabled. */
        VolumeFileVoxelColorizer* m_voxelColorizer;
        
        mutable CaretMutex m_splineMutex;
        
        mutable bool m_splinesValid;
        
        mutable std::vector<bool> m_frameSplineValid;
        
        mutable std::vector<VolumeSpline> m_frameSplines;
        
    public:
        
        enum InterpType
        {
            ENCLOSING_VOXEL,
            TRILINEAR,
            CUBIC
        };
        
        const static float INVALID_INTERP_VALUE;
        
        /** Enables coloring.  Coloring is almost always not needed for command line operations */
        static bool s_voxelColoringEnabled;
        
        static void setVoxelColoringEnabled(const bool enabled);
        
        VolumeFile();
        VolumeFile(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1, SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY);
        //convenience method for unsigned
        VolumeFile(const std::vector<uint64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const uint64_t numComponents = 1, SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY);
        ~VolumeFile();
        
        virtual void clear();
        
        ///recreates the volume file storage with new size and spacing
        void reinitialize(const std::vector<int64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const int64_t numComponents = 1, SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY);
        void reinitialize(const std::vector<uint64_t>& dimensionsIn, const std::vector<std::vector<float> >& indexToSpace, const uint64_t numComponents = 1, SubvolumeAttributes::VolumeType whatType = SubvolumeAttributes::ANATOMY);
        
        void setType(SubvolumeAttributes::VolumeType whatType);
        
        SubvolumeAttributes::VolumeType getType() const;
        
        void validateSplines(const int64_t brickIndex = 0, const int64_t component = 0) const;

        float interpolateValue(const float* coordIn, InterpType interp = TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0) const;

        float interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp = TRILINEAR, bool* validOut = NULL, const int64_t brickIndex = 0, const int64_t component = 0) const;

        ///returns true if volume space matches in spatial dimensions and sform
        bool matchesVolumeSpace(const VolumeFile* right) const;
        
        ///returns true if volume space matches in spatial dimensions and sform
        bool matchesVolumeSpace(const int64_t dims[3], const std::vector<std::vector<float> >& sform) const;
        
        void readFile(const AString& filename) throw (DataFileException);

        void writeFile(const AString& filename) throw (DataFileException);

        bool isEmpty() const { return VolumeBase::isEmpty(); }
        
        virtual void setModified();
        
        virtual void clearModified();
        
        virtual bool isModified() const;
        
        BoundingBox getVoxelSpaceBoundingBox() const;
        
        /**
         * @return The structure for this file.
         */
        StructureEnum::Enum getStructure() const;
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        void setStructure(const StructureEnum::Enum structure);
        
        /**
         * @return Get access to the file's metadata.
         */
        GiftiMetaData* getFileMetaData() { return NULL; }//doesn't seem to be a spot for generic metadata in the nifti caret extension
        
        /**
         * @return Get access to unmodifiable file's metadata.
         */
        const GiftiMetaData* getFileMetaData() const { return NULL; }
        
        bool isSurfaceMappable() const { return false; }
        
        bool isVolumeMappable() const { return true; }
        
        int32_t getNumberOfMaps() const { return m_dimensions[3]; }
        
        AString getMapName(const int32_t mapIndex) const;
        
        void setMapName(const int32_t mapIndex,
                                const AString& mapName);
        
        const GiftiMetaData* getMapMetaData(const int32_t mapIndex) const;
        
        GiftiMetaData* getMapMetaData(const int32_t mapIndex);
        
        const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex);
        
        const FastStatistics* getMapFastStatistics(const int32_t mapIndex);
        
        const Histogram* getMapHistogram(const int32_t mapIndex);
        
        const DescriptiveStatistics* getMapStatistics(const int32_t mapIndex,
                                                      const float mostPositiveValueInclusive,
                                                      const float leastPositiveValueInclusive,
                                                      const float leastNegativeValueInclusive,
                                                      const float mostNegativeValueInclusive,
                                                      const bool includeZeroValues);
        
        const Histogram* getMapHistogram(const int32_t mapIndex,
                                                              const float mostPositiveValueInclusive,
                                                              const float leastPositiveValueInclusive,
                                                              const float leastNegativeValueInclusive,
                                                              const float mostNegativeValueInclusive,
                                                              const bool includeZeroValues);
        
        bool isMappedWithPalette() const;
        
        PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex);
        
        const PaletteColorMapping* getMapPaletteColorMapping(const int32_t mapIndex) const;
        
        bool isMappedWithLabelTable() const;
        
        GiftiLabelTable* getMapLabelTable(const int32_t mapIndex);
        
        const GiftiLabelTable* getMapLabelTable(const int32_t mapIndex) const;

        AString getMapUniqueID(const int32_t mapIndex) const;
        
        void updateScalarColoringForMap(const int32_t mapIndex,
                                     const PaletteFile* paletteFile);
        
        void getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                         const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                         const int64_t sliceIndex,
                                         uint8_t* rgbaOut) const;

        void getVoxelValuesForSliceInMap(const int32_t mapIndex,
                                         const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                         const int64_t sliceIndex,
                                         float* sliceValues) const;
        
        void getVoxelColorInMap(const int64_t i,
                                const int64_t j,
                                const int64_t k,
                                const int64_t mapIndex,
                                uint8_t rgbaOut[4]) const;
        
        void clearVoxelColoringForMap(const int64_t mapIndex);
        
        void setVoxelColorInMap(const int64_t i,
                                 const int64_t j,
                                 const int64_t k,
                                 const int64_t mapIndex,
                                 const float rgba[4]);
        
    private:
        friend class VolumeFileVoxelColorizer;
    };

}

#endif //__VOLUME_FILE_H__
