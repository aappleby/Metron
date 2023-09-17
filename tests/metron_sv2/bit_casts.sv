`include "metron/tools/metron_tools.sv"

// Logics can be casted to various sizes via bN() or bx<N>()

module Module (
  // test_bN() ports
  output int test_bN_ret,
  // test_bx_const() ports
  output int test_bx_const_ret,
  // test_bN_offset() ports
  output int test_bN_offset_ret,
  // test_bx_param() ports
  output int test_bx_param_ret,
  // test2() ports
  output int test2_ret
);
/*public:*/

  always_comb begin : test_bN
    logic[63:0] src;
    logic[63:0] dst64;
    logic[62:0] dst63;
    logic[61:0] dst62;
    logic[60:0] dst61;
    logic[59:0] dst60;
    logic[58:0] dst59;
    logic[57:0] dst58;
    logic[56:0] dst57;
    logic[55:0] dst56;
    logic[54:0] dst55;
    logic[53:0] dst54;
    logic[52:0] dst53;
    logic[51:0] dst52;
    logic[50:0] dst51;
    logic[49:0] dst50;
    logic[48:0] dst49;
    logic[47:0] dst48;
    logic[46:0] dst47;
    logic[45:0] dst46;
    logic[44:0] dst45;
    logic[43:0] dst44;
    logic[42:0] dst43;
    logic[41:0] dst42;
    logic[40:0] dst41;
    logic[39:0] dst40;
    logic[38:0] dst39;
    logic[37:0] dst38;
    logic[36:0] dst37;
    logic[35:0] dst36;
    logic[34:0] dst35;
    logic[33:0] dst34;
    logic[32:0] dst33;
    logic[31:0] dst32;
    logic[30:0] dst31;
    logic[29:0] dst30;
    logic[28:0] dst29;
    logic[27:0] dst28;
    logic[26:0] dst27;
    logic[25:0] dst26;
    logic[24:0] dst25;
    logic[23:0] dst24;
    logic[22:0] dst23;
    logic[21:0] dst22;
    logic[20:0] dst21;
    logic[19:0] dst20;
    logic[18:0] dst19;
    logic[17:0] dst18;
    logic[16:0] dst17;
    logic[15:0] dst16;
    logic[14:0] dst15;
    logic[13:0] dst14;
    logic[12:0] dst13;
    logic[11:0] dst12;
    logic[10:0] dst11;
    logic[9:0] dst10;
    logic[8:0] dst9;
    logic[7:0] dst8;
    logic[6:0] dst7;
    logic[5:0] dst6;
    logic[4:0] dst5;
    logic[3:0] dst4;
    logic[2:0] dst3;
    logic[1:0] dst2;
    logic dst1;
    src = 64'h1234567812345678;
    dst64 = src[63:0];
    dst63 = src[62:0];
    dst62 = src[61:0];
    dst61 = src[60:0];
    dst60 = src[59:0];

    dst59 = src[58:0];
    dst58 = src[57:0];
    dst57 = src[56:0];
    dst56 = src[55:0];
    dst55 = src[54:0];
    dst54 = src[53:0];
    dst53 = src[52:0];
    dst52 = src[51:0];
    dst51 = src[50:0];
    dst50 = src[49:0];

    dst49 = src[48:0];
    dst48 = src[47:0];
    dst47 = src[46:0];
    dst46 = src[45:0];
    dst45 = src[44:0];
    dst44 = src[43:0];
    dst43 = src[42:0];
    dst42 = src[41:0];
    dst41 = src[40:0];
    dst40 = src[39:0];

    dst39 = src[38:0];
    dst38 = src[37:0];
    dst37 = src[36:0];
    dst36 = src[35:0];
    dst35 = src[34:0];
    dst34 = src[33:0];
    dst33 = src[32:0];
    dst32 = src[31:0];
    dst31 = src[30:0];
    dst30 = src[29:0];

    dst29 = src[28:0];
    dst28 = src[27:0];
    dst27 = src[26:0];
    dst26 = src[25:0];
    dst25 = src[24:0];
    dst24 = src[23:0];
    dst23 = src[22:0];
    dst22 = src[21:0];
    dst21 = src[20:0];
    dst20 = src[19:0];

    dst19 = src[18:0];
    dst18 = src[17:0];
    dst17 = src[16:0];
    dst16 = src[15:0];
    dst15 = src[14:0];
    dst14 = src[13:0];
    dst13 = src[12:0];
    dst12 = src[11:0];
    dst11 = src[10:0];
    dst10 = src[9:0];

    dst9 = src[8:0];
    dst8 = src[7:0];
    dst7 = src[6:0];
    dst6 = src[5:0];
    dst5 = src[4:0];
    dst4 = src[3:0];
    dst3 = src[2:0];
    dst2 = src[1:0];
    dst1 = src[0];
    test_bN_ret = 0;
  end

  always_comb begin : test_bx_const
    logic[63:0] src;
    logic[62:0] dst63;
    logic[61:0] dst62;
    logic[60:0] dst61;
    logic[59:0] dst60;
    logic[58:0] dst59;
    logic[57:0] dst58;
    logic[56:0] dst57;
    logic[55:0] dst56;
    logic[54:0] dst55;
    logic[53:0] dst54;
    logic[52:0] dst53;
    logic[51:0] dst52;
    logic[50:0] dst51;
    logic[49:0] dst50;
    logic[48:0] dst49;
    logic[47:0] dst48;
    logic[46:0] dst47;
    logic[45:0] dst46;
    logic[44:0] dst45;
    logic[43:0] dst44;
    logic[42:0] dst43;
    logic[41:0] dst42;
    logic[40:0] dst41;
    logic[39:0] dst40;
    logic[38:0] dst39;
    logic[37:0] dst38;
    logic[36:0] dst37;
    logic[35:0] dst36;
    logic[34:0] dst35;
    logic[33:0] dst34;
    logic[32:0] dst33;
    logic[31:0] dst32;
    logic[30:0] dst31;
    logic[29:0] dst30;
    logic[28:0] dst29;
    logic[27:0] dst28;
    logic[26:0] dst27;
    logic[25:0] dst26;
    logic[24:0] dst25;
    logic[23:0] dst24;
    logic[22:0] dst23;
    logic[21:0] dst22;
    logic[20:0] dst21;
    logic[19:0] dst20;
    logic[18:0] dst19;
    logic[17:0] dst18;
    logic[16:0] dst17;
    logic[15:0] dst16;
    logic[14:0] dst15;
    logic[13:0] dst14;
    logic[12:0] dst13;
    logic[11:0] dst12;
    logic[10:0] dst11;
    logic[9:0] dst10;
    logic[8:0] dst9;
    logic[7:0] dst8;
    logic[6:0] dst7;
    logic[5:0] dst6;
    logic[4:0] dst5;
    logic[3:0] dst4;
    logic[2:0] dst3;
    logic[1:0] dst2;
    logic dst1;
    src = 64'h1234567812345678;
    dst63 = src[62:0];
    dst62 = src[61:0];
    dst61 = src[60:0];
    dst60 = src[59:0];

    dst59 = src[58:0];
    dst58 = src[57:0];
    dst57 = src[56:0];
    dst56 = src[55:0];
    dst55 = src[54:0];
    dst54 = src[53:0];
    dst53 = src[52:0];
    dst52 = src[51:0];
    dst51 = src[50:0];
    dst50 = src[49:0];

    dst49 = src[48:0];
    dst48 = src[47:0];
    dst47 = src[46:0];
    dst46 = src[45:0];
    dst45 = src[44:0];
    dst44 = src[43:0];
    dst43 = src[42:0];
    dst42 = src[41:0];
    dst41 = src[40:0];
    dst40 = src[39:0];

    dst39 = src[38:0];
    dst38 = src[37:0];
    dst37 = src[36:0];
    dst36 = src[35:0];
    dst35 = src[34:0];
    dst34 = src[33:0];
    dst33 = src[32:0];
    dst32 = src[31:0];
    dst31 = src[30:0];
    dst30 = src[29:0];

    dst29 = src[28:0];
    dst28 = src[27:0];
    dst27 = src[26:0];
    dst26 = src[25:0];
    dst25 = src[24:0];
    dst24 = src[23:0];
    dst23 = src[22:0];
    dst22 = src[21:0];
    dst21 = src[20:0];
    dst20 = src[19:0];

    dst19 = src[18:0];
    dst18 = src[17:0];
    dst17 = src[16:0];
    dst16 = src[15:0];
    dst15 = src[14:0];
    dst14 = src[13:0];
    dst13 = src[12:0];
    dst12 = src[11:0];
    dst11 = src[10:0];
    dst10 = src[9:0];

    dst9 = src[8:0];
    dst8 = src[7:0];
    dst7 = src[6:0];
    dst6 = src[5:0];
    dst5 = src[4:0];
    dst4 = src[3:0];
    dst3 = src[2:0];
    dst2 = src[1:0];
    dst1 = src[0];
    test_bx_const_ret = 0;
  end

  always_comb begin : test_bN_offset
    logic[63:0] src;
    logic[7:0] dst0;
    logic[7:0] dst1;
    logic[7:0] dst2;
    logic[7:0] dst3;
    logic[7:0] dst4;
    logic[7:0] dst5;
    logic[7:0] dst6;
    logic[7:0] dst7;
    logic[7:0] dst8;
    logic[7:0] dst9;
    src = 64'h1234567812345678;

    dst0 = src[7:0];
    dst1 = src[8:1];
    dst2 = src[9:2];
    dst3 = src[10:3];
    dst4 = src[11:4];
    dst5 = src[12:5];
    dst6 = src[13:6];
    dst7 = src[14:7];
    dst8 = src[15:8];
    dst9 = src[16:9];
    test_bN_offset_ret = 0;
  end

  localparam /*static*/ /*const*/ int some_size1 = 64;
  localparam /*static*/ /*const*/ int some_size2 = 8;

  always_comb begin : test_bx_param
    logic[some_size1-1:0] a;
    logic[some_size2-1:0] b;
    logic[some_size2-1:0] b0;
    logic[some_size2-1:0] b1;
    logic[some_size2-1:0] b2;
    logic[some_size2-1:0] b3;
    logic[some_size2-1:0] b4;
    logic[some_size2-1:0] b5;
    logic[some_size2-1:0] b6;
    logic[some_size2-1:0] b7;
    logic[some_size2-1:0] b8;
    logic[some_size2-1:0] b9;
    a = 10;
    b = a[some_size2-1:0];

    b0 = a[some_size2-1:0];
    b1 = a[some_size2:1];
    b2 = a[some_size2+1:2];
    b3 = a[some_size2+2:3];
    b4 = a[some_size2+3:4];
    b5 = a[some_size2+4:5];
    b6 = a[some_size2+5:6];
    b7 = a[some_size2+6:7];
    b8 = a[some_size2+7:8];
    b9 = a[some_size2+8:9];

    test_bx_param_ret = 0;
  end


  always_comb begin : test2
    logic[31:0] a;
    logic b;
    logic[6:0] c;
    logic e;
    logic[6:0] f;
    a = 32'hDEADBEEF;

    b = a[3]; //static bit extract with literal offset, width 1
    c = a[9:3]; //static bit extract with literal offset, width N

    e = a[some_size1]; //static bit extract with variable offset, width 1
    f = a[6+some_size2:some_size2]; //static bit extract with variable offset, width N

    test2_ret = 0;
  end



endmodule