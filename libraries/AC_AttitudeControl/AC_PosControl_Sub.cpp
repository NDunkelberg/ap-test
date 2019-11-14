#include "AC_PosControl_Sub.h"
#include "AP_StereoVision/AP_StereoVision.h"

// table of user settable parameters
const AP_Param::GroupInfo AC_PosControl_Sub::var_info[] = {
    // parameters from parent vehicle
    AP_NESTEDGROUPINFO(AC_PosControl, 0),

    // @Param: DIST_P
    // @DisplayName: Distance controller P gain
    // @Description: Distance controller P gain.
    // @Range: 0.0 0.30
    // @Increment: 0.005
    // @User: Standard

    // @Param: DIST_I
    // @DisplayName: Distance controller I gain
    // @Description: Distance controller I gain.
    // @Range: 0.0 0.5
    // @Increment: 0.01
    // @User: Standard

    // @Param: DIST_IMAX
    // @DisplayName: Distance controller I gain maximum
    // @Description: Distance controller I gain maximum.
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: DIST_D
    // @DisplayName: Distance controller D gain
    // @Description: Distance controller D gain.
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: DIST_FF
    // @DisplayName: Distance controller feed forward
    // @Description: Distance controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: DIST_FILT
    // @DisplayName: Distance controller input frequency in Hz
    // @Description: Distance controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_dist, "_DIST_", 1, AC_PosControl_Sub, AC_PID),

    // @Param: MSH_CNT_P
    // @DisplayName: Mesh count controller P gain
    // @Description: Mesh count controller P gain.
    // @Range: 0.0 0.30
    // @Increment: 0.005
    // @User: Standard

    // @Param: MSH_CNT_I
    // @DisplayName: Mesh count controller I gain
    // @Description: Mesh count controller I gain.
    // @Range: 0.0 0.5
    // @Increment: 0.01
    // @User: Standard

    // @Param: MSH_CNT_IMAX
    // @DisplayName: Mesh count controller I gain maximum
    // @Description: Mesh count controller I gain maximum.
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: MSH_CNT_D
    // @DisplayName: Mesh count controller D gain
    // @Description: Mesh count controller D gain.
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: MSH_CNT_FF
    // @DisplayName: Mesh count controller feed forward
    // @Description: Mesh count controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: MSH_CNT_FILT
    // @DisplayName: Mesh count controller input frequency in Hz
    // @Description: Mesh count controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_mesh_cnt, "_MSH_CNT_", 2, AC_PosControl_Sub, AC_PID),

    // @Param: OPTFL_P
    // @DisplayName: Optical flow controller P gain
    // @Description: Optical flow controller P gain.
    // @Range: 0.0 0.30
    // @Increment: 0.005
    // @User: Standard

    // @Param: OPTFL_I
    // @DisplayName: Optical flow controller I gain
    // @Description: Optical flow controller I gain.
    // @Range: 0.0 0.5
    // @Increment: 0.01
    // @User: Standard

    // @Param: OPTFL_IMAX
    // @DisplayName: Optical flow controller I gain maximum
    // @Description: Optical flow controller I gain maximum.
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: OPTFL_D
    // @DisplayName: Optical flow controller D gain
    // @Description: Optical flow controller D gain.
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: OPTFL_FF
    // @DisplayName: Optical flow controller feed forward
    // @Description: Optical flow controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: OPTFL_FILT
    // @DisplayName: Optical flow controller input frequency in Hz
    // @Description: Optical flow controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_optfl, "_OPTFL_", 3, AC_PosControl_Sub, AC_PID),

    AP_GROUPEND
};

AC_PosControl_Sub::AC_PosControl_Sub(AP_AHRS_View& ahrs, const AP_InertialNav& inav,
                                     const AP_Motors& motors, AC_AttitudeControl& attitude_control) :
    AC_PosControl(ahrs, inav, motors, attitude_control),
    _alt_max(0.0f),
    _alt_min(0.0f),
    _pid_dist(POSCONTROL_DIST_P, POSCONTROL_DIST_I, POSCONTROL_DIST_D, POSCONTROL_DIST_IMAX, POSCONTROL_DIST_FILT_HZ, POSCONTROL_DIST_DT),
    _pid_mesh_cnt(POSCONTROL_MESH_CNT_P, POSCONTROL_MESH_CNT_I, POSCONTROL_MESH_CNT_D, POSCONTROL_MESH_CNT_IMAX, POSCONTROL_MESH_CNT_FILT_HZ, POSCONTROL_MESH_CNT_DT),
    _pid_optfl(POSCONTROL_OPTFL_P, POSCONTROL_OPTFL_I, POSCONTROL_OPTFL_D, POSCONTROL_OPTFL_IMAX, POSCONTROL_OPTFL_FILT_HZ, POSCONTROL_OPTFL_DT)
{}

