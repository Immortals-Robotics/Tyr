// FilteredObject.h: interface for the FilteredObject class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

/**
 * Class FilteredObject
 * This filtered object class gives the predicted position and velocity using Kalman Filter
 * @author Chee Yong Lee
 * Developed from Matlab Code filtDesign written by Raffaello D'Andrea.
 * The updating equation is as follows :
 * filtOut   = Cimp*filtState + Dimp*posMeasure
 * filtState = Aimp*filtState + Bimp*posMeasure
 *
 * on loss of future information
 * filtState = inv(I-Aimp)Bimp + posMeasure, where we denote lossVec = inv(I-Aimp)Bimp
 * Date : June 2002
 **/

namespace Tyr::Vision
{
class FilteredObject
{
public:
    /// Default constructor
    FilteredObject() = default;
    /// Constructor with initialization matrices fed in
    FilteredObject(float A[2][2], float B[2], float C[2][2], float D[2], float lossMat[2]);
    /// Constructor from a file
    FilteredObject(std::filesystem::path t_filename1, std::filesystem::path t_filename2);

    /// Initialize the position whenever it is lost and refound. Use this for the first initial state too.
    void initializePos(Common::Vec2 t_pos);

    /** update the internal state using known vision data
     * @param t_in_pos is the measured position.
     * @param t_out_pos is a pointer to whether the output position is output to
     * @param t_out_vel is a pointer to whether the output velocity is output to
     * @return filtered position and velocity in param filtOut
     **/
    void updatePosition(Common::Vec2 t_in_pos, Common::Vec2 *t_out_pos, Common::Vec2 *t_out_vel);

private:
    /** FilterState : Internal representation for Kalman filter
     * The first component represent the dimension. 0 for x axis, 1 for y axis
     * The second component represents the current position and velocity component
     * This is a column vector
     **/
    float filtState[2][2];
    float filtStateP[2][2];

    //./ filtered matrices that is determined by matlab
    float Aimp[2][2], Bimp[2], Cimp[2][2], Dimp[2], lossVec[2];
    float CimpInv[2][2];
    float AimpP[2][2], BimpP[2], CimpP[2][2], DimpP[2], lossVecP[2];

    /// The number of time it has been used
    long usageCount = 0;
};
} // namespace Tyr::Vision
