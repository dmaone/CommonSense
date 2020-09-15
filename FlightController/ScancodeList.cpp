#include "ScancodeList.h"

QStringList populateScancodeList_() {
  QStringList list{};
  list << "----";      // 0x00
  list << "DEAD";     // 0x01
  list << "FwLdr";   // 0x02
  list << "ExpTgl"; // 0x03
  list << "A";     // 0x04
  list << "B";
  list << "C";
  list << "D";
  list << "E";
  list << "F";
  list << "G";
  list << "H";
  list << "I";
  list << "J";
  list << "K";
  list << "L";
  list << "M";
  list << "N";
  list << "O";
  list << "P";
  list << "Q";
  list << "R";
  list << "S";
  list << "T";
  list << "U";
  list << "V";
  list << "W";
  list << "X";
  list << "Y";
  list << "Z";
  list << "1";
  list << "2";
  list << "3";
  list << "4";
  list << "5";
  list << "6";
  list << "7";
  list << "8";
  list << "9";
  list << "0";
  list << "Enter";
  list << "Esc";
  list << "BkSp";
  list << "Tab";
  list << "Space";
  list << "-";
  list << "=";
  list << "[";
  list << "]";
  list << "\\";
  list << "ISO~";
  list << ";";
  list << "'";
  list << "`";
  list << ",";
  list << ".";
  list << "/";
  list << "CAPS";
  list << "F1";
  list << "F2";
  list << "F3";
  list << "F4";
  list << "F5";
  list << "F6";
  list << "F7";
  list << "F8";
  list << "F9";
  list << "F10";
  list << "F11";
  list << "F12";
  list << "PrtSc";
  list << "ScrLk";
  list << "Pause";
  list << "Ins";
  list << "Home";
  list << "PgUp";
  list << "Del";
  list << "End";
  list << "PgDn";
  list << "→";    // right
  list << "←";   // left
  list << "↓";  // down
  list << "↑"; // up
  list << "NumLk";
  list << "KP/";
  list << "KP*";
  list << "KP-";
  list << "KP+";
  list << "KPEnt";
  list << "KP1";
  list << "KP2";
  list << "KP3";
  list << "KP4";
  list << "KP5";
  list << "KP6";
  list << "KP7";
  list << "KP8";
  list << "KP9";
  list << "KP0";
  list << "KP.";
  list << "ISO\\|";
  list << "Win";
  list << "Power";
  list << "KP=";
  list << "F13";
  list << "F14";
  list << "F15";
  list << "F16";
  list << "F17";
  list << "F18";
  list << "F19";
  list << "F20";
  list << "F21";
  list << "F22";
  list << "F23";
  list << "F24";
  list << "Exec";
  list << "Help";
  list << "Menu";
  list << "Selct";
  list << "Stop";
  list << "Again";
  list << "Undo";
  list << "Cut";
  list << "Copy";
  list << "Paste";
  list << "Find";
  list << "Mute";
  list << "VolUp";
  list << "VolDn";
  list << "LCaps";
  list << "LNum";
  list << "LScr";
  list << "KP,";
  list << "KP=";
  list << "BR/?";
  list << "かな";
  list << "¥";
  list << "変換";     // henkan, "conversion", JP kbd
  list << "無変換";  // muhenkan, "no conversion", JP kbd
  list << "AX";
  list << "WChr";
  list << "INT8";
  list << "INT9";
  list << "한/영";     // Han/Yeong (Hangul/English), KR kbd
  list << "漢字";     // Hanja (Hangul -> Chinese), KR kbd
  list << "カタカナ";  // カタカナ Katakana, JP kbd
  list << "ひらがな"; // ひらがな Hiragana, JP kbd
  list << "半/全"; // 半角/全角 half/full AKA hankaku/zenkaku, JP kbd
  list << "LNG6";
  list << "LNG7";
  list << "LNG8";
  list << "LNG9";
  list << "Erase";
  list << "Attn";
  list << "Cancl";
  list << "Clear";
  list << "Prior";
  list << "Retrn";
  list << "Sep";
  list << "Out";
  list << "Oper";
  list << "Cl/Agn";
  list << "CrSel";
  list << "ExSel";
  list << "Power";  // 0xa5-7 reserved range, remapped
  list << "Sleep";
  list << "Wake";
  list << "Fn1"; // 0xa8 - reserved, remapped to layer manipulations
  list << "Fn2";
  list << "Fn3";
  list << "Fn4";
  list << "LLck1";
  list << "LLck2";
  list << "LLck3";
  list << "LLck4";
  list << "00";
  list << "000";
  list << "ThSep";
  list << "DcSep";
  list << "$";
  list << "c";
  list << "KP(";
  list << "KP)";
  list << "KP{";
  list << "KP}";
  list << "KPTab";
  list << "KPBsp";
  list << "KPA";
  list << "KPB";
  list << "KPC";
  list << "KPD";
  list << "KPE";
  list << "KPF";
  list << "KPXOR";
  list << "KP^";
  list << "KP%";
  list << "KP<";
  list << "KP>";
  list << "KP&";
  list << "KP&&";
  list << "KP|";
  list << "KP||";
  list << "KP:";
  list << "KP#";
  list << "KPSpc";
  list << "KP@";
  list << "KP!";
  list << "KPMS";
  list << "KPRC";
  list << "KPMC";
  list << "KPM+";
  list << "KPM-";
  list << "KPM*";
  list << "KPM/";
  list << "KP+-";
  list << "KpClr";
  list << "ClEnt";
  list << "KPBin";
  list << "KPOct";
  list << "KPDec";
  list << "KPHex";
  list << "-r-E";
  list << "-r-F";
  list << "LCtrl"; // 0xE0
  list << "LShft";
  list << "LAlt";
  list << "LGUI";
  list << "RCtrl";
  list << "RShft";
  list << "RAlt";
  list << "RGUI";  // 0xe7, below is reserved range mapped to media
  list << ">/||"; // Play/Pause
  list << "Mute";
  list << "Vol++";
  list << "Vol--";
  list << "Eject";
  list << "-r-D";
  list << "-r-E";
  list << "-r-F";
  list << "Play"; // 0xf0, reserved range mapped to media
  list << "||";
  list << "Rec";
  list << ">>";
  list << "<<";
  list << "NTrk";
  list << "PTrk";
  list << "Stop";
  list << "-r-8";
  list << "-r-9";
  list << "-r-A";
  list << "-r-B";
  list << "-r-C";
  list << "-r-D";
  list << "-r-E";
  list << "-r-F";

  return list;
}

/* static */ QStringList* ScancodeList::getScancodeList_() {
  static QStringList list = populateScancodeList_();
  return &list;
}
