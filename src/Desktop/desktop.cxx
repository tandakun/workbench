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
/*LICENSE_END*/


#include <QApplication>
#include <QDesktopWidget>
#include <QGLFormat>
#include <QGLPixelBuffer>
#include <QSplashScreen>
#include <QThread>

#include <cstdlib>
#include <iostream>

#include "BrainBrowserWindow.h"
#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EventBrowserWindowNew.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "ProgramParameters.h"
#include "SessionManager.h"
#include "SplashScreen.h"
#include "SystemUtilities.h"
#include "WuQtUtilities.h"

static bool caretLoggerIsValid = false;

using namespace caret;
using namespace std;

/**
 * Handles message produced by Qt.
 */
static void
messageHandlerForQt(QtMsgType type, const char* msg)
{
    const AString backtrace = SystemUtilities::getBackTrace();
    
    const AString message = (AString(msg) + "\n" + backtrace);
    const char* messageChars = message.toCharArray();
    
    if (caretLoggerIsValid) {
        bool abortFlag = false;
        switch (type) {
            case QtDebugMsg:
                if (CaretLogger::getLogger()->isInfo()) {
                    CaretLogInfo(message);
                }
                else {
                    std::cerr << "Qt Debug: " << messageChars << std::endl;
                }
                break;
            case QtWarningMsg:
                if (CaretLogger::getLogger()->isWarning()) {
                    CaretLogWarning(message);
                }
                else {
                    std::cerr << "Qt Warning: " << messageChars << std::endl;
                }
                break;
            case QtCriticalMsg:
                if (CaretLogger::getLogger()->isSevere()) {
                    CaretLogSevere(message);
                }
                else {
                    std::cerr << "Qt Critical: " << messageChars << std::endl;
                }
                break;
            case QtFatalMsg:
                if (CaretLogger::getLogger()->isSevere()) {
                    CaretLogSevere(message);
                }
                else {
                    std::cerr << "Qt Fatal: " << messageChars << std::endl;
                }
                abortFlag = true;
                break;
        }
        
        /*
         * Beep to alert user about an error!!!
         */
        GuiManager::get()->beep(5);
        
        if (abortFlag) {
            std::abort();
        }
    }
    else {
        switch (type) {
            case QtDebugMsg:
                std::cerr << "Qt Debug: " << messageChars << std::endl;
                break;
            case QtWarningMsg:
                std::cerr << "Qt Warning: " << messageChars << std::endl;
                break;
            case QtCriticalMsg:
                std::cerr << "Qt Critical: " << messageChars << std::endl;
                break;
            case QtFatalMsg:
                std::cerr << "Qt Fatal: " << messageChars << std::endl;
                std::abort();
                break;
        }
    }
    
    delete[] messageChars;
}

//struct for communicating stuff back to main from parseCommandLine
struct ProgramState
{
    vector<AString> fileList;
    int specLoadType;
    int windowSizeXY[2];
    bool showSplash;
    
    ProgramState();
};


/*
// maximum mumber of lines the output console should have

static const WORD MAX_CONSOLE_LINES = 500;
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

void RedirectIOToConsole()
{

    int hConHandle;

    long lStdHandle;

    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    FILE *fp;

    // allocate a console for this app

    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),

    &coninfo);

    coninfo.dwSize.Y = MAX_CONSOLE_LINES;

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),

    coninfo.dwSize);

    // redirect unbuffered STDOUT to the console

    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen( hConHandle, "w" );

    *stdout = *fp;

    setvbuf( stdout, NULL, _IONBF, 0 );

    // redirect unbuffered STDIN to the console

    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);

    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen( hConHandle, "r" );

    *stdin = *fp;

    setvbuf( stdin, NULL, _IONBF, 0 );

    // redirect unbuffered STDERR to the console

    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);

    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

    fp = _fdopen( hConHandle, "w" );

    *stderr = *fp;

    setvbuf( stderr, NULL, _IONBF, 0 );

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog

    // point to console as well

    ios::sync_with_stdio();

}*/
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

