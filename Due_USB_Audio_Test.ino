#include "USBAudio.h"

#define NBUFS 5
#define BUF_SIZE 64
#define ZERO_BUF_SIZE 16

// Dynamic sample rate
#define MIN_SAMPLE_RATE 47950
#define NOM_SAMPLE_RATE 48000
#define MAX_SAMPLE_RATE 48050

// Artifically increase resolution of period to help stabilize sample rate
#define PERIOD_ADJ_SPEED 6

uint32_t period = (F_CPU/NOM_SAMPLE_RATE) << PERIOD_ADJ_SPEED;

// Ring buffer of buffers
struct Buffer {
  uint16_t len;
  uint16_t buf[BUF_SIZE];
};

volatile Buffer bufs[NBUFS];
uint8_t read_buffer, write_buffer;

// Flag to drop incoming buffers if we're really behind on playing
bool purge_bufs;

// Empty buffer to insert when no data available
uint16_t zero_buf[ZERO_BUF_SIZE];

// Return amount of filled buffers in ring buffer of buffers
uint8_t bufs_filled() {
  uint8_t diff = write_buffer - read_buffer;
  if(diff > NBUFS)
    diff += NBUFS;
  return diff;
}

void set_buffer() {
  // Check amount of buffers available
  uint8_t filled = bufs_filled();

  // If there is some data, play it with DMA
  if(filled) {
    PDC_DACC->PERIPH_TNPR = (uint32_t)bufs[read_buffer].buf;
    PDC_DACC->PERIPH_TNCR = bufs[read_buffer].len;

    // Increment read buffer
    read_buffer++;
    if(read_buffer >= NBUFS)
      read_buffer = 0;
    filled--;
  }

  // Otherwise insert a bit of silence
  else {
    PDC_DACC->PERIPH_TNPR = (uint32_t)zero_buf;
    PDC_DACC->PERIPH_TNCR = ZERO_BUF_SIZE;
  }

  // Update sample rate based on current buffer fill level
  if(filled > 1)
    period = max(period-1, (F_CPU/MAX_SAMPLE_RATE) << PERIOD_ADJ_SPEED);
  else if(filled == 0)
    period = min(period+1, (F_CPU/MIN_SAMPLE_RATE) << PERIOD_ADJ_SPEED);
  PWM->PWM_CH_NUM[0].PWM_CPRDUPD = period >> PERIOD_ADJ_SPEED;
}

// ISR called when one buffer has been played
void DACC_Handler() {
  set_buffer();
}

void setup() {
  // "Zero" is actually 50% full scale
  for(int x = 0; x < ZERO_BUF_SIZE; x++)
    zero_buf[x] = (1 << 11);

  // Enable clock to DAC and PWM
  PMC->PMC_PCER1 = (1 << (ID_DACC-32)) | (1 << (ID_PWM-32));

  // Set up PWM as reference clock
  PWM->PWM_ELMR[0] = PWM_ELMR_CSEL0; // Generate an event on line 0 when compare 0 matches
  PWM->PWM_CMP[0].PWM_CMPV = 1; // Set compare 0 to trigger when channel 0 is 1 (zero doesn't work)
  PWM->PWM_CMP[0].PWM_CMPM = PWM_CMPM_CEN; // Enable compare 0
  PWM->PWM_CH_NUM[0].PWM_CPRD = period >> PERIOD_ADJ_SPEED; // Set period to 48kHz
  PWM->PWM_CH_NUM[0].PWM_CCNT = 0; // Reset counter

  // Set up DAC
  DACC->DACC_MR = DACC_MR_TRGEN // Enable DAC trigger
  | DACC_MR_TRGSEL(4) // Trigger from PWM event line 0
  | DACC_MR_USER_SEL_CHANNEL1 // Select channel 1
  | DACC_MR_REFRESH(10)
  | DACC_MR_STARTUP_1024;
  DACC->DACC_CHER = DACC_CHER_CH1; // Enable channel 1
  DACC->DACC_IDR = 0xFFFFFFFF; // Disable interrupts
  DACC->DACC_IER = DACC_IER_ENDTX; // Enable PDC end of buffer interrupt
  DACC->DACC_ACR = DACC_ACR_IBCTLDACCORE(0b01) | DACC_ACR_IBCTLCH1(0b10);

  // Allow interrupts from the DAC
  NVIC->ISER[1] = (1 << (DACC_IRQn-32));

  // Set up DAC PDC
  set_buffer();
  PDC_DACC->PERIPH_PTCR = PERIPH_PTCR_TXTEN;

  // Start PWM as DAC clock
  PWM->PWM_ENA = PWM_ENA_CHID0;
}

void loop() {
  if(USBAudio.available()) {
    // Temporary place for unprocessed USB audio samples
    static int16_t rx_buf[BUF_SIZE];

    uint32_t len = USBAudio.read(rx_buf, BUF_SIZE*2);
    len /= 2; // Two bytes per sample

    uint8_t filled = bufs_filled();

    // Stop catching up/purging if we have done enough
    if(purge_bufs && filled < 2)
      purge_bufs = false;

    // Drop buffers for a bit if there isn't enough space so playing can catch up
    if(!purge_bufs && filled < NBUFS-2) {
      // Convert samples from signed 16 bit to unsigned 12 bit
      for(uint32_t x = 0; x < len; x++)
        bufs[write_buffer].buf[x] = ((int32_t)rx_buf[x] + 0x8000) >> 4;

      bufs[write_buffer].len = len;

      // Increment buffer
      write_buffer++;
      if(write_buffer >= NBUFS)
        write_buffer = 0;
    }
    
    // Start purging if there isn't enough space
    else purge_bufs = true;
  }
}
