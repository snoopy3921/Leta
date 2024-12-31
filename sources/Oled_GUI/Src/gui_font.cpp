#include "gui_font.h"


struct char_map {
    uint32_t character_encoding;
    uint16_t val_to_bitmap;
};

char_map special_chars[] = {

    /* Vietnamese special characters */
    // Lowercase characters
    {'á', 95}, {'ạ', 96}, {'à', 97}, {'ả', 98}, 
    {'ã', 99}, {'ă', 100}, {'ắ', 101}, {'ặ', 102}, {'ằ', 103},
    {'ẳ', 104}, {'ẵ', 105}, {'â', 106}, {'ấ', 107}, {'ậ', 108},
    {'ầ', 109}, {'ẩ', 110}, {'ẫ', 111},
    {'é', 112}, {'ẹ', 113}, {'è', 114}, {'ẻ', 115},
    {'ẽ', 116}, {'ê', 117}, {'ế', 118}, {'ệ', 119}, {'ề', 120},
    {'ể', 121}, {'ễ', 122},
    {'í', 123}, {'ị', 124}, {'ì', 125}, {'ỉ', 126},
    {'ĩ', 127},
    {'ó', 128}, {'ọ', 129}, {'ò', 130}, {'ỏ', 131}, 
    {'õ', 132}, {'ô', 133}, {'ố', 134}, {'ộ', 135}, {'ồ', 136},
    {'ổ', 137}, {'ỗ', 138}, {'ơ', 139}, {'ớ', 140}, {'ợ', 141},
    {'ờ', 142}, {'ở', 143}, {'ỡ', 144},
    {'ú', 145}, {'ụ', 146}, {'ù', 147}, {'ủ', 148},
    {'ũ', 149}, {'ư', 150}, {'ứ', 151}, {'ự', 152}, {'ừ', 153},
    {'ử', 154}, {'ữ', 155}, 
    {'ý', 156}, {'ỵ', 157}, {'ỳ', 158}, {'ỷ', 159},
    {'ỹ', 160}, {'đ', 161},

    // Uppercase characters
    {'Á', 162}, {'Ạ', 163}, {'À', 164}, {'Ả', 165},
    {'Ã', 166}, {'Ă', 167}, {'Ắ', 168}, {'Ặ', 169}, {'Ằ', 170},
    {'Ẳ', 171}, {'Ẵ', 172}, {'Â', 173}, {'Ấ', 174}, {'Ậ', 175},
    {'Ầ', 176}, {'Ẩ', 177}, {'Ẫ', 178},
    {'É', 179}, {'Ẹ', 180}, {'È', 181}, {'Ẻ', 182},
    {'Ẽ', 183}, {'Ê', 184}, {'Ế', 185}, {'Ệ', 186}, {'Ề', 187},
    {'Ể', 188}, {'Ễ', 189},
    {'Í', 190}, {'Ị', 191}, {'Ì', 192}, {'Ỉ', 193},
    {'Ĩ', 194},
    {'Ó', 195}, {'Ọ', 196}, {'Ò', 197}, {'Ỏ', 198},
    {'Õ', 199}, {'Ô', 200}, {'Ố', 201}, {'Ộ', 202}, {'Ồ', 203},
    {'Ổ', 204}, {'Ỗ', 205}, {'Ơ', 206}, {'Ớ', 207}, {'Ợ', 208},
    {'Ờ', 209}, {'Ở', 210}, {'Ỡ', 211},
    {'Ú', 212}, {'Ụ', 213}, {'Ù', 214}, {'Ủ', 215},
    {'Ũ', 216}, {'Ư', 217}, {'Ứ', 218}, {'Ự', 219}, {'Ừ', 220},
    {'Ử', 221}, {'Ữ', 222},
    {'Ý', 223}, {'Ỵ', 224}, {'Ỳ', 225}, {'Ỷ', 226},
    {'Ỹ', 227}, {'Đ', 228}
};

/**
 *  @link: https://gregtatum.com/writing/2021/encoding-text-utf-8-unicode/
 * 
 *  110x_xxxx Signals that the code point requires 2 bytes to represent it.
 *  1110_xxxx Signals that the code point requires 3 bytes to represent it.
 *  1111_0xxx Signals that the code point requires 4 bytes to represent it.
 *  The variable length of bytes after this leading byte all start with a 10xx_xxxx * 
 *  
 *   Byte Count   Bytes and Bit Pattern                    Available bits
 *   ==========   ======================================   ==========================   =======
 *   1            0xxxxxxx  -         -         -                            xxx_xxxx    7 bits
 *   2            110xxxxx  10xxxxxx  -         -                       xxx_xxxx_xxxx   11 bits
 *   3            1110xxxx  10xxxxxx  10xxxxxx  -                 xxxx_xxxx_xxxx_xxxx   16 bits
 *   4            11110xxx  10xxxxxx  10xxxxxx  10xxxxxx   x_xxxx_xxxx_xxxx_xxxx_xxxx   21 bits
 *
 *   Example with 32-bit register for character ⇨ 
 *   CODEPOINT    UTF-8 ENCODING                        CODEPOINT NAME   
 *   ==========   =================================     ================
 *   U+21E8       11100010  10000111  10101000          RIGHTWARDS WHITE ARROW
 *
 */
#include "system.h"
uint16_t get_char_index(uint32_t c)
{   
    SYS_PRINT("UTF-8 encoded bytes:  0x%08X\r\n", c);
    if(c < 128)
    {
        return c - ' ';
    }
    for(int i = 0; i < sizeof(special_chars); i++)
    {
        if( c == special_chars[i].character_encoding)  return special_chars[i].val_to_bitmap;
    }
    return 0;
    
}