//declare the functions associated with command line
void printHelp(const AString& progName);
void parseCommandLine(const AString& progName, ProgramParameters* myParams, ProgramState& myState);
int 
main(int argc, char* argv[])
{
    //MS Windows code to allocate a new console, will have a preference to set this up
    //RedirectIOToConsole();
    int result;
    {
        /*
        * Handle uncaught exceptions
        */
        SystemUtilities::setHandlersForUnexpected();
        
        qInstallMsgHandler(messageHandlerForQt);
        //change the default graphics system on mac to avoid rendering performance issues with qwtplotter
#ifdef CARET_OS_MACOSX
        QApplication::setGraphicsSystem("raster");
#endif //CARET_OS_MACOSX        
        QApplication app(argc, argv);
        QApplication::addLibraryPath(
            QApplication::applicationDirPath()
            + QDir::separator()
            + "plugins");
        QApplication::setApplicationName("Connectome Workbench");
        QApplication::setApplicationVersion("0");
        QApplication::setOrganizationDomain("brainvis.wustl.edu");
        QApplication::setOrganizationName("Van Essen Lab");
        /*
         * Override the system local to US - English
         */
        QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
        
        /*
        * Make sure OpenGL is available.
        */
        if (!QGLFormat::hasOpenGL()) { 
            qWarning( "This system has no OpenGL support. Exiting." );
            return -1;
        }
            
        /*
        * Setup OpenGL
        */
        QGLFormat glfmt;
        glfmt.setRedBufferSize(8);
        glfmt.setGreenBufferSize(8);
        glfmt.setBlueBufferSize(8);
        glfmt.setDoubleBuffer(true);
        glfmt.setDirectRendering(true);
        QGLFormat::setDefaultFormat(glfmt);
        
        
        /*
        * Create the session manager.
        */
        SessionManager::createSessionManager();
        caretLoggerIsValid = true;

#ifdef NDEBUG
        CaretLogConfig("Compiled with debugging OFF");
#else
        CaretLogConfig("Compiled with debugging ON");
#endif // NDEBUG
        
        /*
        * Parameters for the program.
        */
        ProgramParameters* parameters = new ProgramParameters(argc, argv);

        /*
        * Log the command parameters.
        */
        CaretLogFine("Running: "
                    + AString(argv[0])
                    + " "
                    + parameters->getAllParametersInString());
        
        //begin parsing command line
        ProgramState myState;
        FileInformation progInfo(argv[0]);
        AString progName = progInfo.getFileName();
        parseCommandLine(progName, parameters, myState);
        //sanity check command line
        bool haveSpec = false;
        bool haveFiles = false;
        for (int i = 0; i < (int)myState.fileList.size(); ++i)
        {
            if (myState.fileList[i].endsWith(".spec"))
            {
                if (haveSpec)
                {
                    cerr << "error, cannot load multiple spec files at this time" << endl;
                    return -1;
                }
                haveSpec = true;
            } else {
                haveFiles = true;
            }
        }
        //if error to have both data and spec files
        /*if (haveFiles && haveSpec)
        {
            cerr << "error, cannot specify both spec files and data files on the command line" << endl;
            return -1;
        }//*/
        
        /*
         * Enabled the desired splash screen based upon user preferences
         * and command line options.  Do not show selection splash screen
         * if there has listed files on the command line.
         */ 
        CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
        bool showSelectionSplashScreen = preferences->isSplashScreenEnabled();
        if (myState.fileList.empty() == false) {
            showSelectionSplashScreen = false;
        }
        bool showImageSplashScreen = (! showSelectionSplashScreen);
        if (myState.showSplash == false) {
            showSelectionSplashScreen = false;
            showImageSplashScreen = false;
        }
        
        /*
        * Splash Screen
        */
        QPixmap splashPixmap;
        QSplashScreen splashScreen;
        if (showImageSplashScreen) {
            if (WuQtUtilities::loadPixmap(":/splash_hcp.png", splashPixmap)) {
                splashScreen.setPixmap(splashPixmap);
                splashScreen.showMessage("Starting Workbench...");
                splashScreen.show();
                app.processEvents();
                SystemUtilities::sleepSeconds(2);
            }
        }
        
        /*
         * Create the GUI Manager.
         */
        GuiManager::createGuiManager();
        
        /*
         * Show file selection splash screen if enabled via user's preferences
         */
        if (showSelectionSplashScreen) {
            /*
             * Show selection splash screen.
             * Need to process events since QApplication::exec() has not
             * been called.
             */
            SplashScreen splashScreen(NULL);
            app.processEvents();
            if (splashScreen.exec()) {
                const QString specFileName = splashScreen.getSelectedSpecFileName();
                if (specFileName.isEmpty() == false) {
                    myState.fileList.clear();
                    myState.fileList.push_back(specFileName);
                    myState.specLoadType = 0; // which means use BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE;
                    haveSpec = true;
                    haveFiles = false;
                }
            }
        }
        
        /*
        * Create and display a main window.
        * If not done as pointer, the event object is listed as an
        * object that was not deleted by CaretObject::printListOfObjectsNotDeleted
        * since it does not go out of scope.
        */
        
        EventBrowserWindowNew newBrowserWindow(NULL, NULL);
        EventManager::get()->sendEvent(newBrowserWindow.getPointer());
        splashScreen.close();

        BrainBrowserWindow* myWindow = GuiManager::get()->getBrowserWindowByWindowIndex(0);
        
        if ((myState.windowSizeXY[0] > 0) 
            && (myState.windowSizeXY[1] > 0)) {
            myWindow->setFixedSize(myState.windowSizeXY[0],
                                   myState.windowSizeXY[1]);
        }
        
        //use command line
        if (haveFiles)
        {
            myWindow->loadFilesFromCommandLine(myState.fileList, BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG);//second parameter unused in this case
        }
        if (haveSpec)
        {
            switch (myState.specLoadType)
            {
                case 0://dialog
                    myWindow->loadFilesFromCommandLine(myState.fileList, BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE);
                    break;
                case 1://load all
                    myWindow->loadFilesFromCommandLine(myState.fileList, BrainBrowserWindow::LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE);
                    break;
                default:
                    CaretAssert(false);
            }
        }
        
        if (QGLPixelBuffer::hasOpenGLPbuffers()) {
            CaretLogConfig("OpenGL PBuffers are supported");
        }
        else {
            CaretLogConfig("OpenGL PBuffers are NOT supported");
        }
        
        /*
         * Log local (language, country)
         */
        QLocale sytemLocale = QLocale::system();
        CaretLogConfig("Local Language="
                       + QLocale::languageToString(sytemLocale.language())
                       + " Country="
                       + QLocale::countryToString(sytemLocale.country()));

        /*
         * Resolution of screens
         */
        AString screenSizeText = "Screen Sizes: ";
        QDesktopWidget* dw = QApplication::desktop();
        const int numScreens = dw->screenCount();
        for (int i = 0; i < numScreens; i++) {
            const QRect rect = dw->screenGeometry(i);
            const int w = rect.width();
            const int h = rect.height();
            screenSizeText += ("("
                               + AString::number(w)
                               + ", "
                               + AString::number(h)
                               + ")  ");
        }
        CaretLogConfig(screenSizeText);
        
        /*
        * Start the app which will launch the main window.
        */
        result = app.exec();
        
        /*
        * Hiding the window removes it from the event loop on Windows, which is necessary to
        * prevent paint events from causing assertion errors when the Window is destroyed
        * Although this is a Window's only bug, it's probably good practice to do on all platforms
        */
        //theMainWindow->hide();
        
        /*
        * Delete the GUI Manager.
        */
        GuiManager::deleteGuiManager();
        
        /*
        * Delete the session manager.
        */
        SessionManager::deleteSessionManager();
        
        CaretHttpManager::deleteHttpManager();
        
        delete parameters;
    }
    /*
     * See if any objects were not deleted.
     */
    CaretObject::printListOfObjectsNotDeleted(true);
    //FreeConsole();
    
    return result;
}

