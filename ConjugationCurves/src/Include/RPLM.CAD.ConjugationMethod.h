#pragma once

#include <Geometry/Curves/NURBSCurve.h>

namespace RPLM::CAD
{
	namespace ConjugationCurves
	{
		class ConjugationMethod
		{
		public:
			/// <summary>���������� ������ � ��������������� ������������� � ������ � � �������� �����������</summary>
			/// <param name="iCurve"></param>
			/// <param name="iOrderFixFirstDeriv">������� ������ �����������</param>
			/// <param name="iOrderFixLastDeriv">������� �������� �����������</param>
			/// <returns>���������� ������</returns>
			RGK::NURBSCurve conjugateCurve(const RGK::NURBSCurve& iCurve, bool fixBeginningCurve, bool fixEndCurve);
		};
	}
}
