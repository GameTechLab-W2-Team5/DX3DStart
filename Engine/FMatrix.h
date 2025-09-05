#pragma once
struct FMatrix
{
    float M[4][4] = { 0, };

    // 행렬 곱셈
    FMatrix operator*(const FMatrix& rhs) const
    {
        FMatrix result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.M[i][j] =
                    M[i][0] * rhs.M[0][j] +
                    M[i][1] * rhs.M[1][j] +
                    M[i][2] * rhs.M[2][j] +
                    M[i][3] * rhs.M[3][j];
            }
        }
        return result;
    }

    // Transpose 함수 - 행과 열을 바꿈
    FMatrix Transpose() const
    {
        FMatrix result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.M[i][j] = M[j][i];  // 행과 열 인덱스 교환
            }
        }
        return result;
    }

    // Static 버전 (선택사항)
    static FMatrix Transpose(const FMatrix& matrix)
    {
        return matrix.Transpose();
    }

    // Translation 행렬 생성 유틸리티 함수 (편의용)
    static FMatrix CreateTranslation(float x, float y, float z)
    {
        return { {
            {1, 0, 0, x},
            {0, 1, 0, y},
            {0, 0, 1, z},
            {0, 0, 0, 1}
        } };
    }

    // Scale 행렬 생성 유틸리티 함수 (편의용)
    static FMatrix CreateScale(float x, float y, float z)
    {
        return { {
            {x, 0, 0, 0},
            {0, y, 0, 0},
            {0, 0, z, 0},
            {0, 0, 0, 1}
        } };
    }
};