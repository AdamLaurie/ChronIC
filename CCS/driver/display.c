// *************************************************************************************************
//
//      Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
//
//
//        Redistribution and use in source and binary forms, with or without
//        modification, are permitted provided that the following conditions
//        are met:
//
//          Redistributions of source code must retain the above copyright
//          notice, this list of conditions and the following disclaimer.
//
//          Redistributions in binary form must reproduce the above copyright
//          notice, this list of conditions and the following disclaimer in the
//          documentation and/or other materials provided with the
//          distribution.
//
//          Neither the name of Texas Instruments Incorporated nor the names of
//          its contributors may be used to endorse or promote products derived
//          from this software without specific prior written permission.
//
//        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//        LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//        DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//        THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// Display functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section

// system
#include <project.h>
#include <string.h>

// driver
#include "display.h"

// logic
#include "clock.h"
#include "user.h"
#include "date.h"
#include "stopwatch.h"
//#include "temperature.h"

// *************************************************************************************************
// Prototypes section
void write_lcd_mem(u8 * lcdmem, u8 bits, u8 bitmask, u8 state);
void clear_line(u8 line);
void display_symbol(u8 symbol, u8 mode);
void display_char(u8 segment, u8 chr, u8 mode);
void display_chars(u8 segments, u8 * str, u8 mode);

// *************************************************************************************************
// Defines section

// *************************************************************************************************
// Global Variable section

// Table with memory bit assignment for digits "0" to "9" and characters "A" to "Z"
const u8 lcd_font[] = {
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F,         // Displays "0"
    SEG_B + SEG_C,                                         // Displays "1"
    SEG_A + SEG_B + SEG_D + SEG_E + SEG_G,                 // Displays "2"
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_G,                 // Displays "3"
    SEG_B + SEG_C + SEG_F + SEG_G,                         // Displays "4"
    SEG_A + SEG_C + SEG_D + SEG_F + SEG_G,                 // Displays "5"
    SEG_A + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,         // Displays "6"
    SEG_A + SEG_B + SEG_C,                                 // Displays "7"
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G, // Displays "8"
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G,         // Displays "9"
    0,                                                     // Displays " "
    0,                                                     // Displays " "
    0,                                                     // Displays " "
    0,                                                     // Displays " "
    0,                                                     // Displays " "
    SEG_D + SEG_E + SEG_G,                                 // Displays "c"
    0,                                                     // Displays " "
    SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G,         // Displays "A"
    SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,                 // Displays "b"
    SEG_A + SEG_D + SEG_E + SEG_F,                         // Displays "C"
    SEG_B + SEG_C + SEG_D + SEG_E + SEG_G,                 // Displays "d"
    SEG_A + +SEG_D + SEG_E + SEG_F + SEG_G,                // Displays "E"
    SEG_A + SEG_E + SEG_F + SEG_G,                         // Displays "F"
    //  SEG_A+      SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,       // Displays "G"
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G,         // Displays "g"
    SEG_B + SEG_C + SEG_E + SEG_F + SEG_G,                 // Displays "H"
    SEG_E + SEG_F,                                         // Displays "I"
    SEG_A + SEG_B + SEG_C + SEG_D,                         // Displays "J"
    //              SEG_B+SEG_C+      SEG_E+SEG_F+SEG_G,     // Displays "k"
    SEG_D + SEG_E + SEG_F + SEG_G,                         // Displays "k"
    SEG_D + SEG_E + SEG_F,                                 // Displays "L"
    SEG_A + SEG_B + SEG_C + SEG_E + SEG_F,                 // Displays "M"
    SEG_C + SEG_E + SEG_G,                                 // Displays "n"
    SEG_C + SEG_D + SEG_E + SEG_G,                         // Displays "o"
    SEG_A + SEG_B + SEG_E + SEG_F + SEG_G,                 // Displays "P"
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F,         // Displays "Q"
    SEG_E + SEG_G,                                         // Displays "r"
    SEG_A + SEG_C + SEG_D + SEG_F + SEG_G,                 // Displays "S"
    SEG_D + SEG_E + SEG_F + SEG_G,                         // Displays "t"
    SEG_C + SEG_D + SEG_E,                                 // Displays "u"
    SEG_C + SEG_D + SEG_E,                                 // Displays "u"
    SEG_G,                                                 // Displays "-"
    SEG_B + SEG_C + +SEG_E + SEG_F + SEG_G,                // Displays "X"
    SEG_B + SEG_C + SEG_D + SEG_F + SEG_G,                 // Displays "Y"
    SEG_A + SEG_B + SEG_D + SEG_E + SEG_G,                 // Displays "Z"
};

