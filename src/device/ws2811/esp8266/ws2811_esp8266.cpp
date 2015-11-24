#include "global.h"
#include "eagle_soc.h"
#include "device/ws2811/esp8266/ws2811_esp8266.h"
#include "sys/util.h"


#if defined(WS2811_THREE_SAMPLE)
#ifndef WS2811_I2S_BCK
#define WS2811_I2S_BCK 22
#endif
#ifndef WS2811_I2S_DIV
#define WS2811_I2S_DIV 4
#endif
#elif defined(WS2811_FOUR_SAMPLE)
#ifndef WS2811_I2S_BCK
#define WS2811_I2S_BCK 17
#endif
#ifndef WS2811_I2S_DIV
#define WS2811_I2S_DIV 4
#endif
#else
#error You need to either define WS2811_THREE_SAMPLE or WS2811_FOUR_SAMPLE
#endif


const uint16_t WS2811::ESP8266Driver::bitPatterns[16] =
{
#if defined(WS2811_THREE_SAMPLE)
	0b100100100100, 0b100100100110, 0b100100110100, 0b100100110110,
	0b100110100100, 0b100110100110, 0b100110110100, 0b100110110110,
	0b110100100100, 0b110100100110, 0b110100110100, 0b110100110110,
	0b110110100100, 0b110110100110, 0b110110110100, 0b110110110110,
#elif defined(WS2811_FOUR_SAMPLE)
	0b1000100010001000, 0b1000100010001110, 0b1000100011101000, 0b1000100011101110,
	0b1000111010001000, 0b1000111010001110, 0b1000111011101000, 0b1000111011101110,
	0b1110100010001000, 0b1110100010001110, 0b1110100011101000, 0b1110100011101110,
	0b1110111010001000, 0b1110111010001110, 0b1110111011101000, 0b1110111011101110,
#endif
};


WS2811::ESP8266Driver::ESP8266Driver()
{
	//Reset DMA
	SET_PERI_REG_MASK(SLC_CONF0, SLC_RXLINK_RST);//|SLC_TXLINK_RST);
	CLEAR_PERI_REG_MASK(SLC_CONF0, SLC_RXLINK_RST);//|SLC_TXLINK_RST);

	//Enable and configure DMA
	CLEAR_PERI_REG_MASK(SLC_CONF0, (SLC_MODE << SLC_MODE_S));
	SET_PERI_REG_MASK(SLC_CONF0, (1 << SLC_MODE_S));
	SET_PERI_REG_MASK(SLC_RX_DSCR_CONF, SLC_INFOR_NO_REPLACE | SLC_TOKEN_NO_REPLACE);
	CLEAR_PERI_REG_MASK(SLC_RX_DSCR_CONF, SLC_RX_FILL_EN | SLC_RX_EOF_MODE | SLC_RX_FILL_MODE);

    memset(&i2sBufDescOut, 0, sizeof(i2sBufDescOut));
	i2sBufDescOut.owner = 1;
	i2sBufDescOut.eof = 1;
	i2sBufDescOut.sub_sof = 0;
	i2sBufDescOut.datalen = sizeof(i2sBlock);  //Size (in bytes)
	i2sBufDescOut.blocksize = sizeof(i2sBlock); //Size (in bytes)
	i2sBufDescOut.buf_ptr = (uint32_t)i2sBlock;
	i2sBufDescOut.next_link_ptr = (uint32_t)&i2sBufDescZeroes; //At the end, just redirect the DMA to the zero readPtr.

    memset(&i2sBufDescZeroes, 0, sizeof(i2sBufDescZeroes));
	i2sBufDescZeroes.owner = 1;
	i2sBufDescZeroes.eof = 1;
	i2sBufDescZeroes.sub_sof = 0;
	i2sBufDescZeroes.datalen = sizeof(i2sZeroes);
	i2sBufDescZeroes.blocksize = sizeof(i2sZeroes);
	i2sBufDescZeroes.buf_ptr = (uint32_t)i2sZeroes;
	i2sBufDescZeroes.next_link_ptr = (uint32_t)&i2sBufDescOut;


	for (uint32_t i = 0; i < ARRAYLEN(i2sZeroes); i++) i2sZeroes[i] = 0;
	for (uint32_t i = 0; i < ARRAYLEN(i2sBlock); i++) i2sBlock[i] = 0;

	CLEAR_PERI_REG_MASK(SLC_RX_LINK, SLC_RXLINK_DESCADDR_MASK);
	SET_PERI_REG_MASK(SLC_RX_LINK, ((uint32_t)&i2sBufDescOut) & SLC_RXLINK_DESCADDR_MASK);

	SET_PERI_REG_MASK(SLC_RX_LINK, SLC_RXLINK_START);

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_I2SO_DATA);

	i2c_writeReg_Mask_def(i2c_bbpll, i2c_bbpll_en_audio_clock_out, 1);

	//Reset I2S subsystem
	CLEAR_PERI_REG_MASK(I2SCONF, I2S_I2S_RESET_MASK);
	SET_PERI_REG_MASK(I2SCONF, I2S_I2S_RESET_MASK);
	CLEAR_PERI_REG_MASK(I2SCONF, I2S_I2S_RESET_MASK);

	//Select 16bits per channel (FIFO_MOD=0), no DMA access (FIFO only)
	CLEAR_PERI_REG_MASK(I2S_FIFO_CONF, I2S_I2S_DSCR_EN | (I2S_I2S_RX_FIFO_MOD << I2S_I2S_RX_FIFO_MOD_S)
                                     | (I2S_I2S_TX_FIFO_MOD << I2S_I2S_TX_FIFO_MOD_S));
	//Enable DMA in i2s subsystem
	SET_PERI_REG_MASK(I2S_FIFO_CONF, I2S_I2S_DSCR_EN);

	//trans master&rece slave, MSB shift, right_first, msb right
	CLEAR_PERI_REG_MASK(I2SCONF, I2S_TRANS_SLAVE_MOD | (I2S_BITS_MOD << I2S_BITS_MOD_S)
                               | (I2S_BCK_DIV_NUM << I2S_BCK_DIV_NUM_S)
                               | (I2S_CLKM_DIV_NUM << I2S_CLKM_DIV_NUM_S));
	SET_PERI_REG_MASK(I2SCONF, I2S_RIGHT_FIRST | I2S_MSB_RIGHT | I2S_RECE_SLAVE_MOD
                             | I2S_RECE_MSB_SHIFT | I2S_TRANS_MSB_SHIFT
                             | (((WS2811_I2S_BCK - 1) & I2S_BCK_DIV_NUM) << I2S_BCK_DIV_NUM_S)
                             | (((WS2811_I2S_DIV - 1) & I2S_CLKM_DIV_NUM) << I2S_CLKM_DIV_NUM_S));

	//Start transmission
	SET_PERI_REG_MASK(I2SCONF, I2S_I2S_TX_START);
}

