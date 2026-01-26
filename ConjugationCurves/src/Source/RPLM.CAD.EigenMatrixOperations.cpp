#include "RPLM.CAD.EigenMatrixOperations.h"

// В ручную привязываю библиотеку Eigen. Изменить если узнать как сделать динамический путь
#include "Eigen\Dense"

using namespace Eigen;

// Переводит двумерный вектор в матрицу класса Eigen
inline MatrixXd vector2DToMatrix(const IMatrixOperations::vector2D& iMatrix)
{
    size_t rows = iMatrix.size(), cols = iMatrix[0].size();
    MatrixXd matrix = MatrixXd::Constant(rows, cols, 0);

    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
        {
            matrix(i, j) = iMatrix[i][j];
        }
    }

    return matrix;
}

// Переводит матрицу класса Eigen в двумерный вектор
inline IMatrixOperations::vector2D matrixToVector2D(const MatrixXd& iMatrix)
{
    auto rows = iMatrix.rows(), cols = iMatrix.cols();
    IMatrixOperations::vector2D vec2D(rows, std::vector<double>(cols));

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            vec2D[i][j] = iMatrix(i, j);
        }
    }

    return vec2D;
}

IMatrixOperations::vector2D EigenMatrixOperations::solveEquation(const vector2D& iCoefficients, const vector2D& iFreeMembers)
{
    // Переводим двумерные векторы в матрицу класса Eigen
    MatrixXd coefficientMatrix = vector2DToMatrix(iCoefficients);
    MatrixXd freeTermMatrix = vector2DToMatrix(iFreeMembers);

    // Решаем СЛАУ
    Eigen::MatrixXd decisionMatrix = Eigen::MatrixXd::Constant(iFreeMembers.size(), iFreeMembers[0].size(), 0);
    decisionMatrix = coefficientMatrix.lu().solve(freeTermMatrix);

    // Revert convertion
    vector2D decisionVector2D = matrixToVector2D(decisionMatrix);

    return decisionVector2D;
}

double EigenMatrixOperations::getMatrixDet(const vector2D& iMatrix)
{
    MatrixXd matrix = vector2DToMatrix(iMatrix);
    return matrix.determinant();
}

int EigenMatrixOperations::getMatrixRank(const vector2D& iMatrix)
{
    MatrixXd m = vector2DToMatrix(iMatrix);
    // Используем LU-разложение
    FullPivLU<MatrixXd> lu_decomp(m);
    return static_cast<int>(lu_decomp.rank());
}