// Table with memory address for each display element
const u8 *segments_lcdmem[] = {
    LCD_SYMB_AM_MEM,
    LCD_SYMB_PM_MEM,
    LCD_SYMB_ARROW_UP_MEM,
    LCD_SYMB_ARROW_DOWN_MEM,
    LCD_SYMB_PERCENT_MEM,
    LCD_SYMB_TOTAL_MEM,
    LCD_SYMB_AVERAGE_MEM,
    LCD_SYMB_MAX_MEM,
    LCD_SYMB_BATTERY_MEM,
    LCD_UNIT_L1_FT_MEM,
    LCD_UNIT_L1_K_MEM,
    LCD_UNIT_L1_M_MEM,
    LCD_UNIT_L1_I_MEM,
    LCD_UNIT_L1_PER_S_MEM,
    LCD_UNIT_L1_PER_H_MEM,
    LCD_UNIT_L1_DEGREE_MEM,
    LCD_UNIT_L2_KCAL_MEM,
    LCD_UNIT_L2_KM_MEM,
    LCD_UNIT_L2_MI_MEM,
    LCD_ICON_HEART_MEM,
    LCD_ICON_STOPWATCH_MEM,
    LCD_ICON_RECORD_MEM,
    LCD_ICON_ALARM_MEM,
    LCD_ICON_BEEPER1_MEM,
    LCD_ICON_BEEPER2_MEM,
    LCD_ICON_BEEPER3_MEM,
    LCD_SEG_L1_3_MEM,
    LCD_SEG_L1_2_MEM,
    LCD_SEG_L1_1_MEM,
    LCD_SEG_L1_0_MEM,
    LCD_SEG_L1_COL_MEM,
    LCD_SEG_L1_DP1_MEM,
    LCD_SEG_L1_DP0_MEM,
    LCD_SEG_L2_5_MEM,
    LCD_SEG_L2_4_MEM,
    LCD_SEG_L2_3_MEM,
    LCD_SEG_L2_2_MEM,
    LCD_SEG_L2_1_MEM,
    LCD_SEG_L2_0_MEM,
    LCD_SEG_L2_COL1_MEM,
    LCD_SEG_L2_COL0_MEM,
    LCD_SEG_L2_DP_MEM,
};

// Table with bit mask for each display element
const u8 segments_bitmask[] = {
    LCD_SYMB_AM_MASK,
    LCD_SYMB_PM_MASK,
    LCD_SYMB_ARROW_UP_MASK,
    LCD_SYMB_ARROW_DOWN_MASK,
    LCD_SYMB_PERCENT_MASK,
    LCD_SYMB_TOTAL_MASK,
    LCD_SYMB_AVERAGE_MASK,
    LCD_SYMB_MAX_MASK,
    LCD_SYMB_BATTERY_MASK,
    LCD_UNIT_L1_FT_MASK,
    LCD_UNIT_L1_K_MASK,
    LCD_UNIT_L1_M_MASK,
    LCD_UNIT_L1_I_MASK,
    LCD_UNIT_L1_PER_S_MASK,
    LCD_UNIT_L1_PER_H_MASK,
    LCD_UNIT_L1_DEGREE_MASK,
    LCD_UNIT_L2_KCAL_MASK,
    LCD_UNIT_L2_KM_MASK,
    LCD_UNIT_L2_MI_MASK,
    LCD_ICON_HEART_MASK,
    LCD_ICON_STOPWATCH_MASK,
    LCD_ICON_RECORD_MASK,
    LCD_ICON_ALARM_MASK,
    LCD_ICON_BEEPER1_MASK,
    LCD_ICON_BEEPER2_MASK,
    LCD_ICON_BEEPER3_MASK,
    LCD_SEG_L1_3_MASK,
    LCD_SEG_L1_2_MASK,
    LCD_SEG_L1_1_MASK,
    LCD_SEG_L1_0_MASK,
    LCD_SEG_L1_COL_MASK,
    LCD_SEG_L1_DP1_MASK,
    LCD_SEG_L1_DP0_MASK,
    LCD_SEG_L2_5_MASK,
    LCD_SEG_L2_4_MASK,
    LCD_SEG_L2_3_MASK,
    LCD_SEG_L2_2_MASK,
    LCD_SEG_L2_1_MASK,
    LCD_SEG_L2_0_MASK,
    LCD_SEG_L2_COL1_MASK,
    LCD_SEG_L2_COL0_MASK,
    LCD_SEG_L2_DP_MASK,
};

