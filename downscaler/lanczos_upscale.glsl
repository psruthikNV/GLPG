#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_16bit_storage : require
#extension GL_EXT_shader_explicit_arithmetic_types : require
layout(set=0,binding=0) uniform const_buffer
{
    float kDetectRatio;
    float kDetectThres;
    float kMinContrastRatio;
    float kRatioNorm;
    float kContrastBoost;
    float kEps;
    float kSharpStartY;
    float kSharpScaleY;
    float kSharpStrengthMin;
    float kSharpStrengthScale;
    float kSharpLimitMin;
    float kSharpLimitScale;
    float kScaleX;
    float kScaleY;
    float kDstNormX;
    float kDstNormY;
    float kSrcNormX;
    float kSrcNormY;
    uint kInputViewportOriginX;
    uint kInputViewportOriginY;
    uint kInputViewportWidth;
    uint kInputViewportHeight;
    uint kOutputViewportOriginX;
    uint kOutputViewportOriginY;
    uint kOutputViewportWidth;
    uint kOutputViewportHeight;
    float reserved0;
    float reserved1;
};
layout(set=0,binding=1) uniform sampler samplerLinearClamp;
layout(set=0,binding=2) uniform texture2D in_texture;
layout(set=0,binding=3) uniform writeonly image2D out_texture;
// Set the number of invocations in the work group.
// In this case, we operate on the image in 32x24 pixel tiles.
layout (local_size_x = 32, local_size_y = 24) in;
#define RESULT_TYPE vec4
precision highp float;
const float coef_lanczos[17][5] = {
 {0.0,          0.0,          1.0,         0.0,          0.0},
 {0.0,         -0.01953125,  1.0,          0.01953125,  0.0},
 {0.0,         -0.03515625,  0.9921875,   0.04296875,  0.0},
 {-0.00390625, -0.046875,    0.984375,    0.0703125,  -0.00390625},
 {-0.00390625, -0.05859375,  0.96875,     0.09765625, -0.00390625},
 {-0.00390625, -0.0703125,   0.953125,    0.12890625, -0.0078125},
 {-0.0078125,  -0.078125,    0.93359375,  0.1640625,  -0.01171875},
 {-0.0078125,  -0.08203125,  0.90234375,  0.19921875, -0.01171875},
 {-0.01171875, -0.0859375,   0.8828125,   0.234375,   -0.01953125},
 {-0.01171875, -0.0859375,   0.84765625,  0.2734375,  -0.0234375},
 {-0.015625,   -0.0859375,   0.8125,      0.31640625, -0.02734375},
 {-0.015625,   -0.0859375,   0.78125,     0.35546875, -0.03515625},
 {-0.01953125, -0.08203125,  0.7421875,   0.3984375,  -0.0390625},
 {-0.01953125, -0.078125,    0.703125,    0.44140625, -0.046875},
 {-0.01953125, -0.07421875,  0.65625,     0.48828125, -0.05078125},
 {-0.0234375,  -0.0703125,   0.62109375,  0.53125,    -0.05859375},
 {0.0,        -0.0625,      0.5625,      0.5625,     -0.0625}};
RESULT_TYPE filter_5tap(RESULT_TYPE p0, RESULT_TYPE p1, RESULT_TYPE p2, RESULT_TYPE p3, RESULT_TYPE p4, float phase_frac) {
    int phase_index = int(phase_frac * 32);
    if(phase_index < 0) {
        phase_index = phase_index + 32;
    }
    RESULT_TYPE filter_out;
    if(phase_index >= 16) {
        phase_index = 32 - phase_index;
        float coef[5] = coef_lanczos[phase_index];
       filter_out = p0 * coef[4] + p1 * coef[3] + p2 * coef[2] + p3 * coef[1] + p4 * coef[0];
    } else {
        float coef[5] = coef_lanczos[phase_index];
        filter_out = p0 * coef[0] + p1 * coef[1] + p2 * coef[2] + p3 * coef[3] + p4 * coef[4];
    }
    return filter_out;
}
void main() {
    // Acquire the coordinates to the texel we are to process.
    ivec2 destPosition = ivec2(gl_GlobalInvocationID.xy);
    RESULT_TYPE tap5_buff[5];
    float inc_x = 1.0 * kInputViewportWidth / kOutputViewportWidth;
    float inc_y = 1.0 * kInputViewportHeight / kOutputViewportHeight;
    float phase_x_base = inc_x / 2 + 0.5;
    float phase_y_base = inc_y / 2 + 0.5;
    // x
    float phase_x = phase_x_base + inc_x * destPosition.x;
    int x0 = int(phase_x  + 0.5);
    // y
    float phase_y = phase_y_base + inc_y * destPosition.y;
    int y0 = int(phase_y  + 0.5);
    for (int dx =-3; dx <= 1; dx ++) {
        int x_offset = 3 + dx;
        float x_pos = (x0 + dx);
        float y_pos = y0;
        RESULT_TYPE row_m2 = texelFetch(sampler2D(in_texture, samplerLinearClamp),
                    ivec2(x_pos, y_pos - 3), 0);
        RESULT_TYPE row_m1 = texelFetch(sampler2D(in_texture, samplerLinearClamp),
                    ivec2(x_pos, y_pos - 2), 0);
        RESULT_TYPE row_p0 = texelFetch(sampler2D(in_texture, samplerLinearClamp),
                    ivec2(x_pos, y_pos - 1), 0);
        RESULT_TYPE row_p1 = texelFetch(sampler2D(in_texture, samplerLinearClamp),
                   ivec2(x_pos, y_pos), 0);
        RESULT_TYPE row_p2 = texelFetch(sampler2D(in_texture, samplerLinearClamp),
                    ivec2(x_pos, y_pos + 1), 0);
        tap5_buff[x_offset] = filter_5tap(row_m2,row_m1,row_p0,row_p1,row_p2, phase_y - y0);
    }
    RESULT_TYPE result =
            filter_5tap(tap5_buff[0],tap5_buff[1], tap5_buff[2], tap5_buff[3], tap5_buff[4], phase_x - x0);
    imageStore(out_texture, destPosition, vec4(result));
}