void WS2811::ESP8266Driver::sendFrame(void** data, int pixels)
{
    uint8_t* readPtr = (uint8_t*)*data;
    pixels *= 3;
    
#if defined(WS2811_THREE_SAMPLE)
	uint8_t* writePtr = (uint8_t*)i2sBlock;

	int pl = 0;
    bool last = false;
	while (!last)
	{
		uint8_t b;
		b = readPtr[pl++]; uint16_t c1a = bitPatterns[b & 0xf]; uint16_t c1b = bitPatterns[b >> 4];
		b = readPtr[pl++]; uint16_t c2a = bitPatterns[b & 0xf]; uint16_t c2b = bitPatterns[b >> 4];
		b = readPtr[pl++]; uint16_t c3a = bitPatterns[b & 0xf]; uint16_t c3b = bitPatterns[b >> 4];
		b = readPtr[pl++]; uint16_t c4a = bitPatterns[b & 0xf]; uint16_t c4b = bitPatterns[b >> 4];

		if (pl >= pixels)
		{
            last = true;
			if (pl - 1 >= pixels) c4a = c4b = 0;
			if (pl - 2 >= pixels) c3a = c3b = 0;
			if (pl - 3 >= pixels) c2a = c2b = 0;
			if (pl - 4 >= pixels) c1a = c1b = 0;
		}
        
		//Order of bits on wire: Reverse from how they appear here.
#define STEP1(x) (c##x##b >> 4 )
#define STEP2(x) ((c##x##b << 4 ) | ( c##x##a>>8 ))
#define STEP3(x) (c##x##a & 0xff )

		*writePtr++ = STEP1(2);
		*writePtr++ = STEP3(1);
		*writePtr++ = STEP2(1);
		*writePtr++ = STEP1(1);

		*writePtr++ = STEP2(3);
		*writePtr++ = STEP1(3);
		*writePtr++ = STEP3(2);
		*writePtr++ = STEP2(2);

		*writePtr++ = STEP3(4);
		*writePtr++ = STEP2(4);
		*writePtr++ = STEP1(4);
		*writePtr++ = STEP3(3);
	}

#elif defined(WS2811_FOUR_SAMPLE)
	uint16_t* writePtr = (uint16_t*)i2sBlock;

	for (place = 0; place < pixels; place++)
	{
		uint8_t byte = readPtr[place];
		*writePtr++ = bitPatterns[byte & 0xf];
		*writePtr++ = bitPatterns[byte >> 4];
	}
#endif

	while (writePtr < i2sBlock + WS2811_BLOCKSIZE) *writePtr++ = 0;
}

bool WS2811::ESP8266Driver::isSending()
{
    return false;
}

void WS2811::ESP8266Driver::cancelSending()
{
}
