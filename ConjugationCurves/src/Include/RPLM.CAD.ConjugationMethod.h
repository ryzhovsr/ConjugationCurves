#pragma once

#include <Geometry/Curves/NURBSCurve.h>
#include <Geometry/RGPGeometryForward.h>

namespace RPLM::CAD
{
	namespace ConjugationCurves
	{
		class ConjugationMethod
		{
		public:
			/// <summary>Сопряжение кривой с дополнительными ограничениями в начале и в конце кривой</summary>
			/// <param name="iCurve">Оригинальная кривая</param>
			/// <param name="iFixBeginningCurve">Зафиксировать начало кривой</param>
			/// <param name="iFixEndCurve">Зафиксировать конец кривой</param>
			/// <returns>Сопряжённая кривая</returns>
			RGK::NURBSCurve conjugateCurve(const RGK::NURBSCurve& iCurve, bool iFixBeginningCurve, bool iFixEndCurve);

		private:
			/// <summary>Разбивает NURBS кривую на кривые Безье</summary>
			/// <param name="iCurve">Оригинальная кривая</param>
			/// <returns>Вектор кривых Безье</returns>
			RGK::Vector<RGK::NURBSCurve> _divideCurveIntoBezierCurves(const RGK::NURBSCurve& iCurve);

			/// <summary>Заполнение матрицы коэффициентов</summary>
			/// <param name="iCoefficientMatrix">Матрица коэффициентов</param>
			/// <param name="iBasisFuncs">Базисные функции</param>
			/// <param name="iNumberEpsilons">Количество эпсилон</param>
			/// <param name="iNumberBreakPoints">Число точек останова</param>
			void _fillCoefficientsMatrix(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, size_t iNumberEpsilons, size_t iNumberBreakPoints);

			/// <summary>Фиксирует начало и конец кривой (зануление определенных столбцов и строк у матрицы коэффициентов)</summary>
			/// <param name="iCoefficientMatrix">Матрица коэффициентов</param>
			/// <param name="iNumberEpsilons">Количество эпсилон</param>
			/// <param name="iNumberBasisFuncs">Количество базисных функций</param>
			/// <param name="iFixBeginningCurve">Зафиксировать начало кривой</param>
			/// <param name="iFixEndCurve">Зафиксировать конец кривой</param>
			void _fixateCurve(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, size_t iNumberEpsilons, size_t iNumberBasisFuncs, bool iFixBeginningCurve, bool iFixEndCurve);

			/// <summary>Заполняет матрицу свободных членов</summary>
			/// <param name="iFreeMembersMatrix">Матрица свободных членов</param>
			/// <param name="iControlPointsBezierCurves">Контрольные точки кривых Безье</param>
			/// <param name="iBasisFuncs">Базисные функции</param>
			/// <param name="iReverseBasisFuncs">Обратные базисные функции</param>
			/// <param name="iNumberEpsilons">Количество эпсилон</param>
			void _fillFreeMemberMatrix(RGK::Vector<RGK::Vector<double>>& iFreeMembersMatrix, const RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, RGK::Vector<RGK::Vector<double>>& iReverseBasisFuncs, size_t iNumberEpsilons);

			/// <summary>Вычисляет точки сдвига для полного сопряжения кривой</summary>
			/// <param name="iCoefficientMatrix">Матрица коэффициентов</param>
			/// <param name="iFreeMembersMatrix">Матрица свободных членов</param>
			/// <returns>Точки сдвига</returns>
			RGK::Vector<RGK::Vector<double>> _calculateShiftPoints(const RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, const RGK::Vector<RGK::Vector<double>>& iFreeMembersMatrix);

			/// <summary>Регулирует котрольные точки кривых Безье</summary>
			/// <param name="iControlPointsBezierCurves">Контрольные точки кривых Безье</param>
			/// <param name="iShiftPoints">Точки сдвига</param>
			/// <param name="iNumberBezierCurves">Количество контрольных точек</param>
			void _adjustControlPoints(RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, RGK::Vector<RGK::Vector<double>>& iShiftPoints, size_t iNumberBezierCurves);
			
			/// <summary>Создаёт вектор кривых Безье из заданного вектора контрольных многоугольников кривых Безье</summary>
			/// <param name="iControlPointsBezierCurves">Контрольные точки кривых Безье</param>
			/// <param name="iNumberBezierCurves">Количество кривых Безье</param>
			/// <param name="iDegree">Степень</param>
			/// <returns>Вектор кривых Безье</returns>
			RGK::Vector<RGK::NURBSCurve> _createBezierCurves(RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, size_t iNumberBezierCurves, int iDegree);

			/// <summary>Переводит вектор кривых Безье в один NURBS</summary>
			/// <param name="iBezierCurves">Вектор кривых Безье</param>
			/// <param name="iDegree">Степень</param>
			/// <returns>Сопряжённая кривая</returns>
			RGK::NURBSCurve _bezierCurvesToNURBS(const RGK::Vector<RGK::NURBSCurve>& iBezierCurves, int iDegree);
		};
	}
}
