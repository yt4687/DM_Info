///////////////////////////////////////
//
//  ARIB8->UNICODE
//
// ==== 利用条件等 ====
// このファイルはEpgDataCap_Bon(人柱版9.46)のコードを利用しており、
// これを再利用する場合は、以下のEpgDataCap_Bonの利用条件に
// したがって利用してください。
// <<<< EpgDataCap_BonのReadme.txtより転載 >>>>
// ■ソース（src.zip）の取り扱いについて
// 　特にGPLとかにはしないのでフリーソフトに限っては自由に改変してもらった
// 　り組み込んでもらって構わないです。
// 　改変したり組み込んだりして公開する場合は該当部分のソースぐらいは一緒
// 　に公開してください。（強制ではないので別に公開しなくてもいいです）
// 　商用、シェアウェアなどに組み込むのは不可です。
//
///////////////////////////////////////

#include <windows.h>

#include "ARIB8CharDecode.h"

//文字符号集合
//Gセット
#define MF_JIS_KANJI1 0x39 //JIS互換漢字1面
#define MF_JIS_KANJI2 0x3A //JIS互換漢字2面
#define MF_KIGOU 0x3B //追加記号
#define MF_ASCII 0x4A //英数
#define MF_HIRA  0x30 //平仮名
#define MF_KANA  0x31 //片仮名
#define MF_KANJI 0x42 //漢字
#define MF_MOSAIC_A 0x32 //モザイクA
#define MF_MOSAIC_B 0x33 //モザイクB
#define MF_MOSAIC_C 0x34 //モザイクC
#define MF_MOSAIC_D 0x35 //モザイクD
#define MF_PROP_ASCII 0x36 //プロポーショナル英数
#define MF_PROP_HIRA  0x37 //プロポーショナル平仮名
#define MF_PROP_KANA  0x38 //プロポーショナル片仮名
#define MF_JISX_KANA 0x49 //JIX X0201片仮名
//DRCS
#define MF_DRCS_0 0x40 //DRCS-0
#define MF_DRCS_1 0x41 //DRCS-1
#define MF_DRCS_2 0x42 //DRCS-2
#define MF_DRCS_3 0x43 //DRCS-3
#define MF_DRCS_4 0x44 //DRCS-4
#define MF_DRCS_5 0x45 //DRCS-5
#define MF_DRCS_6 0x46 //DRCS-6
#define MF_DRCS_7 0x47 //DRCS-7
#define MF_DRCS_8 0x48 //DRCS-8
#define MF_DRCS_9 0x49 //DRCS-9
#define MF_DRCS_10 0x4A //DRCS-10
#define MF_DRCS_11 0x4B //DRCS-11
#define MF_DRCS_12 0x4C //DRCS-12
#define MF_DRCS_13 0x4D //DRCS-13
#define MF_DRCS_14 0x4E //DRCS-14
#define MF_DRCS_15 0x4F //DRCS-15
#define MF_MACRO 0x70 //マクロ

//符号集合の分類
#define MF_MODE_G 1 //Gセット
#define MF_MODE_DRCS 2 //DRCS
#define MF_MODE_OTHER 3 //その他

const static WCHAR AsciiTable[][3]={
	L"！",L"”",L"＃",L"＄",L"％",L"＆",L"’",
	L"（",L"）",L"＊",L"＋",L"，",L"－",L"．",L"／",
	L"０",L"１",L"２",L"３",L"４",L"５",L"６",L"７",
	L"８",L"９",L"：",L"；",L"＜",L"＝",L"＞",L"？",
	L"＠",L"Ａ",L"Ｂ",L"Ｃ",L"Ｄ",L"Ｅ",L"Ｆ",L"Ｇ",
	L"Ｈ",L"Ｉ",L"Ｊ",L"Ｋ",L"Ｌ",L"Ｍ",L"Ｎ",L"Ｏ",
	L"Ｐ",L"Ｑ",L"Ｒ",L"Ｓ",L"Ｔ",L"Ｕ",L"Ｖ",L"Ｗ",
	L"Ｘ",L"Ｙ",L"Ｚ",L"［",L"￥",L"］",L"＾",L"＿",
	L"‘",L"ａ",L"ｂ",L"ｃ",L"ｄ",L"ｅ",L"ｆ",L"ｇ",
	L"ｈ",L"ｉ",L"ｊ",L"ｋ",L"ｌ",L"ｍ",L"ｎ",L"ｏ",
	L"ｐ",L"ｑ",L"ｒ",L"ｓ",L"ｔ",L"ｕ",L"ｖ",L"ｗ",
	L"ｘ",L"ｙ",L"ｚ",L"｛",L"｜",L"｝",L"￣"
};
const static WCHAR HiraTable[][3]={
	L"ぁ",L"あ",L"ぃ",L"い",L"ぅ",L"う",L"ぇ",
	L"え",L"ぉ",L"お",L"か",L"が",L"き",L"ぎ",L"く",
	L"ぐ",L"け",L"げ",L"こ",L"ご",L"さ",L"ざ",L"し",
	L"じ",L"す",L"ず",L"せ",L"ぜ",L"そ",L"ぞ",L"た",
	L"だ",L"ち",L"ぢ",L"っ",L"つ",L"づ",L"て",L"で",
	L"と",L"ど",L"な",L"に",L"ぬ",L"ね",L"の",L"は",
	L"ば",L"ぱ",L"ひ",L"び",L"ぴ",L"ふ",L"ぶ",L"ぷ",
	L"へ",L"べ",L"ぺ",L"ほ",L"ぼ",L"ぽ",L"ま",L"み",
	L"む",L"め",L"も",L"ゃ",L"や",L"ゅ",L"ゆ",L"ょ",
	L"よ",L"ら",L"り",L"る",L"れ",L"ろ",L"ゎ",L"わ",
	L"ゐ",L"ゑ",L"を",L"ん",L"　",L"　",L"　",L"ゝ",
	L"ゞ",L"ー",L"。",L"「",L"」",L"、",L"・"
};
const static WCHAR KanaTable[][3]={
	L"ァ",L"ア",L"ィ",L"イ",L"ゥ",L"ウ",L"ェ",
	L"エ",L"ォ",L"オ",L"カ",L"ガ",L"キ",L"ギ",L"ク",
	L"グ",L"ケ",L"ゲ",L"コ",L"ゴ",L"サ",L"ザ",L"シ",
	L"ジ",L"ス",L"ズ",L"セ",L"ゼ",L"ソ",L"ゾ",L"タ",
	L"ダ",L"チ",L"ヂ",L"ッ",L"ツ",L"ヅ",L"テ",L"デ",
	L"ト",L"ド",L"ナ",L"ニ",L"ヌ",L"ネ",L"ノ",L"ハ",
	L"バ",L"パ",L"ヒ",L"ビ",L"ピ",L"フ",L"ブ",L"プ",
	L"ヘ",L"ベ",L"ペ",L"ホ",L"ボ",L"ポ",L"マ",L"ミ",
	L"ム",L"メ",L"モ",L"ャ",L"ヤ",L"ュ",L"ユ",L"ョ",
	L"ヨ",L"ラ",L"リ",L"ル",L"レ",L"ロ",L"ヮ",L"ワ",
	L"ヰ",L"ヱ",L"ヲ",L"ン",L"ヴ",L"ヵ",L"ヶ",L"ヽ",
	L"ヾ",L"ー",L"。",L"「",L"」",L"、",L"・"
};

