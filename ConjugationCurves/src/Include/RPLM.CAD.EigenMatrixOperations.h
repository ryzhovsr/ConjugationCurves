#pragma once

#include "RPLM.CAD.IMatrixOperations.h"

class EigenMatrixOperations final : public IMatrixOperations
{
public:
    // Решает СЛАУ
    vector2D solveEquation(const vector2D& iCoefficients, const vector2D& iFreeMembers) override;

    // Возвращает ранг матрицы
    int getMatrixRank(const vector2D& iMatrix) override;

    // Возвращает определитель матрицы
    double getMatrixDet(const vector2D& iMatrix) override;
};
