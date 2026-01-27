#include "RPLM.CAD.CurveParser.h"
#include "Geometry/Curves/NURBSCurve.h"
#include "RGPSession.h"
#include <fstream>

namespace RPLM::CAD
{
	RGK::Vector<RGK::Math::Vector3D> CurveParser::ReadControlPointsFromFile(const Base::Framework::String& iFilePath)
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

	Math::Geometry2D::Geometry::DoubleArray CurveParser::ReadKnotsFromFile(const Base::Framework::String& iFilePath)
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

    RGK::NURBSCurve CurveParser::ReadCurveFromFile(const Base::Framework::String& iFilePath)
    {
        if (iFilePath.empty())
            return nullptr;

        std::ifstream inStream(iFilePath.c_str());

        if (!inStream.is_open())
            return nullptr;

        int degree = 0;
        bool isPeriodic = false;
        RGK::Math::Vector3DArray controlPoints;
        std::vector<double> weights;
        std::vector<double> knots;

        std::string line;
        // Ожидаемое количество элементов в соответствующих секциях.
        // Значение -1 означает "ещё не встречали эту секцию".
        int expectedControlPoints = -1;
        int expectedWeights = -1;
        int expectedKnots = -1;

        while (std::getline(inStream, line))
        {
            // Удаляем все пробелы, табуляции и переводы строк слева
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty())
                continue;

            // Степень
            if (line.rfind("Degree:", 0) == 0)
            {
                degree = std::stoi(line.substr(line.find(':') + 1));
                continue;
            }

            // Периодичность
            if (line.rfind("IsPeriodic:", 0) == 0)
            {
                std::string value = line.substr(line.find(':') + 1);
                value.erase(0, value.find_first_not_of(" \t"));

                isPeriodic = (value == "true" || value == "1");
                continue;
            }

            // Контрольные точки
            if (line.rfind("Control Points[", 0) == 0)
            {
                auto l = line.find('[');
                auto r = line.find(']');

                expectedControlPoints = std::stoi(line.substr(l + 1, r - l - 1));
                controlPoints.reserve(expectedControlPoints);

                for (int i = 0; i != expectedControlPoints; ++i)
                {
                    if (!std::getline(inStream, line))
                        return {};

                    std::istringstream iss(line);
                    double x, y, z;

                    if (!(iss >> x >> y >> z))
                        return {};

                    controlPoints.push_back(RGK::Math::Vector3D(x, y, z));
                }

                continue;
            }

            // Весовые коэффициенты
            if (line.rfind("Weights[", 0) == 0)
            {
                auto l = line.find('[');
                auto r = line.find(']');

                expectedWeights = std::stoi(line.substr(l + 1, r - l - 1));
                weights.reserve(expectedWeights);

                // читаем до тех пор, пока не соберём все веса
                while (weights.size() < expectedWeights && std::getline(inStream, line))
                {
                    std::replace(line.begin(), line.end(), ',', ' ');
                    std::istringstream iss(line);
                    double w;

                    while (iss >> w)
                        weights.push_back(w);
                }

                if (weights.size() != expectedWeights)
                    return {};

                continue;
            }

            // Узловые коэффициенты
            if (line.rfind("Knots[", 0) == 0)
            {
                auto l = line.find('[');
                auto r = line.find(']');

                expectedKnots = std::stoi(line.substr(l + 1, r - l - 1));
                knots.reserve(expectedKnots);

                while (knots.size() < expectedKnots && std::getline(inStream, line))
                {
                    std::replace(line.begin(), line.end(), ',', ' ');
                    std::istringstream iss(line);
                    double k;

                    while (iss >> k)
                        knots.push_back(k);
                }

                if (knots.size() != expectedKnots)
                    return nullptr;

                continue;
            }
        }

        RGK::Context rgkContext;
        EP::Model::Session::GetSession()->GetRGKSession().CreateMainContext(rgkContext);
        RGK::NURBSCurve curve;

        if (RGK::NURBSCurve::Create(rgkContext, controlPoints, degree, knots, isPeriodic, curve) != RGK::Result::Success)
            return nullptr;

        return curve;
    }

	void CurveParser::SaveCurveInFile(const RGK::NURBSCurve& iCurve, const Base::Framework::String& iFilePath)
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
			outStream << "Weights[" << weights.size() << "]:\n";
			SaveOneDimensionalArrayInFile(weights);

			const auto& knots = iCurve.GetKnots();
			outStream << "Knots[" << knots.size() << "]:\n";
			SaveOneDimensionalArrayInFile(knots);

			outStream.close();
		}
	}
}
