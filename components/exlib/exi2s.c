#include "exi2s.h"

void exi2s_new_channel(i2s_port_t i2s_num, i2s_chan_handle_t *tx_chan, i2s_chan_handle_t *rx_chan)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(i2s_num, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, tx_chan, rx_chan));
}

void exi2s_del_channel(i2s_chan_handle_t chan)
{
    ESP_ERROR_CHECK(i2s_channel_disable(chan));
    ESP_ERROR_CHECK(i2s_del_channel(chan));
}

void exi2s_std_channel_init(i2s_chan_handle_t chan, gpio_num_t mclk, gpio_num_t bclk, gpio_num_t ws, gpio_num_t dout, gpio_num_t din)
{
    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = mclk,
            .bclk = bclk,
            .ws   = ws,
            .dout = dout,
            .din  = din,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(chan, &std_cfg));
}

void exi2s_std_reconfig(i2s_chan_handle_t chan, int sample_rate_hz, int channel_num, int data_bit_width)
{
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate_hz);
    i2s_data_bit_width_t bit_width = I2S_DATA_BIT_WIDTH_8BIT;
    i2s_slot_mode_t slot_mode = I2S_SLOT_MODE_STEREO;
    if(channel_num == 1)
        slot_mode = I2S_SLOT_MODE_MONO;
    switch(data_bit_width)
    {
        case 16:bit_width = I2S_DATA_BIT_WIDTH_16BIT;clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;break;
        case 24:bit_width = I2S_DATA_BIT_WIDTH_24BIT;clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;break;
        case 32:bit_width = I2S_DATA_BIT_WIDTH_32BIT;clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;break;
    }
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bit_width, slot_mode);
    i2s_channel_reconfig_std_slot(chan, &slot_cfg);
    i2s_channel_reconfig_std_clock(chan, &clk_cfg);
}

void exi2s_pdm_tx_channel_init(i2s_chan_handle_t tx_chan, gpio_num_t clk, gpio_num_t dout)
{
    i2s_pdm_tx_config_t pdm_tx_cfg = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(36000),
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = clk,
            .dout = dout,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    i2s_channel_init_pdm_tx_mode(tx_chan, &pdm_tx_cfg);
}

void exi2s_pdm_rx_channel_init(i2s_chan_handle_t rx_chan, gpio_num_t clk, gpio_num_t din)
{
    i2s_pdm_rx_config_t pdm_rx_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(36000),
        .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = clk,
            .din = din,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    i2s_channel_init_pdm_rx_mode(rx_chan, &pdm_rx_cfg);
}

void exi2s_tdm_channel_init(i2s_chan_handle_t chan, gpio_num_t mclk, gpio_num_t bclk, gpio_num_t ws, gpio_num_t dout, gpio_num_t din)
{
    i2s_tdm_config_t tdm_cfg = {
        .clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_TDM_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO,
                    I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3),
        .gpio_cfg = {
            .mclk = mclk,
            .bclk = bclk,
            .ws = ws,
            .dout = dout,
            .din = din,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    i2s_channel_init_tdm_mode(chan, &tdm_cfg);
}