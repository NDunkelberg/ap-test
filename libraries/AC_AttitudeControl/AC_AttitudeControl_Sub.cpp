#include "AC_AttitudeControl_Sub.h"
#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>
#include <GCS_MAVLink/GCS.h>

// table of user settable parameters
const AP_Param::GroupInfo AC_AttitudeControl_Sub::var_info[] = {
    // parameters from parent vehicle
    AP_NESTEDGROUPINFO(AC_AttitudeControl, 0),

    // @Param: RAT_RLL_P
    // @DisplayName: Roll axis rate controller P gain
    // @Description: Roll axis rate controller P gain.  Converts the difference between desired roll rate and actual roll rate into a motor speed output
    // @Range: 0.0 0.30
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_RLL_I
    // @DisplayName: Roll axis rate controller I gain
    // @Description: Roll axis rate controller I gain.  Corrects long-term difference in desired roll rate vs actual roll rate
    // @Range: 0.0 0.5
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_RLL_IMAX
    // @DisplayName: Roll axis rate controller I gain maximum
    // @Description: Roll axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_RLL_D
    // @DisplayName: Roll axis rate controller D gain
    // @Description: Roll axis rate controller D gain.  Compensates for short-term change in desired roll rate vs actual roll rate
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FF
    // @DisplayName: Roll axis rate controller feed forward
    // @Description: Roll axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FLTT
    // @DisplayName: Roll axis rate controller input frequency in Hz
    // @Description: Roll axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    // @Param: RAT_RLL_FLTE
    // @DisplayName: Roll axis rate controller input frequency in Hz
    // @Description: Roll axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    // @Param: RAT_RLL_FLTD
    // @DisplayName: Roll axis rate controller input frequency in Hz
    // @Description: Roll axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_roll, "RAT_RLL_", 1, AC_AttitudeControl_Sub, AC_PID),

    // @Param: RAT_PIT_P
    // @DisplayName: Pitch axis rate controller P gain
    // @Description: Pitch axis rate controller P gain.  Converts the difference between desired pitch rate and actual pitch rate into a motor speed output
    // @Range: 0.0 0.30
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_PIT_I
    // @DisplayName: Pitch axis rate controller I gain
    // @Description: Pitch axis rate controller I gain.  Corrects long-term difference in desired pitch rate vs actual pitch rate
    // @Range: 0.0 0.5
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_PIT_IMAX
    // @DisplayName: Pitch axis rate controller I gain maximum
    // @Description: Pitch axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_PIT_D
    // @DisplayName: Pitch axis rate controller D gain
    // @Description: Pitch axis rate controller D gain.  Compensates for short-term change in desired pitch rate vs actual pitch rate
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FF
    // @DisplayName: Pitch axis rate controller feed forward
    // @Description: Pitch axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FLTT
    // @DisplayName: Pitch axis rate controller input frequency in Hz
    // @Description: Pitch axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    // @Param: RAT_PIT_FLTE
    // @DisplayName: Pitch axis rate controller input frequency in Hz
    // @Description: Pitch axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    // @Param: RAT_PIT_FLTD
    // @DisplayName: Pitch axis rate controller input frequency in Hz
    // @Description: Pitch axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_pitch, "RAT_PIT_", 2, AC_AttitudeControl_Sub, AC_PID),

    // @Param: RAT_YAW_P
    // @DisplayName: Yaw axis rate controller P gain
    // @Description: Yaw axis rate controller P gain.  Converts the difference between desired yaw rate and actual yaw rate into a motor speed output
    // @Range: 0.0 0.50
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_YAW_I
    // @DisplayName: Yaw axis rate controller I gain
    // @Description: Yaw axis rate controller I gain.  Corrects long-term difference in desired yaw rate vs actual yaw rate
    // @Range: 0.0 0.05
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_YAW_IMAX
    // @DisplayName: Yaw axis rate controller I gain maximum
    // @Description: Yaw axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_YAW_D
    // @DisplayName: Yaw axis rate controller D gain
    // @Description: Yaw axis rate controller D gain.  Compensates for short-term change in desired yaw rate vs actual yaw rate
    // @Range: 0.000 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FF
    // @DisplayName: Yaw axis rate controller feed forward
    // @Description: Yaw axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FLTT
    // @DisplayName: Yaw axis rate controller input frequency in Hz
    // @Description: Yaw axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    // @Param: RAT_YAW_FLTE
    // @DisplayName: Yaw axis rate controller input frequency in Hz
    // @Description: Yaw axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    // @Param: RAT_YAW_FLTD
    // @DisplayName: Yaw axis rate controller input frequency in Hz
    // @Description: Yaw axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_yaw, "RAT_YAW_", 3, AC_AttitudeControl_Sub, AC_PID),

    // @Param: THR_MIX_MIN
    // @DisplayName: Throttle Mix Minimum
    // @Description: Throttle vs attitude control prioritisation used when landing (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.1 0.25
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MIN", 4, AC_AttitudeControl_Sub, _thr_mix_min, AC_ATTITUDE_CONTROL_MIN_DEFAULT),

    // @Param: THR_MIX_MAX
    // @DisplayName: Throttle Mix Maximum
    // @Description: Throttle vs attitude control prioritisation used during active flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.5 0.9
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MAX", 5, AC_AttitudeControl_Sub, _thr_mix_max, AC_ATTITUDE_CONTROL_MAX_DEFAULT),

    // @Param: THR_MIX_MAN
    // @DisplayName: Throttle Mix Manual
    // @Description: Throttle vs attitude control prioritisation used during manual flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.5 0.9
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MAN", 6, AC_AttitudeControl_Sub, _thr_mix_man, AC_ATTITUDE_CONTROL_MAN_DEFAULT),

    // @Param: RAT_RLL_FILT
    // @DisplayName: Roll axis rate controller input frequency in Hz
    // @Description: Roll axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    // @Param: RAT_PIT_FILT
    // @DisplayName: Pitch axis rate controller input frequency in Hz
    // @Description: Pitch axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    // @Param: RAT_YAW_FILT
    // @DisplayName: Yaw axis rate controller input frequency in Hz
    // @Description: Yaw axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard

    AP_GROUPEND
};

