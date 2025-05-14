#include "decode.h"

void decode_wav(uint8_t *wav)
{
    uint32_t sample_rate = *(uint32_t *)(wav + 24); // 采样率
    uint16_t channal_num = *(uint16_t *)(wav + 22); // 声道数
    uint32_t bit_width = *(uint32_t *)(wav + 34);   // 位深度
    uint32_t data_size = *(uint32_t *)(wav + 40);   // 数据大小
}