#ifndef READER_H
#define READER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stddef.h>

#include "data_lite.h"

void read_bytes(const uint8_t *buffer, size_t *pos, void *data, size_t count);

void read_bits8(const uint8_t *buffer, size_t *pos, struct bits8_t *data);

void read_bits16(const uint8_t *buffer, size_t *pos, struct bits16_t *data);

void read_bits32(const uint8_t *buffer, size_t *pos, struct bits32_t *data);

void read_uint8(const uint8_t *buffer, size_t *pos, uint8_t *data);

void read_uint16(const uint8_t *buffer, size_t *pos, uint16_t *data);

void read_uint32(const uint8_t *buffer, size_t *pos, uint32_t *data);

void read_float_h(const uint8_t *buffer, size_t *pos, union FLOAT_32 *data);

void read_float(const uint8_t *buffer, size_t *pos, union FLOAT_32 *data);

void read_v2f(const uint8_t *buffer, size_t *pos, struct Vector2f_V2 *data);

void read_v2f_h(const uint8_t *buffer, size_t *pos, struct Vector2f_V2 *data);

void read_v3f(const uint8_t *buffer, size_t *pos, struct Vector3f *data);

void read_v3f_h(const uint8_t *buffer, size_t *pos, struct Vector3f *data);

void read_v4f(const uint8_t *buffer, size_t *pos, struct Vector4f *data);

void read_v4f_h(const uint8_t *buffer, size_t *pos, struct Vector4f *data);

uint8_t read_robot_command_fixed(const uint8_t *buffer, size_t size, struct RobotCommand *data);

uint8_t read_robot_command_fixed_V2(const uint8_t *buffer, size_t size, struct RobotCommand_V2 *data);

uint8_t read_robot_config_fixed(const uint8_t *buffer, size_t size, struct RobotConfig *data);

uint8_t read_robot_matrix_fixed(const uint8_t *buffer, size_t size, struct RobotMatrix *data);

uint8_t read_robot_feedback_fixed(const uint8_t *buffer, size_t size, struct RobotFeedback *data);

uint8_t read_robot_feedback_custom_fixed(const uint8_t *buffer, size_t size, struct RobotFeedbackCustom *data);

void read_float_h_from_2x_buf(const uint8_t *buffer, float *float_out);

#ifdef __cplusplus
}
#endif

#endif