AC_AttitudeControl_Sub::AC_AttitudeControl_Sub(AP_AHRS_View &ahrs, const AP_Vehicle::MultiCopter &aparm, AP_MotorsMulticopter& motors, float dt) :
    AC_AttitudeControl(ahrs, aparm, motors, dt),
    _motors_multi(motors),
    _pid_rate_roll(AC_ATC_SUB_RATE_RP_P, AC_ATC_SUB_RATE_RP_I, AC_ATC_SUB_RATE_RP_D, 0.0f, AC_ATC_SUB_RATE_RP_IMAX, AC_ATC_SUB_RATE_RP_FILT_HZ, 0.0f, AC_ATC_SUB_RATE_RP_FILT_HZ, dt),
    _pid_rate_pitch(AC_ATC_SUB_RATE_RP_P, AC_ATC_SUB_RATE_RP_I, AC_ATC_SUB_RATE_RP_D, 0.0f, AC_ATC_SUB_RATE_RP_IMAX, AC_ATC_SUB_RATE_RP_FILT_HZ, 0.0f, AC_ATC_SUB_RATE_RP_FILT_HZ, dt),
    _pid_rate_yaw(AC_ATC_SUB_RATE_YAW_P, AC_ATC_SUB_RATE_YAW_I, AC_ATC_SUB_RATE_YAW_D, 0.0f, AC_ATC_SUB_RATE_YAW_IMAX, AC_ATC_SUB_RATE_YAW_FILT_HZ, 0.0f, AC_ATC_SUB_RATE_YAW_FILT_HZ, dt),
    _last_yaw_err_negative(false),
    _yaw_accumulated(0.0f),
    _relax_roll(false),
    _relax_pitch(false),
    _relax_yaw(false)
{
    AP_Param::setup_object_defaults(this, var_info);

    // Sub-specific defaults for parent class
    _p_angle_roll.kP().set_default(AC_ATC_SUB_ANGLE_P);
    _p_angle_pitch.kP().set_default(AC_ATC_SUB_ANGLE_P);
    _p_angle_yaw.kP().set_default(AC_ATC_SUB_ANGLE_P);

    _accel_yaw_max.set_default(AC_ATC_SUB_ACCEL_Y_MAX);

    _pitch_error_filter.set_cutoff_frequency(AC_ATTITUDE_CONTROL_PITCH_ERROR_CUTOFF_FREQ);
    _yaw_error_filter.set_cutoff_frequency(AC_ATTITUDE_CONTROL_YAW_ERROR_CUTOFF_FREQ);

    _last_yaw = degrees(_ahrs.get_current_yaw());
}

