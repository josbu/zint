/*
    libzint - the open source barcode library
    Copyright (C) 2019-2025 Robin Stuart <rstuart114@gmail.com>

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the project nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
 */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "testcommon.h"

static void test_hrt(const testCtx *const p_ctx) {
    int debug = p_ctx->debug;

    struct item {
        int option_2;
        int output_options;
        const char *data;
        int length;

        const char *expected;
    };
    /* s/\/\*[ 0-9]*\*\//\=printf("\/\*%3d*\/", line(".") - line("'<")): */
    static const struct item data[] = {
        /*  0*/ { -1, -1, "1", -1, "01" },
        /*  1*/ { -1, BARCODE_RAW_TEXT, "1", -1, "01" }, /* No difference */
        /*  2*/ { 3, -1, "1", -1, "01" },
        /*  3*/ { 3, BARCODE_RAW_TEXT, "1", -1, "01" },
        /*  4*/ { 3, -1, "12", -1, "12" },
        /*  5*/ { 3, BARCODE_RAW_TEXT, "12", -1, "12" },
        /*  6*/ { 4, -1, "123", -1, "123" },
        /*  7*/ { 4, BARCODE_RAW_TEXT, "123", -1, "123" },
        /*  8*/ { 5, -1, "123", -1, "0123" },
        /*  9*/ { 5, BARCODE_RAW_TEXT, "123", -1, "0123" },
        /* 10*/ { 5, -1, "12", -1, "0012" },
        /* 11*/ { 5, BARCODE_RAW_TEXT, "12", -1, "0012" },
        /* 12*/ { 5, -1, "1", -1, "0001" },
        /* 13*/ { 5, BARCODE_RAW_TEXT, "1", -1, "0001" },
        /* 14*/ { 5, -1, "1234", -1, "1234" },
        /* 15*/ { 5, BARCODE_RAW_TEXT, "1234", -1, "1234" },
        /* 16*/ { 6, -1, "1234", -1, "01234" },
        /* 17*/ { 6, BARCODE_RAW_TEXT, "1234", -1, "01234" },
        /* 18*/ { 6, -1, "123", -1, "00123" },
        /* 19*/ { 6, BARCODE_RAW_TEXT, "123", -1, "00123" },
        /* 20*/ { 6, -1, "12", -1, "00012" },
        /* 21*/ { 6, BARCODE_RAW_TEXT, "12", -1, "00012" },
        /* 22*/ { 6, -1, "1", -1, "00001" },
        /* 23*/ { 6, BARCODE_RAW_TEXT, "1", -1, "00001" },
        /* 24*/ { 7, -1, "1234", -1, "001234" },
        /* 25*/ { 7, BARCODE_RAW_TEXT, "1234", -1, "001234" },
        /* 26*/ { 7, -1, "12345", -1, "012345" },
        /* 27*/ { 7, BARCODE_RAW_TEXT, "12345", -1, "012345" },
        /* 28*/ { 7, -1, "123456", -1, "123456" },
        /* 29*/ { 7, BARCODE_RAW_TEXT, "123456", -1, "123456" },
        /* 30*/ { 7, -1, "1", -1, "000001" },
        /* 31*/ { 7, BARCODE_RAW_TEXT, "1", -1, "000001" },
        /* 32*/ { 8, -1, "12345", -1, "0012345" },
        /* 33*/ { 8, BARCODE_RAW_TEXT, "12345", -1, "0012345" },
        /* 34*/ { 8, -1, "123456", -1, "0123456" },
        /* 35*/ { 8, BARCODE_RAW_TEXT, "123456", -1, "0123456" },
        /* 36*/ { 8, -1, "1234567", -1, "1234567" },
        /* 37*/ { 8, BARCODE_RAW_TEXT, "1234567", -1, "1234567" },
        /* 38*/ { 8, -1, "12", -1, "0000012" },
        /* 39*/ { 8, BARCODE_RAW_TEXT, "12", -1, "0000012" },
        /* 40*/ { 8, -1, "1", -1, "0000001" },
        /* 41*/ { 8, BARCODE_RAW_TEXT, "1", -1, "0000001" },
    };
    const int data_size = ARRAY_SIZE(data);
    int i, length, ret;
    struct zint_symbol *symbol = NULL;
    int expected_length;

    testStartSymbol(p_ctx->func_name, &symbol);

    for (i = 0; i < data_size; i++) {

        if (testContinue(p_ctx, i)) continue;

        symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        length = testUtilSetSymbol(symbol, BARCODE_CHANNEL, -1 /*input_mode*/, -1 /*eci*/,
                    -1 /*option_1*/, data[i].option_2, -1, data[i].output_options,
                    data[i].data, data[i].length, debug);
        expected_length = (int) strlen(data[i].expected);

        ret = ZBarcode_Encode(symbol, TCU(data[i].data), length);
        assert_zero(ret, "i:%d ZBarcode_Encode ret %d != 0 %s\n", i, ret, symbol->errtxt);

        assert_equal(symbol->text_length, expected_length, "i:%d text_length %d != expected_length %d\n",
                    i, symbol->text_length, expected_length);
        assert_zero(strcmp((char *) symbol->text, data[i].expected), "i:%d strcmp(%s, %s) != 0\n",
                    i, symbol->text, data[i].expected);
        if (symbol->output_options & BARCODE_RAW_TEXT) {
            assert_nonnull(symbol->raw_segs, "i:%d raw_segs NULL\n", i);
            assert_nonnull(symbol->raw_segs[0].source, "i:%d raw_segs[0].source NULL\n", i);
            assert_equal(symbol->raw_segs[0].length, expected_length,
                        "i:%d raw_segs[0].length %d != expected_length %d\n",
                        i, symbol->raw_segs[0].length, expected_length);
            assert_zero(memcmp(symbol->raw_segs[0].source, data[i].expected, expected_length),
                        "i:%d memcmp(%.*s, %s, %d) != 0\n",
                        i, symbol->raw_segs[0].length, symbol->raw_segs[0].source, data[i].expected, expected_length);
        } else {
            assert_null(symbol->raw_segs, "i:%d raw_segs not NULL\n", i);
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

static void test_input(const testCtx *const p_ctx) {
    int debug = p_ctx->debug;

    struct item {
        int option_2;
        const char *data;
        int ret;
        int expected_rows;
        int expected_width;
        const char *expected_errtxt;
        int expected_option_2;
    };
    /* s/\/\*[ 0-9]*\*\//\=printf("\/\*%3d*\/", line(".") - line("'<")): */
    static const struct item data[] = {
        /*  0*/ { -1, "0", 0, 1, 19, "", 3 }, /* < 3 ignored */
        /*  1*/ { 0, "0", 0, 1, 19, "", 3 },
        /*  2*/ { 1, "0", 0, 1, 19, "", 3 },
        /*  3*/ { 2, "0", 0, 1, 19, "", 3 },
        /*  4*/ { 9, "0", 0, 1, 19, "", 3 }, /* > 8 ignored */
        /*  5*/ { -1, "00", 0, 1, 19, "", 3 },
        /*  6*/ { 3, "00", 0, 1, 19, "", 3 },
        /*  7*/ { -1, "26", 0, 1, 19, "", 3 },
        /*  8*/ { 3, "26", 0, 1, 19, "", 3 },
        /*  9*/ { 3, "27", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 335: Input value \"27\" out of range (0 to 26 for 3 channels)", 3 },
        /* 10*/ { 3, "027", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 335: Input value \"27\" out of range (0 to 26 for 3 channels)", 3 },
        /* 11*/ { 3, "000", 0, 1, 19, "", 3 },
        /* 12*/ { 3, "001", 0, 1, 19, "", 3 },
        /* 13*/ { 3, "026", 0, 1, 19, "", 3 },
        /* 14*/ { -1, "27", 0, 1, 23, "", 4 }, /* Channel 4 */
        /* 15*/ { -1, "026", 0, 1, 23, "", 4 }, /* Defaults to channel 4 due to length */
        /* 16*/ { 3, "0026", 0, 1, 19, "", 3 },
        /* 17*/ { 3, "1234", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 335: Input value \"1234\" out of range (0 to 26 for 3 channels)", 3 },
        /* 18*/ { 4, "000", 0, 1, 23, "", 4 },
        /* 19*/ { -1, "000", 0, 1, 23, "", 4 }, /* Defaults to channel 4 due to length */
        /* 20*/ { 4, "026", 0, 1, 23, "", 4 },
        /* 21*/ { 4, "0000026", 0, 1, 23, "", 4 },
        /* 22*/ { 4, "0000", 0, 1, 23, "", 4 },
        /* 23*/ { 4, "292", 0, 1, 23, "", 4 },
        /* 24*/ { 4, "293", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 335: Input value \"293\" out of range (0 to 292 for 4 channels)", 4 },
        /* 25*/ { 4, "000293", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 335: Input value \"293\" out of range (0 to 292 for 4 channels)", 4 },
        /* 26*/ { -1, "293", 0, 1, 27, "", 5 }, /* Channel 5 */
        /* 27*/ { 5, "0000", 0, 1, 27, "", 5 },
        /* 28*/ { -1, "0000", 0, 1, 27, "", 5 }, /* Defaults to channel 5 due to length */
        /* 29*/ { -1, "3493", 0, 1, 27, "", 5 },
        /* 30*/ { 5, "3493", 0, 1, 27, "", 5 },
        /* 31*/ { 5, "3494", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 335: Input value \"3494\" out of range (0 to 3493 for 5 channels)", 5 },
        /* 32*/ { -1, "3494", 0, 1, 31, "", 6 }, /* Channel 6 */
        /* 33*/ { 6, "00000", 0, 1, 31, "", 6 },
        /* 34*/ { -1, "00000", 0, 1, 31, "", 6 }, /* Defaults to channel 6 due to length */
        /* 35*/ { -1, "44072", 0, 1, 31, "", 6 },
        /* 36*/ { 6, "44072", 0, 1, 31, "", 6 },
        /* 37*/ { 6, "44073", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 335: Input value \"44073\" out of range (0 to 44072 for 6 channels)", 6 },
        /* 38*/ { -1, "44073", 0, 1, 35, "", 7 }, /* Channel 7 */
        /* 39*/ { 7, "000000", 0, 1, 35, "", 7 },
        /* 40*/ { -1, "000000", 0, 1, 35, "", 7 }, /* Defaults to channel 7 due to length */
        /* 41*/ { 7, "576688", 0, 1, 35, "", 7 },
        /* 42*/ { 7, "576689", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 335: Input value \"576689\" out of range (0 to 576688 for 7 channels)", 7 },
        /* 43*/ { 7, "0576688", 0, 1, 35, "", 7 },
        /* 44*/ { -1, "1234567", 0, 1, 39, "", 8 },
        /* 45*/ { -1, "576689", 0, 1, 39, "", 8 }, /* Channel 8 */
        /* 46*/ { 8, "0000000", 0, 1, 39, "", 8 },
        /* 47*/ { -1, "0000000", 0, 1, 39, "", 8 }, /* Defaults to channel 8 due to length */
        /* 48*/ { 8, "1234567", 0, 1, 39, "", 8 },
        /* 49*/ { 8, "7742863", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 318: Input value \"7742863\" out of range (0 to 7742862)", 8 },
        /* 50*/ { 8, "01234567", ZINT_ERROR_TOO_LONG, -1, -1, "Error 333: Input length 8 too long (maximum 7)", 8 },
        /* 51*/ { 8, "00000000", ZINT_ERROR_TOO_LONG, -1, -1, "Error 333: Input length 8 too long (maximum 7)", 8 },
        /* 52*/ { 9, "7742863", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 318: Input value \"7742863\" out of range (0 to 7742862)", 9 },
        /* 53*/ { -1, "A", ZINT_ERROR_INVALID_DATA, -1, -1, "Error 334: Invalid character at position 1 in input (digits only)", 0 },
    };
    const int data_size = ARRAY_SIZE(data);
    int i, length, ret;
    struct zint_symbol *symbol = NULL;

    testStartSymbol(p_ctx->func_name, &symbol);

    for (i = 0; i < data_size; i++) {

        if (testContinue(p_ctx, i)) continue;

        symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        length = testUtilSetSymbol(symbol, BARCODE_CHANNEL, -1 /*input_mode*/, -1 /*eci*/, -1 /*option_1*/, data[i].option_2, -1, -1 /*output_options*/, data[i].data, -1, debug);

        ret = ZBarcode_Encode(symbol, TCU(data[i].data), length);
        assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d\n", i, ret, data[i].ret);
        assert_equal(symbol->errtxt[0] == '\0', ret == 0, "i:%d symbol->errtxt not %s (%s)\n", i, ret ? "set" : "empty", symbol->errtxt);
        assert_zero(strcmp(symbol->errtxt, data[i].expected_errtxt), "i:%d strcmp(%s, %s) != 0\n", i, symbol->errtxt, data[i].expected_errtxt);
        assert_equal(symbol->option_2, data[i].expected_option_2, "i:%d symbol->option_2 %d != %d\n",
                    i, symbol->option_2, data[i].expected_option_2);

        if (ret < ZINT_ERROR) {
            assert_equal(symbol->rows, data[i].expected_rows, "i:%d symbol->rows %d != %d (%s)\n", i, symbol->rows, data[i].expected_rows, data[i].data);
            assert_equal(symbol->width, data[i].expected_width, "i:%d symbol->width %d != %d (%s)\n", i, symbol->width, data[i].expected_width, data[i].data);
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

static void test_encode(const testCtx *const p_ctx) {
    int debug = p_ctx->debug;

    int do_bwipp = (debug & ZINT_DEBUG_TEST_BWIPP) && testUtilHaveGhostscript(); /* Only do BWIPP test if asked, too slow otherwise */

    struct item {
        int option_2;
        const char *data;
        int ret;

        int expected_rows;
        int expected_width;
        const char *comment;
        const char *expected;
    };
    static const struct item data[] = {
        /*  0*/ { -1, "1234", 0, 1, 27, "ANSI/AIM BC12-1998 Figure 1",
                    "101010101001010010011110011"
                },
        /*  1*/ { -1, "00", 0, 1, 19, "ANSI/AIM BC12-1998 Figure F1 Channel 3 top",
                    "1010101010110100011"
                },
        /*  2*/ { -1, "02", 0, 1, 19, "ANSI/AIM BC12-1998 Figure F1 Channel 3 2nd",
                    "1010101010110010011"
                },
        /*  3*/ { -1, "05", 0, 1, 19, "ANSI/AIM BC12-1998 Figure F1 Channel 3 3rd",
                    "1010101010110001101"
                },
        /*  4*/ { -1, "08", 0, 1, 19, "ANSI/AIM BC12-1998 Figure F1 Channel 3 4th",
                    "1010101010111000101"
                },
        /*  5*/ { -1, "10", 0, 1, 19, "ANSI/AIM BC12-1998 Figure F1 Channel 3 5th",
                    "1010101010010110011"
                },
        /*  6*/ { -1, "26", 0, 1, 19, "ANSI/AIM BC12-1998 Figure F1 Channel 3 bottom",
                    "1010101010001110101"
                },
        /*  7*/ { -1, "000", 0, 1, 23, "ANSI/AIM BC12-1998 Figure F1 Channel 4 top",
                    "10101010101101010000111"
                },
        /*  8*/ { -1, "004", 0, 1, 23, "ANSI/AIM BC12-1998 Figure F1 Channel 4 2nd",
                    "10101010101101001100011"
                },
        /*  9*/ { -1, "007", 0, 1, 23, "ANSI/AIM BC12-1998 Figure F1 Channel 4 3rd",
                    "10101010101101000110011"
                },
        /* 10*/ { -1, "010", 0, 1, 23, "ANSI/AIM BC12-1998 Figure F1 Channel 4 4th",
                    "10101010101101000011011"
                },
        /* 11*/ { -1, "100", 0, 1, 23, "ANSI/AIM BC12-1998 Figure F1 Channel 4 5th",
                    "10101010100101011100011"
                },
        /* 12*/ { -1, "292", 0, 1, 23, "ANSI/AIM BC12-1998 Figure F1 Channel 4 bottom",
                    "10101010100001110110101"
                },
        /* 13*/ { -1, "0000", 0, 1, 27, "ANSI/AIM BC12-1998 Figure F1 Channel 5 top",
                    "101010101011010101100000111"
                },
        /* 14*/ { -1, "0005", 0, 1, 27, "ANSI/AIM BC12-1998 Figure F1 Channel 5 2nd",
                    "101010101011010100111000011"
                },
        /* 15*/ { -1, "0010", 0, 1, 27, "ANSI/AIM BC12-1998 Figure F1 Channel 5 3rd",
                    "101010101011010100011110001"
                },
        /* 16*/ { -1, "0100", 0, 1, 27, "ANSI/AIM BC12-1998 Figure F1 Channel 5 4th",
                    "101010101011010001000111101"
                },
        /* 17*/ { -1, "1000", 0, 1, 27, "ANSI/AIM BC12-1998 Figure F1 Channel 5 5th",
                    "101010101011100001011011001"
                },
        /* 18*/ { -1, "3493", 0, 1, 27, "ANSI/AIM BC12-1998 Figure F1 Channel 5 bottom",
                    "101010101000001111010110101"
                },
        /* 19*/ { -1, "00000", 0, 1, 31, "ANSI/AIM BC12-1998 Figure F1 Channel 6 top",
                    "1010101010110101011010000001111"
                },
        /* 20*/ { -1, "00010", 0, 1, 31, "ANSI/AIM BC12-1998 Figure F1 Channel 6 2nd",
                    "1010101010110101011000111000011"
                },
        /* 21*/ { -1, "00100", 0, 1, 31, "ANSI/AIM BC12-1998 Figure F1 Channel 6 3rd",
                    "1010101010110101001100001111001"
                },
        /* 22*/ { -1, "01000", 0, 1, 31, "ANSI/AIM BC12-1998 Figure F1 Channel 6 4th",
                    "1010101010110100110011000100111"
                },
        /* 23*/ { -1, "10000", 0, 1, 31, "ANSI/AIM BC12-1998 Figure F1 Channel 6 5th",
                    "1010101010111011000100010110011"
                },
        /* 24*/ { -1, "44072", 0, 1, 31, "ANSI/AIM BC12-1998 Figure F1 Channel 6 bottom",
                    "1010101010000001111101010110101"
                },
        /* 25*/ { -1, "000000", 0, 1, 35, "ANSI/AIM BC12-1998 Figure F1 Channel 7 top",
                    "10101010101101010110101000000011111"
                },
        /* 26*/ { -1, "000100", 0, 1, 35, "ANSI/AIM BC12-1998 Figure F1 Channel 7 2nd",
                    "10101010101101010110111110001000001"
                },
        /* 27*/ { -1, "001000", 0, 1, 35, "ANSI/AIM BC12-1998 Figure F1 Channel 7 3rd",
                    "10101010101101010010100001111100011"
                },
        /* 28*/ { -1, "010000", 0, 1, 35, "ANSI/AIM BC12-1998 Figure F1 Channel 7 4th",
                    "10101010101101001010111111000100001"
                },
        /* 29*/ { -1, "100000", 0, 1, 35, "ANSI/AIM BC12-1998 Figure F1 Channel 7 5th",
                    "10101010101100001001001111101101001"
                },
        /* 30*/ { -1, "576688", 0, 1, 35, "ANSI/AIM BC12-1998 Figure F1 Channel 7 bottom",
                    "10101010100000001111101101010110101"
                },
        /* 31*/ { -1, "0000000", 0, 1, 39, "ANSI/AIM BC12-1998 Figure F1 Channel 8 top",
                    "101010101011010101101010110000000011111"
                },
        /* 32*/ { -1, "0001000", 0, 1, 39, "ANSI/AIM BC12-1998 Figure F1 Channel 8 2nd",
                    "101010101011010101101100010000010011111"
                },
        /* 33*/ { -1, "0010000", 0, 1, 39, "ANSI/AIM BC12-1998 Figure F1 Channel 8 3rd",
                    "101010101011010101110000110001101100011"
                },
        /* 34*/ { -1, "0100000", 0, 1, 39, "ANSI/AIM BC12-1998 Figure F1 Channel 8 4th",
                    "101010101011010111010110101100000000111"
                },
        /* 35*/ { -1, "1000000", 0, 1, 39, "ANSI/AIM BC12-1998 Figure F1 Channel 8 5th",
                    "101010101011001110100100100001111001011"
                },
        /* 36*/ { -1, "7742862", 0, 1, 39, "ANSI/AIM BC12-1998 Figure F1 Channel 8 bottom",
                    "101010101000000001111110101101010110101"
                },
        /* 37*/ { -1, "01", 0, 1, 19, "Edge case for initial_precalcs table",
                    "1010101010110110001"
                },
        /* 38*/ { -1, "001", 0, 1, 23, "Edge case for initial_precalcs table",
                    "10101010101101011000011"
                },
        /* 39*/ { -1, "0001", 0, 1, 27, "Edge case for initial_precalcs table",
                    "101010101011010101110000011"
                },
        /* 40*/ { -1, "00001", 0, 1, 31, "Edge case for initial_precalcs table",
                    "1010101010110101011011000000111"
                },
        /* 41*/ { -1, "000001", 0, 1, 35, "Edge case for initial_precalcs table",
                    "10101010101101010110101100000001111"
                },
        /* 42*/ { -1, "0000001", 0, 1, 39, "Edge case for initial_precalcs table",
                    "101010101011010101101010111000000001111"
                },
        /* 43*/ { -1, "115337", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010101110100101001000111100011"
                },
        /* 44*/ { -1, "115338", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010101110100101001000111110001"
                },
        /* 45*/ { -1, "115339", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010101110100101001000010011111"
                },
        /* 46*/ { -1, "230675", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010100101100011001111010011001"
                },
        /* 47*/ { -1, "230676", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010100101100011001111010001011"
                },
        /* 48*/ { -1, "230677", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010100101100011001111010001101"
                },
        /* 49*/ { -1, "346013", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010100110011101010001011000111"
                },
        /* 50*/ { -1, "346014", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010100110011101010001011100011"
                },
        /* 51*/ { -1, "346015", 0, 1, 35, "Edge case for channel_precalcs7 table",
                    "10101010100110011101010001011110001"
                },
        /* 52*/ { -1, "0119120", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011010111000011000100111010011"
                },
        /* 53*/ { -1, "0119121", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011010111000011000100111011001"
                },
        /* 54*/ { -1, "0119122", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011010111000011000100111001011"
                },
        /* 55*/ { -1, "0238241", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011010001011011001100011110001"
                },
        /* 56*/ { -1, "0238242", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011010001011011001100001001111"
                },
        /* 57*/ { -1, "0238243", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011010001011011001100001100111"
                },
        /* 58*/ { -1, "1072088", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011001111101000101100011001001"
                },
        /* 59*/ { -1, "1072089", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011001111101000101100011000101"
                },
        /* 60*/ { -1, "1072090", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101011001111101000101100001010011"
                },
        /* 61*/ { -1, "4169234", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101001101001101101111011000100001"
                },
        /* 62*/ { -1, "4169235", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101001101001101101111011000010001"
                },
        /* 63*/ { -1, "4169236", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101001101001101101111011000001001"
                },
        /* 64*/ { -1, "6075170", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101000100011101010001110111001011"
                },
        /* 65*/ { -1, "6075171", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101000100011101010001110111001101"
                },
        /* 66*/ { -1, "6075172", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101000100011101010001110111101001"
                },
        /* 67*/ { -1, "7623743", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101000001110010101101100101001111"
                },
        /* 68*/ { -1, "7623744", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101000001110010101101100101100111"
                },
        /* 69*/ { -1, "7623745", 0, 1, 39, "Edge case for channel_precalcs8 table",
                    "101010101000001110010101101100101110011"
                },
    };
    const int data_size = ARRAY_SIZE(data);
    int i, length, ret;
    struct zint_symbol *symbol = NULL;

    char escaped[1024];
    char bwipp_buf[8192];
    char bwipp_msg[1024];

    testStartSymbol(p_ctx->func_name, &symbol);

    for (i = 0; i < data_size; i++) {

        if (testContinue(p_ctx, i)) continue;

        symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        length = testUtilSetSymbol(symbol, BARCODE_CHANNEL, -1 /*input_mode*/, -1 /*eci*/, -1 /*option_1*/, data[i].option_2, -1, -1 /*output_options*/, data[i].data, -1, debug);

        ret = ZBarcode_Encode(symbol, TCU(data[i].data), length);
        assert_equal(ret, data[i].ret, "i:%d ZBarcode_Encode ret %d != %d (%s)\n", i, ret, data[i].ret, symbol->errtxt);

        if (p_ctx->generate) {
            printf("        /*%3d*/ { %d, \"%s\", %s, %d, %d, \"%s\",\n",
                    i, data[i].option_2, testUtilEscape(data[i].data, length, escaped, sizeof(escaped)),
                    testUtilErrorName(data[i].ret), symbol->rows, symbol->width, data[i].comment);
            testUtilModulesPrint(symbol, "                    ", "\n");
            printf("                },\n");
        } else {
            if (ret < ZINT_ERROR) {
                int width, row;

                assert_equal(symbol->rows, data[i].expected_rows, "i:%d symbol->rows %d != %d (%s)\n", i, symbol->rows, data[i].expected_rows, data[i].data);
                assert_equal(symbol->width, data[i].expected_width, "i:%d symbol->width %d != %d (%s)\n", i, symbol->width, data[i].expected_width, data[i].data);

                ret = testUtilModulesCmp(symbol, data[i].expected, &width, &row);
                assert_zero(ret, "i:%d testUtilModulesCmp ret %d != 0 width %d row %d (%s)\n", i, ret, width, row, data[i].data);

                if (do_bwipp && testUtilCanBwipp(i, symbol, -1, data[i].option_2, -1, debug)) {
                    ret = testUtilBwipp(i, symbol, -1, data[i].option_2, -1, data[i].data, length, NULL, bwipp_buf, sizeof(bwipp_buf), NULL);
                    assert_zero(ret, "i:%d %s testUtilBwipp ret %d != 0\n", i, testUtilBarcodeName(symbol->symbology), ret);

                    ret = testUtilBwippCmp(symbol, bwipp_msg, bwipp_buf, data[i].expected);
                    assert_zero(ret, "i:%d %s testUtilBwippCmp %d != 0 %s\n  actual: %s\nexpected: %s\n",
                                   i, testUtilBarcodeName(symbol->symbology), ret, bwipp_msg, bwipp_buf, data[i].expected);
                }
            }
        }

        ZBarcode_Delete(symbol);
    }

    testFinish();
}

/* Dummy to generate pre-calculated tables for channels 7/8 */
static void test_generate(const testCtx *const p_ctx) {

    struct item {
        const char *data;
    };
    static const struct item data[] = { { "576688" }, { "7742862" } };
    const int data_size = ARRAY_SIZE(data);
    int i, length, ret;
    struct zint_symbol *symbol;

    if (!p_ctx->generate) {
        return;
    }

    for (i = 0; i < data_size; i++) {
        symbol = ZBarcode_Create();
        assert_nonnull(symbol, "Symbol not created\n");

        length = testUtilSetSymbol(symbol, BARCODE_CHANNEL, -1 /*input_mode*/, -1 /*eci*/, -1 /*option_1*/, -1, -1, -1 /*output_options*/, data[i].data, -1, 0);

        ret = ZBarcode_Encode(symbol, TCU(data[i].data), length);
        assert_zero(ret, "i:%d ZBarcode_Encode ret %d != 0 (%s)\n", i, ret, symbol->errtxt);

        ZBarcode_Delete(symbol);
    }
}

int main(int argc, char *argv[]) {

    testFunction funcs[] = { /* name, func */
        { "test_hrt", test_hrt },
        { "test_input", test_input },
        { "test_encode", test_encode },
        { "test_generate", test_generate },
    };

    testRun(argc, argv, funcs, ARRAY_SIZE(funcs));

    testReport();

    return 0;
}

/* vim: set ts=4 sw=4 et : */