typedef struct _GAIJI_TABLE{
	unsigned short usARIB8;
	const WCHAR* strChar;
} GAIJI_TABLE;

const static GAIJI_TABLE GaijiTable[]={
	{0x7A4D, L"10."},
	{0x7A4E, L"11."},
	{0x7A4F, L"12."},
	{0x7A50, L"[HV]"}, //90区48点
	{0x7A51, L"[SD]"},
	{0x7A52, L"[Ｐ]"},
	{0x7A53, L"[Ｗ]"},
	{0x7A54, L"[MV]"},
	{0x7A55, L"[手]"},
	{0x7A56, L"[字]"},
	{0x7A57, L"[双]"},
	{0x7A58, L"[デ]"},
	{0x7A59, L"[Ｓ]"},
	{0x7A5A, L"[二]"},
	{0x7A5B, L"[多]"},
	{0x7A5C, L"[解]"},
	{0x7A5D, L"[SS]"},
	{0x7A5E, L"[Ｂ]"},
	{0x7A5F, L"[Ｎ]"},//
	{0x7A60, L"■"},//90区64点
	{0x7A61, L"●"},
	{0x7A62, L"[天]"},
	{0x7A63, L"[交]"},
	{0x7A64, L"[映]"},
	{0x7A65, L"[無]"},
	{0x7A66, L"[料]"},
	{0x7A67, L"[・]"},
	{0x7A68, L"[前]"},
	{0x7A69, L"[後]"},
	{0x7A6A, L"[再]"},
	{0x7A6B, L"[新]"},
	{0x7A6C, L"[初]"},
	{0x7A6D, L"[終]"},
	{0x7A6E, L"[生]"},
	{0x7A6F, L"[販]"},
	{0x7A70, L"[声]"},//90区80点
	{0x7A71, L"[吹]"},
	{0x7A72, L"[PPV]"},
	{0x7A73, L"(秘)"},
	{0x7A74, L"ほか"},
	//91区は飛ばす
	{0x7C21, L"→"},//92区1点
	{0x7C22, L"←"},
	{0x7C23, L"↑"},
	{0x7C24, L"↓"},
	{0x7C25, L"・"},
	{0x7C26, L"・"},
	{0x7C27, L"年"},
	{0x7C28, L"月"},
	{0x7C29, L"日"},
	{0x7C2A, L"円"},
	{0x7C2B, L"m^2"},
	{0x7C2C, L"m^3"},
	{0x7C2D, L"cm"},
	{0x7C2E, L"cm^2"},
	{0x7C2F, L"cm^3"},
	{0x7C30, L"０."},//92区16点
	{0x7C31, L"１."},
	{0x7C32, L"２."},
	{0x7C33, L"３."},
	{0x7C34, L"４."},
	{0x7C35, L"５."},
	{0x7C36, L"６."},
	{0x7C37, L"７."},
	{0x7C38, L"８."},
	{0x7C39, L"９."},
	{0x7C3A, L"氏"},
	{0x7C3B, L"副"},
	{0x7C3C, L"元"},
	{0x7C3D, L"故"},
	{0x7C3E, L"前"},
	{0x7C3F, L"後"},
	{0x7C40, L"０,"},//92区32点
	{0x7C41, L"１,"},
	{0x7C42, L"２,"},
	{0x7C43, L"３,"},
	{0x7C44, L"４,"},
	{0x7C45, L"５,"},
	{0x7C46, L"６,"},
	{0x7C47, L"７,"},
	{0x7C48, L"８,"},
	{0x7C49, L"９,"},
	{0x7C4A, L"[社]"},
	{0x7C4B, L"[財]"},
	{0x7C4C, L"[有]"},
	{0x7C4D, L"[株]"},
	{0x7C4E, L"[代]"},
	{0x7C4F, L"(問)"},
	{0x7C50, L"・"},//92区48点
	{0x7C51, L"・"},
	{0x7C52, L"・"},
	{0x7C53, L"・"},
	{0x7C54, L"・"},
	{0x7C55, L"・"},
	{0x7C56, L"・"},
	{0x7C57, L"(CD)"},
	{0x7C58, L"(vn)"},
	{0x7C59, L"(ob)"},
	{0x7C5A, L"(cb)"},
	{0x7C5B, L"(ce"},
	{0x7C5C, L"mb)"},
	{0x7C5D, L"(hp)"},
	{0x7C5E, L"(br)"},
	{0x7C5F, L"(ｐ)"},
	{0x7C60, L"(ｓ)"},//92区64点
	{0x7C61, L"(ms)"},
	{0x7C62, L"(ｔ)"},
	{0x7C63, L"(bs)"},
	{0x7C64, L"(ｂ)"},
	{0x7C65, L"(tb)"},
	{0x7C66, L"(tp)"},
	{0x7C67, L"(ds)"},
	{0x7C68, L"(ag)"},
	{0x7C69, L"(eg)"},
	{0x7C6A, L"(vo)"},
	{0x7C6B, L"(fl)"},
	{0x7C6C, L"(ke"},
	{0x7C6D, L"y)"},
	{0x7C6E, L"(sa"},
	{0x7C6F, L"x)"},
	{0x7C70, L"(sy"},//92区80点
	{0x7C71, L"n)"},
	{0x7C72, L"(or"},
	{0x7C73, L"g)"},
	{0x7C74, L"(pe"},
	{0x7C75, L"r)"},
	{0x7C76, L"(Ｒ)"},
	{0x7C77, L"(Ｃ)"},
	{0x7C78, L"(箏)"},
	{0x7C79, L"ＤＪ"},
	{0x7C7A, L"[演]"},
	{0x7C7B, L"Fax"},
	{0x7D21, L"(月)"},//93区1点
	{0x7D22, L"(火)"},
	{0x7D23, L"(水)"},
	{0x7D24, L"(木)"},
	{0x7D25, L"(金)"},
	{0x7D26, L"(土)"},
	{0x7D27, L"(日)"},
	{0x7D28, L"(祝)"},
	{0x7D29, L"㍾"},
	{0x7D2A, L"㍽"},
	{0x7D2B, L"㍼"},
	{0x7D2C, L"㍻"},
	{0x7D2D, L"No."},
	{0x7D2E, L"Tel"},
	{0x7D2F, L"(〒)"},
	{0x7D30, L"()()"},//93区16点
	{0x7D31, L"[本]"},
	{0x7D32, L"[三]"},
	{0x7D33, L"[二]"},
	{0x7D34, L"[安]"},
	{0x7D35, L"[点]"},
	{0x7D36, L"[打]"},
	{0x7D37, L"[盗]"},
	{0x7D38, L"[勝]"},
	{0x7D39, L"[敗]"},
	{0x7D3A, L"[Ｓ]"},
	{0x7D3B, L"[投]"},
	{0x7D3C, L"[捕]"},
	{0x7D3D, L"[一]"},
	{0x7D3E, L"[二]"},
	{0x7D3F, L"[三]"},
	{0x7D40, L"[遊]"},//93区32点
	{0x7D41, L"[左]"},
	{0x7D42, L"[中]"},
	{0x7D43, L"[右]"},
	{0x7D44, L"[指]"},
	{0x7D45, L"[走]"},
	{0x7D46, L"[打]"},
	{0x7D47, L"l"},
	{0x7D48, L"kg"},
	{0x7D49, L"Hz"},
	{0x7D4A, L"ha"},
	{0x7D4B, L"km"},
	{0x7D4C, L"km^2"},
	{0x7D4D, L"hPa"},
	{0x7D4E, L"・"},
	{0x7D4F, L"・"},
	{0x7D50, L"1/2"},//93区48点
	{0x7D51, L"0/3"},
	{0x7D52, L"1/3"},
	{0x7D53, L"2/3"},
	{0x7D54, L"3/3"},
	{0x7D55, L"1/4"},
	{0x7D56, L"3/4"},
	{0x7D57, L"1/5"},
	{0x7D58, L"2/5"},
	{0x7D59, L"3/5"},
	{0x7D5A, L"4/5"},
	{0x7D5B, L"1/6"},
	{0x7D5C, L"1/7"},
	{0x7D5D, L"1/8"},
	{0x7D5E, L"1/9"},
	{0x7D5F, L"1/10"},
	{0x7D6E, L"!!"},//93区78点
	{0x7D6F, L"!?"},
	{0x7E21, L"Ⅰ"},//94区1点
	{0x7E22, L"Ⅱ"},
	{0x7E23, L"Ⅲ"},
	{0x7E24, L"Ⅳ"},
	{0x7E25, L"Ⅴ"},
	{0x7E26, L"Ⅵ"},
	{0x7E27, L"Ⅶ"},
	{0x7E28, L"Ⅷ"},
	{0x7E29, L"Ⅸ"},
	{0x7E2A, L"Ⅹ"},
	{0x7E2B, L"XI"},
	{0x7E2C, L"XII"},
	{0x7E2D, L"⑰"},
	{0x7E2E, L"⑱"},
	{0x7E2F, L"⑲"},
	{0x7E30, L"⑳"},//94区16点
	{0x7E31, L"(１)"},
	{0x7E32, L"(２)"},
	{0x7E33, L"(３)"},
	{0x7E34, L"(４)"},
	{0x7E35, L"(５)"},
	{0x7E36, L"(６)"},
	{0x7E37, L"(７)"},
	{0x7E38, L"(８)"},
	{0x7E39, L"(９)"},
	{0x7E3A, L"(10)"},
	{0x7E3B, L"(11)"},
	{0x7E3C, L"(12)"},
	{0x7E3D, L"(21)"},
	{0x7E3E, L"(22)"},
	{0x7E3F, L"(23)"},
	{0x7E40, L"(24)"},//94区32点
	{0x7E41, L"(Ａ)"},
	{0x7E42, L"(Ｂ)"},
	{0x7E43, L"(Ｃ)"},
	{0x7E44, L"(Ｄ)"},
	{0x7E45, L"(Ｅ)"},
	{0x7E46, L"(Ｆ)"},
	{0x7E47, L"(Ｇ)"},
	{0x7E48, L"(Ｈ)"},
	{0x7E49, L"(Ｉ)"},
	{0x7E4A, L"(Ｊ)"},
	{0x7E4B, L"(Ｋ)"},
	{0x7E4C, L"(Ｌ)"},
	{0x7E4D, L"(Ｍ)"},
	{0x7E4E, L"(Ｎ)"},
	{0x7E4F, L"(Ｏ)"},
	{0x7E50, L"(Ｐ)"},//94区48点
	{0x7E51, L"(Ｑ)"},
	{0x7E52, L"(Ｒ)"},
	{0x7E53, L"(Ｓ)"},
	{0x7E54, L"(Ｔ)"},
	{0x7E55, L"(Ｕ)"},
	{0x7E56, L"(Ｖ)"},
	{0x7E57, L"(Ｗ)"},
	{0x7E58, L"(Ｘ)"},
	{0x7E59, L"(Ｙ)"},
	{0x7E5A, L"(Ｚ)"},
	{0x7E5B, L"(25)"},
	{0x7E5C, L"(26)"},
	{0x7E5D, L"(27)"},
	{0x7E5E, L"(28)"},
	{0x7E5F, L"(29)"},
	{0x7E60, L"(30)"},//94区64点
	{0x7E61, L"①"},
	{0x7E62, L"②"},
	{0x7E63, L"③"},
	{0x7E64, L"④"},
	{0x7E65, L"⑤"},
	{0x7E66, L"⑥"},
	{0x7E67, L"⑦"},
	{0x7E68, L"⑧"},
	{0x7E69, L"⑨"},
	{0x7E6A, L"⑩"},
	{0x7E6B, L"⑪"},
	{0x7E6C, L"⑫"},
	{0x7E6D, L"⑬"},
	{0x7E6E, L"⑭"},
	{0x7E6F, L"⑮"},
	{0x7E70, L"⑯"},//94区80点
	{0x7E71, L"(１)"},
	{0x7E72, L"(２)"},
	{0x7E73, L"(３)"},
	{0x7E74, L"(４)"},
	{0x7E75, L"(５)"},
	{0x7E76, L"(６)"},
	{0x7E77, L"(７)"},
	{0x7E78, L"(８)"},
	{0x7E79, L"(９)"},
	{0x7E7A, L"(10)"},
	{0x7E7B, L"(11)"},
	{0x7E7C, L"(12)"},
	{0x7E7D, L"(31)"}
};