// Update Alt_Hold angle maximum
void AC_AttitudeControl_Sub::update_althold_lean_angle_max(float throttle_in)
{
    // calc maximum tilt angle based on throttle
    float thr_max = _motors_multi.get_throttle_thrust_max();

    // divide by zero check
    if (is_zero(thr_max)) {
        _althold_lean_angle_max = 0.0f;
        return;
    }

    float althold_lean_angle_max = acosf(constrain_float(throttle_in/(AC_ATTITUDE_CONTROL_ANGLE_LIMIT_THROTTLE_MAX * thr_max), 0.0f, 1.0f));
    _althold_lean_angle_max = _althold_lean_angle_max + (_dt/(_dt+_angle_limit_tc))*(althold_lean_angle_max-_althold_lean_angle_max);
}

void AC_AttitudeControl_Sub::input_euler_roll_pitch_yaw_accumulate(float euler_roll_angle_cd, float euler_pitch_angle_offs_cd, float euler_yaw_offs_cd, float dt, bool update_target)
{
    if (update_target)
    {
        float current_roll, current_pitch, current_yaw;

        Quaternion vehicle_attitude;
        _ahrs.get_quat_body_to_ned(vehicle_attitude);
        vehicle_attitude.to_euler(current_roll, current_pitch, current_yaw);

        // update cut off frequency
        _yaw_error_filter.set_cutoff_frequency(_yaw_filter_cut_off);

        if (_last_yaw_err_negative != (euler_yaw_offs_cd < 0))
            _yaw_error_filter.reset(0.0f);
        _last_yaw_err_negative = euler_yaw_offs_cd < 0;

        // get lowpass filtered pitch and yaw errors
        float pitch_error = _pitch_error_filter.apply(euler_pitch_angle_offs_cd, dt);
        float yaw_error = _yaw_error_filter.apply(euler_yaw_offs_cd, dt);

        // take roll target angle directly from absolute input
        // get pitch and yaw target angle by accumulating input offset to current attitude
        _target_roll_cd = euler_roll_angle_cd;
        _target_pitch_cd = RadiansToCentiDegrees(current_pitch) - pitch_error;
        _target_yaw_cd = RadiansToCentiDegrees(current_yaw) - yaw_error;

        _target_yaw_cd = wrap_180_cd(_target_yaw_cd);
    }

    input_euler_angle_roll_pitch_yaw(_target_roll_cd, _target_pitch_cd, _target_yaw_cd, true);
}

void AC_AttitudeControl_Sub::keep_nose_horizontal()
{
    // Get the rotation matrix describing the vehicles attitude with regard to the inertial frame
    Quaternion vehicle_attitude;
    _ahrs.get_quat_body_to_ned(vehicle_attitude);
    Matrix3f veh_att_rot_matrix;
    vehicle_attitude.rotation_matrix(veh_att_rot_matrix);

    // z axis (inertial frame)
    Vector3f z_I = Vector3f(0.0f, 0.0f, 1.0f);
    // normalized z-axis of the body frame expressed in the inertial frame
    Vector3f z_B = veh_att_rot_matrix * z_I;
    float z_B_norm = z_B.length();
    if (z_B_norm > 0)
        z_B /= z_B_norm;

    // If the vehicle's x-axis is not in the horizontal plane, the vehicle is supposed to rotate about the yaw axis
    // until the x-axis reaches the horizontal plane.
    // The desired x-axis of the vehicle is therefore parallel to the inertial xy-plane as well as to the vehicle's xy-plane
    // Thus it can be expressed by the cross product of the normal vectors z_I and z_B of these planes

    // desired body-frame x-axis
    Vector3f x_d = z_I % z_B;
    float x_d_length = x_d.length();
    if (x_d_length > 0)
        x_d /= x_d_length;

    // the body-frame x-axis expressed with regard to the inertial frame
    Vector3f x_B = veh_att_rot_matrix * Vector3f(1.0f, 0.0f, 0.0f);

    // if the body-frame x-axis and the desired x-axis point in opposite directions, negate the desired x-axis
    if (x_d * x_B < 0.0f)
        x_d *= -1.0f;

    // x_B is supposed to be rotated until it reaches the desired (horizontal) orientation x_d
    // the rotation axis is perpendicular to both vectors, thus expressed by the cross product
    Vector3f rot_axis = x_B % x_d;

    // the angle of the necessary rotation is retrieved by the dot product
    float correction_angle = acosf(constrain_float(x_d * x_B, -1.0f, 1.0f));

    // the rotation axis and the body-frame's z-Axis are parallel but might have opposite directions
    // use sign of dot product to check for opposing directions.
    // If so, the angle is negated, so it describes the required rotation about the vehicle's z-Axis to keep the x-axis horizontal
    if (rot_axis * z_B < 0.0f)
        correction_angle = -correction_angle;

    // express the required rotation as a quaternion
    Quaternion correction_quat;
    correction_quat.from_axis_angle(z_B, correction_angle);

    // the target quarternion consists of the correcting rotation applied to the current vehicle attitude
    Quaternion target_quat = correction_quat * vehicle_attitude;

    // express the new target orientation as euler angles
    float roll, pitch, yaw;
    target_quat.to_euler(roll, pitch, yaw);

    _target_roll_cd = RadiansToCentiDegrees(roll);
    _target_pitch_cd = RadiansToCentiDegrees(pitch);
    _target_yaw_cd = RadiansToCentiDegrees(yaw);

    // call attitude controller to correct for the new target angles
    input_euler_angle_roll_pitch_yaw(_target_roll_cd, _target_pitch_cd, _target_yaw_cd, true);
}

