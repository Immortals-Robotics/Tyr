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

void FilteredObject::initialize(const char *filename1, const char *filename2)
{
    // open the file 1
    std::ifstream matFile(filename1);
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
        usageCount = 0;

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
        Common::logError("Invalid initialization file 1");
    }

    // open the file 2
    std::ifstream matFile2(filename2);
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
        Common::logError("Invalid initialization file 2");
    }
}

void FilteredObject::initializePos(const Common::Vec2 t_pos)
{
    const Common::Vec2 pos_conv = t_pos / 10.0f;

    filtState[0][0] = lossVec[0] * pos_conv.x;
    filtState[0][1] = lossVec[1] * pos_conv.x;
    filtState[1][0] = lossVec[0] * pos_conv.y;
    filtState[1][1] = lossVec[1] * pos_conv.y;

    filtStateP[0][0] = lossVecP[0] * pos_conv.x;
    filtStateP[0][1] = lossVecP[1] * pos_conv.x;
    filtStateP[1][0] = lossVecP[0] * pos_conv.y;
    filtStateP[1][1] = lossVecP[1] * pos_conv.y;
}

void FilteredObject::updatePosition(const Common::Vec2 t_in_pos, Common::Vec2 *t_out_pos, Common::Vec2 *t_out_vel)
{
    float filtOutP[2][2];

    if (usageCount == 0)
        initializePos(t_in_pos);
    usageCount++;

    const Common::Vec2 in_pos_conv = t_in_pos / 10.0f;

    /***********************************************
     filtOut   = Cimp*filtState + Dimp*posMeasure
    ************************************************/
    // x axis
    t_out_pos->x   = Cimp[0][0] * filtState[0][0] + Cimp[0][1] * filtState[0][1] + Dimp[0] * in_pos_conv.x;
    t_out_vel->x   = Cimp[1][0] * filtState[0][0] + Cimp[1][1] * filtState[0][1] + Dimp[1] * in_pos_conv.x;
    filtOutP[0][0] = CimpP[0][0] * filtStateP[0][0] + CimpP[0][1] * filtStateP[0][1] + DimpP[0] * in_pos_conv.x;
    filtOutP[0][1] = CimpP[1][0] * filtStateP[0][0] + CimpP[1][1] * filtStateP[0][1] + DimpP[1] * in_pos_conv.x;

    // y axis
    t_out_pos->y   = Cimp[0][0] * filtState[1][0] + Cimp[0][1] * filtState[1][1] + Dimp[0] * in_pos_conv.y;
    t_out_vel->y   = Cimp[1][0] * filtState[1][0] + Cimp[1][1] * filtState[1][1] + Dimp[1] * in_pos_conv.y;
    filtOutP[1][0] = CimpP[0][0] * filtStateP[1][0] + CimpP[0][1] * filtStateP[1][1] + DimpP[0] * in_pos_conv.y;
    filtOutP[1][1] = CimpP[1][0] * filtStateP[1][0] + CimpP[1][1] * filtStateP[1][1] + DimpP[1] * in_pos_conv.y;

    // add in the check to revert to the fast filter state if certain conditions are met
    // filtState = CimpInv * (filtOutP - Dimp*z)
    // x-axis
    if (std::fabs(t_out_vel->x - filtOutP[0][1]) > VELOCITY_THRES)
    {
        // x-axis
        filtState[0][0] = CimpInv[0][0] * (filtOutP[0][0] - Dimp[0] * in_pos_conv.x) +
                          CimpInv[0][1] * (filtOutP[0][1] - Dimp[1] * in_pos_conv.x);
        filtState[0][1] = CimpInv[1][0] * (filtOutP[0][0] - Dimp[0] * in_pos_conv.x) +
                          CimpInv[1][1] * (filtOutP[0][1] - Dimp[1] * in_pos_conv.x);
        // since there is a change in filter state, update output again
        // x-axis
        t_out_pos->x = Cimp[0][0] * filtState[0][0] + Cimp[0][1] * filtState[0][1] + Dimp[0] * in_pos_conv.x;
        t_out_vel->x = Cimp[1][0] * filtState[0][0] + Cimp[1][1] * filtState[0][1] + Dimp[1] * in_pos_conv.x;
    }

    if (std::fabs(t_out_vel->y - filtOutP[1][1]) > VELOCITY_THRES)
    {
        // y-axis
        filtState[1][0] = CimpInv[0][0] * (filtOutP[1][0] - Dimp[0] * in_pos_conv.y) +
                          CimpInv[0][1] * (filtOutP[1][1] - Dimp[1] * in_pos_conv.y);
        filtState[1][1] = CimpInv[1][0] * (filtOutP[1][0] - Dimp[0] * in_pos_conv.y) +
                          CimpInv[1][1] * (filtOutP[1][1] - Dimp[1] * in_pos_conv.y);

        // y axis
        t_out_pos->y = Cimp[0][0] * filtState[1][0] + Cimp[0][1] * filtState[1][1] + Dimp[0] * in_pos_conv.y;
        t_out_vel->y = Cimp[1][0] * filtState[1][0] + Cimp[1][1] * filtState[1][1] + Dimp[1] * in_pos_conv.y;
    }

    (*t_out_pos) *= 10.0f;
    (*t_out_vel) *= 10.0f;

    /***********************************************
    filtState = Aimp*filtState + Bimp*posMeasure
    ************************************************/
    // x axis

    // Aimp*filtState + Bimp*posMeasure

    // remember temporary matrix fast and applying the update
    float tempfiltState[2][2];
    float tempfiltStateP[2][2];

    tempfiltState[0][0]  = Aimp[0][0] * filtState[0][0] + Aimp[0][1] * filtState[0][1] + Bimp[0] * in_pos_conv.x;
    tempfiltState[0][1]  = Aimp[1][0] * filtState[0][0] + Aimp[1][1] * filtState[0][1] + Bimp[1] * in_pos_conv.x;
    tempfiltStateP[0][0] = AimpP[0][0] * filtStateP[0][0] + AimpP[0][1] * filtStateP[0][1] + BimpP[0] * in_pos_conv.x;
    tempfiltStateP[0][1] = AimpP[1][0] * filtStateP[0][0] + AimpP[1][1] * filtStateP[0][1] + BimpP[1] * in_pos_conv.x;

    // y axis
    tempfiltState[1][0]  = Aimp[0][0] * filtState[1][0] + Aimp[0][1] * filtState[1][1] + Bimp[0] * in_pos_conv.y;
    tempfiltState[1][1]  = Aimp[1][0] * filtState[1][0] + Aimp[1][1] * filtState[1][1] + Bimp[1] * in_pos_conv.y;
    tempfiltStateP[1][0] = AimpP[0][0] * filtStateP[1][0] + AimpP[0][1] * filtStateP[1][1] + BimpP[0] * in_pos_conv.y;
    tempfiltStateP[1][1] = AimpP[1][0] * filtStateP[1][0] + AimpP[1][1] * filtStateP[1][1] + BimpP[1] * in_pos_conv.y;

    // copy the components of the temp back to the actual

    filtState[0][0] = tempfiltState[0][0];
    filtState[0][1] = tempfiltState[0][1];
    filtState[1][0] = tempfiltState[1][0];
    filtState[1][1] = tempfiltState[1][1];

    filtStateP[0][0] = tempfiltStateP[0][0];
    filtStateP[0][1] = tempfiltStateP[0][1];
    filtStateP[1][0] = tempfiltStateP[1][0];
    filtStateP[1][1] = tempfiltStateP[1][1];
}
} // namespace Tyr::Vision
