#pragma once

#include <vector>
#include <memory>

class IMatrixOperations;
using IMatrixOperationsPtr = std::shared_ptr<IMatrixOperations>;

// Содержит названия библиотек для работы с матрицами
enum OperationClass
{
    eigen
};

class IMatrixOperations
{
public:
    using vector2D = std::vector<std::vector<double>>;

    // Решает СЛАУ
    virtual vector2D solveEquation(const vector2D& iCoefficients, const vector2D& iFreeMembers) = 0;

    // Возвращает ранг матрицы
    virtual int getMatrixRank(const vector2D& iMatrix) = 0;

    // Возвращает определитель матрицы
    virtual double getMatrixDet(const vector2D& iMatrix) = 0;

    // Возвращяет текущую выбранную библиотеку в перечислении
    static IMatrixOperationsPtr GetMatrixOperationsClass (OperationClass className);
};