void AC_AttitudeControl_Sub::keep_current_attitude()
{
    input_euler_angle_roll_pitch_yaw(_target_roll_cd, _target_pitch_cd, _target_yaw_cd, true);
}

void AC_AttitudeControl_Sub::set_levelled_target_attitude()
{
    // set target angles resulting in horizontal attitude
    _target_roll_cd = 0.0f;
    _target_pitch_cd = 0.0f;
    _target_yaw_cd = RadiansToCentiDegrees(_ahrs.get_current_yaw());
}

void AC_AttitudeControl_Sub::start_trajectory(Vector3f target_euler_angles_cd, uint32_t duration, bool relative)
{
    // current vehicle attitude
    Quaternion vehicle_attitude;
    _ahrs.get_quat_body_to_ned(vehicle_attitude);

    // the relative attitude difference during the trajectory
    Quaternion relative_attitude;

    if (relative)
    {
        // if relative is set to true

        // relative rotation during trajectory
        float relative_roll = radians(target_euler_angles_cd[0] * 0.01f);
        float relative_pitch = radians(target_euler_angles_cd[1] * 0.01f);
        float relative_yaw = radians(target_euler_angles_cd[2] * 0.01f);

        // relative attitude (yaw-pitch-roll-sequence about consecutively rotated axes)
        relative_attitude.from_euler(relative_roll, relative_pitch, relative_yaw);
    }
    else
    {
        // This scope (absolute rotational trajectories) is not tested yet

        // absolute target angles of trajectory
        float target_roll = radians(target_euler_angles_cd[0] * 0.01f);
        float target_pitch = radians(target_euler_angles_cd[1] * 0.01f);
        float target_yaw = radians(target_euler_angles_cd[2] * 0.01f);

        // absolute target attitude
        Quaternion absolute_target_attitude;
        absolute_target_attitude.from_euler(target_roll, target_pitch, target_yaw);

        // difference attitude
        relative_attitude = vehicle_attitude.inverse() * absolute_target_attitude;
    }

    // interprete the relative attitude change of the trajectory as a rotation about a single axis with a specific angle
    // the length of the resulting vector is equal to the angle of the rotation about this vector
    relative_attitude.to_axis_angle(_trajectory_axis_angle);

    // store current attitude as starting attitude of the trajectory
    _trajectory_start_attitude = vehicle_attitude;

    // store duration
    // if duration is set to zero, compute dynamic duration based on axis angle
    float max_duration = 12000.0f;
    _trajectory_duration_ms = duration == 0 ? static_cast<uint32_t>(max_duration / M_PI * fabs(_trajectory_axis_angle.length())) : duration;
    // store time stamp of trajectory start
    _trajectory_start_ms = AP_HAL::millis();
}