// Quick integer to array conversion table for most common integer values
const u8 int_to_array_conversion_table[][3] = {
    "000", "001", "002", "003", "004", "005", "006", "007", "008", "009", "010", "011", "012",
    "013", "014", "015",
    "016", "017", "018", "019", "020", "021", "022", "023", "024", "025", "026", "027", "028",
    "029", "030", "031",
    "032", "033", "034", "035", "036", "037", "038", "039", "040", "041", "042", "043", "044",
    "045", "046", "047",
    "048", "049", "050", "051", "052", "053", "054", "055", "056", "057", "058", "059", "060",
    "061", "062", "063",
    "064", "065", "066", "067", "068", "069", "070", "071", "072", "073", "074", "075", "076",
    "077", "078", "079",
    "080", "081", "082", "083", "084", "085", "086", "087", "088", "089", "090", "091", "092",
    "093", "094", "095",
    "096", "097", "098", "099", "100", "101", "102", "103", "104", "105", "106", "107", "108",
    "109", "110", "111",
    "112", "113", "114", "115", "116", "117", "118", "119", "120", "121", "122", "123", "124",
    "125", "126", "127",
    "128", "129", "130", "131", "132", "133", "134", "135", "136", "137", "138", "139", "140",
    "141", "142", "143",
    "144", "145", "146", "147", "148", "149", "150", "151", "152", "153", "154", "155", "156",
    "157", "158", "159",
    "160", "161", "162", "163", "164", "165", "166", "167", "168", "169", "170", "171", "172",
    "173", "174", "175",
    "176", "177", "178", "179", "180",
};

// Display flags
volatile s_display_flags display;

// Global return string for int_to_array function
u8 int_to_array_str[8];

// *************************************************************************************************
// Extern section
extern void (*fptr_lcd_function_line1)(u8 line, u8 update);
extern void (*fptr_lcd_function_line2)(u8 line, u8 update);

// *************************************************************************************************
// @fn          lcd_init
// @brief       Erase LCD memory. Init LCD peripheral.
// @param       none
// @return      none
// *************************************************************************************************
void lcd_init(void)
{
    // Clear entire display memory
    LCDBMEMCTL |= LCDCLRBM + LCDCLRM;

    // LCD_FREQ = ACLK/16/8 = 256Hz
    // Frame frequency = 256Hz/4 = 64Hz, LCD mux 4, LCD on
    LCDBCTL0 = (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3) | (LCDPRE0 + LCDPRE1) | LCD4MUX | LCDON;

    // LCB_BLK_FREQ = ACLK/8/4096 = 1Hz
    LCDBBLKCTL = LCDBLKPRE0 | LCDBLKPRE1 | LCDBLKDIV0 | LCDBLKDIV1 | LCDBLKDIV2 | LCDBLKMOD0;

    // I/O to COM outputs
    P5SEL |= (BIT5 | BIT6 | BIT7);
    P5DIR |= (BIT5 | BIT6 | BIT7);

    // Activate LCD output
    LCDBPCTL0 = 0xFFFF;         // Select LCD segments S0-S15
    LCDBPCTL1 = 0x00FF;         // Select LCD segments S16-S22

#ifdef USE_LCD_CHARGE_PUMP
    // Charge pump voltage generated internally, internal bias (V2-V4) generation
    LCDBVCTL = LCDCPEN | VLCD_2_72;
#endif
}

// *************************************************************************************************
// @fn          clear_display_all
// @brief       Erase LINE1 and LINE2 segments. Clear also function-specific content.
// @param       none
// @return      none
// *************************************************************************************************
void clear_display_all(void)
{
    // Clear generic content
    clear_line(LINE1);
    clear_line(LINE2);

    // Clean up function-specific content
    fptr_lcd_function_line1(LINE1, DISPLAY_LINE_CLEAR);
    fptr_lcd_function_line2(LINE2, DISPLAY_LINE_CLEAR);
}

// *************************************************************************************************
// @fn          clear_display
// @brief       Erase LINE1 and LINE2 segments. Keep icons.
// @param       none
// @return      none
// *************************************************************************************************
void clear_display(void)
{
    clear_line(LINE1);
    clear_line(LINE2);
}

