#pragma once

#include "../ecs/ecs.h"

typedef struct {
    float torso_corner_radius;
    float head_width_ratio;
    float head_height_ratio;
    float head_forward_offset;

    float foot_width_ratio;
    float foot_height_ratio;
    float foot_lateral_offset;
    float foot_corner_radius;

    float arm_corner_radius;

    float pistol_arm_width_ratio;
    float pistol_arm_height_ratio;
    float pistol_shoulder_x_ratio;
    float pistol_shoulder_y_offset;
    float pistol_grip_y_offset;
    float pistol_arm_aim_angle;

    float rifle_aim_torso_angle;

    float rifle_arm_width_ratio;
    float rifle_arm_height_ratio;
    float rifle_shoulder_x_ratio;
    float rifle_l_shoulder_y_offset;
    float rifle_r_shoulder_y_offset;
    float rifle_grip_y_offset;
    float rifle_l_arm_aim_angle;
    float rifle_r_arm_aim_angle;

    float rifle_l_arm_midpoint_x_scale;
    float rifle_l_arm_midpoint_y_scale;

    float walk_stride_amplitude;
    float run_stride_amplitude;
    float walk_stride_speed;
    float run_stride_speed;

    float leg_phase;
    float current_stride_offset;
} HumanoidRender;

COMPONENT_DEFINE(HumanoidRender, humanoid_render);