const static GAIJI_TABLE GaijiTbl2[]={
	{0x7521, L"〓"},
	{0x7522, L"〓"},
	{0x7523, L"〓"},
	{0x7524, L"〓"},
	{0x7525, L"侚"},
	{0x7526, L"俉"},
	{0x7527, L"〓"},
	{0x7528, L"〓"},
	{0x7529, L"〓"},
	{0x752A, L"〓"}, //10
	{0x752B, L"匇"},
	{0x752C, L"〓"},
	{0x752D, L"〓"},
	{0x752E, L"詹"},
	{0x752F, L"〓"},
	{0x7530, L"〓"},
	{0x7531, L"〓"},
	{0x7532, L"〓"},
	{0x7533, L"咩"},
	{0x7534, L"〓"}, //20
	{0x7535, L"〓"},
	{0x7536, L"〓"},
	{0x7537, L"〓"},
	{0x7538, L"〓"},
	{0x7539, L"〓"},
	{0x753A, L"塚"},
	{0x753B, L"〓"},
	{0x753C, L"〓"},
	{0x753D, L"〓"},
	{0x753E, L"〓"}, //30
	{0x753F, L"寬"},
	{0x7540, L"﨑"},
	{0x7541, L"〓"},
	{0x7542, L"〓"},
	{0x7543, L"弴"},
	{0x7544, L"彅"},
	{0x7545, L"德"},
	{0x7546, L"〓"},
	{0x7547, L"〓"},
	{0x7548, L"愰"}, //40
	{0x7549, L"昤"},
	{0x754A, L"〓"},
	{0x754B, L"曙"},
	{0x754C, L"曺"},
	{0x754D, L"曻"},
	{0x754E, L"〓"},
	{0x754F, L"〓"},
	{0x7550, L"〓"},
	{0x7551, L"〓"},
	{0x7552, L"〓"}, //50
	{0x7553, L"〓"},
	{0x7554, L"櫛"},
	{0x7555, L"〓"},
	{0x7556, L"〓"},
	{0x7557, L"〓"},
	{0x7558, L"〓"},
	{0x7559, L"〓"},
	{0x755A, L"〓"},
	{0x755B, L"〓"},
	{0x755C, L"〓"}, //60
	{0x755D, L"〓"},
	{0x755E, L"〓"},
	{0x755F, L"〓"},
	{0x7560, L"〓"},
	{0x7561, L"〓"},
	{0x7562, L"〓"},
	{0x7563, L"〓"},
	{0x7564, L"〓"},
	{0x7565, L"煇"},
	{0x7566, L"燁"}, //70
	{0x7567, L"〓"},
	{0x7568, L"〓"},
	{0x7569, L"〓"},
	{0x756A, L"珉"},
	{0x756B, L"珖"},
	{0x756C, L"〓"},
	{0x756D, L"〓"},
	{0x756E, L"〓"},
	{0x756F, L"琦"},
	{0x7570, L"琪"}, //80
	{0x7571, L"〓"},
	{0x7572, L"〓"},
	{0x7573, L"〓"},
	{0x7574, L"〓"},
	{0x7575, L"〓"},
	{0x7576, L"〓"},
	{0x7577, L"〓"},
	{0x7578, L"〓"},
	{0x7579, L"〓"},
	{0x757A, L"〓"}, //90
	{0x757B, L"祇"},
	{0x757C, L"禮"},
	{0x757D, L"〓"},
	{0x757E, L"〓"},
	{0x7621, L"〓"},
	{0x7622, L"〓"},
	{0x7623, L"〓"},
	{0x7624, L"〓"},
	{0x7625, L"〓"},
	{0x7626, L"〓"}, //100
	{0x7627, L"〓"},
	{0x7628, L"羡"},
	{0x7629, L"〓"},
	{0x762A, L"〓"},
	{0x762B, L"〓"},
	{0x762C, L"〓"},
	{0x762D, L"葛"},
	{0x762E, L"蓜"},
	{0x762F, L"蓬"},
	{0x7630, L"蕙"}, //110
	{0x7631, L"〓"},
	{0x7632, L"蝕"},
	{0x7633, L"〓"},
	{0x7634, L"〓"},
	{0x7635, L"裵"},
	{0x7636, L"角"},
	{0x7637, L"諶"},
	{0x7638, L"〓"},
	{0x7639, L"辻"},
	{0x763A, L"〓"}, //120
	{0x763B, L"〓"},
	{0x763C, L"鄧"},
	{0x763D, L"鄭"},
	{0x763E, L"〓"},
	{0x763F, L"〓"},
	{0x7640, L"銈"},
	{0x7641, L"錡"},
	{0x7642, L"鍈"},
	{0x7643, L"閒"},
	{0x7644, L"〓"}, //130
	{0x7645, L"餃"},
	{0x7646, L"〓"},
	{0x7647, L"髙"},
	{0x7648, L"鯖"},
	{0x7649, L"〓"},
	{0x764A, L"〓"},
	{0x764B, L"〓"}
};

