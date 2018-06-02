---------------------------------------------------------------------
	PIC32MX220F032B で USB HOST サンプル.
---------------------------------------------------------------------

■ 概要

-  Pinguino のコンパイラを使用して、MicroChipのUSB-Host-Keyboardサンプル
   をビルドします。




■ 配線         PIC32MX220F032B 

                3.3V
                 |
                 *------10Ω--------------+
                10k                       |
                 |       ___    ___       | 0.1u
   ラ   -->  ----*-MCLR [1  |__| 28] AVDD-*-||---GND
   イ   -->  --PGD3/RA0 [2       27] AVSS--------GND  LED
   ター -->  --PGC3/RA1 [3       26] RB15--1kΩ-------|＞|--GND
                    RB0 [4       25] RB14
                    RB1 [5       24] RB13
               SDA2/RB2 [6       23] Vusb3v3--------3.3V
               SCL2/RB3 [7       22] usb D-
    Xtal     GND----Vss [8       21] usb D+   +10uF
 +-----------------OSC1 [9       20] Vcap------||---GND
 *--|□|--*--------OSC2 [10      19] Vss------------GND
 |  8MHz  |    U1TX/RB4 [11      18] RB9
 22pF    22pF  U1RX/RA4 [12      17] RB8
 |        |   3.3v--Vdd [13      16] RB7
 |        |         RB5 [14      15] Vbus-----------USB Vbus(5V)
 GND    GND              ~~~~~~~~~~
            
            
■ 開発環境 
            
- Windows XP / Vista / 7 / 8 のどれかを用意します。

- Pinguino X.3 安定版を下記サイトから入手してインストールします。
  http://wiki.pinguino.cc/index.php/Main_Page/ja

- Pinguino X.3 コンパイラーにパスを通します。

  setenv.bat
    PATH C:\PinguinoX.3\win32\p32\bin;%PATH%


■ コンパイル方法

- コマンドラインから 

  D:>  make

  でビルドしてください。


■ コンパイル上の注意点

- Pinguinoに付属のmake.exe を使用してください。

    PATH C:\PinguinoX.3\win32\p32\bin;%PATH%

- ここで使用するMakefileはCygwinやMinGWのshellに依存しない (cmd.exeを呼び出す) make
- でないと正しく動かないようです。


■ 書き込み方法

- pic32progを想定しています。
  http://code.google.com/p/pic32prog/

  pic32prog.exeをパスの通った場所に設置してある場合は
  w.bat を起動すると書き込めます。

- 各種の書き込み方法は下記ＨＰを参照してください。
  http://hp.vector.co.jp/authors/VA000177/html/PIC32MX.html


■ ファームウェアの動作説明

-  PIC32MX220F032B の RB15 に接続されたLED を点滅させます。

-  U1TX/RB4 U1RX/RA4 経由の シリアルポート(57600bps)にデバッグ用のprintメッセージ
　 が出ます。適当なUSB-serial( 3.3V ロジックレベル )変換器を用意してPCでモニタリングします。

-  デバイス用のUSB-TypeBコネクタ（実装によっては違うかもしれませんが）に USB Keyboard を
　 繋ぐため、Type-Aメス<===>Type-Aメスのような変換基板を用意して USB Keyboard を繋ぎます。
   変換基板の代用品としてはPC互換機のUSB外だしケーブルを改造して使うとか、100均でUSB延長
   ケーブルを買ってきて改造するとか工夫します。もちろんPIC32MX220基板にUSB-TypeAメスを並列
   に実装してもＯＫです。

-  USB端子に 適当なUSBキーボードを接続すると、押されたキーに対応するコードを含んだHID Report
   をシリアルポート経由でprintします。



■ メモリーマップ（全体）

PIC32のメモリーマップです。
- 物理割り当てされているエリアは 0000_0000 ～ 2000_0000 の512MBです。
- 物理割り当てされている512MBと全く同じものが KSEG0とKSEG1にもマップされます。
- KSEG0とKSEG1の違いはキャッシュ無効/有効で分けられています。

FFFF_FFFF +---------------+
          |               |
          | Reserved      |
          |               |
C000_0000 +---------------+
          | KSEG1(論理)   | Cacheなし.
A000_0000 +---------------+
          | KSEG0(論理)   | Cacheあり.
8000_0000 +---------------+
          |               |
          | Reserved      |
          |               |
          |               |
          |               |
2000_0000 +---------------+
          | 物理メモリー  | ROM/RAM/PORT
0000_0000 +---------------+



■ メモリーマップ（Flash ROM/RAM領域）

A000_1FFF +---------------+
          |               |
          |   SRAM (8kB)  |
          |               |
A000_0000 +---------------+

(BFC00BFF)
9FC0_0BFF +---------------+
          |BOOT Flash(3kB)| RESET直後の開始番地はBFC0_0000です。
9FC0_0000 +---------------+ Config Fuseは BFC0_0BF0～BFC0_0BFFの16byteです。
(BFC00000)                  割り込みベクターもBOOT Flash内に置かれます。

9D00_7FFF +---------------+
          |               |
          |Program Flash  |
          |    (32kB)     |
          |               |
9D00_0000 +---------------+




■ ツール

- hex2dump:  HEXファイルを１６進数ダンプリストにする. / HEXファイルのコマンド05をフィルタリングする。

- pic32prog.exe : PicKit2を経由してPIC32MXにHEXファイルを書き込む.


■ 謝辞

  pic32mxやpic32progについてのノウハウの多くをすzさんのＨＰにて勉強させていただきました。
  ここに感謝の意を表します。ありがとうございました。


