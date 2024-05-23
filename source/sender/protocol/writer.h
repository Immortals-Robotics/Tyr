#ifndef WRITER_H
#define WRITER_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <stddef.h>

#include "data_lite.h"

void write_bytes(uint8_t *buffer, size_t *pos, const void *data, size_t count);

void write_bits8(uint8_t *buffer, size_t *pos, const struct bits8_t *data);

void write_bits16(uint8_t *buffer, size_t *pos, const struct bits16_t *data);

void write_bits32(uint8_t *buffer, size_t *pos, const struct bits32_t *data);

void write_uint8(uint8_t *buffer, size_t *pos, uint8_t data);

void write_uint16(uint8_t *buffer, size_t *pos, uint16_t data);

void write_uint32(uint8_t *buffer, size_t *pos, uint32_t data);

void write_float_h(uint8_t *buffer, size_t *pos, union FLOAT_32 data);

void write_float(uint8_t *buffer, size_t *pos, union FLOAT_32 data);

void write_v2f_h(uint8_t *buffer, size_t *pos, const struct Vector2f_V2 *data);

void write_v2f(uint8_t *buffer, size_t *pos, const struct Vector2f_V2 *data);

void write_v3f_h(uint8_t *buffer, size_t *pos, const struct Vector3f *data);

void write_v3f(uint8_t *buffer, size_t *pos, const struct Vector3f *data);

void write_v4f_h(uint8_t *buffer, size_t *pos, const struct Vector4f *data);

void write_v4f(uint8_t *buffer, size_t *pos, const struct Vector4f *data);

size_t write_robot_command_fixed(uint8_t *buffer, const struct RobotCommand *data);

size_t write_robot_command_fixed_V2(uint8_t *buffer, const struct RobotCommand_V2 *data);

size_t write_robot_config_fixed(uint8_t *buffer, const struct RobotConfig *data);

size_t write_robot_matrix_fixed(uint8_t *buffer, const struct RobotMatrix *data);

size_t write_robot_feedback_fixed(uint8_t *buffer, const struct RobotFeedback *data);

size_t write_robot_feedback_custom_fixed(uint8_t *buffer, const struct RobotFeedbackCustom *data);

void write_uint16_in_buff(uint8_t *buffer, uint16_t data);

void convert_float_to_2x_buff(uint8_t *buffer, float data);

#ifdef __cplusplus
}
#endif

#endif