//文字サイズ
typedef enum{
	STR_SMALL = 0, //SSZ
	STR_MEDIUM, //MSZ
	STR_NORMAL, //NSZ
	STR_MICRO, //SZX 0x60
	STR_HIGH_W, //SZX 0x41
	STR_WIDTH_W, //SZX 0x44
	STR_W, //SZX 0x45
	STR_SPECIAL_1, //SZX 0x6B
	STR_SPECIAL_2, //SZX 0x64
} STRING_SIZE;

class CARIB8CharDecode
{
public:
	CARIB8CharDecode(void);
	~CARIB8CharDecode(void);

	//PSI/SIを想定したUNICODEへの変換
	BOOL PSISI( const BYTE* pbSrc, DWORD dwSrcSize, WCHAR* pwszDst, DWORD dwDstSize, DWORD* pdwSize ) ;

	//デコードした文字列
protected:
	WCHAR* m_pwszBuf ;
	DWORD m_dwBufSize ;
	DWORD m_dwCnt ;
	DWORD* m_pdwSize ;
	void Append( const WCHAR* pwsz ) ;
	void Append( WCHAR wc ) ;

protected:
	typedef struct _MF_MODE{
		int iMF; //文字符号集合
		int iMode; //符号集合の分類
		int iByte; //読み込みバイト数
		//=オペレーターの処理
		_MF_MODE & operator= (const _MF_MODE & o) {
			iMF = o.iMF;
			iMode = o.iMode;
			iByte = o.iByte;
			return *this;
		}
	} MF_MODE;

	MF_MODE m_G0;
	MF_MODE m_G1;
	MF_MODE m_G2;
	MF_MODE m_G3;
	MF_MODE* m_GL;
	MF_MODE* m_GR;

	//文字サイズ
	STRING_SIZE m_emStrSize;
protected:
	void InitPSISI(void);
	BOOL Analyze( const BYTE* pbSrc, DWORD dwSrcSize, DWORD* pdwReadSize );

	BOOL IsSmallCharMode(void);

