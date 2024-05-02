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
     **/
    void updatePosition(Common::Vec2 t_pos);

    Common::Vec2 getPosition() const
    {
        return m_state_pos_out * 10.0f;
    }

    Common::Vec2 getVelocity() const
    {
        return m_state_vel_out * 10.0f;
    }

private:
    // FilterState : Internal representation for Kalman filter
    Common::Vec2 m_state_pos;
    Common::Vec2 m_state_vel;

    Common::Vec2 m_state_pos_p;
    Common::Vec2 m_state_vel_p;

    Common::Vec2 m_state_pos_out;
    Common::Vec2 m_state_vel_out;

    //./ filtered matrices that is determined by matlab
    float Aimp[2][2], Bimp[2], Cimp[2][2], Dimp[2], lossVec[2];
    float CimpInv[2][2];
    float AimpP[2][2], BimpP[2], CimpP[2][2], DimpP[2], lossVecP[2];

    /// The number of time it has been used
    long usageCount = 0;
};
} // namespace Tyr::Vision
