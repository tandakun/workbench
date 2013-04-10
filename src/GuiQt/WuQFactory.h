#ifndef __WU_Q_FACTORY_H__
#define __WU_Q_FACTORY_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

class QDoubleSpinBox;
class QObject;
class QSpinBox;


namespace caret {

    class WuQFactory {
        
    public:
        static QSpinBox* newSpinBox();
        
        static QSpinBox* newSpinBoxWithSignalInt(QObject* receiver,
                                                 const char* method);
        
        static QSpinBox* newSpinBoxWithMinMaxStep(const int minimumValue,
                                                  const int maximumValue,
                                                  const int stepSize);
        
        static QSpinBox* newSpinBoxWithMinMaxStepSignalInt(const int minimumValue,
                                                           const int maximumValue,
                                                           const int stepSize,
                                                           QObject* receiver,
                                                           const char* method);
        
        static QDoubleSpinBox* newDoubleSpinBox();
        
        static QDoubleSpinBox* newDoubleSpinBoxWithSignalDouble(QObject* receiver,
                                                                const char* method);
        
        static QDoubleSpinBox* newDoubleSpinBoxWithMinMaxStepDecimals(const double minimumValue,
                                                                      const double maximumValue,
                                                                      const double stepSize,
                                                                      const int digitsRightOfDecimal);
        
        static QDoubleSpinBox* newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(const double minimumValue,
                                                                               const double maximumValue,
                                                                               const double stepSize,
                                                                               const int digitsRightOfDecimal,
                                                                               QObject* receiver,
                                                                               const char* method);
        
    private:
        WuQFactory();
        
        virtual ~WuQFactory();
        
        WuQFactory(const WuQFactory&);

        WuQFactory& operator=(const WuQFactory&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_FACTORY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_FACTORY_DECLARE__

} // namespace
#endif  //__WU_Q_FACTORY_H__
