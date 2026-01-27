#pragma once

#include "Common/RPLM.Base.Framework.String.h"
#include "RPLM.GeomCore/RPLM.Math.Geometry2D/Geometry/RGPGeometryForward.h"

namespace RPLM::CAD
{
	namespace Utils
	{
		/// <summary>Считывает контрольные точки кривой из файла</summary>
		/// <param name="iFilePath">Путь к файлу</param>
		/// <returns>Контрольные точки</returns>
		RGK::Vector<RGK::Math::Vector3D> ReadControlPointsFromFile(const Base::Framework::String& iFilePath);

		/// <summary>Считывает узловой вектор из файла</summary>
		/// <param name="iFilePath">Путь к файлу</param>
		/// <returns>Узловой вектор</returns>
		Math::Geometry2D::Geometry::DoubleArray ReadKnotsFromFile(const Base::Framework::String& iFilePath);

        /// <summary>Сохраняет данные кривой в файл</summary>
		/// <param name="iCurve">Кривая</param>
        /// <param name="iFilePath">Путь к файлу</param>
        void SaveCurveInFile(const RGK::NURBSCurve& iCurve, const Base::Framework::String& iFilePath);
	}
}
