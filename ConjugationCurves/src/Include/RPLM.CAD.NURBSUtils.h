#pragma once

#include <RGPSession.h>

namespace RPLM::CAD
{
    // Пространство с функциями для взаимодействия с NURBS
    namespace NURBSUtils
    {
        // Равномерно заполняет узловой вектор кривой
        Math::Geometry2D::Geometry::DoubleArray fillEvenlyNodalVector(int iDegree, int iNumVertices);

        /// <summary>Находит номер-интервал (спан) для заданного curveParameter</summary>
        /// <param name="iNodalVector">Узловой вектор</param>
        /// <param name="iDegree">Степень</param>
        /// <param name="iCurveParameter">Параметр кривой</param>
        /// <returns></returns>
        int findSpanForParameter(const std::vector<double>& iNodalVector, int iDegree, double iCurveParameter);

        /// <summary>Вычисляет нулевые базисные функции</summary>
        /// <param name="iBasisFuncs">Вектор базисных функций</param>
        /// <param name="iTempStorage">Временный вектор для сбора значений базисных функций</param>
        /// <param name="iNodalVector">Узловой вектор</param>
        /// <param name="iDegree">Степень</param>
        /// <param name="iCurveParameter">Параметр кривой</param>
        void calculateZeroBasisFuncs(RGK::Vector<RGK::Vector<double>>& iBasisFuncs, RGK::Vector<RGK::Vector<double>>& iTempStorage, RPLM::Math::Geometry2D::Geometry::DoubleArray& iNodalVector, int iDegree, double iCurveParameter);

        /// <summary>Вычисляет производные базисных функций</summary>
        /// <param name="iBasisFuncs">Базисные функции</param>
        /// <param name="iTempStorage">Временный вектор для сбора значений базисных функций</param>
        /// <param name="iDegree">Степерь</param>
        void calcDerivsBasisFuncs(RGK::Vector<RGK::Vector<double>>& iBasisFuncs, RGK::Vector<RGK::Vector<double>>& iTempStorage, int iDegree);

        /// <summary>Вычисляет базисные функции и их производные кривой</summary>
        /// <param name="iCurve">Оригинальная кривая</param>
        /// <param name="iCurveParameter">Параметра кривой, в которой будут рассчитываться базисные функции</param>
        /// <returns>Вектор базисных функций, где нулевая строка - нулевые производные,
        /// первая строка - первые производные и т.д.</returns>
        RGK::Vector<RGK::Vector<double>> calculateBasisFuncs(const RGK::NURBSCurve& iCurve, double iCurveParameter);
    }
}
