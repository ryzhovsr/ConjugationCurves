#include "RPLM.CAD.ConjugationMethod.h"
#include "RPLM.CAD.IMatrixOperations.h"
#include "RPLM.Math.ConstraintSolver.EquationSolver/EquationsMatrix.h"
#include <RGPSession.h>

namespace RPLM::CAD::ConjugationCurves
{
    // Пространство с функциями для взаимодействия с NURBS
    namespace ImplNURBS
    {
        // Равномерно заполняет узловой вектор кривой
        static Math::Geometry2D::Geometry::DoubleArray fillEvenlyNodalVector(int iDegree, int iNumVertices)
        {
            // Кол-во узлов (длина) реальной части узлового вектора
            int _numRealRangeKnots = iNumVertices - iDegree + 1;
            // Кол-во узлов в узл. векторе
            int numKnots = iNumVertices + iDegree + 1;

            // Начало/конец реального диапазона узл. вектора
            int realRangeStart = iDegree;
            int realRangeEnd = numKnots - iDegree - 1;

            RPLM::Math::Geometry2D::Geometry::DoubleArray knots(numKnots);

            // Шаг в реальном диапазоне
            double step = 1 / static_cast<double>(_numRealRangeKnots - 1);

            // Заполняем реальный диапазон
            for (int i = realRangeStart + 1; i < realRangeEnd; ++i)
                knots[i] = knots[i - 1] + step;

            // Заполняем последние параметры единицами
            for (size_t i = realRangeEnd; i < knots.size(); ++i)
                knots[i] = 1;

            return knots;
        }

        /// <summary>Находит номер-интервал (спан) для заданного curveParameter</summary>
        /// <param name="iNodalVector">Узловой вектор</param>
        /// <param name="iDegree">Степень</param>
        /// <param name="iCurveParameter">Параметр кривой</param>
        /// <returns></returns>
        int findSpanForParameter(const std::vector<double>& iNodalVector, int iDegree, double iCurveParameter)
        {
            size_t numKnots = iNodalVector.size();

            if (iCurveParameter < iNodalVector[iDegree] || iCurveParameter > iNodalVector[numKnots - iDegree - 1])
            {
                throw _STR("Error! _findSpanForParameter: параметр вышел за реальный диапазон!");
            }

            if (iCurveParameter == iNodalVector[numKnots - iDegree - 1])
            {
                return static_cast<int>(numKnots) - iDegree - 2;
            }

            int low = iDegree;
            int high = static_cast<int>(numKnots) - iDegree - 1;
            int middle = (low + high) / 2;

            while ((iCurveParameter < iNodalVector[middle]) || (iCurveParameter >= iNodalVector[middle + 1]))
            {
                if (iCurveParameter < iNodalVector[middle])
                {
                    high = middle;
                }
                else
                {
                    low = middle;
                }

                middle = (low + high) / 2;
            }

            return middle;
        }

        /// <summary>Вычисляет нулевые базисные функции</summary>
        /// <param name="iBasisFuncs">Вектор базисных функций</param>
        /// <param name="iTempStorage">Временный вектор для сбора значений базисных функций</param>
        /// <param name="iNodalVector">Узловой вектор</param>
        /// <param name="iDegree">Степень</param>
        /// <param name="iCurveParameter">Параметр кривой</param>
        void calculateZeroBasisFuncs(RGK::Vector<RGK::Vector<double>>& iBasisFuncs, RGK::Vector<RGK::Vector<double>>& iTempStorage, RPLM::Math::Geometry2D::Geometry::DoubleArray& iNodalVector, int iDegree, double iCurveParameter)
        {
            int span = ImplNURBS::findSpanForParameter(iNodalVector, iDegree, iCurveParameter);

            RGK::Vector<double> left(iDegree + 1), right(iDegree + 1);
            iTempStorage[0][0] = 1;

            for (int i = 1; i < iDegree + 1; ++i)
            {
                left[i] = iCurveParameter - iNodalVector[span + 1 - i];
                right[i] = iNodalVector[span + i] - iCurveParameter;
                double saved = 0;

                for (int j = 0; j < i; ++j)
                {
                    // Нижний треугольник
                    iTempStorage[i][j] = right[j + 1] + left[i - j];
                    double temp = iTempStorage[j][i - 1] / iTempStorage[i][j];
                    // Верхний треугольник
                    iTempStorage[j][i] = saved + right[j + 1] * temp;
                    saved = left[i - j] * temp;
                }

                iTempStorage[i][i] = saved;
            }

            for (int i = 0; i <= iDegree; ++i)
            {
                iBasisFuncs[0][i] = iTempStorage[i][iDegree];
            }
        }

