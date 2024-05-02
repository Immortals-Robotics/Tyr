// FilteredObject.cpp: implementation of the FilteredObject class.
//
//////////////////////////////////////////////////////////////////////

#include "filtered_object.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const float VELOCITY_THRES = 0.5;

namespace Tyr::Vision
{
FilteredObject::FilteredObject(float A[2][2], float B[2], float C[2][2], float D[2], float lossMat[2])
{
    // setup up the initial variables
    for (int i = 0; i < 2; i++)
    {
        Bimp[i]    = B[i];
        Dimp[i]    = D[i];
        lossVec[i] = lossMat[i];
        for (int k = 0; k < 2; k++)
        {
            Aimp[i][k] = A[i][k];
            Cimp[i][k] = C[i][k];
        }
    }
}

FilteredObject::FilteredObject(const std::filesystem::path t_filename1, const std::filesystem::path t_filename2)
{
    // open the file 1
    std::ifstream matFile(t_filename1);
    if (matFile.is_open())
    {
        matFile >> Aimp[0][0];
        matFile >> Aimp[0][1];
        matFile >> Aimp[1][0];
        matFile >> Aimp[1][1];
        matFile >> Bimp[0];
        matFile >> Bimp[1];
        matFile >> Cimp[0][0];
        matFile >> Cimp[0][1];
        matFile >> Cimp[1][0];
        matFile >> Cimp[1][1];
        matFile >> Dimp[0];
        matFile >> Dimp[1];
        matFile >> lossVec[0];
        matFile >> lossVec[1];
        // close the file
        matFile.close();

        // calculate the inverse of C matrix
        // inv(C)= inv(a b) =  (1/(ad-bc)) (d  -b)
        //            (c d)                (-c  a)

        float determinant = 1.0f / (Cimp[0][0] * Cimp[1][1] - Cimp[0][1] * Cimp[1][0]);
        CimpInv[0][0]     = determinant * Cimp[1][1];
        CimpInv[0][1]     = -determinant * Cimp[0][1];
        CimpInv[1][0]     = -determinant * Cimp[1][0];
        CimpInv[1][1]     = determinant * Cimp[0][0];
    }
    else
    {
        Common::logError("Invalid initialization file 1 from {}", t_filename1.string());
    }

    // open the file 2
    std::ifstream matFile2(t_filename2);
    if (matFile2.is_open())
    {
        matFile2 >> AimpP[0][0];
        matFile2 >> AimpP[0][1];
        matFile2 >> AimpP[1][0];
        matFile2 >> AimpP[1][1];
        matFile2 >> BimpP[0];
        matFile2 >> BimpP[1];
        matFile2 >> CimpP[0][0];
        matFile2 >> CimpP[0][1];
        matFile2 >> CimpP[1][0];
        matFile2 >> CimpP[1][1];
        matFile2 >> DimpP[0];
        matFile2 >> DimpP[1];
        matFile2 >> lossVecP[0];
        matFile2 >> lossVecP[1];
        // close the file
        matFile2.close();
    }
    else
    {
        Common::logError("Invalid initialization file 2 from {}", t_filename2.string());
    }
}

void FilteredObject::initializePos(Common::Vec2 t_pos)
{
    t_pos /= 10.0f;

    m_state_pos.x = lossVec[0] * t_pos.x;
    m_state_vel.x = lossVec[1] * t_pos.x;
    m_state_pos.y = lossVec[0] * t_pos.y;
    m_state_vel.y = lossVec[1] * t_pos.y;

    m_state_pos_p.x = lossVecP[0] * t_pos.x;
    m_state_vel_p.x = lossVecP[1] * t_pos.x;
    m_state_pos_p.y = lossVecP[0] * t_pos.y;
    m_state_vel_p.y = lossVecP[1] * t_pos.y;

    m_initialized = true;
}

void FilteredObject::updatePosition(Common::Vec2 t_pos)
{
    Common::Vec2 filt_out_pos_p{};
    Common::Vec2 filt_out_vel_p{};

    if (!m_initialized)
        initializePos(t_pos);

    t_pos /= 10.0f;

    /***********************************************
     filtOut   = Cimp*m_state + Dimp*posMeasure
    ************************************************/
    // x axis
    m_state_pos_out.x = Cimp[0][0] * m_state_pos.x + Cimp[0][1] * m_state_vel.x + Dimp[0] * t_pos.x;
    m_state_vel_out.x = Cimp[1][0] * m_state_pos.x + Cimp[1][1] * m_state_vel.x + Dimp[1] * t_pos.x;

    filt_out_pos_p.x = CimpP[0][0] * m_state_pos_p.x + CimpP[0][1] * m_state_vel_p.x + DimpP[0] * t_pos.x;
    filt_out_vel_p.x = CimpP[1][0] * m_state_pos_p.x + CimpP[1][1] * m_state_vel_p.x + DimpP[1] * t_pos.x;

    // y axis
    m_state_pos_out.y = Cimp[0][0] * m_state_pos.y + Cimp[0][1] * m_state_vel.y + Dimp[0] * t_pos.y;
    m_state_vel_out.y = Cimp[1][0] * m_state_pos.y + Cimp[1][1] * m_state_vel.y + Dimp[1] * t_pos.y;

    filt_out_pos_p.y = CimpP[0][0] * m_state_pos_p.y + CimpP[0][1] * m_state_vel_p.y + DimpP[0] * t_pos.y;
    filt_out_vel_p.y = CimpP[1][0] * m_state_pos_p.y + CimpP[1][1] * m_state_vel_p.y + DimpP[1] * t_pos.y;

    // add in the check to revert to the fast filter state if certain conditions are met
    // m_state = CimpInv * (filtOutP - Dimp*z)
    // x-axis
    if (std::fabs(m_state_vel_out.x - filt_out_vel_p.x) > VELOCITY_THRES)
    {
        // x-axis
        m_state_pos.x = CimpInv[0][0] * (filt_out_pos_p.x - Dimp[0] * t_pos.x) +
                        CimpInv[0][1] * (filt_out_vel_p.x - Dimp[1] * t_pos.x);
        m_state_vel.x = CimpInv[1][0] * (filt_out_pos_p.x - Dimp[0] * t_pos.x) +
                        CimpInv[1][1] * (filt_out_vel_p.x - Dimp[1] * t_pos.x);
        // since there is a change in filter state, update output again
        // x-axis
        m_state_pos_out.x = Cimp[0][0] * m_state_pos.x + Cimp[0][1] * m_state_vel.x + Dimp[0] * t_pos.x;
        m_state_vel_out.x = Cimp[1][0] * m_state_pos.x + Cimp[1][1] * m_state_vel.x + Dimp[1] * t_pos.x;
    }

    if (std::fabs(m_state_vel_out.y - filt_out_vel_p.y) > VELOCITY_THRES)
    {
        // y-axis
        m_state_pos.y = CimpInv[0][0] * (filt_out_pos_p.y - Dimp[0] * t_pos.y) +
                        CimpInv[0][1] * (filt_out_vel_p.y - Dimp[1] * t_pos.y);
        m_state_vel.y = CimpInv[1][0] * (filt_out_pos_p.y - Dimp[0] * t_pos.y) +
                        CimpInv[1][1] * (filt_out_vel_p.y - Dimp[1] * t_pos.y);

        // y axis
        m_state_pos_out.y = Cimp[0][0] * m_state_pos.y + Cimp[0][1] * m_state_vel.y + Dimp[0] * t_pos.y;
        m_state_vel_out.y = Cimp[1][0] * m_state_pos.y + Cimp[1][1] * m_state_vel.y + Dimp[1] * t_pos.y;
    }

    /***********************************************
    m_state = Aimp*m_state + Bimp*posMeasure
    ************************************************/
    // x axis

    // Aimp*m_state + Bimp*posMeasure

    m_state_pos.x   = Aimp[0][0] * m_state_pos.x + Aimp[0][1] * m_state_vel.x + Bimp[0] * t_pos.x;
    m_state_vel.x   = Aimp[1][0] * m_state_pos.x + Aimp[1][1] * m_state_vel.x + Bimp[1] * t_pos.x;
    m_state_pos_p.x = AimpP[0][0] * m_state_pos_p.x + AimpP[0][1] * m_state_vel_p.x + BimpP[0] * t_pos.x;
    m_state_vel_p.x = AimpP[1][0] * m_state_pos_p.x + AimpP[1][1] * m_state_vel_p.x + BimpP[1] * t_pos.x;

    // y axis
    m_state_pos.y   = Aimp[0][0] * m_state_pos.y + Aimp[0][1] * m_state_vel.y + Bimp[0] * t_pos.y;
    m_state_vel.y   = Aimp[1][0] * m_state_pos.y + Aimp[1][1] * m_state_vel.y + Bimp[1] * t_pos.y;
    m_state_pos_p.y = AimpP[0][0] * m_state_pos_p.y + AimpP[0][1] * m_state_vel_p.y + BimpP[0] * t_pos.y;
    m_state_vel_p.y = AimpP[1][0] * m_state_pos_p.y + AimpP[1][1] * m_state_vel_p.y + BimpP[1] * t_pos.y;
}
} // namespace Tyr::Vision
