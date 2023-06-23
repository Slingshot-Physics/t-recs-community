#include "signature_manager.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <vector>

template <int N>
struct component
{
   int thing;
};

TEST_CASE("signature instantiation", "[SignatureManager]")
{
   trecs::DefaultSignatureManager sig_manager;
   REQUIRE( true );
}

TEST_CASE("retrieve unregistered signatures", "[SignatureManager]")
{
   trecs::DefaultSignatureManager sig_manager;

   auto ret_sig = sig_manager.getSignature<complicatedType_t<6> >();

   REQUIRE( ret_sig == trecs::error_signature );
}

TEST_CASE("add signatures", "[SignatureManager]")
{
   trecs::DefaultSignatureManager sig_manager;
   
   auto float_sig = sig_manager.registerComponent<float>();
   auto int_sig = sig_manager.registerComponent<int>();
   auto rigid_body_sig = sig_manager.registerComponent<complicatedType_t<0> >();

   REQUIRE( float_sig != int_sig );
   REQUIRE( int_sig != rigid_body_sig );
   REQUIRE( float_sig != rigid_body_sig );
}

TEST_CASE("add and retrieve registered signatures", "[SignatureManager]")
{
   trecs::DefaultSignatureManager sig_manager;
   
   auto float_sig = sig_manager.registerComponent<float>();
   auto int_sig = sig_manager.registerComponent<int>();
   auto rigid_body_sig = sig_manager.registerComponent<complicatedType_t<0> >();

   auto ret_float_sig = sig_manager.getSignature<float>();
   auto ret_int_sig = sig_manager.getSignature<int>();
   auto ret_rigid_body_sig = sig_manager.getSignature<complicatedType_t<0> >();

   REQUIRE( float_sig == ret_float_sig );
   REQUIRE( int_sig == ret_int_sig );
   REQUIRE( rigid_body_sig == ret_rigid_body_sig );

   REQUIRE( float_sig != int_sig );
   REQUIRE( int_sig != rigid_body_sig );
   REQUIRE( float_sig != rigid_body_sig );
}

// Verifies that the signatures for more than 32 unique registered components
// are all different.
TEST_CASE("register more than 32 components", "[SignatureManager]")
{
   trecs::DefaultSignatureManager sig_manager;

   trecs::signature_t signatures[72];

   signatures[0] = sig_manager.registerComponent<component<0> >();
   signatures[1] = sig_manager.registerComponent<component<1> >();
   signatures[2] = sig_manager.registerComponent<component<2> >();
   signatures[3] = sig_manager.registerComponent<component<3> >();
   signatures[4] = sig_manager.registerComponent<component<4> >();
   signatures[5] = sig_manager.registerComponent<component<5> >();
   signatures[6] = sig_manager.registerComponent<component<6> >();
   signatures[7] = sig_manager.registerComponent<component<7> >();
   signatures[8] = sig_manager.registerComponent<component<8> >();
   signatures[9] = sig_manager.registerComponent<component<9> >();
   signatures[10] = sig_manager.registerComponent<component<10> >();
   signatures[11] = sig_manager.registerComponent<component<11> >();
   signatures[12] = sig_manager.registerComponent<component<12> >();
   signatures[13] = sig_manager.registerComponent<component<13> >();
   signatures[14] = sig_manager.registerComponent<component<14> >();
   signatures[15] = sig_manager.registerComponent<component<15> >();
   signatures[16] = sig_manager.registerComponent<component<16> >();
   signatures[17] = sig_manager.registerComponent<component<17> >();
   signatures[18] = sig_manager.registerComponent<component<18> >();
   signatures[19] = sig_manager.registerComponent<component<19> >();
   signatures[20] = sig_manager.registerComponent<component<20> >();
   signatures[21] = sig_manager.registerComponent<component<21> >();
   signatures[22] = sig_manager.registerComponent<component<22> >();
   signatures[23] = sig_manager.registerComponent<component<23> >();
   signatures[24] = sig_manager.registerComponent<component<24> >();
   signatures[25] = sig_manager.registerComponent<component<25> >();
   signatures[26] = sig_manager.registerComponent<component<26> >();
   signatures[27] = sig_manager.registerComponent<component<27> >();
   signatures[28] = sig_manager.registerComponent<component<28> >();
   signatures[29] = sig_manager.registerComponent<component<29> >();
   signatures[30] = sig_manager.registerComponent<component<30> >();
   signatures[31] = sig_manager.registerComponent<component<31> >();
   signatures[32] = sig_manager.registerComponent<component<32> >();
   signatures[33] = sig_manager.registerComponent<component<33> >();
   signatures[34] = sig_manager.registerComponent<component<34> >();
   signatures[35] = sig_manager.registerComponent<component<35> >();
   signatures[36] = sig_manager.registerComponent<component<36> >();
   signatures[37] = sig_manager.registerComponent<component<37> >();
   signatures[38] = sig_manager.registerComponent<component<38> >();
   signatures[39] = sig_manager.registerComponent<component<39> >();
   signatures[40] = sig_manager.registerComponent<component<40> >();
   signatures[41] = sig_manager.registerComponent<component<41> >();
   signatures[42] = sig_manager.registerComponent<component<42> >();
   signatures[43] = sig_manager.registerComponent<component<43> >();
   signatures[44] = sig_manager.registerComponent<component<44> >();
   signatures[45] = sig_manager.registerComponent<component<45> >();
   signatures[46] = sig_manager.registerComponent<component<46> >();
   signatures[47] = sig_manager.registerComponent<component<47> >();
   signatures[48] = sig_manager.registerComponent<component<48> >();
   signatures[49] = sig_manager.registerComponent<component<49> >();
   signatures[50] = sig_manager.registerComponent<component<50> >();
   signatures[51] = sig_manager.registerComponent<component<51> >();
   signatures[52] = sig_manager.registerComponent<component<52> >();
   signatures[53] = sig_manager.registerComponent<component<53> >();
   signatures[54] = sig_manager.registerComponent<component<54> >();
   signatures[55] = sig_manager.registerComponent<component<55> >();
   signatures[56] = sig_manager.registerComponent<component<56> >();
   signatures[57] = sig_manager.registerComponent<component<57> >();
   signatures[58] = sig_manager.registerComponent<component<58> >();
   signatures[59] = sig_manager.registerComponent<component<59> >();
   signatures[60] = sig_manager.registerComponent<component<60> >();
   signatures[61] = sig_manager.registerComponent<component<61> >();
   signatures[62] = sig_manager.registerComponent<component<62> >();
   signatures[63] = sig_manager.registerComponent<component<63> >();
   signatures[64] = sig_manager.registerComponent<component<64> >();
   signatures[65] = sig_manager.registerComponent<component<65> >();
   signatures[66] = sig_manager.registerComponent<component<66> >();
   signatures[67] = sig_manager.registerComponent<component<67> >();
   signatures[68] = sig_manager.registerComponent<component<68> >();
   signatures[69] = sig_manager.registerComponent<component<69> >();
   signatures[70] = sig_manager.registerComponent<component<70> >();
   signatures[71] = sig_manager.registerComponent<component<71> >();

   for (int i = 0; i < 71; ++i)
   {
      if (i < trecs::max_num_signatures)
      {
         REQUIRE( signatures[i] != trecs::error_signature );
      }
      else
      {
         REQUIRE( signatures[i] == trecs::error_signature );
      }
      for (int j = i + 1; j < 71; ++j)
      {
         if (i < trecs::max_num_signatures && j < trecs::max_num_signatures)
         {
            REQUIRE( signatures[i] != signatures[j] );
         }
      }
   }

}