        /// <summary>Вычисляет производные базисных функций</summary>
        /// <param name="iBasisFuncs">Базисные функции</param>
        /// <param name="iTempStorage">Временный вектор для сбора значений базисных функций</param>
        /// <param name="iDegree">Степерь</param>
        void calcDerivsBasisFuncs(RGK::Vector<RGK::Vector<double>>& iBasisFuncs, RGK::Vector<RGK::Vector<double>>& iTempStorage, int iDegree)
        {
            // Хранит два вычесленных ряда
            RGK::Vector<RGK::Vector<double>> rows(2, RGK::Vector<double>(iDegree + 1));

            for (int i = 0; i < iDegree + 1; ++i)
            {
                int s1 = 0, s2 = 1;
                rows[0][0] = 1;

                for (int j = 1; j <= iDegree; ++j)
                {
                    double d = 0;
                    double rk = i - j;
                    double pk = iDegree - j;

                    if (i >= j)
                    {
                        rows[s2][0] = rows[s1][0] / iTempStorage[pk + 1][rk];
                        d = rows[s2][0] * iTempStorage[rk][pk];
                    }

                    double j1 = 0, j2 = 0;

                    if (rk >= -1)
                    {
                        j1 = 1;
                    }
                    else
                    {
                        j1 = -rk;
                    }

                    if (i - 1 <= pk)
                    {
                        j2 = j - 1;
                    }
                    else
                    {
                        j2 = iDegree - i;
                    }

                    for (int j = j1; j <= j2; ++j)
                    {
                        rows[s2][j] = (rows[s1][j] - rows[s1][j - 1]) / iTempStorage[pk + 1][rk + j];
                        d += rows[s2][j] * iTempStorage[rk + j][pk];
                    }

                    if (i <= pk)
                    {
                        rows[s2][j] = -rows[s1][j - 1] / iTempStorage[pk + 1][i];
                        d += rows[s2][j] * iTempStorage[i][pk];
                    }

                    iBasisFuncs[j][i] = d;
                    int temp = s1;
                    s1 = s2;
                    s2 = temp;
                }
            }

            double k = iDegree;

            // Умножаем на коэффициенты
            for (int i = 1; i <= iDegree; ++i)
            {
                for (int j = 0; j < iDegree + 1; ++j)
                {
                    iBasisFuncs[i][j] *= k;
                }

                k *= iDegree - i;
            }
        }

