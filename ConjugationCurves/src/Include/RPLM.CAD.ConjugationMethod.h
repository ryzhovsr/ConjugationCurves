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
			/// <summary>Сопряжение кривой с дополнительными ограничениями в первой и в конечной производной</summary>
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

			/// <summary>Заполняет элементы матрицы коээфициентов над её главной диагональю</summary>
			/// <param name="iCoefficientMatrix"></param>
			/// <param name="iBasisFuncs"></param>
			/// <param name="iNumberEpsilons"></param>
			/// <param name="iNumberBreakPoints"></param>
			void _fillUpperTriangularCoefficientMatrix(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, size_t iNumberEpsilons, size_t iNumberBreakPoints);

			/// <summary>Заполняет элементы матрицы коээфициентов под её главной диагональю</summary>
			/// <param name="iCoefficientMatrix"></param>
			/// <param name="iBasisFuncs"></param>
			/// <param name="iNumberEpsilons"></param>
			/// <param name="iNumberBreakPoints"></param>
			void _fillLowerTriangularCoefficientMatrix(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, size_t iNumberEpsilons, size_t iNumberBreakPoints);

			/// <summary>Заполнение матрицы коэффициентов</summary>
			/// <param name="iCoefficientMatrix"></param>
			/// <param name="iBasisFuncs"></param>
			/// <param name="iNumberEpsilons"></param>
			/// <param name="iNumberBreakPoints"></param>
			void _fillCoefficientsMatrix(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, size_t iNumberEpsilons, size_t iNumberBreakPoints);

			/// <summary>Фиксирует первую и последнюю точки кривой и их первые производные (зануление определенных столбцов и строк у матрицы коэффициентов)</summary>
			/// <param name="iCoefficientMatrix"></param>
			/// <param name="iNumberEpsilons"></param>
			/// <param name="iNumberBasisFuncs"></param>
			/// <param name="iFixBeginningCurve"></param>
			/// <param name="iFixEndCurve"></param>
			void _fixateCurve(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, size_t iNumberEpsilons, size_t iNumberBasisFuncs, bool iFixBeginningCurve, bool iFixEndCurve);

			/// <summary>Заполняет матрицу свободных членов</summary>
			/// <param name="iFreeMembersMatrix"></param>
			/// <param name="iControlPointsBezierCurves"></param>
			/// <param name="iBasisFuncs"></param>
			/// <param name="iReverseBasisFuncs"></param>
			/// <param name="iNumberEpsilons"></param>
			void _fillFreeMemberMatrix(RGK::Vector<RGK::Vector<double>>& iFreeMembersMatrix, const RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, RGK::Vector<RGK::Vector<double>>& iReverseBasisFuncs, size_t iNumberEpsilons);

			/// <summary>Вычисляет точки сдвига для полного сопряжения кривой</summary>
			/// <param name="iCoefficientMatrix"></param>
			/// <param name="iFreeMembersMatrix"></param>
			/// <returns></returns>
			RGK::Vector<RGK::Vector<double>> _calculateShiftPoints(const RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, const RGK::Vector<RGK::Vector<double>>& iFreeMembersMatrix);

			/// <summary>Регулирует котрольные точки Безье кривых для сопряжения</summary>
			/// <param name="iControlPointsBezierCurves"></param>
			/// <param name="iShiftPoints"></param>
			/// <param name="iNumberBezierCurves"></param>
			void _correctionPoints(RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, RGK::Vector<RGK::Vector<double>>& iShiftPoints, size_t iNumberBezierCurves);
			
			/// <summary>Создает вектор кривых Безье из заданного вектора контрольных многоугольников кривых Безье</summary>
			/// <param name="iControlPointsBezierCurves"></param>
			/// <param name="iNumberBezierCurves"></param>
			/// <param name="iDegree"></param>
			/// <returns></returns>
			RGK::Vector<RGK::NURBSCurve> _createBezierCurves(RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, size_t iNumberBezierCurves, int iDegree);

			/// <summary>Переводит вектор кривых Безье в одну кривую NURBS</summary>
			/// <param name="iBezierCurves"></param>
			/// <param name="iDegree"></param>
			/// <returns></returns>
			RGK::NURBSCurve _bezierCurvesToNURBSCurve(const RGK::Vector<RGK::NURBSCurve>& iBezierCurves, int iDegree);
		};
	}
}