	//制御符号
	BOOL C0( const BYTE* pbSrc, DWORD* pdwReadSize );
	BOOL C1( const BYTE* pbSrc, DWORD* pdwReadSize );
	BOOL GL( const BYTE* pbSrc, DWORD* pdwReadSize );
	BOOL GR( const BYTE* pbSrc, DWORD* pdwReadSize );
	//シングルシフト
	BOOL SS2( const BYTE* pbSrc, DWORD* pdwReadSize );
	BOOL SS3( const BYTE* pbSrc, DWORD* pdwReadSize );
	//エスケープシーケンス
	BOOL ESC( const BYTE* pbSrc, DWORD* pdwReadSize );
	//２バイト文字変換
	BOOL ToUnicode( const BYTE bFirst, const BYTE bSecond );
	BOOL ToCustomFont( const BYTE bFirst, const BYTE bSecond );
	//
	BOOL CSI( const BYTE* pbSrc, DWORD* pdwReadSize );

};

CARIB8CharDecode::CARIB8CharDecode(void)
{
}

CARIB8CharDecode::~CARIB8CharDecode(void)
{
}

BOOL CARIB8CharDecode::PSISI( const BYTE* pbSrc, DWORD dwSrcSize, WCHAR* pwszDst, DWORD dwDstSize, DWORD* pdwSize )
{
	if( pbSrc == NULL || dwSrcSize == 0 ){
		return FALSE;
	}
	if ( pwszDst == NULL ) dwDstSize = 0 ;
	else *pwszDst = 0 ;
	m_pwszBuf = pwszDst ;
	m_dwBufSize = dwDstSize ;
	m_dwCnt = 0 ;
	m_pdwSize = pdwSize ;
	if ( m_pdwSize ) *m_pdwSize = 1 ;
	InitPSISI();
	DWORD dwReadSize = 0;

	return Analyze(pbSrc, dwSrcSize, &dwReadSize );
}

void CARIB8CharDecode::InitPSISI(void)
{
	m_G0.iMF = MF_JIS_KANJI1;
	m_G0.iMode = MF_MODE_G;
	m_G0.iByte = 2;

	m_G1.iMF = MF_ASCII;
	m_G1.iMode = MF_MODE_G;
	m_G1.iByte = 1;

	m_G2.iMF = MF_HIRA;
	m_G2.iMode = MF_MODE_G;
	m_G2.iByte = 1;

	m_G3.iMF = MF_KANA;
	m_G3.iMode = MF_MODE_G;
	m_G3.iByte = 1;

	m_GL = &m_G0;
	m_GR = &m_G2;

	m_emStrSize = STR_NORMAL;
}

void CARIB8CharDecode::Append( const WCHAR* pwsz )
{
	while( *pwsz ) {
		Append( *pwsz ) ;
		pwsz++ ;
	}
}

void CARIB8CharDecode::Append( WCHAR wc )
{
	if ( m_dwBufSize > m_dwCnt+1 ) {
		m_pwszBuf[m_dwCnt] = wc ;
		m_pwszBuf[m_dwCnt+1] = 0 ;
	}
	m_dwCnt++ ;
	if ( m_pdwSize ) *m_pdwSize = m_dwCnt + 1 ;
}

BOOL CARIB8CharDecode::IsSmallCharMode(void)
{
	BOOL bRet = FALSE;
	switch(m_emStrSize){
		case STR_SMALL:
			bRet = TRUE;
			break;
		case STR_MEDIUM:
			bRet = TRUE;
			break;
		case STR_NORMAL:
			bRet = FALSE;
			break;
		case STR_MICRO:
			bRet = TRUE;
			break;
		case STR_HIGH_W:
			bRet = FALSE;
			break;
		case STR_WIDTH_W:
			bRet = FALSE;
			break;
		case STR_W:
			bRet = FALSE;
			break;
		case STR_SPECIAL_1:
			bRet = FALSE;
			break;
		case STR_SPECIAL_2:
			bRet = FALSE;
			break;
		default:
			break;
	}
	return bRet;
}

BOOL CARIB8CharDecode::Analyze( const BYTE* pbSrc, DWORD dwSrcSize, DWORD* pdwReadSize )
{
	if( pbSrc == NULL || dwSrcSize == 0 || pdwReadSize == NULL){
		return FALSE;
	}
	BOOL bRet = TRUE;
	DWORD dwReadSize = 0;

	while( dwReadSize < dwSrcSize ){
		DWORD dwReadBuff = 0;
		//1バイト目チェック
		if( pbSrc[dwReadSize] <= 0x20 ){
			//C0制御コード
			bRet = C0( pbSrc+dwReadSize, &dwReadBuff );
			dwReadSize += dwReadBuff;
			if( bRet == FALSE ){
				return FALSE;
			}else if( bRet == 2 ){
				bRet = TRUE;
				break;
			}
		}else if( pbSrc[dwReadSize] > 0x20 && pbSrc[dwReadSize] < 0x7F ){
			//GL符号領域
			if( GL( pbSrc+dwReadSize, &dwReadBuff ) == FALSE ){
				return FALSE;
			}
			dwReadSize += dwReadBuff;
		}else if( pbSrc[dwReadSize] >= 0x7F && pbSrc[dwReadSize] <= 0xA0 ){
			//C1制御コード
			bRet = C1( pbSrc+dwReadSize, &dwReadBuff );
			dwReadSize += dwReadBuff;
			if( bRet == FALSE ){
				return FALSE;
			}else if( bRet == 2 ){
				bRet = TRUE;
				break;
			}
		}else if( pbSrc[dwReadSize] > 0xA0 && pbSrc[dwReadSize] < 0xFF ){
			//GR符号領域
			if( GR( pbSrc+dwReadSize, &dwReadBuff ) == FALSE ){
				return FALSE;
			}
			dwReadSize += dwReadBuff;
		}
		else {
			*pdwReadSize = 1 ;
			bRet = TRUE ;
			break ;
		}
	}

	*pdwReadSize = dwReadSize;
	return bRet;
}

BOOL CARIB8CharDecode::C0( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	if( pbSrc == NULL || pdwReadSize == NULL ){
		return FALSE;
	}

	DWORD dwReadSize = 0;
	DWORD dwReadBuff = 0;

	BOOL bRet = TRUE;

	switch(pbSrc[0]){
	case 0x20:
		//SP 空白
		//空白は文字サイズの影響あり
		if( IsSmallCharMode() == FALSE ){
			Append( L"　" ) ;
		}else{
			Append( L' ' ) ;
		}
		dwReadSize = 1;
		break;
	case 0x0D:
		//APR 改行
		Append( L"\r\n" ) ;
		dwReadSize = 1;
		break;
	case 0x0E:
		//LS1 GLにG1セット
		m_GL = &m_G1;
		dwReadSize = 1;
		break;
	case 0x0F:
		//LS0 GLにG0セット
		m_GL = &m_G0;
		dwReadSize = 1;
		break;
	case 0x19:
		//SS2 シングルシフト
		if( SS2( pbSrc+1, &dwReadBuff ) == FALSE ){
			return FALSE;
		}
		dwReadSize = 1+dwReadBuff;
		break;
	case 0x1D:
		//SS3 シングルシフト
		if( SS3( pbSrc+1, &dwReadBuff ) == FALSE ){
			return FALSE;
		}
		dwReadSize = 1+dwReadBuff;
		break;
	case 0x1B:
		//ESC エスケープシーケンス
		if( ESC( pbSrc+1, &dwReadBuff ) == FALSE ){
			return FALSE;
		}
		dwReadSize = 1+dwReadBuff;
		break;
	default:
		//未サポートの制御コード
		if( pbSrc[0] == 0x16 ){
			//PAPF
			dwReadSize = 2;
		}else if( pbSrc[0] == 0x1C ){
			//APS
			dwReadSize = 3;
		}else if( pbSrc[0] == 0x0C ){
			//CS
			dwReadSize = 1;
			bRet = 2;
		}else{
			//APB、APF、APD、APU
			dwReadSize = 1;
		}
		break;
	}

	*pdwReadSize = dwReadSize;

	return bRet;
}