        /// <summary>Вычисляет базисные функции и их производные кривой</summary>
        /// <param name="iCurve">Оригинальная кривая</param>
        /// <param name="iCurveParameter">Параметра кривой, в которой будут рассчитываться базисные функции</param>
        /// <returns>Вектор базисных функций, где нулевая строка - нулевые производные,
        /// первая строка - первые производные и т.д.</returns>
        RGK::Vector<RGK::Vector<double>> calculateBasisFuncs(const RGK::NURBSCurve& iCurve, double iCurveParameter)
        {
            int degree = iCurve.GetDegree();

            // Для хранения базисных функций и их производных (нулевая строка - нулевые производные, первая строка - первые произв. и т.д.)
            RGK::Vector<RGK::Vector<double>> basisFuncs(degree + 1, RGK::Vector<double>(degree + 1));
            // Для хранения базисных функций и узлов различия
            RGK::Vector<RGK::Vector<double>> tempStorage(degree + 1, RGK::Vector<double>(degree + 1));

            // Вычисляем нулевые базисные функции
            ImplNURBS::calculateZeroBasisFuncs(basisFuncs, tempStorage, iCurve.GetKnots(), degree, iCurveParameter);
            // Вычисляем все производные базисных функций
            ImplNURBS::calcDerivsBasisFuncs(basisFuncs, tempStorage, degree);

            double sum = 0;

            // Для контроля суммируем значения базисных функций в точке
            for (int i = 0; i < degree + 1; ++i)
            {
                sum += basisFuncs[0][i];
            }

            // Сумма базисных функций должна = 1
            if (sum < (1 - 1e-10) || (sum > 1 + 1e-10))
            {
                throw _STR("Error! _calculateBasisFuncs: cумма базисных функций не равна 1!");
            }

            return basisFuncs;
        }
    }

    RGK::NURBSCurve ConjugationMethod::conjugateCurve(const RGK::NURBSCurve& iCurve, bool iFixBeginningCurve = false, bool iFixEndCurve = false)
    {
        // Разбиваем NURBS кривую на кривые Безье
        RGK::Vector<RGK::NURBSCurve> bezierCurves = _divideCurveIntoBezierCurves(iCurve);

        // Вычисляем базисные функции и их производные оригинальной кривой в параметре 1
        double curveParameter = 1;
        RGK::Vector<RGK::Vector<double>> basisFuncs = ImplNURBS::calculateBasisFuncs(bezierCurves[0], curveParameter);

        const size_t NUMBER_BASIS_FUNCS = static_cast<size_t>(iCurve.GetDegree()) + 1;                      // Количество базисных функций
        const size_t NUMBER_BEZIER_CURVES = bezierCurves.size();                                            // Количество кривых Безье
        const size_t NUMBER_BREAK_POINTS = NUMBER_BEZIER_CURVES - 1;                                        // Количество потенциальных точек разрыва между кривыми
        const size_t NUMBER_EPSILONS = bezierCurves.size() * bezierCurves[0].GetControlPoints().size();     // Количество эпсилон, которые будут регулировать контрольные точки
        const size_t MATRIX_SIZE = NUMBER_BASIS_FUNCS * (2 * NUMBER_BEZIER_CURVES - 1);                     // Размер матрицы коэффициентов

        // Матрица коэффициентов
        RGK::Vector<RGK::Vector<double>> coefficientMatrix(MATRIX_SIZE, RGK::Vector<double>(MATRIX_SIZE));

        // Заполняем матрицу коэффициентов
        _fillCoefficientsMatrix(coefficientMatrix, basisFuncs, NUMBER_EPSILONS, NUMBER_BREAK_POINTS);

        // Фиксируем начало и конец кривой
        _fixateCurve(coefficientMatrix, NUMBER_EPSILONS, NUMBER_BASIS_FUNCS, iFixBeginningCurve, iFixEndCurve);

        // Контрольные точки кривых Безье
        RGK::Vector<RGK::Math::Vector3DArray> controlPointsBezierCurves(NUMBER_BEZIER_CURVES);

        for (size_t i = 0; i != NUMBER_BEZIER_CURVES; ++i)
        {
            controlPointsBezierCurves[i] = bezierCurves[i].GetControlPoints();
        }

        // Матрица свободных членов. RGK::Vector<double>(3) - потому что 3 координаты x, y, z. Можно испрвить в дальнейшем, если узнать тип данных для точки
        RGK::Vector<RGK::Vector<double>> freeMembersMatrix(MATRIX_SIZE, RGK::Vector<double>(3));

        // Вычисляем реверсивные базисные функции и их производные в параметре 0
        curveParameter = 0;
        RGK::Vector<RGK::Vector<double>> reverseBasisFuncs = ImplNURBS::calculateBasisFuncs(bezierCurves[0], curveParameter);

        // Заполняем матрицу свободных членов
        _fillFreeMemberMatrix(freeMembersMatrix, controlPointsBezierCurves, basisFuncs, reverseBasisFuncs, NUMBER_EPSILONS);

        // Вычисляем точки смещения для новых контрольных точек
        RGK::Vector<RGK::Vector<double>> shiftPoints = _calculateShiftPoints(coefficientMatrix, freeMembersMatrix);

        // Делаем сдивг исходных контрольных точек для сопряжения
        _correctionPoints(controlPointsBezierCurves, shiftPoints, NUMBER_BEZIER_CURVES);

        // Вычисляем новые кривые Безье, которые будут непрерывны
        RGK::Vector<RGK::NURBSCurve> newBezierCurves = _createBezierCurves(controlPointsBezierCurves, NUMBER_BEZIER_CURVES, bezierCurves[0].GetDegree());

        // Представляем вектор кривых Безье как одну кривую NURBS
        RGK::NURBSCurve merdgedCurve = _bezierCurvesToNURBSCurve(newBezierCurves, bezierCurves[0].GetDegree());

        return merdgedCurve;
    }

