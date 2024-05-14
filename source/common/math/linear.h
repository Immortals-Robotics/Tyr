// GNU General Public License Agreement
// Copyright (C) 2004-2010 CodeCogs, Zyba Ltd, Broadwood, Holford, TA5 1DU, England.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by CodeCogs.
// You must retain a copy of this licence in all copies.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------
//! Calculates the linear regression parameters and evaluates the regression line at arbitrary abscissas

#pragma once

namespace Tyr::Common
{
//! Given a set of points, this class calculates the linear regression parameters and evaluates the regression line at
//! arbitrary abscissas.
class Linear
{
public:
    void calculate(int n, float *x, float *y);

    //! Evaluates the linear regression function at the given abscissa.
    inline float getValue(float x)
    {
        return m_a + m_b * x;
    }

    //! Returns the slope of the regression line
    inline float getSlope()
    {
        return m_b;
    }

    //! Returns the intercept on the Y axis of the regression line
    inline float getIntercept()
    {
        return m_a;
    }

    //! Returns the linear regression coefficient
    inline float getCoefficient()
    {
        return m_coeff;
    }

    inline bool isAmoodi()
    {
        return amoodi;
    }

    inline float getXIntercept()
    {
        return xinter;
    }

    inline float getDisToPoint(Vec2 p)
    {
        if (amoodi)
            return std::fabs(p.x - xinter);
        return std::fabs(m_b * p.x - p.y + m_a) / sqrt(m_b * m_b + 1.0);
    }

    inline void chapeKon()
    {
        m_a = -m_a / m_b;
        m_b = 1.0f / m_b;
    }

private:
    float m_a, m_b, m_coeff, xinter;
    bool  amoodi;
};
} // namespace Tyr::Common