/// set_alt_target_from_climb_rate - adjusts target up or down using a climb rate in cm/s
///     should be called continuously (with dt set to be the expected time between calls)
///     actual position target will be moved no faster than the speed_down and speed_up
///     target will also be stopped if the motors hit their limits or leash length is exceeded
void AC_PosControl_Sub::set_alt_target_from_climb_rate(float climb_rate_cms, float dt, bool force_descend)
{
    // adjust desired alt if motors have not hit their limits
    // To-Do: add check of _limit.pos_down?
    if ((climb_rate_cms<0 && (!_motors.limit.throttle_lower || force_descend)) || (climb_rate_cms>0 && !_motors.limit.throttle_upper && !_limit.pos_up)) {
        _pos_target.z += climb_rate_cms * dt;
    }

    // do not let target alt get above limit
    if (_alt_max < 100 && _pos_target.z > _alt_max) {
        _pos_target.z = _alt_max;
        _limit.pos_up = true;
    }

    // do not let target alt get below limit
    if (_alt_min < 0 && _alt_min < _alt_max && _pos_target.z < _alt_min) {
        _pos_target.z = _alt_min;
        _limit.pos_down = true;
    }

    // do not use z-axis desired velocity feed forward
    // vel_desired set to desired climb rate for reporting and land-detector
    _flags.use_desvel_ff_z = false;
    _vel_desired.z = climb_rate_cms;
}

/// set_alt_target_from_climb_rate_ff - adjusts target up or down using a climb rate in cm/s using feed-forward
///     should be called continuously (with dt set to be the expected time between calls)
///     actual position target will be moved no faster than the speed_down and speed_up
///     target will also be stopped if the motors hit their limits or leash length is exceeded
///     set force_descend to true during landing to allow target to move low enough to slow the motors
void AC_PosControl_Sub::set_alt_target_from_climb_rate_ff(float climb_rate_cms, float dt, bool force_descend)
{
    // calculated increased maximum acceleration if over speed
    float accel_z_cms = _accel_z_cms;
    if (_vel_desired.z < _speed_down_cms && !is_zero(_speed_down_cms)) {
        accel_z_cms *= POSCONTROL_OVERSPEED_GAIN_Z * _vel_desired.z / _speed_down_cms;
    }
    if (_vel_desired.z > _speed_up_cms && !is_zero(_speed_up_cms)) {
        accel_z_cms *= POSCONTROL_OVERSPEED_GAIN_Z * _vel_desired.z / _speed_up_cms;
    }
    accel_z_cms = constrain_float(accel_z_cms, 0.0f, 750.0f);

    // jerk_z is calculated to reach full acceleration in 1000ms.
    float jerk_z = accel_z_cms * POSCONTROL_JERK_RATIO;

    float accel_z_max = MIN(accel_z_cms, safe_sqrt(2.0f*fabsf(_vel_desired.z - climb_rate_cms)*jerk_z));

    _accel_last_z_cms += jerk_z * dt;
    _accel_last_z_cms = MIN(accel_z_max, _accel_last_z_cms);

    float vel_change_limit = _accel_last_z_cms * dt;
    _vel_desired.z = constrain_float(climb_rate_cms, _vel_desired.z-vel_change_limit, _vel_desired.z+vel_change_limit);
    _flags.use_desvel_ff_z = true;

    // adjust desired alt if motors have not hit their limits
    // To-Do: add check of _limit.pos_down?
    if ((_vel_desired.z<0 && (!_motors.limit.throttle_lower || force_descend)) || (_vel_desired.z>0 && !_motors.limit.throttle_upper && !_limit.pos_up)) {
        _pos_target.z += _vel_desired.z * dt;
    }

    // do not let target alt get above limit
    if (_alt_max < 100 && _pos_target.z > _alt_max) {
        _pos_target.z = _alt_max;
        _limit.pos_up = true;
        // decelerate feed forward to zero
        _vel_desired.z = constrain_float(0.0f, _vel_desired.z-vel_change_limit, _vel_desired.z+vel_change_limit);
    }

    // do not let target alt get below limit
    if (_alt_min < 0 && _alt_min < _alt_max && _pos_target.z < _alt_min) {
        _pos_target.z = _alt_min;
        _limit.pos_down = true;
        // decelerate feed forward to zero
        _vel_desired.z = constrain_float(0.0f, _vel_desired.z-vel_change_limit, _vel_desired.z+vel_change_limit);
    }
}

