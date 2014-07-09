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

#include "AlgorithmBorderToVertices.h"
#include "AlgorithmException.h"
#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"

#include <set>
#include <utility>

using namespace caret;
using namespace std;

AString AlgorithmBorderToVertices::getCommandSwitch()
{
    return "-border-to-vertices";
}

AString AlgorithmBorderToVertices::getShortDescription()
{
    return "DRAW BORDER AS VERTICES IN METRIC FILE";
}

OperationParameters* AlgorithmBorderToVertices::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface the borders are drawn on");
    
    ret->addBorderParameter(2, "border-file", "the border file");
    
    ret->addMetricOutputParameter(3, "metric-out", "the output metric file");
    
    OptionalParameter* borderOpt = ret->createOptionalParameter(4, "-border", "create ROI for only one border");
    borderOpt->addStringParameter(1, "name", "the name of the border");
    
    ret->setHelpText(
        AString("This is where you set the help text.  ") +
        "DO NOT add the info about what the command line format is, and do not give the command switch, " +
        "short description, or the short descriptions of parameters.  " +
        "Do not indent, manually break long lines, or format the text in any way " +
        "other than to separate paragraphs within the help text prose, usually with two newlines."
    );
    return ret;
}

void AlgorithmBorderToVertices::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    BorderFile* myBorderFile = myParams->getBorder(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    bool borderSelectMode = false;
    AString borderName;
    OptionalParameter* borderOpt = myParams->getOptionalParameter(4);
    if (borderOpt->m_present)
    {
        borderName = borderOpt->getString(1);
        borderSelectMode = true;
    }
    if (borderSelectMode)
    {
        AlgorithmBorderToVertices(myProgObj, mySurf, myBorderFile, myMetricOut, borderName);
    } else {
        AlgorithmBorderToVertices(myProgObj, mySurf, myBorderFile, myMetricOut);
    }
}

AlgorithmBorderToVertices::AlgorithmBorderToVertices(ProgressObject* myProgObj, const SurfaceFile* mySurf, const BorderFile* myBorderFile, MetricFile* myMetricOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    //TODO: check structure against surface
    set<pair<AString, AString> > borderCounter;//"multi-part border" behavior
    int numBorderParts = myBorderFile->getNumberOfBorders();//total number of parts
    for (int i = 0; i < numBorderParts; ++i)
    {
        const Border* thisBorderPart = myBorderFile->getBorder(i);
        borderCounter.insert(make_pair(thisBorderPart->getName(), thisBorderPart->getClassName()));
    }
    int numBorders = (int)borderCounter.size();
    myMetricOut->setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), numBorders);
    myMetricOut->setStructure(mySurf->getStructure());
    CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
    vector<bool> used(numBorderParts, false);//tracking for "multi-part border" behavior
    int curBorder = 0;
    for (int i = 0; i < numBorderParts; ++i)//visit borders in file order (of first piece), not set sorted order
    {
        if (used[i]) continue;//skip previously used parts
        vector<float> colScratch(mySurf->getNumberOfNodes(), 0.0f);
        const Border* refBorderPart = myBorderFile->getBorder(i);
        AString borderName = refBorderPart->getName(), className = refBorderPart->getClassName();
        myMetricOut->setColumnName(curBorder, borderName);
        for (int j = i; j < numBorderParts; ++j)//we could store the matching info in the counting set instead
        {
            const Border* thisBorderPart = myBorderFile->getBorder(j);
            if (thisBorderPart->getName() == borderName && thisBorderPart->getClassName() == className)
            {
                used[j] = true;//now we can actually do stuff with the border part
                int numBorderPoints = thisBorderPart->getNumberOfPoints();
                if (numBorderPoints < 1) continue;
                const SurfaceProjectionBarycentric* thisBary = thisBorderPart->getPoint(0)->getBarycentricProjection();
                CaretAssert(thisBary->isValid());
                const float* thisWeights = thisBary->getTriangleAreas();
                int useNode = 0;
                for (int n = 1; n < 3; ++n) if (thisWeights[n] > thisWeights[useNode]) useNode = n;//get closest node to point
                int curNode = thisBary->getTriangleNodes()[useNode];
                colScratch[curNode] = 1.0f;//if there is only one border point, make sure it leaves a mark
                for (int k = 1; k < numBorderPoints; ++k)
                {
                    thisBary = thisBorderPart->getPoint(k)->getBarycentricProjection();
                    CaretAssert(thisBary->isValid());
                    thisWeights = thisBary->getTriangleAreas();
                    useNode = 0;
                    for (int n = 1; n < 3; ++n) if (thisWeights[n] > thisWeights[useNode]) useNode = n;
                    int nextNode = thisBary->getTriangleNodes()[useNode];
                    vector<int32_t> pathNodes;
                    vector<float> pathDists;//not used, but mandatory output
                    myGeoHelp->getPathToNode(curNode, nextNode, pathNodes, pathDists);
                    int pathLength = (int)pathNodes.size();
                    for (int m = 0; m < pathLength; ++m)
                    {
                        colScratch[pathNodes[m]] = 1.0f;
                    }
                    curNode = nextNode;
                }
            }
        }
        myMetricOut->setValuesForColumn(curBorder, colScratch.data());
        ++curBorder;
    }
}