BOOL CARIB8CharDecode::C1( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	if( pbSrc == NULL || pdwReadSize == NULL ){
		return FALSE;
	}
	DWORD dwReadSize = 0;
	DWORD dwReadBuff = 0;

	BOOL bRet = TRUE;

	switch(pbSrc[0]){
	case 0x89:
		//MSZ 半角指定
		m_emStrSize = STR_MEDIUM;
		dwReadSize = 1;
		break;
	case 0x8A:
		//NSZ 全角指定
		m_emStrSize = STR_NORMAL;
		dwReadSize = 1;
		break;
	case 0x80:
		//BKF 文字黒
		dwReadSize = 1;
		break;
	case 0x81:
		//RDF 文字赤
		dwReadSize = 1;
		break;
	case 0x82:
		//GRF 文字緑
		dwReadSize = 1;
		break;
	case 0x83:
		//YLF 文字黄
		dwReadSize = 1;
		break;
	case 0x84:
		//BLF 文字青
		dwReadSize = 1;
		break;
	case 0x85:
		//MGF 文字マゼンタ
		dwReadSize = 1;
		break;
	case 0x86:
		//CNF 文字シアン
		dwReadSize = 1;
		break;
	case 0x87:
		//WHF 文字白
		dwReadSize = 1;
		break;
	case 0x88:
		//SSZ 小型サイズ
		m_emStrSize = STR_SMALL;
		dwReadSize = 1;
		break;
	case 0x8B:
		//SZX 指定サイズ
		if( pbSrc[1] == 0x60 ){
			m_emStrSize = STR_MICRO;
		}else if( pbSrc[1] == 0x41 ){
			m_emStrSize = STR_HIGH_W;
		}else if( pbSrc[1] == 0x44 ){
			m_emStrSize = STR_WIDTH_W;
		}else if( pbSrc[1] == 0x45 ){
			m_emStrSize = STR_W;
		}else if( pbSrc[1] == 0x6B ){
			m_emStrSize = STR_SPECIAL_1;
		}else if( pbSrc[1] == 0x64 ){
			m_emStrSize = STR_SPECIAL_2;
		}
		dwReadSize = 2;
		break;
	case 0x90:
		//COL 色指定
		if( pbSrc[1] == 0x20 ){
			dwReadSize = 3;
		}else{
			dwReadSize = 2;
		}
		break;
	case 0x91:
		//FLC フラッシング制御
		//未サポート
		dwReadSize = 2;
		break;
	case 0x93:
		//POL パターン極性
		//未サポート
		dwReadSize = 2;
		break;
	case 0x94:
		//WMM 書き込みモード変更
		//未サポート
		dwReadSize = 2;
		break;
	case 0x95:
		//MACRO マクロ定義
		//未サポート
		{
		DWORD dwCount = 0;
		do{
			dwCount++;
		}while( pbSrc[dwCount] != 0x4F );
		dwReadSize = dwCount;
		}
		break;
	case 0x97:
		//HLC 囲み制御
		//未サポート
		dwReadSize = 2;
		break;
	case 0x98:
		//RPC 文字繰り返し
		//未サポート
		dwReadSize = 2;
		break;
	case 0x99:
		//SPL アンダーライン モザイクの終了
		bRet = 2;
		dwReadSize = 1;
		break;
	case 0x9A:
		//STL アンダーライン モザイクの開始
		dwReadSize = 1;
		break;
	case 0x9D:
		//TIME 時間制御
		if( pbSrc[1] == 0x20 ){
			dwReadSize = 3;
		}else{
			DWORD dwCount = 0;
			do{
				dwCount++;
			}while( pbSrc[dwCount] != 0x43 && pbSrc[dwCount] != 0x40 && pbSrc[dwCount] != 0x41 && pbSrc[dwCount] != 0x42 );
			dwReadSize = dwCount;
		}
		break;
	case 0x9B:
		//CSI コントロールシーケンス
		if( CSI( pbSrc, &dwReadBuff ) == FALSE ){
			return FALSE;
		}
		dwReadSize = dwReadBuff;
	default:
		//未サポートの制御コード
		dwReadSize = 1;
		break;
	}

	*pdwReadSize = dwReadSize;

	return bRet;
}

BOOL CARIB8CharDecode::GL( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	if( pbSrc == NULL || pdwReadSize == NULL ){
		return FALSE;
	}

	DWORD dwReadSize = 0;
	if( m_GL->iMode == MF_MODE_G ){
		//文字コード
		switch( m_GL->iMF ){
			case MF_JISX_KANA:
				//JISX X0201の0x7FまではASCIIと同じ
			case MF_ASCII:
			case MF_PROP_ASCII:
				{
				if( IsSmallCharMode() == FALSE ){
					//全角なのでテーブルからUNICODE取得
					Append( AsciiTable[pbSrc[0]-0x21] ) ;
				}else{
					//半角なのでそのまま入れる
					Append( pbSrc[0] ) ;
				}
				dwReadSize = 1;
				}
				break;
			case MF_HIRA:
			case MF_PROP_HIRA:
				{
				//半角ひらがな
				//テーブルからUNICODE取得
				Append( HiraTable[pbSrc[0]-0x21] ) ;
				dwReadSize = 1;
				}
				break;
			case MF_KANA:
			case MF_PROP_KANA:
				{
				//半角カタカナ
				//テーブルからUNICODE取得
				Append( KanaTable[pbSrc[0]-0x21] ) ;
				dwReadSize = 1;
				}
				break;
			case MF_MACRO:
				//マクロ
				//PSI/SIでは未サポート
				dwReadSize = 1;
				break;
			case MF_KANJI:
			case MF_JIS_KANJI1:
			case MF_JIS_KANJI2:
			case MF_KIGOU:
				//漢字
				{
				if( ToUnicode( pbSrc[0], pbSrc[1] ) == FALSE ){
					ToCustomFont( pbSrc[0], pbSrc[1] );
				}
				dwReadSize = 2;
				}
				break;
			default:
				dwReadSize = m_GL->iByte;
				break;
		}
	}else{
		if( m_GL->iMF == MF_MACRO){
			//マクロ
			//PSI/SIでは未サポート
			dwReadSize = 1;
		}else{
			dwReadSize = m_GL->iByte;
		}
	}

	*pdwReadSize = dwReadSize;

	return TRUE;
}


