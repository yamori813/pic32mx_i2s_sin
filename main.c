/*******************************************************************************
  MPLAB Harmony Project Main Source File

  Company:
    Microchip Technology Inc.
  
  File Name:
    main.c

  Summary:
    This file contains the "main" function for an MPLAB Harmony project.

  Description:
    This file contains the "main" function for an MPLAB Harmony project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state 
    machines of all MPLAB Harmony modules in the system and it calls the 
    "SYS_Tasks" function from within a system-wide "super" loop to maintain 
    their correct operation. These two functions are implemented in 
    configuration-specific files (usually "system_init.c" and "system_tasks.c")
    in a configuration-specific folder under the "src/system_config" folder 
    within this project's top-level folder.  An MPLAB Harmony project may have
    more than one configuration, each contained within it's own folder under
    the "system_config" folder.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

//Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "HardwareProfile.h"

#include "wavetable.h"

#define BUFFER_LENGTH 64

// sample is 24 bit
#define HIRES

void init_i2s1();
void delay_ms(unsigned int count);
void i2s_init_DMA();
#ifdef HIRES
void generate_sine(unsigned long* buffer_pp);
#else
void generate_sine(short* buffer_pp);
#endif

#ifdef HIRES
unsigned long buffer_a[BUFFER_LENGTH];
unsigned long buffer_b[BUFFER_LENGTH];
#else
short buffer_a[BUFFER_LENGTH];
short buffer_b[BUFFER_LENGTH];
#endif

volatile unsigned char bufferAFull = 0;
volatile unsigned char bufferBFull = 0;

// test variables - for debugging purposes only!
unsigned long accum1t = 0;
unsigned long accum2t = 0;
unsigned long tuningWord1t = 59055800;
unsigned long tuningWord2t = 59055800;
//--------------------------------------------------

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    SYSTEMConfig(GetSystemClock(), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);   
    INTEnableSystemMultiVectoredInt();
        
    TRISA = 0x0000;
    TRISB = 0x0000;
    PORTA = 0x0000;
    
    // Fill all buffers first at start.
    generate_sine(&buffer_a[0]);
    generate_sine(&buffer_b[0]);
    
    delay_ms(5);
        
    init_i2s1();
    i2s_init_DMA();
   
    // Trigger the DMA to start the transfer by switching the SPI1 transmit complete interrupt flag up.
    IFS1bits.SPI1TXIF = 1;
             
    while ( 1 )
    {
        // source: http://chipkit.net/forum/viewtopic.php?t=3137
        if (bufferAFull == 0) {
            generate_sine(&buffer_a[0]);
            bufferAFull = 1;
            
        }
        if (bufferBFull == 0) {
            generate_sine(&buffer_b[0]);
            bufferBFull = 1;
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}

void init_i2s1() {
    // http://chipkit.net/forum/viewtopic.php?f=6&t=3137&start=10
    /* The following code example will initialize the SPI1 Module in I2S Master mode. */
    /* It assumes that none of the SPI1 input pins are shared with an analog input. */
    unsigned int rData;
    IEC0CLR = 0x03800000; // disable all interrupts
    IFS1bits.SPI1TXIF = 0;
    SPI1CON = 0; // Stops and resets the SPI1.
    SPI1CON2 = 0; // Reset audio settings
    SPI1BRG = 0; // Reset Baud rate register
    rData = SPI1BUF; // clears the receive buffer
    
    SPI1STATCLR = 0x40; // clear the Overflow
    SPI1CON2 = 0x00000080; // I2S Mode, AUDEN = 1, AUDMON = 0
    SPI1CON2bits.IGNROV = 1; // Ignore Receive Overflow bit (for Audio Data Transmissions)
    SPI1CON2bits.IGNTUR = 1; //  Ignore Transmit Underrun bit (for Audio Data Transmissions) 1 = A TUR is not a critical error and zeros are transmitted until thSPIxTXB is not empty 0 = A TUR is a critical error which stop SPI operation
    
    SPI1CONbits.ENHBUF = 1; // 1 = Enhanced Buffer mode is enabled
    SPI1CON = 0x00000060; // Master mode, SPI ON, CKP = 1, 16-bit audio channel
    SPI1CONbits.STXISEL = 0b11;
    SPI1CONbits.DISSDI = 1; // 0 = Disable SDI bit
    SPI1CONSET = 0x00008000;
    
    IFS1CLR = 0x000000f0;
    IPC7CLR = 0x1F000000;
    IPC7SET = 0x1C000000;

    // REFCLK is used by the Baud Rate Generator
    SPI1CONbits.MCLKSEL = 1;
