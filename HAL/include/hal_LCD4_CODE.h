
#define _lcd_dp         0x01
#define _lcd_g          0x40
#define _lcd_f          0x04
#define _lcd_e          0x02
#define _lcd_d          0x10
#define _lcd_c          0x20
#define _lcd_b          0x80
#define _lcd_a          0x08
#define _lcd_blank      0x00


const char char_gen[] = {                           // definitions for digits
    _lcd_a+_lcd_b+_lcd_c+_lcd_d+_lcd_e+_lcd_f,          // Displays "0"
    _lcd_b+_lcd_c,                                      // Displays "1"
    _lcd_a+_lcd_b+_lcd_d+_lcd_e+_lcd_g,                 // Displays "2"
    _lcd_a+_lcd_b+_lcd_c+_lcd_d+_lcd_g,                 // Displays "3"
    _lcd_b+_lcd_c+_lcd_f+_lcd_g,                        // Displays "4"
    _lcd_a+_lcd_c+_lcd_d+_lcd_f+_lcd_g,                 // Displays "5"
    _lcd_a+_lcd_c+_lcd_d+_lcd_e+_lcd_f+_lcd_g,          // Displays "6"
    _lcd_a+_lcd_b+_lcd_c,                               // Displays "7"
    _lcd_a+_lcd_b+_lcd_c+_lcd_d+_lcd_e+_lcd_f+_lcd_g,   // Displays "8"
    _lcd_a+_lcd_b+_lcd_c+_lcd_d+_lcd_f+_lcd_g,          // Displays "9"

    _lcd_a+_lcd_b+_lcd_c+_lcd_e+_lcd_f+_lcd_g,          // Displays "A"
    _lcd_c+_lcd_d+_lcd_e+_lcd_f+_lcd_g,                 // Displays "b"
    _lcd_a+_lcd_d+_lcd_e+_lcd_f,                        // Displays "C"
    _lcd_b+_lcd_c+_lcd_d+_lcd_e+_lcd_g,                 // Displays "d"
    _lcd_a+_lcd_d+_lcd_e+_lcd_f+_lcd_g,                 // Displays "E"
    _lcd_a+_lcd_e+_lcd_f+_lcd_g,                        // Displays "F"
};

