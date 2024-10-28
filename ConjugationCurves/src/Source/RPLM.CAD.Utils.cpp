#include "RPLM.CAD.Utils.h"
#include <fstream>

namespace RPLM::CAD
{
	RGK::Vector<RGK::Math::Vector3D> Utils::readControlPointsFromFile(const Base::Framework::String& iFilePath)
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

	Math::Geometry2D::Geometry::DoubleArray Utils::readKnotsFromFile(const Base::Framework::String& iFilePath)
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

	void Utils::saveControlPointsInFile(const Base::Framework::String& iFilePath, const RGK::Vector<RGK::Math::Vector3D>& iControlPoints)
	{
		std::ofstream outStream(iFilePath);

		if (outStream.is_open())
		{
			for (const auto& curvePoints : iControlPoints)
			{
				outStream << curvePoints.GetX() << " " << curvePoints.GetY() << " " << curvePoints.GetZ() << '\n';
			}

			outStream.close();
		}
	}
}