// *************************************************************************************************
// @fn          clear_line
// @brief       Erase segments of a given line.
// @param       u8 line LINE1, LINE2
// @return      none
// *************************************************************************************************
void clear_line(u8 line)
{
    display_chars(switch_seg(line, LCD_SEG_L1_3_0, LCD_SEG_L2_5_0), NULL, SEG_OFF);
    if (line == LINE1)
    {
        display_symbol(LCD_SEG_L1_DP1, SEG_OFF);
        display_symbol(LCD_SEG_L1_DP0, SEG_OFF);
        display_symbol(LCD_SEG_L1_COL, SEG_OFF);
    }
    else                        // line == LINE2
    {
        display_symbol(LCD_SEG_L2_DP, SEG_OFF);
        display_symbol(LCD_SEG_L2_COL1, SEG_OFF);
        display_symbol(LCD_SEG_L2_COL0, SEG_OFF);
    }
}

// *************************************************************************************************
// @fn          write_segment
// @brief       Write to one or multiple LCD segments
// @param       lcdmem          Pointer to LCD byte memory
//                              bits            Segments to address
//                              bitmask         Bitmask for particular display item
//                              mode            On, off or blink segments
// @return
// *************************************************************************************************
void write_lcd_mem(u8 * lcdmem, u8 bits, u8 bitmask, u8 state)
{
    if (state == SEG_ON)
    {
        // Clear segments before writing
        *lcdmem = (u8) (*lcdmem & ~bitmask);

        // Set visible segments
        *lcdmem = (u8) (*lcdmem | bits);
    }
    else if (state == SEG_OFF)
    {
        // Clear segments
        *lcdmem = (u8) (*lcdmem & ~bitmask);
    }
    else if (state == SEG_ON_BLINK_ON)
    {
        // Clear visible / blink segments before writing
        *lcdmem = (u8) (*lcdmem & ~bitmask);
        *(lcdmem + 0x20) = (u8) (*(lcdmem + 0x20) & ~bitmask);

        // Set visible / blink segments
        *lcdmem = (u8) (*lcdmem | bits);
        *(lcdmem + 0x20) = (u8) (*(lcdmem + 0x20) | bits);
    }
    else if (state == SEG_ON_BLINK_OFF)
    {
        // Clear visible segments before writing
        *lcdmem = (u8) (*lcdmem & ~bitmask);

        // Set visible segments
        *lcdmem = (u8) (*lcdmem | bits);

        // Clear blink segments
        *(lcdmem + 0x20) = (u8) (*(lcdmem + 0x20) & ~bitmask);
    }
    else if (state == SEG_OFF_BLINK_OFF)
    {
        // Clear segments
        *lcdmem = (u8) (*lcdmem & ~bitmask);

        // Clear blink segments
        *(lcdmem + 0x20) = (u8) (*(lcdmem + 0x20) & ~bitmask);
    }
}

// *************************************************************************************************
// @fn          int_to_array
// @brief       Generic integer to array routine. Converts integer n to string.
//                              Default conversion result has leading zeros, e.g. "00123"
//                              Option to convert leading '0' into whitespace (blanks)
// @param       u32 n                   integer to convert
//                              u8 digits               number of digits
//                              u8 blanks               fill up result string with number of
// whitespaces instead of leading zeros
// @return      u8                              string
// *************************************************************************************************
u8 *int_to_array(u32 n, u8 digits, u8 blanks)
{
    u8 i;
    u8 digits1 = digits;

    // Preset result string
    memcpy(int_to_array_str, "0000000", 7);

    // Return empty string if number of digits is invalid (valid range for digits: 1-7)
    if ((digits == 0) || (digits > 7))
        return (int_to_array_str);

    // Numbers 0 .. 180 can be copied from int_to_array_conversion_table without conversion
    if (n <= 180)
    {
        if (digits >= 3)
        {
            memcpy(int_to_array_str + (digits - 3), int_to_array_conversion_table[n], 3);
        }
        else                    // digits == 1 || 2
        {
            memcpy(int_to_array_str, int_to_array_conversion_table[n] + (3 - digits), digits);
        }
    }
    else                        // For n > 180 need to calculate string content
    {
        // Calculate digits from least to most significant number
        do
        {
            int_to_array_str[digits - 1] = n % 10 + '0';
            n /= 10;
        }
        while (--digits > 0);
    }

    // Remove specified number of leading '0', always keep last one
    i = 0;
    while ((int_to_array_str[i] == '0') && (i < digits1 - 1))
    {
        if (blanks > 0)
        {
            // Convert only specified number of leading '0'
            int_to_array_str[i] = ' ';
            blanks--;
        }
        i++;
    }

    return (int_to_array_str);
}

