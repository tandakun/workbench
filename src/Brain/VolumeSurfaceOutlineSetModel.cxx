
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <algorithm>

#define __VOLUME_SURFACE_OUTLINE_SET_MODEL_DECLARE__
#include "VolumeSurfaceOutlineSetModel.h"
#undef __VOLUME_SURFACE_OUTLINE_SET_MODEL_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGetAll.h"
#include "EventManager.h"
#include "ModelSurface.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModel.h"


using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineSetModel 
 * \brief Holds a set of VolumeSurfaceOutlineModels
 *
 * Holds a set of VolumeSurfaceOutlineModels.  Users 
 * may add additional surface outline models up to 
 * a fixed limit.  There is also a minimum number
 * that are displayed.
 */
/**
 * Constructor.
 */
VolumeSurfaceOutlineSetModel::VolumeSurfaceOutlineSetModel()
: CaretObject()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        this->outlineModels[i] = new VolumeSurfaceOutlineModel();
    }
    this->numberOfDisplayedVolumeSurfaceOutlines = BrainConstants::MINIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES;
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineSetModel::~VolumeSurfaceOutlineSetModel()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        delete this->outlineModels[i];
    }
}

/**
 * @return The number of volume surface outlines.
 */
int32_t 
VolumeSurfaceOutlineSetModel::getNumberOfDislayedVolumeSurfaceOutlines() const
{
    return this->numberOfDisplayedVolumeSurfaceOutlines;
}

/**
 * Set the number of volume surface outlines.
 * @param numberDisplayed
 *    Number of displayed volume surface outlines.
 */
void 
VolumeSurfaceOutlineSetModel::setNumberOfDisplayedVolumeSurfaceOutlines(const int32_t numberDisplayed)
{
    this->numberOfDisplayedVolumeSurfaceOutlines = numberDisplayed;
    
    if (this->numberOfDisplayedVolumeSurfaceOutlines <
        BrainConstants::MINIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES) {
        this->numberOfDisplayedVolumeSurfaceOutlines = BrainConstants::MINIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES;
    }
    
    if (this->numberOfDisplayedVolumeSurfaceOutlines >
        BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES) {
        this->numberOfDisplayedVolumeSurfaceOutlines = BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES;
    }
}

/**
 * @return The volume surface outline model at the given index.
 * @param indx
 *    Index of volume surface outline model.
 */
VolumeSurfaceOutlineModel* 
VolumeSurfaceOutlineSetModel::getVolumeSurfaceOutlineModel(const int32_t indx)
{
    CaretAssertArrayIndex(this->outlineModels, 
                          BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES, 
                          indx);
    return this->outlineModels[indx];
}

/**
 * @return The volume surface outline model at the given index.
 * @param indx
 *    Index of volume surface outline model.
 */
const VolumeSurfaceOutlineModel* 
VolumeSurfaceOutlineSetModel::getVolumeSurfaceOutlineModel(const int32_t indx) const
{
    CaretAssertArrayIndex(this->outlineModels, 
                          BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES, 
                          indx);
    return this->outlineModels[indx];
}

/**
 * Set the default selected surfaces after a spec file is loaded.
 * @searchForTabs
 *   If true, examine the loaded tabs to find left and right surfaces.
 */ 