bool AC_AttitudeControl_Sub::update_trajectory()
{
    // current time stamp
    float t = static_cast<float>(AP_HAL::millis() - _trajectory_start_ms);

    if (t <= _trajectory_duration_ms)
    {
        // get axis angle from 5th order polynomial trajectory (defined in AP_Math/polynomial5.h)
        // the angle of the trajectory starts with zero and reaches it's target value along polynomial trajectory
        // the polynomial trajectories coefficients are chosen in a way, that the angular velocity of the vehicle at the
        // start and finish of the trajectory is zero, resulting in smooth acceleration and deceleration
        float current_angle;
        float start_angle = 0.0f;
        // the convention here is that the target angle of the rotation is stored as the length of the rotation axis
        float target_angle = _trajectory_axis_angle.length();
        polynomial_trajectory(current_angle, start_angle, target_angle, _trajectory_duration_ms, t);

        // calculate current target attitude
        Quaternion cur_target_attitude;

        if (!is_zero(_trajectory_axis_angle.length()))
        {
            // scale the rotation axis by the current rotation angle from the polynomial trajectory
            Vector3f cur_axis_angle = _trajectory_axis_angle / _trajectory_axis_angle.length() * current_angle;

            // retrieve the relative attitude
            Quaternion cur_rel_attitude;
            cur_rel_attitude.from_axis_angle(cur_axis_angle);

            // retrieve the current absolute target attitude (rotations applied from right to left in quaternion multiplication)
            // so the relative rotation is basically added to the vehicles starting attitude
            cur_target_attitude = _trajectory_start_attitude * cur_rel_attitude;
        }
        else
        {
            // relative rotation is zero, set target to start altitude
            cur_target_attitude = _trajectory_start_attitude;
        }

        // convert current target attitude to euler angles
        float target_roll, target_pitch, target_yaw;
        cur_target_attitude.to_euler(target_roll, target_pitch, target_yaw);

        // update the target angles for attitude controller
        _target_roll_cd = RadiansToCentiDegrees(target_roll);
        _target_pitch_cd = RadiansToCentiDegrees(target_pitch);
        _target_yaw_cd = RadiansToCentiDegrees(target_yaw);

        // perform attitude control
        input_euler_angle_roll_pitch_yaw(_target_roll_cd, _target_pitch_cd, _target_yaw_cd, true);

        return false;
    }
    else
    {
        // perform attitude control
        input_euler_angle_roll_pitch_yaw(_target_roll_cd, _target_pitch_cd, _target_yaw_cd, true);

        return true;
    }
}

void AC_AttitudeControl_Sub::set_throttle_out(float throttle_in, bool apply_angle_boost, float filter_cutoff)
{
    _throttle_in = throttle_in;
    update_althold_lean_angle_max(throttle_in);
    _motors.set_throttle_filter_cutoff(filter_cutoff);
    if (apply_angle_boost) {
        // Apply angle boost
        throttle_in = get_throttle_boosted(throttle_in);
    }else{
        // Clear angle_boost for logging purposes
        _angle_boost = 0.0f;
    }
    _motors.set_throttle(throttle_in);
    _motors.set_throttle_avg_max(get_throttle_avg_max(MAX(throttle_in, _throttle_in)));
}

// returns a throttle including compensation for roll/pitch angle
// throttle value should be 0 ~ 1
float AC_AttitudeControl_Sub::get_throttle_boosted(float throttle_in)
{
    if (!_angle_boost_enabled) {
        _angle_boost = 0;
        return throttle_in;
    }
    // inverted_factor is 1 for tilt angles below 60 degrees
    // inverted_factor reduces from 1 to 0 for tilt angles between 60 and 90 degrees

    float cos_tilt = _ahrs.cos_pitch() * _ahrs.cos_roll();
    float inverted_factor = constrain_float(2.0f*cos_tilt, 0.0f, 1.0f);
    float boost_factor = 1.0f/constrain_float(cos_tilt, 0.5f, 1.0f);

    float throttle_out = throttle_in*inverted_factor*boost_factor;
    _angle_boost = constrain_float(throttle_out - throttle_in,-1.0f,1.0f);
    return throttle_out;
}

// returns a throttle including compensation for roll/pitch angle
// throttle value should be 0 ~ 1
float AC_AttitudeControl_Sub::get_throttle_avg_max(float throttle_in)
{
    throttle_in = constrain_float(throttle_in, 0.0f, 1.0f);
    return MAX(throttle_in, throttle_in*MAX(0.0f,1.0f-_throttle_rpy_mix)+_motors.get_throttle_hover()*_throttle_rpy_mix);
}

// update_throttle_rpy_mix - slew set_throttle_rpy_mix to requested value
void AC_AttitudeControl_Sub::update_throttle_rpy_mix()
{
    // slew _throttle_rpy_mix to _throttle_rpy_mix_desired
    if (_throttle_rpy_mix < _throttle_rpy_mix_desired) {
        // increase quickly (i.e. from 0.1 to 0.9 in 0.4 seconds)
        _throttle_rpy_mix += MIN(2.0f*_dt, _throttle_rpy_mix_desired-_throttle_rpy_mix);
    } else if (_throttle_rpy_mix > _throttle_rpy_mix_desired) {
        // reduce more slowly (from 0.9 to 0.1 in 1.6 seconds)
        _throttle_rpy_mix -= MIN(0.5f*_dt, _throttle_rpy_mix-_throttle_rpy_mix_desired);
    }
    _throttle_rpy_mix = constrain_float(_throttle_rpy_mix, 0.1f, AC_ATTITUDE_CONTROL_MAX);
}