/// relax_alt_hold_controllers - set all desired and targets to measured
void AC_PosControl_Sub::relax_alt_hold_controllers()
{
    // set altitude buffer;
    float alt_buffer = _inav.get_velocity_z() * _alt_brake_tc;
    _pos_target.z = _inav.get_altitude() + alt_buffer;
    _vel_desired.z = 0.0f;
    _flags.use_desvel_ff_z = false;
    _vel_target.z = _inav.get_velocity_z();
    _vel_last.z = _inav.get_velocity_z();
    _accel_desired.z = 0.0f;
    _accel_last_z_cms = 0.0f;
    _flags.reset_rate_to_accel_z = true;
    _accel_target.z = -(_ahrs.get_accel_ef_blended().z + GRAVITY_MSS) * 100.0f;
    _pid_accel_z.reset_filter();
}

void AC_PosControl_Sub::update_dist_controller(float& target_forward, float distance_error, float dt, bool update)
{
    // simple pid controller for distance control
    // todo: check if useful to use implemented xy-position controller
    float p, i, d;

    if (update)
    {
        _pid_dist.set_dt(dt);
        _pid_dist.set_input_filter_all(distance_error);
    }

    // separately calculate p, i, d values for logging
    // probably better to use square root constrain
    p = _pid_dist.get_p();
    p = constrain_float(p, -POSCONTROL_DIST_PMAX, POSCONTROL_DIST_PMAX);

    // get i term
    i = _pid_dist.get_i();

    // get d term
    d = _pid_dist.get_d();

    target_forward = p + i + d;

    // set the cutoff frequency of the motors forward input filter
    _motors.set_forward_filter_cutoff(POSCONTROL_FORWARD_CUTOFF_FREQ);
}

void AC_PosControl_Sub::update_mesh_cnt_controller(float& target_forward, float mesh_cnt_error, float dt, bool update)
{
    // simple pid controller for distance control
    // todo: check if useful to use implemented xy-position controller
    float p, i, d;

    if (update)
    {
        _pid_mesh_cnt.set_dt(dt);
        _pid_mesh_cnt.set_input_filter_all(mesh_cnt_error);
    }

    // separately calculate p, i, d values for logging
    // probably better to use square root constrain
    p = _pid_mesh_cnt.get_p();
    p = constrain_float(p, -POSCONTROL_MESH_CNT_PMAX, POSCONTROL_MESH_CNT_PMAX);

    // get i term
    i = _pid_mesh_cnt.get_i();

    // get d term
    d = _pid_mesh_cnt.get_d();

    target_forward = p + i + d;

    // set the cutoff frequency of the motors forward input filter
    _motors.set_forward_filter_cutoff(POSCONTROL_FORWARD_CUTOFF_FREQ);
}


void AC_PosControl_Sub::update_optfl_controller(float& target_lateral, float optfl_error, float dt, bool update)
{
    // simple pid controller for optfl control
    float p, i, d;

    if (update)
    {
        _pid_optfl.set_dt(dt);
        _pid_optfl.set_input_filter_all(optfl_error);
    }

    // probably better to use square root constrain
    p = _pid_optfl.get_p();
    p = constrain_float(p, -POSCONTROL_OPTFL_PMAX, POSCONTROL_OPTFL_PMAX);

    // get i term
    i = _pid_optfl.get_i();

    // get d term
    d = _pid_optfl.get_d();

    target_lateral = p + i + d;

    // set the cutoff frequency of the motors lateral input filter
    _motors.set_lateral_filter_cutoff(POSCONTROL_LATERAL_CUTOFF_FREQ);
}