#ifdef HIRES
    // 24-bit Data, 32-bit FIFO, 32-bit Channel/64-bit Frame
    SPI1CONbits.MODE32 = 1;
#else
    // 16-bit Data, 16-bit FIFO, 32-bit Channel/64-bit Frame
    SPI1CONbits.MODE32 = 0; 
#endif
    SPI1CONbits.MODE16 = 1; 
    // Baud Rate Generator
    SPI1BRG = 1;
    
    IEC1bits.SPI1TXIE = 0;

    // data, 32 bits per frame
    // from here, the device is ready to receive and transmit data
    /* Note: A few of bits related to frame settings are not required to be set in the SPI1CON */
    /* register during audio mode operation. Please refer to the notes in the SPIxCON2 register.*/
    PPSUnLock;
    PPSOutput(3, RPA4, REFCLKO);	// MCLK
    PPSOutput(1, RPB4, SS1);		// LRCLK
    PPSOutput(2, RPA1,SDO1);		// SDATA
    // RB14 is BCLK
    PPSLock;

    REFOCONbits.OE = 0;
    REFOCONbits.ON = 0;
    REFOCONbits.ROSEL = 6;
// This value from AN1422
// 48 KHz (12.2880MHz)
    REFOCONbits.RODIV = 3;
    REFOTRIM = 464<<23;
// 44.1 KHz (11.28893MHz)
//    REFOCONbits.RODIV = 4;
//    REFOTRIM = 128<<23;
// 32 KHz (8.1920MHz)
//    REFOCONbits.RODIV = 5;
//    REFOTRIM = 440<<23;
    REFOCONSET = 0x00000200;
    REFOCONbits.OE = 1;
    REFOCONbits.ON = 1;
}

