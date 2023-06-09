#include <ledParamTables.h>
/* Efficiency is 1024 @25C , the value is 990*1024/1000 when efficiency is 99.0% */ 
static const TempParams_t TEMP_CURVE_TABLE_RED[TEMP_CMP_NUM_MAX] = 
{
    /*T(1C) efficiency deltaV(mV) x(%), y(%) */
/*0*/{1180U,36,1020U,1032U},
/*2*/{1167U,32,1020U,1031U},
/*4*/{1154U,29,1021U,1030U},
/*6*/{1141U,25,1021U,1030U},
/*8*/{1128U,23,1021U,1029U},
/*10*/{1115U,18,1021U,1028U},
/*12*/{1102U,15,1022U,1028U},
/*14*/{1089U,13,1022U,1027U},
/*16*/{1076U,10,1022U,1026U},
/*18*/{1063U,8,1023U,1026U},
/*20*/{1050U,5,1023U,1025U},
/*22*/{1037U,2,1023U,1024U},
/*24*/{1024U,0,1024U,1024U},
/*26*/{1010U,-3,1024U,1023U},
/*28*/{997U,-5,1024U,1022U},
/*30*/{984U,-7,1024U,1021U},
/*32*/{971U,-10,1025U,1021U},
/*34*/{958U,-12,1025U,1020U},
/*36*/{945U,-15,1025U,1019U},
/*38*/{932U,-16,1026U,1019U},
/*40*/{919U,-19,1026U,1018U},
/*42*/{906U,-22,1026U,1017U},
/*44*/{893U,-25,1026U,1017U},
/*46*/{880U,-27,1027U,1016U},
/*48*/{867U,-30,1027U,1015U},
/*50*/{854U,-33,1027U,1015U},
/*52*/{841U,-36,1028U,1014U},
/*54*/{828U,-38,1028U,1013U},
/*56*/{815U,-41,1028U,1013U},
/*58*/{802U,-44,1029U,1012U},
/*60*/{789U,-46,1029U,1011U},
/*62*/{776U,-49,1029U,1011U},
/*64*/{763U,-52,1029U,1010U},
/*66*/{750U,-55,1030U,1009U},
/*68*/{737U,-57,1030U,1009U},
/*70*/{724U,-59,1030U,1008U},
/*72*/{711U,-62,1031U,1007U},
/*74*/{698U,-65,1031U,1007U},
/*76*/{685U,-67,1031U,1006U},
/*78*/{672U,-69,1031U,1005U},
/*80*/{659U,-74,1032U,1005U},
/*82*/{646U,-77,1032U,1004U},
/*84*/{633U,-80,1032U,1003U},
/*86*/{620U,-85,1033U,1003U},
/*88*/{607U,-89,1033U,1002U},
/*90*/{594U,-92,1033U,1001U},
/*92*/{581U,-96,1034U,1001U},
/*94*/{568U,-98,1034U,1000U},
/*96*/{555U,-103,1034U,999U},
/*98*/{542U,-105,1034U,999U},
/*100*/{529U,-107,1035U,998U},
/*102*/{515U,-111,1035U,997U},
/*104*/{502U,-113,1035U,997U},
/*106*/{489U,-116,1036U,996U},
/*108*/{476U,-119,1036U,995U},
/*110*/{463U,-121,1036U,995U},
/*112*/{450U,-125,1037U,994U},
/*114*/{437U,-127,1037U,993U},
/*116*/{424U,-130,1037U,993U},
/*118*/{411U,-133,1037U,992U},
/*120*/{398U,-136,1038U,991U},


};

static const TempParams_t TEMP_CURVE_TABLE_GREEN[TEMP_CMP_NUM_MAX] = 
{
    /*T(1C) efficiency deltaV(mV) x(%), y(%) */
/*0*/{1056U,28,992U,1010U},
/*2*/{1053U,25,995U,1011U},
/*4*/{1050U,23,997U,1013U},
/*6*/{1048U,20,1000U,1014U},
/*8*/{1045U,18,1002U,1015U},
/*10*/{1042U,15,1005U,1016U},
/*12*/{1040U,13,1008U,1017U},
/*14*/{1037U,11,1010U,1018U},
/*16*/{1034U,8,1013U,1019U},
/*18*/{1032U,6,1016U,1020U},
/*20*/{1029U,4,1018U,1021U},
/*22*/{1026U,2,1021U,1022U},
/*24*/{1024U,0,1024U,1024U},
/*26*/{1021U,-2,1026U,1025U},
/*28*/{1018U,-4,1029U,1024U},
/*30*/{1015U,-6,1031U,1023U},
/*32*/{1012U,-7,1034U,1023U},
/*34*/{1010U,-9,1037U,1022U},
/*36*/{1007U,-11,1039U,1021U},
/*38*/{1004U,-13,1042U,1020U},
/*40*/{1001U,-15,1046U,1020U},
/*42*/{997U,-17,1051U,1019U},
/*44*/{994U,-18,1056U,1019U},
/*46*/{991U,-20,1060U,1018U},
/*48*/{988U,-22,1062U,1018U},
/*50*/{984U,-25,1066U,1017U},
/*52*/{981U,-27,1070U,1017U},
/*54*/{977U,-29,1074U,1016U},
/*56*/{974U,-31,1078U,1016U},
/*58*/{971U,-34,1083U,1015U},
/*60*/{967U,-36,1088U,1014U},
/*62*/{962U,-39,1092U,1014U},
/*64*/{957U,-42,1097U,1013U},
/*66*/{954U,-44,1101U,1012U},
/*68*/{950U,-46,1105U,1012U},
/*70*/{947U,-49,1109U,1011U},
/*72*/{944U,-50,1113U,1010U},
/*74*/{941U,-52,1117U,1010U},
/*76*/{936U,-54,1119U,1010U},
/*78*/{934U,-56,1122U,1009U},
/*80*/{931U,-58,1125U,1009U},
/*82*/{928U,-59,1129U,1008U},
/*84*/{925U,-61,1132U,1008U},
/*86*/{923U,-62,1135U,1007U},
/*88*/{918U,-65,1140U,1006U},
/*90*/{914U,-66,1145U,1005U},
/*92*/{910U,-67,1150U,1005U},
/*94*/{906U,-69,1154U,1004U},
/*96*/{902U,-71,1157U,1004U},
/*98*/{899U,-72,1160U,1003U},
/*100*/{896U,-74,1163U,1003U},
/*102*/{893U,-75,1167U,1002U},
/*104*/{890U,-77,1170U,1002U},
/*106*/{887U,-79,1173U,1001U},
/*108*/{883U,-80,1176U,1000U},
/*110*/{880U,-82,1180U,1000U},
/*112*/{877U,-84,1183U,999U},
/*114*/{874U,-86,1186U,999U},
/*116*/{871U,-87,1190U,998U},
/*118*/{868U,-89,1193U,998U},
/*120*/{865U,-92,1196U,997U},


};


