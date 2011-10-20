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

#include "Brain.h"
#include "ModelDisplayControllerVolume.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this volume controller belongs.
 *
 */
ModelDisplayControllerVolume::ModelDisplayControllerVolume(Brain* brain)
: ModelDisplayController(ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES,
                         YOKING_ALLOWED_NO,
                         ROTATION_ALLOWED_NO)
{
    this->brain = brain;
    this->initializeMembersModelDisplayControllerVolume();
}

/**
 * Destructor
 */
ModelDisplayControllerVolume::~ModelDisplayControllerVolume()
{
}

void
ModelDisplayControllerVolume::initializeMembersModelDisplayControllerVolume()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->sliceViewPlane[i]         = VolumeSliceViewPlaneEnum::AXIAL;
        this->sliceViewMode[i]          = VolumeSliceViewModeEnum::ORTHOGONAL;
        this->montageNumberOfColumns[i] = 3;
        this->montageNumberOfRows[i]    = 3;
        this->montageSliceSpacing[i]    = 5;
        this->volumeSlicesSelected[i].reset();
    }
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelDisplayControllerVolume::getNameForGUI(const bool includeStructureFlag) const
{
    return "ModelDisplayControllerVolume::getNameForGUI_NEEDS_IMPLEMENTATION";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelDisplayControllerVolume::getNameForBrowserTab() const
{
    return "Volume";
}

/**
 * Get the brain that created this controller.
 * @return The brain.
 */
Brain*
ModelDisplayControllerVolume::getBrain()
{
    return this->brain;
}

VolumeFile* 
ModelDisplayControllerVolume::getVolumeFile()
{
    VolumeFile* vf = NULL;
    if (this->brain->getNumberOfVolumeFiles() > 0) {
        vf = this->brain->getVolumeFile(0);
    }
    return vf;
}

/**
 * Return the for axis mode in the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return Axis mode.
 *   
 */
VolumeSliceViewPlaneEnum::Enum 
ModelDisplayControllerVolume::getSliceViewPlane(const int32_t windowTabNumber) const
{    
    return this->sliceViewPlane[windowTabNumber];
}

/**
 * Set the axis mode in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param sliceAxisMode
 *    New value for axis mode
 */
void 
ModelDisplayControllerVolume::setSliceViewPlane(const int32_t windowTabNumber,
                      VolumeSliceViewPlaneEnum::Enum slicePlane)
{   
    this->sliceViewPlane[windowTabNumber] = slicePlane;
}

/**
 * Return the view mode for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   View mode.
 */
VolumeSliceViewModeEnum::Enum 
ModelDisplayControllerVolume::getSliceViewMode(const int32_t windowTabNumber) const
{    
    return this->sliceViewMode[windowTabNumber];
}

/**
 * Set the view mode in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param sliceViewMode
 *    New value for view mode
 */
void 
ModelDisplayControllerVolume::setSliceViewMode(const int32_t windowTabNumber,
                      VolumeSliceViewModeEnum::Enum sliceViewMode)
{    this->sliceViewMode[windowTabNumber] = sliceViewMode;
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
VolumeSliceIndicesSelection* 
ModelDisplayControllerVolume::getSelectedVolumeSlices(const int32_t windowTabNumber)
{
    return &this->volumeSlicesSelected[windowTabNumber];
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
const VolumeSliceIndicesSelection* 
ModelDisplayControllerVolume::getSelectedVolumeSlices(const int32_t windowTabNumber) const
{
    return &this->volumeSlicesSelected[windowTabNumber];
}



/**
 * Return the montage number of columns for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage number of columns 
 */
int32_t 
ModelDisplayControllerVolume::getMontageNumberOfColumns(const int32_t windowTabNumber) const
{    
    return this->montageNumberOfColumns[windowTabNumber];
}


/**
 * Set the montage number of columns in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param 
 *    New value for montage number of columns 
 */
void 
ModelDisplayControllerVolume::setMontageNumberOfColumns(const int32_t windowTabNumber,
                               const int32_t montageNumberOfColumns)
{    
    this->montageNumberOfColumns[windowTabNumber] = montageNumberOfColumns;
}

/**
 * Return the montage number of rows for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage number of rows
 */
int32_t 
ModelDisplayControllerVolume::getMontageNumberOfRows(const int32_t windowTabNumber) const
{
    return this->montageNumberOfRows[windowTabNumber];
}

/**
 * Set the montage number of rows in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param 
 *    New value for montage number of rows 
 */
void 
ModelDisplayControllerVolume::setMontageNumberOfRows(const int32_t windowTabNumber,
                            const int32_t montageNumberOfRows)
{    
    this->montageNumberOfRows[windowTabNumber] = montageNumberOfRows;
}

/**
 * Return the montage slice spacing for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage slice spacing.
 */
int32_t 
ModelDisplayControllerVolume::getMontageSliceSpacing(const int32_t windowTabNumber) const
{    
    return this->montageSliceSpacing[windowTabNumber];
}

/**
 * Set the montage slice spacing in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param 
 *    New value for montage slice spacing 
 */
void 
ModelDisplayControllerVolume::setMontageSliceSpacing(const int32_t windowTabNumber,
                            const int32_t montageSliceSpacing)
{
    this->montageSliceSpacing[windowTabNumber] = montageSliceSpacing;
}

void 
ModelDisplayControllerVolume::updateController(const int32_t windowTabNumber)
{
    VolumeFile* vf = this->getVolumeFile();
    if (vf != NULL) {
        this->volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
    }
}

/**
 * Reset view.  For left and right hemispheres, the default
 * view is a lateral view.
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void
ModelDisplayControllerVolume::resetView(const int32_t windowTabNumber)
{
    ModelDisplayController::resetView(windowTabNumber);
    VolumeFile* vf = this->getVolumeFile();
    if (vf != NULL) {
        this->volumeSlicesSelected[windowTabNumber].selectSlicesAtOrigin(vf);
    }
}