void i2s_init_DMA(void) {
    DMACONCLR = 0x8000; // disable entire DMA.
    IEC1bits.DMA0IE = 1;
    IFS1bits.DMA0IF = 0;
    IPC10bits.DMA0IP = 7;   // Setting DMA0 at highest priority.
    IPC10bits.DMA0IS = 3;   // Setting DMA0 at highest sub-priority.
    DMACONSET = 0x8000; // enable DMA.
    DCH0CON = 0x0000;
    DCRCCON = 0x00; // 
    DCH0INTCLR = 0xff00ff; // clear DMA0 interrupts register.
    DCH0INTbits.CHSDIE = 1; // DMA0 Interrupts when source done enabled.
    DCH0ECON = 0x00;
    DCH0SSA = KVA_TO_PA(&buffer_a[0]); // DMA0 source address.
    DCH0DSA = KVA_TO_PA(&SPI1BUF); // DMA0 destination address.
#ifdef HIRES
    DCH0SSIZ = BUFFER_LENGTH*4; // DMA0 Source size (default).
    DCH0DSIZ = 4;   // DMA0 destination size.
    DCH0CSIZ = 4;   // DMA0 cell size.
#else
    DCH0SSIZ = BUFFER_LENGTH*2; // DMA0 Source size (default).
    DCH0DSIZ = 2;   // DMA0 destination size.
    DCH0CSIZ = 2;   // DMA0 cell size.
#endif
    DCH0ECONbits.CHSIRQ = _SPI1_TX_IRQ; // DMA0 transfer triggered by which interrupt? (On PIC32MX - it is by _IRQ suffix!)
    DCH0ECONbits.AIRQEN = 0; // do not enable DMA0 transfer abort interrupt.
    DCH0ECONbits.SIRQEN = 1; // enable DMA0 transfer start interrupt.
    DCH0CONbits.CHAEN = 0; // DMA Channel 0 is always disabled right after the transfer.
    DCH0CONbits.CHEN = 1;  // DMA Channel 0 is enabled. 
    
    IEC1bits.DMA1IE = 1;
    IFS1bits.DMA1IF = 0;
    IPC10bits.DMA1IP = 7;   // Setting DMA1 at highest priority.
    IPC10bits.DMA1IS = 3;   // Setting DMA1 at highest sub-priority.
    DCH1CON = 0x0000;
    DCH1INTCLR = 0xff00ff; // clear DMA1 interrupts register.
    DCH1INTbits.CHSDIE = 1; // DMA1 Interrupts when source done enabled.
    DCH1ECON = 0x00;
    DCH1SSA = KVA_TO_PA(&buffer_b[0]); // DMA1 source address.
    DCH1DSA = KVA_TO_PA(&SPI1BUF); // DMA1 destination address.
#ifdef HIRES
    DCH1SSIZ = BUFFER_LENGTH*4; // DMA1 Source size (default).
    DCH1DSIZ = 4;   // DMA1 destination size.
    DCH1CSIZ = 4;   // DMA1 cell size.
#else
    DCH1SSIZ = BUFFER_LENGTH*2; // DMA1 Source size (default).
    DCH1DSIZ = 2;   // DMA1 destination size.
    DCH1CSIZ = 2;   // DMA1 cell size.
#endif
    DCH1ECONbits.CHSIRQ = _SPI1_TX_IRQ; // DMA1 transfer triggered by which interrupt? (On PIC32MX - it is by _IRQ suffix!)
    DCH1ECONbits.AIRQEN = 0; // do not enable DMA1 transfer abort interrupt.
    DCH1ECONbits.SIRQEN = 1; // enable DMA1 transfer start interrupt.
    DCH1CONbits.CHAEN = 0; // DMA Channel 1 is always disabled right after the transfer.
    DCH1CONbits.CHEN = 0;  // DMA Channel 1 is enabled. 
     
}

void delay_ms(unsigned int count)
{
	T1CON = 0x8030;		// turn on timer, prescaler to 256 (type B timer)
	while(count--)
	{
		TMR1 = 0;
		while(TMR1 < 0x4e);
	}
	T1CONbits.ON = 0;
}

#ifdef HIRES
void generate_sine(unsigned long* buffer_pp) {
#else
void generate_sine(short* buffer_pp) {
#endif
    
  //source: https://github.com/pyrohaz
  unsigned int n = 0;
  short int sample = 0;
  for (n = 0; n < BUFFER_LENGTH; n++) {
    
    if (n & 0x01) {
      sample = (short int)wavetable[accum1t >> 20];
      accum1t += tuningWord1t;
    }
    else {
      sample = (short int)wavetable[accum2t >> 20];
      accum2t += tuningWord2t;
    }

#ifdef HIRES
    unsigned int tmp;
    if (sample < 0) {
      tmp = 0xffffff + sample * 0x100;
    } else {
      tmp = sample * 0x100;
    }
    buffer_pp[n] = tmp;
#else
    buffer_pp[n] = sample;
#endif

  }
    
}

void __attribute__((interrupt,nomips16)) _IntHandlerSysDmaCh0(void)
{ 
    bufferAFull = 0;  
    DCH1CONSET = 0x0000080;
    DCH0INTCLR = 0x0000ff;
    IFS1CLR    = (1<<28);
    //SYS_DMA_TasksISR(sysObj.sysDma, DMA_CHANNEL_0);
}

void __attribute__((interrupt,nomips16)) _IntHandlerSysDmaCh1(void)
{        
    bufferBFull = 0;
    DCH0CONSET = 0x00000080;
    DCH1INTCLR = 0x0000ff;
    IFS1CLR    = (1<<29);
    //SYS_DMA_TasksISR(sysObj.sysDma, DMA_CHANNEL_1);
}

/*******************************************************************************
 End of File
*/