BOOL CARIB8CharDecode::GR( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	if( pbSrc == NULL || pdwReadSize == NULL ){
		return FALSE;
	}

	DWORD dwReadSize = 0;
	if( m_GR->iMode == MF_MODE_G ){
		//文字コード
		switch( m_GR->iMF ){
			case MF_ASCII:
			case MF_PROP_ASCII:
				{
				if( IsSmallCharMode() == FALSE ){
					//全角なのでテーブルからUNICODE取得
					Append( AsciiTable[(pbSrc[0]&0x7F)-0x21] ) ;
				}else{
					//半角なのでそのまま入れる
					Append( char(pbSrc[0]&0x7F) ) ;
				}
				dwReadSize = 1;
				}
				break;
			case MF_HIRA:
			case MF_PROP_HIRA:
				{
				//半角ひらがな
				//テーブルからUNICODE取得
				Append( HiraTable[(pbSrc[0]&0x7F)-0x21] ) ;
				dwReadSize = 1;
				}
				break;
			case MF_JISX_KANA:
				//JIX X0201の0x80以降は半角カナ
			case MF_KANA:
			case MF_PROP_KANA:
				{
				//半角カタカナ
				//テーブルからUNICODE取得
				Append( KanaTable[(pbSrc[0]&0x7F)-0x21] ) ;
				dwReadSize = 1;
				}
				break;
			case MF_MACRO:
				//マクロ
				//PSI/SIでは未サポート
				dwReadSize = 1;
				break;
			case MF_KANJI:
			case MF_JIS_KANJI1:
			case MF_JIS_KANJI2:
			case MF_KIGOU:
				{
				//漢字
				if( ToUnicode( (pbSrc[0]&0x7F), (pbSrc[1]&0x7F) ) == FALSE ){
					ToCustomFont( (pbSrc[0]&0x7F), (pbSrc[1]&0x7F) );
				}
				dwReadSize = 2;
				}
				break;
			default:
				dwReadSize = m_GR->iByte;
				break;
		}
	}else{
		if( m_GR->iMF == MF_MACRO ){
			//マクロ
			//PSI/SIでは未サポート
			dwReadSize = 1;
		}else{
			dwReadSize = m_GR->iByte;
		}
	}

	*pdwReadSize = dwReadSize;

	return TRUE;
}

BOOL CARIB8CharDecode::ToUnicode( const BYTE bFirst, const BYTE bSecond )
{
	if( bFirst >= 0x75 && bSecond >= 0x21 ){
		return FALSE;
	}

	unsigned char ucFirst = bFirst;
	unsigned char ucSecond = bSecond;
	
	ucFirst = ucFirst - 0x21;
	if( ( ucFirst & 0x01 ) == 0 ){
		ucSecond += 0x1F;
		if( ucSecond >= 0x7F ){
			ucSecond += 0x01;
		}
	}else{
		ucSecond += 0x7E;
	}
	ucFirst = ucFirst>>1;
	if( ucFirst >= 0x1F ){
		ucFirst += 0xC1;
	}else{
		ucFirst += 0x81;
	}

	char cDec[2] ;
	cDec[0] = ucFirst;
	cDec[1] = ucSecond;
	WCHAR wc ;
	MultiByteToWideChar( 932, 0, cDec, 2, &wc, 1 ) ;
	Append( wc ) ;

	return TRUE;
}

BOOL CARIB8CharDecode::ToCustomFont( const BYTE bFirst, const BYTE bSecond )
{
	unsigned short usSrc = (unsigned short)(bFirst<<8) | bSecond;

	if( 0x7521 <= usSrc && usSrc <= 0x757E ){
		Append( GaijiTbl2[usSrc-0x7521].strChar ) ;
	}else if( 0x7621 <= usSrc && usSrc <= 0x764B ){
		Append( GaijiTbl2[usSrc-0x7621+94].strChar ) ;
	}else if( 0x7A4D <= usSrc && usSrc <= 0x7A74 ){
		Append( GaijiTable[usSrc-0x7A4D].strChar ) ;
	}else if(0x7C21 <= usSrc && usSrc <= 0x7C7B ){
		Append( GaijiTable[usSrc-0x7C21+40].strChar ) ;
	}else if(0x7D21 <= usSrc && usSrc <= 0x7D5F ){
		Append( GaijiTable[usSrc-0x7D21+131].strChar ) ;
	}else if(0x7D6E <= usSrc && usSrc <= 0x7D6F ){
		Append( GaijiTable[usSrc-0x7D6E+194].strChar ) ;
	}else if(0x7E21 <= usSrc && usSrc <= 0x7E7D ){
		Append( GaijiTable[usSrc-0x7E21+196].strChar ) ;
	}else{
		Append( L"・" ) ;
		return FALSE;
	}

	return TRUE;
}