// *************************************************************************************************
// @fn          display_value
// @brief       Generic decimal display routine. Used exclusively by set_value function.
// @param       u8 segments                     LCD segments where value is displayed
//                              u32 value                       Integer value to be displayed
//                              u8 digits                       Number of digits to convert
//                              u8 blanks                       Number of leadings blanks in
// int_to_array result string
// @return      none
// *************************************************************************************************
void display_value(u8 segments, u32 value, u8 digits, u8 blanks)
{
    u8 *str;

    str = int_to_array(value, digits, blanks);

    // Display string in blink mode
    display_chars(segments, str, SEG_ON_BLINK_ON);
}

// *************************************************************************************************
// @fn          display_hours
// @brief       Display hours in 24H / 12H time format.
// @param       u8 segments     Segments where to display hour data
//                              u32 value               Hour data
//                              u8 digits               Must be "2"
//                              u8 blanks               Must be "0"
// @return      none
// *************************************************************************************************
void display_hours(u8 segments, u32 value, u8 digits, u8 blanks)
{
    u8 hours;

    if (sys.flag.use_metric_units)
    {
        // Display hours in 24H time format
        display_value(segments, (u16) value, digits, blanks);
    }
    else
    {
        // convert internal 24H time format to 12H time format
        hours = convert_hour_to_12H_format(value);

        // display hours in 12H time format
        display_value(segments, hours, digits, blanks);
        display_am_pm_symbol(value);
    }
}

// *************************************************************************************************
// @fn          display_am_pm_symbol
// @brief       Display AM or PM symbol.
// @param       u8 hour         24H internal time format
// @return      none
// *************************************************************************************************
void display_am_pm_symbol(u8 hour)
{
    // Display AM/PM symbol
    if (is_hour_am(hour))
    {
        display_symbol(LCD_SYMB_AM, SEG_ON);
    }
    else
    {
        // Clear AM segments first - required when changing from AM to PM
        display_symbol(LCD_SYMB_AM, SEG_OFF);
        display_symbol(LCD_SYMB_PM, SEG_ON);
    }
}

// *************************************************************************************************
// @fn          display_symbol
// @brief       Switch symbol on or off on LCD.
// @param       u8 symbol               A valid LCD symbol (index 0..42)
//                              u8 state                SEG_ON, SEG_OFF, SEG_BLINK
// @return      none
// *************************************************************************************************
void display_symbol(u8 symbol, u8 mode)
{
    u8 *lcdmem;
    u8 bits;
    u8 bitmask;

    if (symbol <= LCD_SEG_L2_DP)
    {
        // Get LCD memory address for symbol from table
        lcdmem = (u8 *) segments_lcdmem[symbol];

        // Get bits for symbol from table
        bits = segments_bitmask[symbol];

        // Bitmask for symbols equals bits
        bitmask = bits;

        // Write LCD memory
        write_lcd_mem(lcdmem, bits, bitmask, mode);
    }
}

// *************************************************************************************************
// @fn          display_char
// @brief       Write to 7-segment characters.
// @param       u8 segment              A valid LCD segment
//                              u8 chr                  Character to display
//                              u8 mode         SEG_ON, SEG_OFF, SEG_BLINK
// @return      none
// *************************************************************************************************
void display_char(u8 segment, u8 chr, u8 mode)
{
    u8 *lcdmem;                 // Pointer to LCD memory
    u8 bitmask;                 // Bitmask for character
    u8 bits, bits1;             // Bits to write

    // Write to single 7-segment character
    if ((segment >= LCD_SEG_L1_3) && (segment <= LCD_SEG_L2_DP))
    {
        // Get LCD memory address for segment from table
        lcdmem = (u8 *) segments_lcdmem[segment];

        // Get bitmask for character from table
        bitmask = segments_bitmask[segment];

        // Get bits from font set
        if ((chr >= 0x30) && (chr <= 0x5A))
        {
            // Use font set
            bits = lcd_font[chr - 0x30];
        }
        else if (chr == 0x2D)
        {
            // '-' not in font set
            bits = BIT1;
        }
        else
        {
            // Other characters map to ' ' (blank)
            bits = 0;
        }

        // When addressing LINE2 7-segment characters need to swap high- and low-nibble,
        // because LCD COM/SEG assignment is mirrored against LINE1
        if (segment >= LCD_SEG_L2_5)
        {
            bits1 = ((bits << 4) & 0xF0) | ((bits >> 4) & 0x0F);
            bits = bits1;

            // When addressing LCD_SEG_L2_5, need to convert ASCII '1' and 'L' to 1 bit,
            // because LCD COM/SEG assignment is special for this incomplete character
            if (segment == LCD_SEG_L2_5)
            {
                if ((chr == '1') || (chr == 'L'))
                    bits = BIT7;
            }
        }

        // Physically write to LCD memory
        write_lcd_mem(lcdmem, bits, bitmask, mode);
    }
}