// set target euler angles to current euler angles
void AC_AttitudeControl_Sub::reset_target_attitude()
{
    // retrieve euler angles of current vehicle attitude
    float current_roll, current_pitch, current_yaw;

    Quaternion vehicle_attitude;
    _ahrs.get_quat_body_to_ned(vehicle_attitude);
    vehicle_attitude.to_euler(current_roll, current_pitch, current_yaw);

    // reset angles
    _target_roll_cd = RadiansToCentiDegrees(current_roll);
    _target_pitch_cd = RadiansToCentiDegrees(current_pitch);
    _target_yaw_cd = RadiansToCentiDegrees(current_yaw);
}

void AC_AttitudeControl_Sub::rate_controller_run()
{
    // move throttle vs attitude mixing towards desired (called from here because this is conveniently called on every iteration)
    update_throttle_rpy_mix();

    // calculate required thrust about each axis
    // if a relaxing flag is active, the according thrust is set to zero
    Vector3f gyro_latest = _ahrs.get_gyro_latest();
    float roll_out = _relax_roll ? 0.0f : get_rate_roll_pid().update_all(_rate_target_ang_vel.x, gyro_latest.x, _motors.limit.roll);
    float pitch_out = _relax_pitch ? 0.0f : get_rate_pitch_pid().update_all(_rate_target_ang_vel.y, gyro_latest.y, _motors.limit.pitch);
    float yaw_out = _relax_yaw ? 0.0f : get_rate_yaw_pid().update_all(_rate_target_ang_vel.z, gyro_latest.z, _motors.limit.yaw);

    // send the required thrust to the motors
    _motors.set_roll(roll_out);
    _motors.set_pitch(pitch_out);
    _motors.set_yaw(yaw_out);

    // if relaxation is desired, the relaxing flags need to be activated by the top level code on each iteration
    _relax_roll = false;
    _relax_pitch = false;
    _relax_yaw = false;

    control_monitor_update();

    tangling_monitor_update();
}

// sanity check parameters.  should be called once before takeoff
void AC_AttitudeControl_Sub::parameter_sanity_check()
{
    // sanity check throttle mix parameters
    if (_thr_mix_man < 0.1f || _thr_mix_man > 4.0f) {
        // parameter description recommends thr-mix-man be no higher than 0.9 but we allow up to 4.0
        // which can be useful for very high powered copters with very low hover throttle
        _thr_mix_man.set_and_save(AC_ATTITUDE_CONTROL_MAN_DEFAULT);
    }
    if (_thr_mix_min < 0.1f || _thr_mix_min > 0.25f) {
        _thr_mix_min.set_and_save(AC_ATTITUDE_CONTROL_MIN_DEFAULT);
    }
    if (_thr_mix_max < 0.5f || _thr_mix_max > AC_ATTITUDE_CONTROL_MAX) {
        // parameter description recommends thr-mix-max be no higher than 0.9 but we allow up to 5.0
        // which can be useful for very high powered copters with very low hover throttle
        _thr_mix_max.set_and_save(AC_ATTITUDE_CONTROL_MAX_DEFAULT);
    }
    if (_thr_mix_min > _thr_mix_max) {
        _thr_mix_min.set_and_save(AC_ATTITUDE_CONTROL_MIN_DEFAULT);
        _thr_mix_max.set_and_save(AC_ATTITUDE_CONTROL_MAX_DEFAULT);
    }
}

void AC_AttitudeControl_Sub::tangling_monitor_update()
{
    // current yaw (radians)
    float cur_yaw = _ahrs.get_current_yaw();

    // get difference yaw angle with regard to last measurement
    // if yaw angle jumped from 180 to -180 add 360 degrees, if yaw angle jumped from 180 to -180 subtract 360 degrees
    float delta_yaw = cur_yaw - _last_yaw;
    _last_yaw = cur_yaw;

    delta_yaw += (delta_yaw > M_PI) ? -2*M_PI : (delta_yaw < -M_PI) ? 2*M_PI : 0.0f;

    _yaw_accumulated += delta_yaw;

}
