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
    [[nodiscard]] static Linear calculate(int t_n, const float *t_x, const float *t_y);

    //! Evaluates the linear regression function at the given abscissa.
    [[nodiscard]] float getValue(const float t_x) const
    {
        return m_a + m_b * t_x;
    }

    //! Returns the slope of the regression line
    [[nodiscard]] float getSlope() const
    {
        return m_b;
    }

    //! Returns the intercept on the Y axis of the regression line
    [[nodiscard]] float getIntercept() const
    {
        return m_a;
    }

    //! Returns the linear regression coefficient
    [[nodiscard]] float getCoefficient() const
    {
        return m_coeff;
    }

    [[nodiscard]] bool isAmoodi() const
    {
        return m_amoodi;
    }

    [[nodiscard]] float getXIntercept() const
    {
        return m_xinter;
    }

    [[nodiscard]] float getDisToPoint(const Vec2 t_p) const
    {
        if (m_amoodi)
            return std::fabs(t_p.x - m_xinter);
        return std::fabs(m_b * t_p.x - t_p.y + m_a) / sqrt(m_b * m_b + 1.0f);
    }

    void chapeKon()
    {
        m_a = -m_a / m_b;
        m_b = 1.0f / m_b;
    }

private:
    float m_a      = 0.0f;
    float m_b      = 0.0f;
    float m_coeff  = 0.0f;
    float m_xinter = 0.0f;

    bool m_amoodi = false;
};
} // namespace Tyr::Common