AlgorithmBorderToVertices::AlgorithmBorderToVertices(ProgressObject* myProgObj, const SurfaceFile* mySurf, const BorderFile* myBorderFile, MetricFile* myMetricOut, const AString& borderName) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    //TODO: check structure against surface
    bool first = true;
    AString className;
    int numBorderParts = myBorderFile->getNumberOfBorders();//total number of parts
    myMetricOut->setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
    myMetricOut->setStructure(mySurf->getStructure());
    myMetricOut->setColumnName(0, borderName);
    CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
    vector<float> colScratch(mySurf->getNumberOfNodes(), 0.0f);
    for (int i = 0; i < numBorderParts; ++i)
    {
        const Border* thisBorderPart = myBorderFile->getBorder(i);
        if (thisBorderPart->getName() == borderName)
        {
            if (first)
            {
                className = thisBorderPart->getClassName();
            } else {
                if (thisBorderPart->getClassName() != className) throw AlgorithmException("border name matches borders with different classes");
            }
        }
        int numBorderPoints = thisBorderPart->getNumberOfPoints();
        if (numBorderPoints < 1) continue;
        const SurfaceProjectionBarycentric* thisBary = thisBorderPart->getPoint(0)->getBarycentricProjection();
        CaretAssert(thisBary->isValid());
        const float* thisWeights = thisBary->getTriangleAreas();
        int useNode = 0;
        for (int n = 1; n < 3; ++n) if (thisWeights[n] > thisWeights[useNode]) useNode = n;//get closest node to point
        int curNode = thisBary->getTriangleNodes()[useNode];
        colScratch[curNode] = 1.0f;//if there is only one border point, make sure it leaves a mark
        for (int k = 1; k < numBorderPoints; ++k)
        {
            thisBary = thisBorderPart->getPoint(k)->getBarycentricProjection();
            CaretAssert(thisBary->isValid());
            thisWeights = thisBary->getTriangleAreas();
            useNode = 0;
            for (int n = 1; n < 3; ++n) if (thisWeights[n] > thisWeights[useNode]) useNode = n;
            int nextNode = thisBary->getTriangleNodes()[useNode];
            vector<int32_t> pathNodes;
            vector<float> pathDists;//not used, but mandatory output
            myGeoHelp->getPathToNode(curNode, nextNode, pathNodes, pathDists);
            int pathLength = (int)pathNodes.size();
            for (int m = 0; m < pathLength; ++m)
            {
                colScratch[pathNodes[m]] = 1.0f;
            }
            curNode = nextNode;
        }
    }
    myMetricOut->setValuesForColumn(0, colScratch.data());
}

float AlgorithmBorderToVertices::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmBorderToVertices::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}