    RGK::Vector<RGK::NURBSCurve> ConjugationMethod::_divideCurveIntoBezierCurves(const RGK::NURBSCurve& iCurve)
    {
        // Контрольные точки оригинальной кривой
        RGK::Vector<RGK::Math::Vector3D> controlPointsOriginalCurve = iCurve.GetControlPoints();
        int degree = iCurve.GetDegree();

        // Число кривых Безье, на которые будет разделена оригинальная кривая
        size_t numberBezierCurves = controlPointsOriginalCurve.size() / degree;
        RGK::Vector<RGK::NURBSCurve> bezierCurves(numberBezierCurves);
        RGK::NURBSCurve tempBezierCurve;

        RGK::Context rgkContext;
        RPLM::EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);

        for (size_t i = 0; i != numberBezierCurves; ++i)
        {
            RGK::Vector<RGK::Math::Vector3D> tempControlPoints;

            // Добавляем по частям контрольные точки оригинальной кривой для каждой отдельной кривой Безье
            for (size_t j = 0; j != static_cast<size_t>(degree) + 1; ++j)
            {
                tempControlPoints.push_back(controlPointsOriginalCurve[j + i * degree]);
            }

            RGK::NURBSCurve::CreateBezier(rgkContext, tempControlPoints, degree, tempBezierCurve);
            bezierCurves[i] = tempBezierCurve;
        }

