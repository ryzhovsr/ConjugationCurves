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
			/// <summary>���������� ������ � ��������������� ������������� � ������ � � ����� ������</summary>
			/// <param name="iCurve">������������ ������</param>
			/// <param name="iFixBeginningCurve">������������� ������ ������</param>
			/// <param name="iFixEndCurve">������������� ����� ������</param>
			/// <returns>���������� ������</returns>
			RGK::NURBSCurve conjugateCurve(const RGK::NURBSCurve& iCurve, bool iFixBeginningCurve, bool iFixEndCurve);

		private:
			/// <summary>��������� NURBS ������ �� ������ �����</summary>
			/// <param name="iCurve">������������ ������</param>
			/// <returns>������ ������ �����</returns>
			RGK::Vector<RGK::NURBSCurve> _divideCurveIntoBezierCurves(const RGK::NURBSCurve& iCurve);

			/// <summary>���������� ������� �������������</summary>
			/// <param name="iCoefficientMatrix">������� �������������</param>
			/// <param name="iBasisFuncs">�������� �������</param>
			/// <param name="iNumberEpsilons">���������� �������</param>
			/// <param name="iNumberBreakPoints">����� ����� ��������</param>
			void _fillCoefficientsMatrix(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, size_t iNumberEpsilons, size_t iNumberBreakPoints);

			/// <summary>��������� ������ � ����� ������ (��������� ������������ �������� � ����� � ������� �������������)</summary>
			/// <param name="iCoefficientMatrix">������� �������������</param>
			/// <param name="iNumberEpsilons">���������� �������</param>
			/// <param name="iNumberBasisFuncs">���������� �������� �������</param>
			/// <param name="iFixBeginningCurve">������������� ������ ������</param>
			/// <param name="iFixEndCurve">������������� ����� ������</param>
			void _fixateCurve(RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, size_t iNumberEpsilons, size_t iNumberBasisFuncs, bool iFixBeginningCurve, bool iFixEndCurve);

			/// <summary>��������� ������� ��������� ������</summary>
			/// <param name="iFreeMembersMatrix">������� ��������� ������</param>
			/// <param name="iControlPointsBezierCurves">����������� ����� ������ �����</param>
			/// <param name="iBasisFuncs">�������� �������</param>
			/// <param name="iReverseBasisFuncs">�������� �������� �������</param>
			/// <param name="iNumberEpsilons">���������� �������</param>
			void _fillFreeMemberMatrix(RGK::Vector<RGK::Vector<double>>& iFreeMembersMatrix, const RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, RGK::Vector<RGK::Vector<double>>& iBasisFuncs, RGK::Vector<RGK::Vector<double>>& iReverseBasisFuncs, size_t iNumberEpsilons);

			/// <summary>��������� ����� ������ ��� ������� ���������� ������</summary>
			/// <param name="iCoefficientMatrix">������� �������������</param>
			/// <param name="iFreeMembersMatrix">������� ��������� ������</param>
			/// <returns>����� ������</returns>
			RGK::Vector<RGK::Vector<double>> _calculateShiftPoints(const RGK::Vector<RGK::Vector<double>>& iCoefficientMatrix, const RGK::Vector<RGK::Vector<double>>& iFreeMembersMatrix);

			/// <summary>���������� ���������� ����� ������ �����</summary>
			/// <param name="iControlPointsBezierCurves">����������� ����� ������ �����</param>
			/// <param name="iShiftPoints">����� ������</param>
			/// <param name="iNumberBezierCurves">���������� ����������� �����</param>
			void _adjustControlPoints(RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, RGK::Vector<RGK::Vector<double>>& iShiftPoints, size_t iNumberBezierCurves);
			
			/// <summary>������ ������ ������ ����� �� ��������� ������� ����������� ��������������� ������ �����</summary>
			/// <param name="iControlPointsBezierCurves">����������� ����� ������ �����</param>
			/// <param name="iNumberBezierCurves">���������� ������ �����</param>
			/// <param name="iDegree">�������</param>
			/// <returns>������ ������ �����</returns>
			RGK::Vector<RGK::NURBSCurve> _createBezierCurves(RGK::Vector<RGK::Math::Vector3DArray>& iControlPointsBezierCurves, size_t iNumberBezierCurves, int iDegree);

			/// <summary>��������� ������ ������ ����� � ���� NURBS</summary>
			/// <param name="iBezierCurves">������ ������ �����</param>
			/// <param name="iDegree">�������</param>
			/// <returns>���������� ������</returns>
			RGK::NURBSCurve _bezierCurvesToNURBS(const RGK::Vector<RGK::NURBSCurve>& iBezierCurves, int iDegree);
		};
	}
}