static const TempParams_t TEMP_CURVE_TABLE_BLUE[TEMP_CMP_NUM_MAX] = 
{
    /*T(1C) efficiency deltaV(mV) x(%), y(%) */
/*0*/{992U,29,1032U,947U},
/*2*/{995U,27,1031U,954U},
/*4*/{997U,26,1030U,960U},
/*6*/{1000U,23,1030U,966U},
/*8*/{1003U,21,1029U,973U},
/*10*/{1005U,19,1028U,979U},
/*12*/{1008U,17,1028U,985U},
/*14*/{1010U,15,1027U,992U},
/*16*/{1013U,12,1026U,998U},
/*18*/{1016U,10,1026U,1004U},
/*20*/{1018U,6,1025U,1011U},
/*22*/{1021U,3,1024U,1017U},
/*24*/{1024U,0,1024U,1024U},
/*26*/{1026U,-4,1023U,1030U},
/*28*/{1029U,-6,1022U,1036U},
/*30*/{1031U,-8,1021U,1043U},
/*32*/{1034U,-12,1021U,1049U},
/*34*/{1037U,-15,1020U,1055U},
/*36*/{1039U,-16,1019U,1062U},
/*38*/{1042U,-19,1019U,1068U},
/*40*/{1044U,-21,1018U,1074U},
/*42*/{1047U,-23,1017U,1081U},
/*44*/{1050U,-25,1017U,1087U},
/*46*/{1052U,-27,1016U,1093U},
/*48*/{1055U,-28,1015U,1100U},
/*50*/{1058U,-30,1014U,1106U},
/*52*/{1060U,-32,1014U,1112U},
/*54*/{1062U,-34,1013U,1119U},
/*56*/{1064U,-36,1012U,1125U},
/*58*/{1065U,-38,1012U,1131U},
/*60*/{1067U,-40,1011U,1138U},
/*62*/{1069U,-42,1010U,1144U},
/*64*/{1070U,-44,1010U,1150U},
/*66*/{1071U,-46,1009U,1157U},
/*68*/{1071U,-48,1008U,1163U},
/*70*/{1073U,-49,1008U,1169U},
/*72*/{1074U,-52,1007U,1176U},
/*74*/{1075U,-55,1006U,1182U},
/*76*/{1075U,-56,1005U,1188U},
/*78*/{1076U,-59,1005U,1195U},
/*80*/{1077U,-61,1004U,1201U},
/*82*/{1077U,-63,1003U,1207U},
/*84*/{1078U,-65,1003U,1214U},
/*86*/{1079U,-67,1002U,1220U},
/*88*/{1079U,-69,1001U,1226U},
/*90*/{1080U,-71,1001U,1233U},
/*92*/{1080U,-73,1000U,1239U},
/*94*/{1080U,-75,999U,1245U},
/*96*/{1081U,-77,999U,1252U},
/*98*/{1081U,-79,998U,1258U},
/*100*/{1081U,-80,997U,1264U},
/*102*/{1081U,-82,996U,1271U},
/*104*/{1081U,-84,996U,1277U},
/*106*/{1081U,-86,995U,1283U},
/*108*/{1081U,-88,994U,1290U},
/*110*/{1082U,-90,994U,1296U},
/*112*/{1082U,-93,993U,1302U},
/*114*/{1082U,-94,992U,1309U},
/*116*/{1082U,-95,992U,1315U},
/*118*/{1083U,-98,991U,1322U},
/*120*/{1083U,-101,990U,1328U},


};



TempParams_t const * LP_GetTempTableRed(LedNum_t ledIndex)
{
    return TEMP_CURVE_TABLE_RED;
}

TempParams_t const * LP_GetTempTableGreen(LedNum_t ledIndex)
{
    return TEMP_CURVE_TABLE_GREEN;
}

TempParams_t const * LP_GetTempTableBlue(LedNum_t ledIndex)
{
    return TEMP_CURVE_TABLE_BLUE;
}



/*
const AccurateColorXYY_t calibratedColortable[] = 
{
  {21845, 21845, 100, 255,255,255},
  {X0, Y0, 100, 254,254,255},
};
*/