        return bezierCurves;
    }

    void ConjugationMethod::_fillUpperTriangularCoefficientMatrix(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, size_t iNumberEpsilons, size_t iNumberBreakPoints)
    {
        // Количество базисных функций
        const size_t NUMBER_BASIS_FUNCS = iBasisFuncs.size();

        // Каждый breakPoint - одна итерация заполнения базисных функций в coefficientMatrix
        for (size_t breakPointsCounter = 0; breakPointsCounter != iNumberBreakPoints; ++breakPointsCounter)
        {
            // Реверс строка для противоположной сторны треугольника
            size_t reverseRow = NUMBER_BASIS_FUNCS * 2 - 1 + NUMBER_BASIS_FUNCS * breakPointsCounter;
            size_t colBasisFunc = 0;

            // Итерируемся по общему числу базисных функций
            for (size_t row = 0 + NUMBER_BASIS_FUNCS * breakPointsCounter; row != NUMBER_BASIS_FUNCS + NUMBER_BASIS_FUNCS * breakPointsCounter; ++row)
            {
                // Строка базисных функций
                size_t rowBasisFunc = 0;
                // Предыдущее значение базисной функции (для правильного заполнения коэффициентов с нужным знаком "+" или "-")
                double prevBasisFuncVal = iBasisFuncs[rowBasisFunc][colBasisFunc];

                for (size_t col = iNumberEpsilons + NUMBER_BASIS_FUNCS * breakPointsCounter; col != iNumberEpsilons + NUMBER_BASIS_FUNCS + NUMBER_BASIS_FUNCS * breakPointsCounter; ++col)
                {
                    double nextBasisFuncVal = iBasisFuncs[rowBasisFunc][colBasisFunc];
                    iCoefficientMatrix[row][col] = nextBasisFuncVal;

                    // Регулируем знак у противоположной части базисных функций
                    if (prevBasisFuncVal < 0 && nextBasisFuncVal < 0)
                    {
                        nextBasisFuncVal *= -1;
                    }
                    else if (prevBasisFuncVal >= 0 && nextBasisFuncVal > 0)
                    {
                        nextBasisFuncVal *= -1;
                    }

                    iCoefficientMatrix[reverseRow][col] = nextBasisFuncVal;
                    prevBasisFuncVal = nextBasisFuncVal;
                    ++rowBasisFunc;
                }

                --reverseRow;
                ++colBasisFunc;
            }
        }
    }

    void ConjugationMethod::_fillLowerTriangularCoefficientMatrix(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, size_t iNumberEpsilons, size_t iNumberBreakPoints)
    {
        // Количество базисных функций
        const size_t NUMBER_BASIS_FUNCS = iBasisFuncs.size();

        // Каждый breakPoint - одна итерация заполнения базисных функций в coefficientMatrix
        for (size_t breakPointCounter = 0; breakPointCounter != iNumberBreakPoints; ++breakPointCounter)
        {
            size_t rowBasisFunc = 0;

            // Итерируемся по общему числу базисных функций
            for (size_t row = iNumberEpsilons + NUMBER_BASIS_FUNCS * breakPointCounter; row != iNumberEpsilons + NUMBER_BASIS_FUNCS + NUMBER_BASIS_FUNCS * breakPointCounter; ++row)
            {
                // Реверс столбец для противоположной части
                size_t reverseCol = NUMBER_BASIS_FUNCS * 2 - 1 + NUMBER_BASIS_FUNCS * breakPointCounter;
                size_t colBasisFunc = 0;
                // Предыдущее значение базисной функции (для правильного заполнения коэффициентов с нужным знаком "+" или "-")
                double prevBasisFuncVal = iBasisFuncs[rowBasisFunc][colBasisFunc];

                for (size_t col = 0 + NUMBER_BASIS_FUNCS * breakPointCounter; col != NUMBER_BASIS_FUNCS + NUMBER_BASIS_FUNCS * breakPointCounter; ++col)
                {
                    double nextBasisFuncVal = iBasisFuncs[rowBasisFunc][colBasisFunc];
                    iCoefficientMatrix[row][col] = nextBasisFuncVal;

                    // Регулируем знак у противоположной части базисных функций
                    if (prevBasisFuncVal < 0 && nextBasisFuncVal < 0 && col != NUMBER_BASIS_FUNCS * breakPointCounter)
                    {
                        nextBasisFuncVal *= -1;
                    }
                    else if (prevBasisFuncVal >= 0 && nextBasisFuncVal > 0)
                    {
                        nextBasisFuncVal *= -1;
                    }

                    prevBasisFuncVal = nextBasisFuncVal;
                    iCoefficientMatrix[row][reverseCol] = nextBasisFuncVal;
                    ++colBasisFunc;
                    --reverseCol;
                }

                ++rowBasisFunc;
            }
        }
    }

    void ConjugationMethod::_fillCoefficientsMatrix(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, size_t iNumberEpsilons, size_t iNumberBreakPoints)
    {
        // Заполняем двойками главную диагональ
        for (size_t i = 0; i != iNumberEpsilons; ++i)
        {
            iCoefficientMatrix[i][i] = 2;
        }

        // Заполняем матрицу коэффциентов базисными функциями над главной диагональю и под ней
        _fillUpperTriangularCoefficientMatrix(iCoefficientMatrix, iBasisFuncs, iNumberEpsilons, iNumberBreakPoints);
        _fillLowerTriangularCoefficientMatrix(iCoefficientMatrix, iBasisFuncs, iNumberEpsilons, iNumberBreakPoints);
    }

    void ConjugationMethod::_fixateCurve(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, size_t iNumberEpsilons, size_t iNumberBasisFuncs, bool iFixBeginningCurve, bool iFixEndCurve)
    {
        int orderFixFirstDeriv = 1;
        int orderFixLastDeriv = 1;

        if (iFixBeginningCurve)
        {
            // Фиксация первой граничной точки кривой
            while (orderFixFirstDeriv >= 0)
            {
                for (size_t row = iNumberEpsilons; row != iNumberEpsilons + iNumberBasisFuncs; ++row)
                {
                    iCoefficientMatrix[orderFixFirstDeriv][row] = 0;
                }

                --orderFixFirstDeriv;
            }
        }

        if (iFixEndCurve)
        {
            for (size_t col = iNumberEpsilons + iNumberBasisFuncs; col != iCoefficientMatrix.size(); ++col)
            {
                iCoefficientMatrix[iNumberEpsilons - 1][col] = 0;
            }
        }


        // Не работает для 1 порядка производной
        //int tempCounter = 1;

        //while (orderFixLastDeriv >= 0)
        //{
        //    for (size_t col = numberEpsilons; col != numberEpsilons + numberBasisFuncs; ++col)
        //    {
        //        coefficientMatrix[numberEpsilons - tempCounter][col] = 0;
        //    }

        //    ++tempCounter;
        //    --orderFixLastDeriv;
        //}
    }

    void ConjugationMethod::_fillFreeMemberMatrix(RGK::Vector<RGK::Vector<double>>& iFreeMembersMatrix, const RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, RGK::Vector<RGK::Vector<double>>& iReverseBasisFuncs, size_t iNumberEpsilons)
    {
        size_t indexFreeMembers = iNumberEpsilons;

        for (size_t row = 0; row != iControlPointsBezierCurves.size() - 1; ++row)
        {
            size_t rowBasisFunc = 0;

            for (size_t col = 0; col != iBasisFuncs[0].size(); ++col)
            {
                for (size_t i = 0; i != iControlPointsBezierCurves[0].size(); ++i)
                {
                    // Текущая кривая
                    iFreeMembersMatrix[indexFreeMembers][0] += iControlPointsBezierCurves[row][i].GetX() * -iBasisFuncs[rowBasisFunc][i];
                    iFreeMembersMatrix[indexFreeMembers][1] += iControlPointsBezierCurves[row][i].GetY() * -iBasisFuncs[rowBasisFunc][i];
                    iFreeMembersMatrix[indexFreeMembers][2] += iControlPointsBezierCurves[row][i].GetZ() * -iBasisFuncs[rowBasisFunc][i];
                    // Следующая кривая
                    iFreeMembersMatrix[indexFreeMembers][0] += iControlPointsBezierCurves[row + 1][i].GetX() * iReverseBasisFuncs[rowBasisFunc][i];
                    iFreeMembersMatrix[indexFreeMembers][1] += iControlPointsBezierCurves[row + 1][i].GetY() * iReverseBasisFuncs[rowBasisFunc][i];
                    iFreeMembersMatrix[indexFreeMembers][2] += iControlPointsBezierCurves[row + 1][i].GetZ() * iReverseBasisFuncs[rowBasisFunc][i];
                }

                ++rowBasisFunc;
                ++indexFreeMembers;
            }
        }
    }
    RGK::Vector<RGK::Vector<double>> ConjugationMethod::_calculateShiftPoints(const RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, const RGK::Vector<RGK::Vector<double>>& iFreeMembersMatrix)
    {
        // Создаём указатель на интерфейс операций СЛАУ
        auto operation = IMatrixOperations::GetMatrixOperationsClass(OperationClass::eigen);

        if (operation == nullptr)
        {
            throw "Error! _calculateShiftPoints: operation = nullptr";
        }

        // Вычисляем определитель матрицы коэффициентов
        double coefficientMatrixDet = operation->getMatrixDet(iCoefficientMatrix);

        if (coefficientMatrixDet == 0)
        {
            throw "Error! _calculateShiftPoints: Определитель матрицы коэффициентов = 0! Возможно, сделайте меньше фиксированных точек в функции fixPointsAtCurve!";
        }

        // Решаем СЛАУ и возвращаем ответ
        return operation->solveEquation(iCoefficientMatrix, iFreeMembersMatrix);
    }

    void ConjugationMethod::_correctionPoints(RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, RGK::Vector<RGK::Vector<double>>& iShiftPoints, size_t iNumberBezierCurves)
    {
        int tempCounter = 0;

        for (size_t i = 0; i != iNumberBezierCurves; ++i)
        {
            for (size_t j = 0; j != iControlPointsBezierCurves[i].size(); ++j)
            {
                double x = iControlPointsBezierCurves[i][j].GetX() + iShiftPoints[tempCounter][0];
                double y = iControlPointsBezierCurves[i][j].GetY() + iShiftPoints[tempCounter][1];
                double z = iControlPointsBezierCurves[i][j].GetZ() + iShiftPoints[tempCounter][2];
                iControlPointsBezierCurves[i][j].SetXYZ(x, y, z);
                ++tempCounter;
            }
        }
    }

    RGK::Vector<RGK::NURBSCurve> ConjugationMethod::_createBezierCurves(RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, size_t iNumberBezierCurves, int iDegree)
    {
        RGK::Vector<RGK::NURBSCurve> newBezierCurves;
        RGK::NURBSCurve tempBezierCurve;

        for (size_t i = 0; i != iNumberBezierCurves; ++i)
        {
            // TODO! Не знаю, нужно ли каждый раз пересоздавать rgkContext и прописывать его в NURBSCurve::CreateBezier...
            RGK::Context rgkContext;
            RPLM::EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);

            // Создаём новую кривую Безье и добавляем в вектор, чтобы функция возвратила его
            RGK::NURBSCurve::CreateBezier(rgkContext, iControlPointsBezierCurves[i], iDegree, tempBezierCurve);
            newBezierCurves.push_back(tempBezierCurve);
        }

        return newBezierCurves;
    }

    RGK::NURBSCurve ConjugationMethod::_bezierCurvesToNURBSCurve(const RGK::Vector<RGK::NURBSCurve>& iBezierCurves, int iDegree)
    {
        RGK::Vector<RGK::Math::Vector3D> newControlPoints;
        // Для того, чтобы не было повторяющихся точек
        bool firstCheck = false;

        for (size_t curveCount = 0; curveCount != iBezierCurves.size(); ++curveCount)
        {
            RGK::Vector<RGK::Math::Vector3D> tempControlPoints = iBezierCurves[curveCount].GetControlPoints();

            for (size_t i = 0; i != tempControlPoints.size(); ++i)
            {
                if (firstCheck && i == 0)
                {
                    continue;
                }
                if (curveCount == 0)
                {
                    firstCheck = true;
                }

                newControlPoints.push_back(tempControlPoints[i]);
            }
        }
        // TODO! Не знаю, нужно ли каждый раз пересоздавать rgkContext и прописывать его в NURBSCurve::CreateBezier...
        RGK::Context rgkContext;
        RPLM::EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);

        Math::Geometry2D::Geometry::DoubleArray knots = ImplNURBS::fillEvenlyNodalVector(iDegree, static_cast<int>(newControlPoints.size()));

        RGK::NURBSCurve newCurve;
        // Создаём новую кривую Безье и добавляем в вектор, чтобы функция возвратила его
        RGK::NURBSCurve::Create(rgkContext, newControlPoints, iDegree, knots, false, newCurve);

        return newCurve;
    }
}