// *************************************************************************************************
// @fn          display_chars
// @brief       Write to consecutive 7-segment characters.
// @param       u8 segments     LCD segment array
//                              u8 * str                Pointer to a string
//                              u8 mode             SEG_ON, SEG_OFF, SEG_BLINK
// @return      none
// *************************************************************************************************
void display_chars(u8 segments, u8 * str, u8 mode)
{
    u8 i;
    u8 length = 0;              // Write length
    u8 char_start;              // Starting point for consecutive write

    switch (segments)
    {
        // LINE1
        case LCD_SEG_L1_3_0:
            length = 4;
            char_start = LCD_SEG_L1_3;
            break;
        case LCD_SEG_L1_2_0:
            length = 3;
            char_start = LCD_SEG_L1_2;
            break;
        case LCD_SEG_L1_1_0:
            length = 2;
            char_start = LCD_SEG_L1_1;
            break;
        case LCD_SEG_L1_3_1:
            length = 3;
            char_start = LCD_SEG_L1_3;
            break;
        case LCD_SEG_L1_3_2:
            length = 2;
            char_start = LCD_SEG_L1_3;
            break;

        // LINE2
        case LCD_SEG_L2_5_0:
            length = 6;
            char_start = LCD_SEG_L2_5;
            break;
        case LCD_SEG_L2_4_0:
            length = 5;
            char_start = LCD_SEG_L2_4;
            break;
        case LCD_SEG_L2_3_0:
            length = 4;
            char_start = LCD_SEG_L2_3;
            break;
        case LCD_SEG_L2_2_0:
            length = 3;
            char_start = LCD_SEG_L2_2;
            break;
        case LCD_SEG_L2_1_0:
            length = 2;
            char_start = LCD_SEG_L2_1;
            break;
        case LCD_SEG_L2_5_4:
            length = 2;
            char_start = LCD_SEG_L2_5;
            break;
        case LCD_SEG_L2_5_2:
            length = 4;
            char_start = LCD_SEG_L2_5;
            break;
        case LCD_SEG_L2_3_2:
            length = 2;
            char_start = LCD_SEG_L2_3;
            break;
        case LCD_SEG_L2_4_2:
            length = 3;
            char_start = LCD_SEG_L2_4;
            break;
    }

    // Write to consecutive digits
    for (i = 0; i < length; i++)
    {
        // Use single character routine to write display memory
        display_char(char_start + i, *(str + i), mode);
    }
}

// *************************************************************************************************
// @fn          switch_seg
// @brief       Returns index of 7-segment character. Required for display routines that can draw
//                              information on both lines.
// @param       u8 line             LINE1 or LINE2
//                              u8 index1               Index of LINE1
//                              u8 index2               Index of LINE2
// @return      uint8
// *************************************************************************************************
u8 switch_seg(u8 line, u8 index1, u8 index2)
{
    if (line == LINE1)
    {
        return index1;
    }
    else                        // line == LINE2
    {
        return index2;
    }
}

// *************************************************************************************************
// @fn          start_blink
// @brief       Start blinking.
// @param       none
// @return      none
// *************************************************************************************************
void start_blink(void)
{
    LCDBBLKCTL |= LCDBLKMOD0;
}

// *************************************************************************************************
// @fn          stop_blink
// @brief       Stop blinking.
// @param       none
// @return      none
// *************************************************************************************************
void stop_blink(void)
{
    LCDBBLKCTL &= ~LCDBLKMOD0;
}

// *************************************************************************************************
// @fn          stop_blink
// @brief       Clear blinking memory.
// @param       none
// @return      none
// *************************************************************************************************
void clear_blink_mem(void)
{
    LCDBMEMCTL |= LCDCLRBM;
}

// *************************************************************************************************
// @fn          set_blink_rate
// @brief       Set blink rate register bits.
// @param       none
// @return      none
// *************************************************************************************************
void set_blink_rate(u8 bits)
{
    LCDBBLKCTL &= ~(BIT7 | BIT6 | BIT5);
    LCDBBLKCTL |= bits;
}