void 
VolumeSurfaceOutlineSetModel::selectSurfacesAfterSpecFileLoaded(Brain* brain,
                                                                const bool searchForTabs)
{
    EventBrowserTabGetAll getAllTabs;
    EventManager::get()->sendEvent(getAllTabs.getPointer());
    
    /*
     * Find tabs with left/right
     */
    int32_t leftTabIndex  = -1;
    int32_t rightTabIndex = -1;
    const int numTabs = getAllTabs.getNumberOfBrowserTabs();
    if (searchForTabs) {
        for (int32_t i = 0; i < numTabs; i++) {
            BrowserTabContent* tabContent = getAllTabs.getBrowserTab(i);
            ModelSurface* surfaceModel = tabContent->getDisplayedSurfaceModel();
            if (surfaceModel != NULL) {
                const StructureEnum::Enum structure = surfaceModel->getSurface()->getStructure();
                switch (structure) {
                    case StructureEnum::CORTEX_LEFT:
                        leftTabIndex = tabContent->getTabNumber();
                        break;
                    case StructureEnum::CORTEX_RIGHT:
                        rightTabIndex = tabContent->getTabNumber();
                        break;
                    default:
                        break;
                }
            }
        }
    }
    else {
        if (numTabs >= 1) {
            leftTabIndex = 0;
        }
        if (numTabs >= 2) {
            rightTabIndex = 1;
        }
    }
    
    Surface* leftMidThickSurface = NULL;
    Surface* leftWhiteSurface    = NULL;
    Surface* leftPialSurface     = NULL;
    BrainStructure* leftBrainStructure = brain->getBrainStructure(StructureEnum::CORTEX_LEFT, 
                                                                             false);
    if (leftBrainStructure != NULL) {
        leftMidThickSurface = leftBrainStructure->getSurfaceContainingTextInName("midthick");
        if (leftMidThickSurface == NULL) {
            leftMidThickSurface = leftBrainStructure->getVolumeInteractionSurface();
        }
        
        leftWhiteSurface    = leftBrainStructure->getSurfaceContainingTextInName("white");
        leftPialSurface     = leftBrainStructure->getSurfaceContainingTextInName("pial");
    }
    
    Surface* rightMidThickSurface = NULL;
    Surface* rightWhiteSurface    = NULL;
    Surface* rightPialSurface     = NULL;
    BrainStructure* rightBrainStructure = brain->getBrainStructure(StructureEnum::CORTEX_RIGHT, 
                                                                              false);
    if (rightBrainStructure != NULL) {
        rightMidThickSurface = rightBrainStructure->getSurfaceContainingTextInName("midthick");
        if (rightMidThickSurface == NULL) {
            rightMidThickSurface = rightBrainStructure->getVolumeInteractionSurface();
        }
        
        rightWhiteSurface    = rightBrainStructure->getSurfaceContainingTextInName("white");
        rightPialSurface     = rightBrainStructure->getSurfaceContainingTextInName("pial");
    }
    
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        this->outlineModels[i]->getColorOrTabModel()->setColor(CaretColorEnum::BLACK);
        this->outlineModels[i]->setThickness(1);
    }
    
    int nextOutlineIndex = 0;
    
    this->addSurfaceOutline(leftMidThickSurface, 
                            5.0, 
                            leftTabIndex, 
                            CaretColorEnum::BLACK, 
                            nextOutlineIndex);
    this->addSurfaceOutline(rightMidThickSurface, 
                            5.0, 
                            rightTabIndex, 
                            CaretColorEnum::BLACK, 
                            nextOutlineIndex);
    
    this->addSurfaceOutline(leftWhiteSurface, 
                            0.5, 
                            -1, 
                            CaretColorEnum::LIME, 
                            nextOutlineIndex);
    this->addSurfaceOutline(rightWhiteSurface, 
                            0.5, 
                            -1, 
                            CaretColorEnum::LIME, 
                            nextOutlineIndex);
    
    this->addSurfaceOutline(leftPialSurface, 
                            0.5, 
                            -1, 
                            CaretColorEnum::BLUE, 
                            nextOutlineIndex);
    this->addSurfaceOutline(rightPialSurface, 
                            0.5, 
                            -1, 
                            CaretColorEnum::BLUE, 
                            nextOutlineIndex);
}

/**
 * Add a surface outline at the given outlineIndex.  The
 * outlineIndex is incremented.
 *
 * @param surface
 *    Surface that is added.  If NULL, no action is taken.
 * @param thickness
 *    Thickness for surface outline.
 * @param browserTabIndex
 *    If greater than or equal to zero, the color source
 *    is set to this tab index.
 * @param color
 *    If browserTabIndex is less than zero, the color source
 *    is set to this color.
 * @param outlineIndex
 *    If an outline was added, it is placed at this value
 *    and it is incremented.  If this index is greater 
 *    than or equal to the number of available surface 
 *    outlines, no action is taken.
 */
void 
VolumeSurfaceOutlineSetModel::addSurfaceOutline(Surface* surface,
                                           const float thickness,
                                           const int32_t browserTabIndex,
                                           const CaretColorEnum::Enum color,
                                           int32_t& outlineIndex)
{
    if (surface != NULL) {
        if (surface->getSurfaceType() == SurfaceTypeEnum::ANATOMICAL) {
            if (outlineIndex < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES) {
                VolumeSurfaceOutlineModel* vsos = this->outlineModels[outlineIndex];
                vsos->getSurfaceSelectionModel()->setSurface(surface);
                vsos->setThickness(thickness);
                if (browserTabIndex >= 0) {
                    vsos->getColorOrTabModel()->setBrowserTabIndex(browserTabIndex);
                }
                else {
                    vsos->getColorOrTabModel()->setColor(color);
                }
                
                outlineIndex++;
            }
        }
    }
}