// Verifies that adding any more than the maximum number of signatures causes
// the SignatureManager to return the error signature.
TEST_CASE("add 512 signatures", "[SignatureManager]")
{
   trecs::DefaultSignatureManager sig_manager;

   trecs::signature_t signatures[512];

   signatures[0] = sig_manager.registerComponent<component<0> >();
   signatures[1] = sig_manager.registerComponent<component<1> >();
   signatures[2] = sig_manager.registerComponent<component<2> >();
   signatures[3] = sig_manager.registerComponent<component<3> >();
   signatures[4] = sig_manager.registerComponent<component<4> >();
   signatures[5] = sig_manager.registerComponent<component<5> >();
   signatures[6] = sig_manager.registerComponent<component<6> >();
   signatures[7] = sig_manager.registerComponent<component<7> >();
   signatures[8] = sig_manager.registerComponent<component<8> >();
   signatures[9] = sig_manager.registerComponent<component<9> >();
   signatures[10] = sig_manager.registerComponent<component<10> >();
   signatures[11] = sig_manager.registerComponent<component<11> >();
   signatures[12] = sig_manager.registerComponent<component<12> >();
   signatures[13] = sig_manager.registerComponent<component<13> >();
   signatures[14] = sig_manager.registerComponent<component<14> >();
   signatures[15] = sig_manager.registerComponent<component<15> >();
   signatures[16] = sig_manager.registerComponent<component<16> >();
   signatures[17] = sig_manager.registerComponent<component<17> >();
   signatures[18] = sig_manager.registerComponent<component<18> >();
   signatures[19] = sig_manager.registerComponent<component<19> >();
   signatures[20] = sig_manager.registerComponent<component<20> >();
   signatures[21] = sig_manager.registerComponent<component<21> >();
   signatures[22] = sig_manager.registerComponent<component<22> >();
   signatures[23] = sig_manager.registerComponent<component<23> >();
   signatures[24] = sig_manager.registerComponent<component<24> >();
   signatures[25] = sig_manager.registerComponent<component<25> >();
   signatures[26] = sig_manager.registerComponent<component<26> >();
   signatures[27] = sig_manager.registerComponent<component<27> >();
   signatures[28] = sig_manager.registerComponent<component<28> >();
   signatures[29] = sig_manager.registerComponent<component<29> >();
   signatures[30] = sig_manager.registerComponent<component<30> >();
   signatures[31] = sig_manager.registerComponent<component<31> >();
   signatures[32] = sig_manager.registerComponent<component<32> >();
   signatures[33] = sig_manager.registerComponent<component<33> >();
   signatures[34] = sig_manager.registerComponent<component<34> >();
   signatures[35] = sig_manager.registerComponent<component<35> >();
   signatures[36] = sig_manager.registerComponent<component<36> >();
   signatures[37] = sig_manager.registerComponent<component<37> >();
   signatures[38] = sig_manager.registerComponent<component<38> >();
   signatures[39] = sig_manager.registerComponent<component<39> >();
   signatures[40] = sig_manager.registerComponent<component<40> >();
   signatures[41] = sig_manager.registerComponent<component<41> >();
   signatures[42] = sig_manager.registerComponent<component<42> >();
   signatures[43] = sig_manager.registerComponent<component<43> >();
   signatures[44] = sig_manager.registerComponent<component<44> >();
   signatures[45] = sig_manager.registerComponent<component<45> >();
   signatures[46] = sig_manager.registerComponent<component<46> >();
   signatures[47] = sig_manager.registerComponent<component<47> >();
   signatures[48] = sig_manager.registerComponent<component<48> >();
   signatures[49] = sig_manager.registerComponent<component<49> >();
   signatures[50] = sig_manager.registerComponent<component<50> >();
   signatures[51] = sig_manager.registerComponent<component<51> >();
   signatures[52] = sig_manager.registerComponent<component<52> >();
   signatures[53] = sig_manager.registerComponent<component<53> >();
   signatures[54] = sig_manager.registerComponent<component<54> >();
   signatures[55] = sig_manager.registerComponent<component<55> >();
   signatures[56] = sig_manager.registerComponent<component<56> >();
   signatures[57] = sig_manager.registerComponent<component<57> >();
   signatures[58] = sig_manager.registerComponent<component<58> >();
   signatures[59] = sig_manager.registerComponent<component<59> >();
   signatures[60] = sig_manager.registerComponent<component<60> >();
   signatures[61] = sig_manager.registerComponent<component<61> >();
   signatures[62] = sig_manager.registerComponent<component<62> >();
   signatures[63] = sig_manager.registerComponent<component<63> >();
   signatures[64] = sig_manager.registerComponent<component<64> >();
   signatures[65] = sig_manager.registerComponent<component<65> >();
   signatures[66] = sig_manager.registerComponent<component<66> >();
   signatures[67] = sig_manager.registerComponent<component<67> >();
   signatures[68] = sig_manager.registerComponent<component<68> >();
   signatures[69] = sig_manager.registerComponent<component<69> >();
   signatures[70] = sig_manager.registerComponent<component<70> >();
   signatures[71] = sig_manager.registerComponent<component<71> >();
   signatures[72] = sig_manager.registerComponent<component<72> >();
   signatures[73] = sig_manager.registerComponent<component<73> >();
   signatures[74] = sig_manager.registerComponent<component<74> >();
   signatures[75] = sig_manager.registerComponent<component<75> >();
   signatures[76] = sig_manager.registerComponent<component<76> >();
   signatures[77] = sig_manager.registerComponent<component<77> >();
   signatures[78] = sig_manager.registerComponent<component<78> >();
   signatures[79] = sig_manager.registerComponent<component<79> >();
   signatures[80] = sig_manager.registerComponent<component<80> >();
   signatures[81] = sig_manager.registerComponent<component<81> >();
   signatures[82] = sig_manager.registerComponent<component<82> >();
   signatures[83] = sig_manager.registerComponent<component<83> >();
   signatures[84] = sig_manager.registerComponent<component<84> >();
   signatures[85] = sig_manager.registerComponent<component<85> >();
   signatures[86] = sig_manager.registerComponent<component<86> >();
   signatures[87] = sig_manager.registerComponent<component<87> >();
   signatures[88] = sig_manager.registerComponent<component<88> >();
   signatures[89] = sig_manager.registerComponent<component<89> >();
   signatures[90] = sig_manager.registerComponent<component<90> >();
   signatures[91] = sig_manager.registerComponent<component<91> >();
   signatures[92] = sig_manager.registerComponent<component<92> >();
   signatures[93] = sig_manager.registerComponent<component<93> >();
   signatures[94] = sig_manager.registerComponent<component<94> >();
   signatures[95] = sig_manager.registerComponent<component<95> >();
   signatures[96] = sig_manager.registerComponent<component<96> >();
   signatures[97] = sig_manager.registerComponent<component<97> >();
   signatures[98] = sig_manager.registerComponent<component<98> >();
   signatures[99] = sig_manager.registerComponent<component<99> >();
   signatures[100] = sig_manager.registerComponent<component<100> >();
   signatures[101] = sig_manager.registerComponent<component<101> >();
   signatures[102] = sig_manager.registerComponent<component<102> >();
   signatures[103] = sig_manager.registerComponent<component<103> >();
   signatures[104] = sig_manager.registerComponent<component<104> >();
   signatures[105] = sig_manager.registerComponent<component<105> >();
   signatures[106] = sig_manager.registerComponent<component<106> >();
   signatures[107] = sig_manager.registerComponent<component<107> >();
   signatures[108] = sig_manager.registerComponent<component<108> >();
   signatures[109] = sig_manager.registerComponent<component<109> >();
   signatures[110] = sig_manager.registerComponent<component<110> >();
   signatures[111] = sig_manager.registerComponent<component<111> >();
   signatures[112] = sig_manager.registerComponent<component<112> >();
   signatures[113] = sig_manager.registerComponent<component<113> >();
   signatures[114] = sig_manager.registerComponent<component<114> >();
   signatures[115] = sig_manager.registerComponent<component<115> >();
   signatures[116] = sig_manager.registerComponent<component<116> >();
   signatures[117] = sig_manager.registerComponent<component<117> >();
   signatures[118] = sig_manager.registerComponent<component<118> >();
   signatures[119] = sig_manager.registerComponent<component<119> >();
   signatures[120] = sig_manager.registerComponent<component<120> >();
   signatures[121] = sig_manager.registerComponent<component<121> >();
   signatures[122] = sig_manager.registerComponent<component<122> >();
   signatures[123] = sig_manager.registerComponent<component<123> >();
   signatures[124] = sig_manager.registerComponent<component<124> >();
   signatures[125] = sig_manager.registerComponent<component<125> >();
   signatures[126] = sig_manager.registerComponent<component<126> >();
   signatures[127] = sig_manager.registerComponent<component<127> >();
   signatures[128] = sig_manager.registerComponent<component<128> >();
   signatures[129] = sig_manager.registerComponent<component<129> >();
   signatures[130] = sig_manager.registerComponent<component<130> >();
   signatures[131] = sig_manager.registerComponent<component<131> >();
   signatures[132] = sig_manager.registerComponent<component<132> >();
   signatures[133] = sig_manager.registerComponent<component<133> >();
   signatures[134] = sig_manager.registerComponent<component<134> >();
   signatures[135] = sig_manager.registerComponent<component<135> >();
   signatures[136] = sig_manager.registerComponent<component<136> >();
   signatures[137] = sig_manager.registerComponent<component<137> >();
   signatures[138] = sig_manager.registerComponent<component<138> >();
   signatures[139] = sig_manager.registerComponent<component<139> >();
   signatures[140] = sig_manager.registerComponent<component<140> >();
   signatures[141] = sig_manager.registerComponent<component<141> >();
   signatures[142] = sig_manager.registerComponent<component<142> >();
   signatures[143] = sig_manager.registerComponent<component<143> >();
   signatures[144] = sig_manager.registerComponent<component<144> >();
   signatures[145] = sig_manager.registerComponent<component<145> >();
   signatures[146] = sig_manager.registerComponent<component<146> >();
   signatures[147] = sig_manager.registerComponent<component<147> >();
   signatures[148] = sig_manager.registerComponent<component<148> >();
   signatures[149] = sig_manager.registerComponent<component<149> >();
   signatures[150] = sig_manager.registerComponent<component<150> >();
   signatures[151] = sig_manager.registerComponent<component<151> >();
   signatures[152] = sig_manager.registerComponent<component<152> >();
   signatures[153] = sig_manager.registerComponent<component<153> >();
   signatures[154] = sig_manager.registerComponent<component<154> >();
   signatures[155] = sig_manager.registerComponent<component<155> >();
   signatures[156] = sig_manager.registerComponent<component<156> >();
   signatures[157] = sig_manager.registerComponent<component<157> >();
   signatures[158] = sig_manager.registerComponent<component<158> >();
   signatures[159] = sig_manager.registerComponent<component<159> >();
   signatures[160] = sig_manager.registerComponent<component<160> >();
   signatures[161] = sig_manager.registerComponent<component<161> >();
   signatures[162] = sig_manager.registerComponent<component<162> >();
   signatures[163] = sig_manager.registerComponent<component<163> >();
   signatures[164] = sig_manager.registerComponent<component<164> >();
   signatures[165] = sig_manager.registerComponent<component<165> >();
   signatures[166] = sig_manager.registerComponent<component<166> >();
   signatures[167] = sig_manager.registerComponent<component<167> >();
   signatures[168] = sig_manager.registerComponent<component<168> >();
   signatures[169] = sig_manager.registerComponent<component<169> >();
   signatures[170] = sig_manager.registerComponent<component<170> >();
   signatures[171] = sig_manager.registerComponent<component<171> >();
   signatures[172] = sig_manager.registerComponent<component<172> >();
   signatures[173] = sig_manager.registerComponent<component<173> >();
   signatures[174] = sig_manager.registerComponent<component<174> >();
   signatures[175] = sig_manager.registerComponent<component<175> >();
   signatures[176] = sig_manager.registerComponent<component<176> >();
   signatures[177] = sig_manager.registerComponent<component<177> >();
   signatures[178] = sig_manager.registerComponent<component<178> >();
   signatures[179] = sig_manager.registerComponent<component<179> >();
   signatures[180] = sig_manager.registerComponent<component<180> >();
   signatures[181] = sig_manager.registerComponent<component<181> >();
   signatures[182] = sig_manager.registerComponent<component<182> >();
   signatures[183] = sig_manager.registerComponent<component<183> >();
   signatures[184] = sig_manager.registerComponent<component<184> >();
   signatures[185] = sig_manager.registerComponent<component<185> >();
   signatures[186] = sig_manager.registerComponent<component<186> >();
   signatures[187] = sig_manager.registerComponent<component<187> >();
   signatures[188] = sig_manager.registerComponent<component<188> >();
   signatures[189] = sig_manager.registerComponent<component<189> >();
   signatures[190] = sig_manager.registerComponent<component<190> >();
   signatures[191] = sig_manager.registerComponent<component<191> >();
   signatures[192] = sig_manager.registerComponent<component<192> >();
   signatures[193] = sig_manager.registerComponent<component<193> >();
   signatures[194] = sig_manager.registerComponent<component<194> >();
   signatures[195] = sig_manager.registerComponent<component<195> >();
   signatures[196] = sig_manager.registerComponent<component<196> >();
   signatures[197] = sig_manager.registerComponent<component<197> >();
   signatures[198] = sig_manager.registerComponent<component<198> >();
   signatures[199] = sig_manager.registerComponent<component<199> >();
   signatures[200] = sig_manager.registerComponent<component<200> >();
   signatures[201] = sig_manager.registerComponent<component<201> >();
   signatures[202] = sig_manager.registerComponent<component<202> >();
   signatures[203] = sig_manager.registerComponent<component<203> >();
   signatures[204] = sig_manager.registerComponent<component<204> >();
   signatures[205] = sig_manager.registerComponent<component<205> >();
   signatures[206] = sig_manager.registerComponent<component<206> >();
   signatures[207] = sig_manager.registerComponent<component<207> >();
   signatures[208] = sig_manager.registerComponent<component<208> >();
   signatures[209] = sig_manager.registerComponent<component<209> >();
   signatures[210] = sig_manager.registerComponent<component<210> >();
   signatures[211] = sig_manager.registerComponent<component<211> >();
   signatures[212] = sig_manager.registerComponent<component<212> >();
   signatures[213] = sig_manager.registerComponent<component<213> >();
   signatures[214] = sig_manager.registerComponent<component<214> >();
   signatures[215] = sig_manager.registerComponent<component<215> >();
   signatures[216] = sig_manager.registerComponent<component<216> >();
   signatures[217] = sig_manager.registerComponent<component<217> >();
   signatures[218] = sig_manager.registerComponent<component<218> >();
   signatures[219] = sig_manager.registerComponent<component<219> >();
   signatures[220] = sig_manager.registerComponent<component<220> >();
   signatures[221] = sig_manager.registerComponent<component<221> >();
   signatures[222] = sig_manager.registerComponent<component<222> >();
   signatures[223] = sig_manager.registerComponent<component<223> >();
   signatures[224] = sig_manager.registerComponent<component<224> >();
   signatures[225] = sig_manager.registerComponent<component<225> >();
   signatures[226] = sig_manager.registerComponent<component<226> >();
   signatures[227] = sig_manager.registerComponent<component<227> >();
   signatures[228] = sig_manager.registerComponent<component<228> >();
   signatures[229] = sig_manager.registerComponent<component<229> >();
   signatures[230] = sig_manager.registerComponent<component<230> >();
   signatures[231] = sig_manager.registerComponent<component<231> >();
   signatures[232] = sig_manager.registerComponent<component<232> >();
   signatures[233] = sig_manager.registerComponent<component<233> >();
   signatures[234] = sig_manager.registerComponent<component<234> >();
   signatures[235] = sig_manager.registerComponent<component<235> >();
   signatures[236] = sig_manager.registerComponent<component<236> >();
   signatures[237] = sig_manager.registerComponent<component<237> >();
   signatures[238] = sig_manager.registerComponent<component<238> >();
   signatures[239] = sig_manager.registerComponent<component<239> >();
   signatures[240] = sig_manager.registerComponent<component<240> >();
   signatures[241] = sig_manager.registerComponent<component<241> >();
   signatures[242] = sig_manager.registerComponent<component<242> >();
   signatures[243] = sig_manager.registerComponent<component<243> >();
   signatures[244] = sig_manager.registerComponent<component<244> >();
   signatures[245] = sig_manager.registerComponent<component<245> >();
   signatures[246] = sig_manager.registerComponent<component<246> >();
   signatures[247] = sig_manager.registerComponent<component<247> >();
   signatures[248] = sig_manager.registerComponent<component<248> >();
   signatures[249] = sig_manager.registerComponent<component<249> >();
   signatures[250] = sig_manager.registerComponent<component<250> >();
   signatures[251] = sig_manager.registerComponent<component<251> >();
   signatures[252] = sig_manager.registerComponent<component<252> >();
   signatures[253] = sig_manager.registerComponent<component<253> >();
   signatures[254] = sig_manager.registerComponent<component<254> >();
   signatures[255] = sig_manager.registerComponent<component<255> >();
   signatures[256] = sig_manager.registerComponent<component<256> >();
   signatures[257] = sig_manager.registerComponent<component<257> >();
   signatures[258] = sig_manager.registerComponent<component<258> >();
   signatures[259] = sig_manager.registerComponent<component<259> >();
   signatures[260] = sig_manager.registerComponent<component<260> >();
   signatures[261] = sig_manager.registerComponent<component<261> >();
   signatures[262] = sig_manager.registerComponent<component<262> >();
   signatures[263] = sig_manager.registerComponent<component<263> >();
   signatures[264] = sig_manager.registerComponent<component<264> >();
   signatures[265] = sig_manager.registerComponent<component<265> >();
   signatures[266] = sig_manager.registerComponent<component<266> >();
   signatures[267] = sig_manager.registerComponent<component<267> >();
   signatures[268] = sig_manager.registerComponent<component<268> >();
   signatures[269] = sig_manager.registerComponent<component<269> >();
   signatures[270] = sig_manager.registerComponent<component<270> >();
   signatures[271] = sig_manager.registerComponent<component<271> >();
   signatures[272] = sig_manager.registerComponent<component<272> >();
   signatures[273] = sig_manager.registerComponent<component<273> >();
   signatures[274] = sig_manager.registerComponent<component<274> >();
   signatures[275] = sig_manager.registerComponent<component<275> >();
   signatures[276] = sig_manager.registerComponent<component<276> >();
   signatures[277] = sig_manager.registerComponent<component<277> >();
   signatures[278] = sig_manager.registerComponent<component<278> >();
   signatures[279] = sig_manager.registerComponent<component<279> >();
   signatures[280] = sig_manager.registerComponent<component<280> >();
   signatures[281] = sig_manager.registerComponent<component<281> >();
   signatures[282] = sig_manager.registerComponent<component<282> >();
   signatures[283] = sig_manager.registerComponent<component<283> >();
   signatures[284] = sig_manager.registerComponent<component<284> >();
   signatures[285] = sig_manager.registerComponent<component<285> >();
   signatures[286] = sig_manager.registerComponent<component<286> >();
   signatures[287] = sig_manager.registerComponent<component<287> >();
   signatures[288] = sig_manager.registerComponent<component<288> >();
   signatures[289] = sig_manager.registerComponent<component<289> >();
   signatures[290] = sig_manager.registerComponent<component<290> >();
   signatures[291] = sig_manager.registerComponent<component<291> >();
   signatures[292] = sig_manager.registerComponent<component<292> >();
   signatures[293] = sig_manager.registerComponent<component<293> >();
   signatures[294] = sig_manager.registerComponent<component<294> >();
   signatures[295] = sig_manager.registerComponent<component<295> >();
   signatures[296] = sig_manager.registerComponent<component<296> >();
   signatures[297] = sig_manager.registerComponent<component<297> >();
   signatures[298] = sig_manager.registerComponent<component<298> >();
   signatures[299] = sig_manager.registerComponent<component<299> >();
   signatures[300] = sig_manager.registerComponent<component<300> >();
   signatures[301] = sig_manager.registerComponent<component<301> >();
   signatures[302] = sig_manager.registerComponent<component<302> >();
   signatures[303] = sig_manager.registerComponent<component<303> >();
   signatures[304] = sig_manager.registerComponent<component<304> >();
   signatures[305] = sig_manager.registerComponent<component<305> >();
   signatures[306] = sig_manager.registerComponent<component<306> >();
   signatures[307] = sig_manager.registerComponent<component<307> >();
   signatures[308] = sig_manager.registerComponent<component<308> >();
   signatures[309] = sig_manager.registerComponent<component<309> >();
   signatures[310] = sig_manager.registerComponent<component<310> >();
   signatures[311] = sig_manager.registerComponent<component<311> >();
   signatures[312] = sig_manager.registerComponent<component<312> >();
   signatures[313] = sig_manager.registerComponent<component<313> >();
   signatures[314] = sig_manager.registerComponent<component<314> >();
   signatures[315] = sig_manager.registerComponent<component<315> >();
   signatures[316] = sig_manager.registerComponent<component<316> >();
   signatures[317] = sig_manager.registerComponent<component<317> >();
   signatures[318] = sig_manager.registerComponent<component<318> >();
   signatures[319] = sig_manager.registerComponent<component<319> >();
   signatures[320] = sig_manager.registerComponent<component<320> >();
   signatures[321] = sig_manager.registerComponent<component<321> >();
   signatures[322] = sig_manager.registerComponent<component<322> >();
   signatures[323] = sig_manager.registerComponent<component<323> >();
   signatures[324] = sig_manager.registerComponent<component<324> >();
   signatures[325] = sig_manager.registerComponent<component<325> >();
   signatures[326] = sig_manager.registerComponent<component<326> >();
   signatures[327] = sig_manager.registerComponent<component<327> >();
   signatures[328] = sig_manager.registerComponent<component<328> >();
   signatures[329] = sig_manager.registerComponent<component<329> >();
   signatures[330] = sig_manager.registerComponent<component<330> >();
   signatures[331] = sig_manager.registerComponent<component<331> >();
   signatures[332] = sig_manager.registerComponent<component<332> >();
   signatures[333] = sig_manager.registerComponent<component<333> >();
   signatures[334] = sig_manager.registerComponent<component<334> >();
   signatures[335] = sig_manager.registerComponent<component<335> >();
   signatures[336] = sig_manager.registerComponent<component<336> >();
   signatures[337] = sig_manager.registerComponent<component<337> >();
   signatures[338] = sig_manager.registerComponent<component<338> >();
   signatures[339] = sig_manager.registerComponent<component<339> >();
   signatures[340] = sig_manager.registerComponent<component<340> >();
   signatures[341] = sig_manager.registerComponent<component<341> >();
   signatures[342] = sig_manager.registerComponent<component<342> >();
   signatures[343] = sig_manager.registerComponent<component<343> >();
   signatures[344] = sig_manager.registerComponent<component<344> >();
   signatures[345] = sig_manager.registerComponent<component<345> >();
   signatures[346] = sig_manager.registerComponent<component<346> >();
   signatures[347] = sig_manager.registerComponent<component<347> >();
   signatures[348] = sig_manager.registerComponent<component<348> >();
   signatures[349] = sig_manager.registerComponent<component<349> >();
   signatures[350] = sig_manager.registerComponent<component<350> >();
   signatures[351] = sig_manager.registerComponent<component<351> >();
   signatures[352] = sig_manager.registerComponent<component<352> >();
   signatures[353] = sig_manager.registerComponent<component<353> >();
   signatures[354] = sig_manager.registerComponent<component<354> >();
   signatures[355] = sig_manager.registerComponent<component<355> >();
   signatures[356] = sig_manager.registerComponent<component<356> >();
   signatures[357] = sig_manager.registerComponent<component<357> >();
   signatures[358] = sig_manager.registerComponent<component<358> >();
   signatures[359] = sig_manager.registerComponent<component<359> >();
   signatures[360] = sig_manager.registerComponent<component<360> >();
   signatures[361] = sig_manager.registerComponent<component<361> >();
   signatures[362] = sig_manager.registerComponent<component<362> >();
   signatures[363] = sig_manager.registerComponent<component<363> >();
   signatures[364] = sig_manager.registerComponent<component<364> >();
   signatures[365] = sig_manager.registerComponent<component<365> >();
   signatures[366] = sig_manager.registerComponent<component<366> >();
   signatures[367] = sig_manager.registerComponent<component<367> >();
   signatures[368] = sig_manager.registerComponent<component<368> >();
   signatures[369] = sig_manager.registerComponent<component<369> >();
   signatures[370] = sig_manager.registerComponent<component<370> >();
   signatures[371] = sig_manager.registerComponent<component<371> >();
   signatures[372] = sig_manager.registerComponent<component<372> >();
   signatures[373] = sig_manager.registerComponent<component<373> >();
   signatures[374] = sig_manager.registerComponent<component<374> >();
   signatures[375] = sig_manager.registerComponent<component<375> >();
   signatures[376] = sig_manager.registerComponent<component<376> >();
   signatures[377] = sig_manager.registerComponent<component<377> >();
   signatures[378] = sig_manager.registerComponent<component<378> >();
   signatures[379] = sig_manager.registerComponent<component<379> >();
   signatures[380] = sig_manager.registerComponent<component<380> >();
   signatures[381] = sig_manager.registerComponent<component<381> >();
   signatures[382] = sig_manager.registerComponent<component<382> >();
   signatures[383] = sig_manager.registerComponent<component<383> >();
   signatures[384] = sig_manager.registerComponent<component<384> >();
   signatures[385] = sig_manager.registerComponent<component<385> >();
   signatures[386] = sig_manager.registerComponent<component<386> >();
   signatures[387] = sig_manager.registerComponent<component<387> >();
   signatures[388] = sig_manager.registerComponent<component<388> >();
   signatures[389] = sig_manager.registerComponent<component<389> >();
   signatures[390] = sig_manager.registerComponent<component<390> >();
   signatures[391] = sig_manager.registerComponent<component<391> >();
   signatures[392] = sig_manager.registerComponent<component<392> >();
   signatures[393] = sig_manager.registerComponent<component<393> >();
   signatures[394] = sig_manager.registerComponent<component<394> >();
   signatures[395] = sig_manager.registerComponent<component<395> >();
   signatures[396] = sig_manager.registerComponent<component<396> >();
   signatures[397] = sig_manager.registerComponent<component<397> >();
   signatures[398] = sig_manager.registerComponent<component<398> >();
   signatures[399] = sig_manager.registerComponent<component<399> >();
   signatures[400] = sig_manager.registerComponent<component<400> >();
   signatures[401] = sig_manager.registerComponent<component<401> >();
   signatures[402] = sig_manager.registerComponent<component<402> >();
   signatures[403] = sig_manager.registerComponent<component<403> >();
   signatures[404] = sig_manager.registerComponent<component<404> >();
   signatures[405] = sig_manager.registerComponent<component<405> >();
   signatures[406] = sig_manager.registerComponent<component<406> >();
   signatures[407] = sig_manager.registerComponent<component<407> >();
   signatures[408] = sig_manager.registerComponent<component<408> >();
   signatures[409] = sig_manager.registerComponent<component<409> >();
   signatures[410] = sig_manager.registerComponent<component<410> >();
   signatures[411] = sig_manager.registerComponent<component<411> >();
   signatures[412] = sig_manager.registerComponent<component<412> >();
   signatures[413] = sig_manager.registerComponent<component<413> >();
   signatures[414] = sig_manager.registerComponent<component<414> >();
   signatures[415] = sig_manager.registerComponent<component<415> >();
   signatures[416] = sig_manager.registerComponent<component<416> >();
   signatures[417] = sig_manager.registerComponent<component<417> >();
   signatures[418] = sig_manager.registerComponent<component<418> >();
   signatures[419] = sig_manager.registerComponent<component<419> >();
   signatures[420] = sig_manager.registerComponent<component<420> >();
   signatures[421] = sig_manager.registerComponent<component<421> >();
   signatures[422] = sig_manager.registerComponent<component<422> >();
   signatures[423] = sig_manager.registerComponent<component<423> >();
   signatures[424] = sig_manager.registerComponent<component<424> >();
   signatures[425] = sig_manager.registerComponent<component<425> >();
   signatures[426] = sig_manager.registerComponent<component<426> >();
   signatures[427] = sig_manager.registerComponent<component<427> >();
   signatures[428] = sig_manager.registerComponent<component<428> >();
   signatures[429] = sig_manager.registerComponent<component<429> >();
   signatures[430] = sig_manager.registerComponent<component<430> >();
   signatures[431] = sig_manager.registerComponent<component<431> >();
   signatures[432] = sig_manager.registerComponent<component<432> >();
   signatures[433] = sig_manager.registerComponent<component<433> >();
   signatures[434] = sig_manager.registerComponent<component<434> >();
   signatures[435] = sig_manager.registerComponent<component<435> >();
   signatures[436] = sig_manager.registerComponent<component<436> >();
   signatures[437] = sig_manager.registerComponent<component<437> >();
   signatures[438] = sig_manager.registerComponent<component<438> >();
   signatures[439] = sig_manager.registerComponent<component<439> >();
   signatures[440] = sig_manager.registerComponent<component<440> >();
   signatures[441] = sig_manager.registerComponent<component<441> >();
   signatures[442] = sig_manager.registerComponent<component<442> >();
   signatures[443] = sig_manager.registerComponent<component<443> >();
   signatures[444] = sig_manager.registerComponent<component<444> >();
   signatures[445] = sig_manager.registerComponent<component<445> >();
   signatures[446] = sig_manager.registerComponent<component<446> >();
   signatures[447] = sig_manager.registerComponent<component<447> >();
   signatures[448] = sig_manager.registerComponent<component<448> >();
   signatures[449] = sig_manager.registerComponent<component<449> >();
   signatures[450] = sig_manager.registerComponent<component<450> >();
   signatures[451] = sig_manager.registerComponent<component<451> >();
   signatures[452] = sig_manager.registerComponent<component<452> >();
   signatures[453] = sig_manager.registerComponent<component<453> >();
   signatures[454] = sig_manager.registerComponent<component<454> >();
   signatures[455] = sig_manager.registerComponent<component<455> >();
   signatures[456] = sig_manager.registerComponent<component<456> >();
   signatures[457] = sig_manager.registerComponent<component<457> >();
   signatures[458] = sig_manager.registerComponent<component<458> >();
   signatures[459] = sig_manager.registerComponent<component<459> >();
   signatures[460] = sig_manager.registerComponent<component<460> >();
   signatures[461] = sig_manager.registerComponent<component<461> >();
   signatures[462] = sig_manager.registerComponent<component<462> >();
   signatures[463] = sig_manager.registerComponent<component<463> >();
   signatures[464] = sig_manager.registerComponent<component<464> >();
   signatures[465] = sig_manager.registerComponent<component<465> >();
   signatures[466] = sig_manager.registerComponent<component<466> >();
   signatures[467] = sig_manager.registerComponent<component<467> >();
   signatures[468] = sig_manager.registerComponent<component<468> >();
   signatures[469] = sig_manager.registerComponent<component<469> >();
   signatures[470] = sig_manager.registerComponent<component<470> >();
   signatures[471] = sig_manager.registerComponent<component<471> >();
   signatures[472] = sig_manager.registerComponent<component<472> >();
   signatures[473] = sig_manager.registerComponent<component<473> >();
   signatures[474] = sig_manager.registerComponent<component<474> >();
   signatures[475] = sig_manager.registerComponent<component<475> >();
   signatures[476] = sig_manager.registerComponent<component<476> >();
   signatures[477] = sig_manager.registerComponent<component<477> >();
   signatures[478] = sig_manager.registerComponent<component<478> >();
   signatures[479] = sig_manager.registerComponent<component<479> >();
   signatures[480] = sig_manager.registerComponent<component<480> >();
   signatures[481] = sig_manager.registerComponent<component<481> >();
   signatures[482] = sig_manager.registerComponent<component<482> >();
   signatures[483] = sig_manager.registerComponent<component<483> >();
   signatures[484] = sig_manager.registerComponent<component<484> >();
   signatures[485] = sig_manager.registerComponent<component<485> >();
   signatures[486] = sig_manager.registerComponent<component<486> >();
   signatures[487] = sig_manager.registerComponent<component<487> >();
   signatures[488] = sig_manager.registerComponent<component<488> >();
   signatures[489] = sig_manager.registerComponent<component<489> >();
   signatures[490] = sig_manager.registerComponent<component<490> >();
   signatures[491] = sig_manager.registerComponent<component<491> >();
   signatures[492] = sig_manager.registerComponent<component<492> >();
   signatures[493] = sig_manager.registerComponent<component<493> >();
   signatures[494] = sig_manager.registerComponent<component<494> >();
   signatures[495] = sig_manager.registerComponent<component<495> >();
   signatures[496] = sig_manager.registerComponent<component<496> >();
   signatures[497] = sig_manager.registerComponent<component<497> >();
   signatures[498] = sig_manager.registerComponent<component<498> >();
   signatures[499] = sig_manager.registerComponent<component<499> >();
   signatures[500] = sig_manager.registerComponent<component<500> >();
   signatures[501] = sig_manager.registerComponent<component<501> >();
   signatures[502] = sig_manager.registerComponent<component<502> >();
   signatures[503] = sig_manager.registerComponent<component<503> >();
   signatures[504] = sig_manager.registerComponent<component<504> >();
   signatures[505] = sig_manager.registerComponent<component<505> >();
   signatures[506] = sig_manager.registerComponent<component<506> >();
   signatures[507] = sig_manager.registerComponent<component<507> >();
   signatures[508] = sig_manager.registerComponent<component<508> >();
   signatures[509] = sig_manager.registerComponent<component<509> >();
   signatures[510] = sig_manager.registerComponent<component<510> >();
   signatures[511] = sig_manager.registerComponent<component<511> >();

   for (int i = 0; i < 512; ++i)
   {
      if (i <= trecs::max_num_signatures)
      {
         REQUIRE(signatures[i] == i);
      }
      else
      {
         REQUIRE(signatures[i] == trecs::error_signature);
      }
   }
}
