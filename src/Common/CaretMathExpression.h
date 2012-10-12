#ifndef __CARET_MATH_EXPRESSION_H__
#define __CARET_MATH_EXPRESSION_H__

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
/*LICENSE_END*/

#include "AString.h"
#include "MathFunctionEnum.h"
#include <vector>

namespace caret {

class CaretMathExpression
{
    struct MathNode
    {
        enum ExprType
        {
            INVALID,
            ADDSUB,
            MULTDIV,
            POW,
            FUNC,
            VAR,
            CONST
        };
        ExprType m_type;
        MathFunctionEnum::Enum m_function;
        double m_constVal;
        int m_varIndex;
        bool m_negate;
        std::vector<bool> m_invert;//whether it is subtract rather than add, or divide instead of multiply
        std::vector<MathNode> m_arguments;
        MathNode();
        double eval(const std::vector<float>& values) const;
        AString toString(const std::vector<AString>& varNames) const;
    };
    void parse(MathNode& node, const AString& input, const int& start, const int& end);//will throw, there is no syntax ambiguity that would need to be resolved by a failed call to this
    bool tryAddSub(MathNode& node, const AString& input, const int& start, const int& end);
    bool tryMultDiv(MathNode& node, const AString& input, const int& start, const int& end);
    bool tryUnaryMinus(MathNode& node, const AString& input, const int& start, const int& end);
    bool tryPow(MathNode& node, const AString& input, const int& start, const int& end);
    bool tryParen(MathNode& node, const AString& input, const int& start, const int& end);
    bool tryFunc(MathNode& node, const AString& input, const int& start, const int& end);
    bool tryVar(MathNode& node, const AString& input, const int& start, const int& end);
    bool tryConst(MathNode& node, const AString& input, const int& start, const int& end);
    MathNode m_root;
    std::vector<AString> m_varNames;
    CaretMathExpression();
public:
    static AString getExpressionHelpInfo();
    CaretMathExpression(const AString& expression);
    double evaluate(const std::vector<float>& variableValues) const;
    const std::vector<AString>& getVarNames() const { return m_varNames; }
    AString toString() const;//the expression, with a lot of parentheses added
};

} // namespace

#endif //__CARET_MATH_EXPRESSION_H__
