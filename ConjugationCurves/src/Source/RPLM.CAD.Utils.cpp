#include "RPLM.CAD.Utils.h"
#include "Geometry/Curves/NURBSCurve.h"
#include <fstream>

namespace RPLM::CAD
{
	RGK::Vector<RGK::Math::Vector3D> Utils::ReadControlPointsFromFile(const Base::Framework::String& iFilePath)
	{
		std::ifstream inStream(iFilePath);
		RGK::Vector<RGK::Math::Vector3D> controlPoints;

		// Счётчик координат
		int coordinateCounter = 0;
		// Размерность координат
		const int coordinateDimension = 3;

		if (inStream.is_open())
		{
			RGK::Vector<double> temp;
			double number = 0;

			while (inStream >> number)
			{
				// Если все три координаты получены
				if (coordinateCounter == coordinateDimension)
				{
					controlPoints.push_back(RGK::Math::Vector3D(temp[0], temp[1], temp[2]));
					temp.clear();
					coordinateCounter = 0;
				}

				temp.push_back(number);
				++coordinateCounter;
			}

			// Добавляем последнюю координату
			if (coordinateCounter == coordinateDimension)
			{
				controlPoints.push_back(RGK::Math::Vector3D(temp[0], temp[1], temp[2]));
			}

			inStream.close();
		}

		return controlPoints;
	}

	Math::Geometry2D::Geometry::DoubleArray Utils::ReadKnotsFromFile(const Base::Framework::String& iFilePath)
	{
		Math::Geometry2D::Geometry::DoubleArray knots;
		std::ifstream inStream(iFilePath);

		if (inStream.is_open())
		{
			double number = 0;

			while (inStream >> number)
			{
				knots.push_back(number);
			}

			inStream.close();
		}

		return knots;
	}

	void Utils::SaveCurveInFile(const RGK::NURBSCurve& iCurve, const Base::Framework::String& iFilePath)
	{
		if (!iCurve || iFilePath.empty())
			return;

		std::ofstream outStream(iFilePath);

		if (outStream.is_open())
		{
			outStream << "Degree: " << iCurve.GetDegree() << "\n";
			outStream << "IsPeriodic: " << iCurve.IsPeriodic() << "\n";

			const auto& controlPoints = iCurve.GetControlPoints();
			outStream << "Control Points[" << controlPoints.size() << "]:\n";

			for (const auto& point : controlPoints)
				outStream << point.GetX() << ", " << point.GetY() << ", " << point.GetZ() << '\n';

			// Выводит одномерный массив в файл по заготовленному шаблону
			const auto& SaveOneDimensionalArrayInFile = [&outStream](const Math::Geometry2D::Geometry::DoubleArray& iArray)
			{
				size_t arraySize = iArray.size();

				for (size_t i = 0; i != arraySize; ++i)
				{
					outStream << iArray[i];

					if (i != arraySize - 1)
					{
						outStream << ", ";
					}
				}

				outStream << "\n";
			};

			const auto& weights = iCurve.GetWeights();
			outStream << "Weights[" << weights.size() << "]\n";
			SaveOneDimensionalArrayInFile(weights);

			const auto& knots = iCurve.GetKnots();
			outStream << "Knots[" << knots.size() << "]\n";
			SaveOneDimensionalArrayInFile(knots);

			outStream.close();
		}
	}
}