void printHelp(const AString& progName)
{
    cout
    << "Usage: " << progName << " [options] [files]" << endl
    << endl
    << "    [files], if present, can be a single spec file, or multiple data files" << endl
    << endl
    << "Options:" << endl
    << "    -help" << endl
    << "        display this usage text" << endl
    << endl
    << "    -no-splash" << endl
    << "        disable all splash screens" << endl
    << endl
    << "    -style <style-name>" << endl
    << "        change the window style to the specified style" << endl
    << "        the following styles should always be valid:" << endl
    << "        motif" << endl
    << "        windows" << endl
    << "        platinum" << endl
    << endl
    << "        other styles may be available" << endl
    << endl
    << "    -spec-load-all" << endl
    << "        load all files in the given spec file, don't show spec file dialog" << endl
    << endl
    << "    -window-size  <X Y>" << endl
    << "        Set the size of the browser window" << endl
    << endl;
}

void parseCommandLine(const AString& progName, ProgramParameters* myParams, ProgramState& myState)
{
    while (myParams->hasNext())
    {
        AString thisParam = myParams->nextString("option");
        if (thisParam[0] == '-')
        {
            if (thisParam == "-style")
            {
                myParams->nextString("style");//discard, QApplication handles this
            } else if (thisParam == "-help") {
                printHelp(progName);
                exit(0);
            } else if (thisParam == "-no-splash") {
                myState.showSplash = false;
            } else if (thisParam == "-spec-load-all") {
                myState.specLoadType = 1;
            } else if (thisParam == "-window-size") {
                if (myParams->hasNext()) {
                    myState.windowSizeXY[0] = myParams->nextInt("Window Size X");
                }
                else {
                    cerr << "Missing X & Y sizes for window" << endl;
                }
                if (myParams->hasNext()) {
                    myState.windowSizeXY[1] = myParams->nextInt("Window Size Y");
                }
                else {
                    cerr << "Missing Y sizes for window" << endl;
                }
            } else {
                cerr << "unrecognized option \"" << thisParam << "\"" << endl;
                printHelp(progName);
                exit(-1);
            }
        } else {
            myState.fileList.push_back(thisParam);
        }
    }
}

ProgramState::ProgramState()
{
    specLoadType = 0;//0: use spec window, 1: all
    windowSizeXY[0] = -1;
    windowSizeXY[1] = -1;
    showSplash = true;
}
