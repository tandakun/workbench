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

#include "FileInformation.h"
#include "OperationZipSpecFile.h"
#include "OperationException.h"
#include "SpecFile.h"

#include "quazip.h"
#include "quazipfile.h"

#include <vector>

using namespace caret;
using namespace std;

AString OperationZipSpecFile::getCommandSwitch()
{
    return "-zip-spec-file";
}

AString OperationZipSpecFile::getShortDescription()
{
    return "ZIP A SPEC FILE AND ITS DATA FILES";
}

OperationParameters* OperationZipSpecFile::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "Zip File", "the zip file that will be created");
    ret->addStringParameter(2, "Spec File", "the specification file to add to zip file");
    ret->addStringParameter(3, "Output Sub-Directory", "the sub-directory created when the zip file is unzipped");
    AString myText = ("The ZIP file must not exist.  If it does, delete it prior to "
                      "running this command.");
    ret->setHelpText(myText);
    return ret;
}

void OperationZipSpecFile::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString zipFileName = myParams->getString(1);
    AString specFileName = myParams->getString(2);
    AString outputSubDirectory = myParams->getString(3);

    if (outputSubDirectory.isEmpty()) {
        throw OperationException("Output Sub-Directory must contain characters");
    }
    
    /*
     * Verify that ZIP file DOES NOT exist
     */
    FileInformation zipFileInfo(zipFileName);
    if (zipFileInfo.exists()) {
        throw OperationException("ZIP file \""
                                 + zipFileName
                                 + "\" exists and this command will not overwrite the ZIP file.");
    }
    
    /*
     * Read the spec file and get the names of its data files.
     * Look for any files that are missing (name in spec file
     * but file not found).
     */
    SpecFile specFile;
    specFile.readFile(specFileName);
    std::vector<AString> allDataFileNames = specFile.getAllDataFileNames();
    allDataFileNames.push_back(specFileName);
    
    /*
     * Verify that all data files exist
     */
    AString missingDataFileNames;
    const int32_t numberOfDataFiles = static_cast<int32_t>(allDataFileNames.size());
    for (int32_t i = 0; i < numberOfDataFiles; i++) {
        FileInformation dataFileInfo(allDataFileNames[i]);
        if (dataFileInfo.exists() == false) {
            if (missingDataFileNames.isEmpty()) {
                missingDataFileNames = "These data file(s) are missing and ZIP file has not been created:\n";
            }
            missingDataFileNames += ("    "
                                     + allDataFileNames[i]);
        }
    }
    if (missingDataFileNames.isEmpty() == false) {
        throw OperationException(missingDataFileNames);
    }
    
    /*
     * Create the ZIP file
     */
    QFile zipFileObject(zipFileName);
    QuaZip zipFile(&zipFileObject);
    if (zipFile.open(QuaZip::mdCreate) == false) {
        throw OperationException("Unable to open ZIP File \""
                                 + zipFileName
                                 + "\" for writing.");
    }
    
    /*
     * Compress each of the files and add them to the zip file
     */
    AString errorMessage;
    for (int32_t i = 0; i < numberOfDataFiles; i++) {
        const AString dataFileName = allDataFileNames[i];
        
        QFile dataFileIn(dataFileName);
        if (dataFileIn.open(QFile::ReadOnly) == false) {
            errorMessage = ("Unable to open \""
                                   + dataFileName
                                   + "\" for reading: "
                                   + dataFileIn.errorString());
            break;
        }
        
        const AString unzippedDataFileName = (outputSubDirectory
                                              + "/"
                                              + dataFileName);
        
        QuaZipNewInfo zipNewInfo(unzippedDataFileName,
                                 dataFileName);
        //zipNewInfo.externalAttr = 0xffff;
        
        QuaZipFile dataFileOut(&zipFile);
        if (dataFileOut.open(QIODevice::WriteOnly,
                             zipNewInfo) == false) {
            errorMessage = ("Unable to open zip output for \""
                            + dataFileName
                            + "\"");
            break;
        }
        
        const qint64 BUFFER_SIZE = 1024 * 10;
        char buffer[BUFFER_SIZE];
        
        while (dataFileIn.QIODevice::atEnd() == false) {
            const qint64 numRead = dataFileIn.read(buffer, BUFFER_SIZE);
            if (numRead > 0) {
                dataFileOut.write(buffer, numRead);
            }
        }
        
        QuaZipFileInfo zipOutFileInfo;
        dataFileOut.getFileInfo(&zipOutFileInfo);
        zipOutFileInfo.externalAttr = 0xffff;
        
        dataFileIn.close();
        dataFileOut.close();
    }
    
    /*
     * Close the zip file
     */
    zipFile.close();
    
    /*
     * If there are errors, remove the ZIP file and
     * indicate an error has occurred.
     */
    if (errorMessage.isEmpty() == false) {
        QFile::remove(zipFileName);
        
        throw OperationException(errorMessage);
    }
}