BOOL CARIB8CharDecode::ESC( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	if( pbSrc == NULL ){
		return FALSE;
	}

	DWORD dwReadSize = 0;
	if( pbSrc[0] == 0x24 ){
		if( pbSrc[1] >= 0x28 && pbSrc[1] <= 0x2B ){
			if( pbSrc[2] == 0x20 ){
				//2バイトDRCS
				switch(pbSrc[1]){
					case 0x28:
						m_G0.iMF = pbSrc[3];
						m_G0.iMode = MF_MODE_DRCS;
						m_G0.iByte = 2;
						break;
					case 0x29:
						m_G1.iMF = pbSrc[3];
						m_G1.iMode = MF_MODE_DRCS;
						m_G1.iByte = 2;
						break;
					case 0x2A:
						m_G2.iMF = pbSrc[3];
						m_G2.iMode = MF_MODE_DRCS;
						m_G2.iByte = 2;
						break;
					case 0x2B:
						m_G3.iMF = pbSrc[3];
						m_G3.iMode = MF_MODE_DRCS;
						m_G3.iByte = 2;
						break;
					default:
						break;
				}
				dwReadSize = 4;
			}else if( pbSrc[2] == 0x28 ){
				//複数バイト、音楽符号
				switch(pbSrc[1]){
					case 0x28:
						m_G0.iMF = pbSrc[3];
						m_G0.iMode = MF_MODE_OTHER;
						m_G0.iByte = 1;
						break;
					case 0x29:
						m_G1.iMF = pbSrc[3];
						m_G1.iMode = MF_MODE_OTHER;
						m_G1.iByte = 1;
						break;
					case 0x2A:
						m_G2.iMF = pbSrc[3];
						m_G2.iMode = MF_MODE_OTHER;
						m_G2.iByte = 1;
						break;
					case 0x2B:
						m_G3.iMF = pbSrc[3];
						m_G3.iMode = MF_MODE_OTHER;
						m_G3.iByte = 1;
						break;
					default:
						break;
				}
				dwReadSize = 4;
			}else{
				//2バイトGセット
				switch(pbSrc[1]){
					case 0x29:
						m_G1.iMF = pbSrc[2];
						m_G1.iMode = MF_MODE_G;
						m_G1.iByte = 2;
						break;
					case 0x2A:
						m_G2.iMF = pbSrc[2];
						m_G2.iMode = MF_MODE_G;
						m_G2.iByte = 2;
						break;
					case 0x2B:
						m_G3.iMF = pbSrc[2];
						m_G3.iMode = MF_MODE_G;
						m_G3.iByte = 2;
						break;
					default:
						break;
				}
				dwReadSize = 3;
			}
		}else{
			//2バイトGセット
			m_G0.iMF = pbSrc[1];
			m_G0.iMode = MF_MODE_G;
			m_G0.iByte = 2;
			dwReadSize = 2;
		}
	}else if( pbSrc[0] >= 0x28 && pbSrc[0] <= 0x2B ){
		if( pbSrc[1] == 0x20 ){
			//1バイトDRCS
			switch(pbSrc[0]){
				case 0x28:
					m_G0.iMF = pbSrc[2];
					m_G0.iMode = MF_MODE_DRCS;
					m_G0.iByte = 1;
					break;
				case 0x29:
					m_G1.iMF = pbSrc[2];
					m_G1.iMode = MF_MODE_DRCS;
					m_G1.iByte = 1;
					break;
				case 0x2A:
					m_G2.iMF = pbSrc[2];
					m_G2.iMode = MF_MODE_DRCS;
					m_G2.iByte = 1;
					break;
				case 0x2B:
					m_G3.iMF = pbSrc[2];
					m_G3.iMode = MF_MODE_DRCS;
					m_G3.iByte = 1;
					break;
				default:
					break;
			}
			dwReadSize = 3;
		}else{
			//1バイトGセット
			switch(pbSrc[0]){
				case 0x28:
					m_G0.iMF = pbSrc[1];
					m_G0.iMode = MF_MODE_G;
					m_G0.iByte = 1;
					break;
				case 0x29:
					m_G1.iMF = pbSrc[1];
					m_G1.iMode = MF_MODE_G;
					m_G1.iByte = 1;
					break;
				case 0x2A:
					m_G2.iMF = pbSrc[1];
					m_G2.iMode = MF_MODE_G;
					m_G2.iByte = 1;
					break;
				case 0x2B:
					m_G3.iMF = pbSrc[1];
					m_G3.iMode = MF_MODE_G;
					m_G3.iByte = 1;
					break;
				default:
					break;
			}
			dwReadSize = 2;
		}
	}else if( pbSrc[0] == 0x6E ){
		//GLにG2セット
		m_GL = &m_G2;
		dwReadSize = 1;
	}else if( pbSrc[0] == 0x6F ){
		//GLにG3セット
		m_GL = &m_G3;
		dwReadSize = 1;
	}else if( pbSrc[0] == 0x7C ){
		//GRにG3セット
		m_GR = &m_G3;
		dwReadSize = 1;
	}else if( pbSrc[0] == 0x7D ){
		//GRにG2セット
		m_GR = &m_G2;
		dwReadSize = 1;
	}else if( pbSrc[0] == 0x7E ){
		//GRにG1セット
		m_GR = &m_G1;
		dwReadSize = 1;
	}else{
		//未サポート
		dwReadSize = 1;
		*pdwReadSize = dwReadSize;
		return FALSE;
	}

	*pdwReadSize = dwReadSize;

	return TRUE;
}

BOOL CARIB8CharDecode::SS2( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	MF_MODE* LastG;
	LastG = m_GL;
	//一時的に入れ替える
	m_GL = &m_G2;
	//GL符号領域
	BOOL bRet = GL( pbSrc, pdwReadSize );
	if( bRet != FALSE ){
		//元に戻す
		m_GL = LastG;
		return bRet;
	}
	//元に戻す
	m_GL = LastG;
	return TRUE;
}

BOOL CARIB8CharDecode::SS3( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	MF_MODE* LastG;
	LastG = m_GL;
	//一時的に入れ替える
	m_GL = &m_G3;
	//GL符号領域
	BOOL bRet = GL( pbSrc, pdwReadSize );
	if( bRet != FALSE ){
		//元に戻す
		m_GL = LastG;
		return bRet;
	}
	//元に戻す
	m_GL = LastG;
	return TRUE;
}

BOOL CARIB8CharDecode::CSI( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	if( pbSrc == NULL || pdwReadSize == NULL ){
		return FALSE;
	}
	DWORD dwReadSize = 0;

	do{
		dwReadSize++;
	}while( pbSrc[dwReadSize] != 0x20 );
	dwReadSize++;
	
	switch(pbSrc[dwReadSize]){
		case 0x53:
			//SWF
			//未サポート
			break;
		case 0x6E:
			//RCS
			//未サポート
			break;
		case 0x61:
			//ACPS
			//未サポート
			break;
		case 0x56:
			//SDF
			//未サポート
			break;
		case 0x5F:
			//SDP
			//未サポート
			break;
		case 0x57:
			//SSM
			//未サポート
			break;
		case 0x58:
			//SHS
			//未サポート
			break;
		case 0x59:
			//SVS
			//未サポート
			break;
		case 0x42:
			//GSM
			//未サポート
			break;
		case 0x5D:
			//GAA
			//未サポート
			break;
		case 0x5E:
			//SRC
			//未サポート
			break;
		case 0x62:
			//TCC
			//未サポート
			break;
		case 0x65:
			//CFS
			//未サポート
			break;
		case 0x63:
			//ORN
			//未サポート
			break;
		case 0x64:
			//MDF
			//未サポート
			break;
		case 0x66:
			//XCS
			//未サポート
			break;
		case 0x68:
			//PRA
			//未サポート
			break;
		case 0x54:
			//CCC
			//未サポート
			break;
		case 0x67:
			//SCR
			//未サポート
			break;
		default:
			break;
	}
	dwReadSize++;

	*pdwReadSize = dwReadSize;

	return TRUE;
}

BOOL ARIB8toUNICODE( const BYTE* pbSrc, DWORD dwSrcSize, WCHAR* pwszDst, DWORD dwDstSize, DWORD* pdwSize )
{
	CARIB8CharDecode decoder ;
	return decoder.PSISI( pbSrc, dwSrcSize, pwszDst, dwDstSize, pdwSize ) ;
}

