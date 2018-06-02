---------------------------------------------------------------------
	PIC32MX220F032B �� USB HOST �T���v��.
---------------------------------------------------------------------

�� �T�v

-  Pinguino �̃R���p�C�����g�p���āAMicroChip��USB-Host-Keyboard�T���v��
   ���r���h���܂��B




�� �z��         PIC32MX220F032B 

                3.3V
                 |
                 *------10��--------------+
                10k                       |
                 |       ___    ___       | 0.1u
   ��   -->  ----*-MCLR [1  |__| 28] AVDD-*-||---GND
   �C   -->  --PGD3/RA0 [2       27] AVSS--------GND  LED
   �^�[ -->  --PGC3/RA1 [3       26] RB15--1k��-------|��|--GND
                    RB0 [4       25] RB14
                    RB1 [5       24] RB13
               SDA2/RB2 [6       23] Vusb3v3--------3.3V
               SCL2/RB3 [7       22] usb D-
    Xtal     GND----Vss [8       21] usb D+   +10uF
 +-----------------OSC1 [9       20] Vcap------||---GND
 *--|��|--*--------OSC2 [10      19] Vss------------GND
 |  8MHz  |    U1TX/RB4 [11      18] RB9
 22pF    22pF  U1RX/RA4 [12      17] RB8
 |        |   3.3v--Vdd [13      16] RB7
 |        |         RB5 [14      15] Vbus-----------USB Vbus(5V)
 GND    GND              ~~~~~~~~~~
            
            
�� �J���� 
            
- Windows XP / Vista / 7 / 8 �̂ǂꂩ��p�ӂ��܂��B

- Pinguino X.3 ����ł����L�T�C�g������肵�ăC���X�g�[�����܂��B
  http://wiki.pinguino.cc/index.php/Main_Page/ja

- Pinguino X.3 �R���p�C���[�Ƀp�X��ʂ��܂��B

  setenv.bat
    PATH C:\PinguinoX.3\win32\p32\bin;%PATH%


�� �R���p�C�����@

- �R�}���h���C������ 

  D:>  make

  �Ńr���h���Ă��������B


�� �R���p�C����̒��ӓ_

- Pinguino�ɕt����make.exe ���g�p���Ă��������B

    PATH C:\PinguinoX.3\win32\p32\bin;%PATH%

- �����Ŏg�p����Makefile��Cygwin��MinGW��shell�Ɉˑ����Ȃ� (cmd.exe���Ăяo��) make
- �łȂ��Ɛ����������Ȃ��悤�ł��B


�� �������ݕ��@

- pic32prog��z�肵�Ă��܂��B
  http://code.google.com/p/pic32prog/

  pic32prog.exe���p�X�̒ʂ����ꏊ�ɐݒu���Ă���ꍇ��
  w.bat ���N������Ə������߂܂��B

- �e��̏������ݕ��@�͉��L�g�o���Q�Ƃ��Ă��������B
  http://hp.vector.co.jp/authors/VA000177/html/PIC32MX.html


�� �t�@�[���E�F�A�̓������

-  PIC32MX220F032B �� RB15 �ɐڑ����ꂽLED ��_�ł����܂��B

-  U1TX/RB4 U1RX/RA4 �o�R�� �V���A���|�[�g(57600bps)�Ƀf�o�b�O�p��print���b�Z�[�W
�@ ���o�܂��B�K����USB-serial( 3.3V ���W�b�N���x�� )�ϊ����p�ӂ���PC�Ń��j�^�����O���܂��B

-  �f�o�C�X�p��USB-TypeB�R�l�N�^�i�����ɂ���Ă͈Ⴄ��������܂��񂪁j�� USB Keyboard ��
�@ �q�����߁AType-A���X<===>Type-A���X�̂悤�ȕϊ����p�ӂ��� USB Keyboard ���q���܂��B
   �ϊ���̑�p�i�Ƃ��Ă�PC�݊��@��USB�O�����P�[�u�����������Ďg���Ƃ��A100�ς�USB����
   �P�[�u���𔃂��Ă��ĉ�������Ƃ��H�v���܂��B�������PIC32MX220���USB-TypeA���X�����
   �Ɏ������Ă��n�j�ł��B

-  USB�[�q�� �K����USB�L�[�{�[�h��ڑ�����ƁA�����ꂽ�L�[�ɑΉ�����R�[�h���܂�HID Report
   ���V���A���|�[�g�o�R��print���܂��B



�� �������[�}�b�v�i�S�́j

PIC32�̃������[�}�b�v�ł��B
- �������蓖�Ă���Ă���G���A�� 0000_0000 �` 2000_0000 ��512MB�ł��B
- �������蓖�Ă���Ă���512MB�ƑS���������̂� KSEG0��KSEG1�ɂ��}�b�v����܂��B
- KSEG0��KSEG1�̈Ⴂ�̓L���b�V������/�L���ŕ������Ă��܂��B

FFFF_FFFF +---------------+
          |               |
          | Reserved      |
          |               |
C000_0000 +---------------+
          | KSEG1(�_��)   | Cache�Ȃ�.
A000_0000 +---------------+
          | KSEG0(�_��)   | Cache����.
8000_0000 +---------------+
          |               |
          | Reserved      |
          |               |
          |               |
          |               |
2000_0000 +---------------+
          | �����������[  | ROM/RAM/PORT
0000_0000 +---------------+



�� �������[�}�b�v�iFlash ROM/RAM�̈�j

A000_1FFF +---------------+
          |               |
          |   SRAM (8kB)  |
          |               |
A000_0000 +---------------+

(BFC00BFF)
9FC0_0BFF +---------------+
          |BOOT Flash(3kB)| RESET����̊J�n�Ԓn��BFC0_0000�ł��B
9FC0_0000 +---------------+ Config Fuse�� BFC0_0BF0�`BFC0_0BFF��16byte�ł��B
(BFC00000)                  ���荞�݃x�N�^�[��BOOT Flash���ɒu����܂��B

9D00_7FFF +---------------+
          |               |
          |Program Flash  |
          |    (32kB)     |
          |               |
9D00_0000 +---------------+




�� �c�[��

- hex2dump:  HEX�t�@�C�����P�U�i���_���v���X�g�ɂ���. / HEX�t�@�C���̃R�}���h05���t�B���^�����O����B

- pic32prog.exe : PicKit2���o�R����PIC32MX��HEX�t�@�C������������.


�� �ӎ�

  pic32mx��pic32prog�ɂ��Ẵm�E�n�E�̑�������z����̂g�o�ɂĕ׋������Ă��������܂����B
  �����Ɋ��ӂ̈ӂ�\���܂��B���肪�Ƃ��������܂����B


