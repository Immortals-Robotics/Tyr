#pragma once

#include <protos/immortals/command.pb.h>

#include <deque>
#include <fstream>
#include <math.h>
#include <numeric>
#include <Eigen/Dense>

#include <kalman/Matrix.hpp>
#include <kalman/LinearizedSystemModel.hpp>
#include <kalman/LinearizedMeasurementModel.hpp>
#include <kalman/ExtendedKalmanFilter.hpp>
