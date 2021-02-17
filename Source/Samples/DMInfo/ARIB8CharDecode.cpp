///////////////////////////////////////
//
//  ARIB8->UNICODE
//
// ==== ���p������ ====
// ���̃t�@�C����EpgDataCap_Bon(�l����9.46)�̃R�[�h�𗘗p���Ă���A
// ������ė��p����ꍇ�́A�ȉ���EpgDataCap_Bon�̗��p������
// ���������ė��p���Ă��������B
// <<<< EpgDataCap_Bon��Readme.txt���]�� >>>>
// ���\�[�X�isrc.zip�j�̎�舵���ɂ���
// �@����GPL�Ƃ��ɂ͂��Ȃ��̂Ńt���[�\�t�g�Ɍ����Ă͎��R�ɉ��ς��Ă������
// �@��g�ݍ���ł�����č\��Ȃ��ł��B
// �@���ς�����g�ݍ��񂾂肵�Č��J����ꍇ�͊Y�������̃\�[�X���炢�͈ꏏ
// �@�Ɍ��J���Ă��������B�i�����ł͂Ȃ��̂ŕʂɌ��J���Ȃ��Ă������ł��j
// �@���p�A�V�F�A�E�F�A�Ȃǂɑg�ݍ��ނ͕̂s�ł��B
//
///////////////////////////////////////

#include <windows.h>

#include "ARIB8CharDecode.h"

//���������W��
//G�Z�b�g
#define MF_JIS_KANJI1 0x39 //JIS�݊�����1��
#define MF_JIS_KANJI2 0x3A //JIS�݊�����2��
#define MF_KIGOU 0x3B //�ǉ��L��
#define MF_ASCII 0x4A //�p��
#define MF_HIRA  0x30 //������
#define MF_KANA  0x31 //�Љ���
#define MF_KANJI 0x42 //����
#define MF_MOSAIC_A 0x32 //���U�C�NA
#define MF_MOSAIC_B 0x33 //���U�C�NB
#define MF_MOSAIC_C 0x34 //���U�C�NC
#define MF_MOSAIC_D 0x35 //���U�C�ND
#define MF_PROP_ASCII 0x36 //�v���|�[�V���i���p��
#define MF_PROP_HIRA  0x37 //�v���|�[�V���i��������
#define MF_PROP_KANA  0x38 //�v���|�[�V���i���Љ���
#define MF_JISX_KANA 0x49 //JIX X0201�Љ���
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
#define MF_MACRO 0x70 //�}�N��

//�����W���̕���
#define MF_MODE_G 1 //G�Z�b�g
#define MF_MODE_DRCS 2 //DRCS
#define MF_MODE_OTHER 3 //���̑�

const static WCHAR AsciiTable[][3]={
	L"�I",L"�h",L"��",L"��",L"��",L"��",L"�f",
	L"�i",L"�j",L"��",L"�{",L"�C",L"�|",L"�D",L"�^",
	L"�O",L"�P",L"�Q",L"�R",L"�S",L"�T",L"�U",L"�V",
	L"�W",L"�X",L"�F",L"�G",L"��",L"��",L"��",L"�H",
	L"��",L"�`",L"�a",L"�b",L"�c",L"�d",L"�e",L"�f",
	L"�g",L"�h",L"�i",L"�j",L"�k",L"�l",L"�m",L"�n",
	L"�o",L"�p",L"�q",L"�r",L"�s",L"�t",L"�u",L"�v",
	L"�w",L"�x",L"�y",L"�m",L"��",L"�n",L"�O",L"�Q",
	L"�e",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"�o",L"�b",L"�p",L"�P"
};
const static WCHAR HiraTable[][3]={
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"�@",L"�@",L"�@",L"�T",
	L"�U",L"�[",L"�B",L"�u",L"�v",L"�A",L"�E"
};
const static WCHAR KanaTable[][3]={
	L"�@",L"�A",L"�B",L"�C",L"�D",L"�E",L"�F",
	L"�G",L"�H",L"�I",L"�J",L"�K",L"�L",L"�M",L"�N",
	L"�O",L"�P",L"�Q",L"�R",L"�S",L"�T",L"�U",L"�V",
	L"�W",L"�X",L"�Y",L"�Z",L"�[",L"�\",L"�]",L"�^",
	L"�_",L"�`",L"�a",L"�b",L"�c",L"�d",L"�e",L"�f",
	L"�g",L"�h",L"�i",L"�j",L"�k",L"�l",L"�m",L"�n",
	L"�o",L"�p",L"�q",L"�r",L"�s",L"�t",L"�u",L"�v",
	L"�w",L"�x",L"�y",L"�z",L"�{",L"�|",L"�}",L"�~",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"��",
	L"��",L"��",L"��",L"��",L"��",L"��",L"��",L"�R",
	L"�S",L"�[",L"�B",L"�u",L"�v",L"�A",L"�E"
};

typedef struct _GAIJI_TABLE{
	unsigned short usARIB8;
	const WCHAR* strChar;
} GAIJI_TABLE;

const static GAIJI_TABLE GaijiTable[]={
	{0x7A4D, L"10."},
	{0x7A4E, L"11."},
	{0x7A4F, L"12."},
	{0x7A50, L"[HV]"}, //90��48�_
	{0x7A51, L"[SD]"},
	{0x7A52, L"[�o]"},
	{0x7A53, L"[�v]"},
	{0x7A54, L"[MV]"},
	{0x7A55, L"[��]"},
	{0x7A56, L"[��]"},
	{0x7A57, L"[�o]"},
	{0x7A58, L"[�f]"},
	{0x7A59, L"[�r]"},
	{0x7A5A, L"[��]"},
	{0x7A5B, L"[��]"},
	{0x7A5C, L"[��]"},
	{0x7A5D, L"[SS]"},
	{0x7A5E, L"[�a]"},
	{0x7A5F, L"[�m]"},//
	{0x7A60, L"��"},//90��64�_
	{0x7A61, L"��"},
	{0x7A62, L"[�V]"},
	{0x7A63, L"[��]"},
	{0x7A64, L"[�f]"},
	{0x7A65, L"[��]"},
	{0x7A66, L"[��]"},
	{0x7A67, L"[�E]"},
	{0x7A68, L"[�O]"},
	{0x7A69, L"[��]"},
	{0x7A6A, L"[��]"},
	{0x7A6B, L"[�V]"},
	{0x7A6C, L"[��]"},
	{0x7A6D, L"[�I]"},
	{0x7A6E, L"[��]"},
	{0x7A6F, L"[��]"},
	{0x7A70, L"[��]"},//90��80�_
	{0x7A71, L"[��]"},
	{0x7A72, L"[PPV]"},
	{0x7A73, L"(��)"},
	{0x7A74, L"�ق�"},
	//91��͔�΂�
	{0x7C21, L"��"},//92��1�_
	{0x7C22, L"��"},
	{0x7C23, L"��"},
	{0x7C24, L"��"},
	{0x7C25, L"�E"},
	{0x7C26, L"�E"},
	{0x7C27, L"�N"},
	{0x7C28, L"��"},
	{0x7C29, L"��"},
	{0x7C2A, L"�~"},
	{0x7C2B, L"m^2"},
	{0x7C2C, L"m^3"},
	{0x7C2D, L"cm"},
	{0x7C2E, L"cm^2"},
	{0x7C2F, L"cm^3"},
	{0x7C30, L"�O."},//92��16�_
	{0x7C31, L"�P."},
	{0x7C32, L"�Q."},
	{0x7C33, L"�R."},
	{0x7C34, L"�S."},
	{0x7C35, L"�T."},
	{0x7C36, L"�U."},
	{0x7C37, L"�V."},
	{0x7C38, L"�W."},
	{0x7C39, L"�X."},
	{0x7C3A, L"��"},
	{0x7C3B, L"��"},
	{0x7C3C, L"��"},
	{0x7C3D, L"��"},
	{0x7C3E, L"�O"},
	{0x7C3F, L"��"},
	{0x7C40, L"�O,"},//92��32�_
	{0x7C41, L"�P,"},
	{0x7C42, L"�Q,"},
	{0x7C43, L"�R,"},
	{0x7C44, L"�S,"},
	{0x7C45, L"�T,"},
	{0x7C46, L"�U,"},
	{0x7C47, L"�V,"},
	{0x7C48, L"�W,"},
	{0x7C49, L"�X,"},
	{0x7C4A, L"[��]"},
	{0x7C4B, L"[��]"},
	{0x7C4C, L"[�L]"},
	{0x7C4D, L"[��]"},
	{0x7C4E, L"[��]"},
	{0x7C4F, L"(��)"},
	{0x7C50, L"�E"},//92��48�_
	{0x7C51, L"�E"},
	{0x7C52, L"�E"},
	{0x7C53, L"�E"},
	{0x7C54, L"�E"},
	{0x7C55, L"�E"},
	{0x7C56, L"�E"},
	{0x7C57, L"(CD)"},
	{0x7C58, L"(vn)"},
	{0x7C59, L"(ob)"},
	{0x7C5A, L"(cb)"},
	{0x7C5B, L"(ce"},
	{0x7C5C, L"mb)"},
	{0x7C5D, L"(hp)"},
	{0x7C5E, L"(br)"},
	{0x7C5F, L"(��)"},
	{0x7C60, L"(��)"},//92��64�_
	{0x7C61, L"(ms)"},
	{0x7C62, L"(��)"},
	{0x7C63, L"(bs)"},
	{0x7C64, L"(��)"},
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
	{0x7C70, L"(sy"},//92��80�_
	{0x7C71, L"n)"},
	{0x7C72, L"(or"},
	{0x7C73, L"g)"},
	{0x7C74, L"(pe"},
	{0x7C75, L"r)"},
	{0x7C76, L"(�q)"},
	{0x7C77, L"(�b)"},
	{0x7C78, L"(�)"},
	{0x7C79, L"�c�i"},
	{0x7C7A, L"[��]"},
	{0x7C7B, L"Fax"},
	{0x7D21, L"(��)"},//93��1�_
	{0x7D22, L"(��)"},
	{0x7D23, L"(��)"},
	{0x7D24, L"(��)"},
	{0x7D25, L"(��)"},
	{0x7D26, L"(�y)"},
	{0x7D27, L"(��)"},
	{0x7D28, L"(�j)"},
	{0x7D29, L"��"},
	{0x7D2A, L"��"},
	{0x7D2B, L"��"},
	{0x7D2C, L"�~"},
	{0x7D2D, L"No."},
	{0x7D2E, L"Tel"},
	{0x7D2F, L"(��)"},
	{0x7D30, L"()()"},//93��16�_
	{0x7D31, L"[�{]"},
	{0x7D32, L"[�O]"},
	{0x7D33, L"[��]"},
	{0x7D34, L"[��]"},
	{0x7D35, L"[�_]"},
	{0x7D36, L"[��]"},
	{0x7D37, L"[��]"},
	{0x7D38, L"[��]"},
	{0x7D39, L"[�s]"},
	{0x7D3A, L"[�r]"},
	{0x7D3B, L"[��]"},
	{0x7D3C, L"[��]"},
	{0x7D3D, L"[��]"},
	{0x7D3E, L"[��]"},
	{0x7D3F, L"[�O]"},
	{0x7D40, L"[�V]"},//93��32�_
	{0x7D41, L"[��]"},
	{0x7D42, L"[��]"},
	{0x7D43, L"[�E]"},
	{0x7D44, L"[�w]"},
	{0x7D45, L"[��]"},
	{0x7D46, L"[��]"},
	{0x7D47, L"l"},
	{0x7D48, L"kg"},
	{0x7D49, L"Hz"},
	{0x7D4A, L"ha"},
	{0x7D4B, L"km"},
	{0x7D4C, L"km^2"},
	{0x7D4D, L"hPa"},
	{0x7D4E, L"�E"},
	{0x7D4F, L"�E"},
	{0x7D50, L"1/2"},//93��48�_
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
	{0x7D6E, L"!!"},//93��78�_
	{0x7D6F, L"!?"},
	{0x7E21, L"�T"},//94��1�_
	{0x7E22, L"�U"},
	{0x7E23, L"�V"},
	{0x7E24, L"�W"},
	{0x7E25, L"�X"},
	{0x7E26, L"�Y"},
	{0x7E27, L"�Z"},
	{0x7E28, L"�["},
	{0x7E29, L"�\"},
	{0x7E2A, L"�]"},
	{0x7E2B, L"XI"},
	{0x7E2C, L"XII"},
	{0x7E2D, L"�P"},
	{0x7E2E, L"�Q"},
	{0x7E2F, L"�R"},
	{0x7E30, L"�S"},//94��16�_
	{0x7E31, L"(�P)"},
	{0x7E32, L"(�Q)"},
	{0x7E33, L"(�R)"},
	{0x7E34, L"(�S)"},
	{0x7E35, L"(�T)"},
	{0x7E36, L"(�U)"},
	{0x7E37, L"(�V)"},
	{0x7E38, L"(�W)"},
	{0x7E39, L"(�X)"},
	{0x7E3A, L"(10)"},
	{0x7E3B, L"(11)"},
	{0x7E3C, L"(12)"},
	{0x7E3D, L"(21)"},
	{0x7E3E, L"(22)"},
	{0x7E3F, L"(23)"},
	{0x7E40, L"(24)"},//94��32�_
	{0x7E41, L"(�`)"},
	{0x7E42, L"(�a)"},
	{0x7E43, L"(�b)"},
	{0x7E44, L"(�c)"},
	{0x7E45, L"(�d)"},
	{0x7E46, L"(�e)"},
	{0x7E47, L"(�f)"},
	{0x7E48, L"(�g)"},
	{0x7E49, L"(�h)"},
	{0x7E4A, L"(�i)"},
	{0x7E4B, L"(�j)"},
	{0x7E4C, L"(�k)"},
	{0x7E4D, L"(�l)"},
	{0x7E4E, L"(�m)"},
	{0x7E4F, L"(�n)"},
	{0x7E50, L"(�o)"},//94��48�_
	{0x7E51, L"(�p)"},
	{0x7E52, L"(�q)"},
	{0x7E53, L"(�r)"},
	{0x7E54, L"(�s)"},
	{0x7E55, L"(�t)"},
	{0x7E56, L"(�u)"},
	{0x7E57, L"(�v)"},
	{0x7E58, L"(�w)"},
	{0x7E59, L"(�x)"},
	{0x7E5A, L"(�y)"},
	{0x7E5B, L"(25)"},
	{0x7E5C, L"(26)"},
	{0x7E5D, L"(27)"},
	{0x7E5E, L"(28)"},
	{0x7E5F, L"(29)"},
	{0x7E60, L"(30)"},//94��64�_
	{0x7E61, L"�@"},
	{0x7E62, L"�A"},
	{0x7E63, L"�B"},
	{0x7E64, L"�C"},
	{0x7E65, L"�D"},
	{0x7E66, L"�E"},
	{0x7E67, L"�F"},
	{0x7E68, L"�G"},
	{0x7E69, L"�H"},
	{0x7E6A, L"�I"},
	{0x7E6B, L"�J"},
	{0x7E6C, L"�K"},
	{0x7E6D, L"�L"},
	{0x7E6E, L"�M"},
	{0x7E6F, L"�N"},
	{0x7E70, L"�O"},//94��80�_
	{0x7E71, L"(�P)"},
	{0x7E72, L"(�Q)"},
	{0x7E73, L"(�R)"},
	{0x7E74, L"(�S)"},
	{0x7E75, L"(�T)"},
	{0x7E76, L"(�U)"},
	{0x7E77, L"(�V)"},
	{0x7E78, L"(�W)"},
	{0x7E79, L"(�X)"},
	{0x7E7A, L"(10)"},
	{0x7E7B, L"(11)"},
	{0x7E7C, L"(12)"},
	{0x7E7D, L"(31)"}
};

const static GAIJI_TABLE GaijiTbl2[]={
	{0x7521, L"��"},
	{0x7522, L"��"},
	{0x7523, L"��"},
	{0x7524, L"��"},
	{0x7525, L"�q"},
	{0x7526, L"�a"},
	{0x7527, L"��"},
	{0x7528, L"��"},
	{0x7529, L"��"},
	{0x752A, L"��"}, //10
	{0x752B, L"��"},
	{0x752C, L"��"},
	{0x752D, L"��"},
	{0x752E, L"��"},
	{0x752F, L"��"},
	{0x7530, L"��"},
	{0x7531, L"��"},
	{0x7532, L"��"},
	{0x7533, L"��"},
	{0x7534, L"��"}, //20
	{0x7535, L"��"},
	{0x7536, L"��"},
	{0x7537, L"��"},
	{0x7538, L"��"},
	{0x7539, L"��"},
	{0x753A, L"��"},
	{0x753B, L"��"},
	{0x753C, L"��"},
	{0x753D, L"��"},
	{0x753E, L"��"}, //30
	{0x753F, L"��"},
	{0x7540, L"��"},
	{0x7541, L"��"},
	{0x7542, L"��"},
	{0x7543, L"��"},
	{0x7544, L"�g"},
	{0x7545, L"��"},
	{0x7546, L"��"},
	{0x7547, L"��"},
	{0x7548, L"��"}, //40
	{0x7549, L"��"},
	{0x754A, L"��"},
	{0x754B, L"��"},
	{0x754C, L"��"},
	{0x754D, L"�f"},
	{0x754E, L"��"},
	{0x754F, L"��"},
	{0x7550, L"��"},
	{0x7551, L"��"},
	{0x7552, L"��"}, //50
	{0x7553, L"��"},
	{0x7554, L"��"},
	{0x7555, L"��"},
	{0x7556, L"��"},
	{0x7557, L"��"},
	{0x7558, L"��"},
	{0x7559, L"��"},
	{0x755A, L"��"},
	{0x755B, L"��"},
	{0x755C, L"��"}, //60
	{0x755D, L"��"},
	{0x755E, L"��"},
	{0x755F, L"��"},
	{0x7560, L"��"},
	{0x7561, L"��"},
	{0x7562, L"��"},
	{0x7563, L"��"},
	{0x7564, L"��"},
	{0x7565, L"�W"},
	{0x7566, L"�Y"}, //70
	{0x7567, L"��"},
	{0x7568, L"��"},
	{0x7569, L"��"},
	{0x756A, L"�a"},
	{0x756B, L"�b"},
	{0x756C, L"��"},
	{0x756D, L"��"},
	{0x756E, L"��"},
	{0x756F, L"�g"},
	{0x7570, L"�h"}, //80
	{0x7571, L"��"},
	{0x7572, L"��"},
	{0x7573, L"��"},
	{0x7574, L"��"},
	{0x7575, L"��"},
	{0x7576, L"��"},
	{0x7577, L"��"},
	{0x7578, L"��"},
	{0x7579, L"��"},
	{0x757A, L"��"}, //90
	{0x757B, L"�_"},
	{0x757C, L"�X"},
	{0x757D, L"��"},
	{0x757E, L"��"},
	{0x7621, L"��"},
	{0x7622, L"��"},
	{0x7623, L"��"},
	{0x7624, L"��"},
	{0x7625, L"��"},
	{0x7626, L"��"}, //100
	{0x7627, L"��"},
	{0x7628, L"��"},
	{0x7629, L"��"},
	{0x762A, L"��"},
	{0x762B, L"��"},
	{0x762C, L"��"},
	{0x762D, L"��"},
	{0x762E, L"�`"},
	{0x762F, L"�H"},
	{0x7630, L"��"}, //110
	{0x7631, L"��"},
	{0x7632, L"�I"},
	{0x7633, L"��"},
	{0x7634, L"��"},
	{0x7635, L"��"},
	{0x7636, L"�p"},
	{0x7637, L"��"},
	{0x7638, L"��"},
	{0x7639, L"��"},
	{0x763A, L"��"}, //120
	{0x763B, L"��"},
	{0x763C, L"��"},
	{0x763D, L"�A"},
	{0x763E, L"��"},
	{0x763F, L"��"},
	{0x7640, L"�_"},
	{0x7641, L"��"},
	{0x7642, L"�^"},
	{0x7643, L"��"},
	{0x7644, L"��"}, //130
	{0x7645, L"�L"},
	{0x7646, L"��"},
	{0x7647, L"��"},
	{0x7648, L"�I"},
	{0x7649, L"��"},
	{0x764A, L"��"},
	{0x764B, L"��"}
};

//�����T�C�Y
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

	//PSI/SI��z�肵��UNICODE�ւ̕ϊ�
	BOOL PSISI( const BYTE* pbSrc, DWORD dwSrcSize, WCHAR* pwszDst, DWORD dwDstSize, DWORD* pdwSize ) ;

	//�f�R�[�h����������
protected:
	WCHAR* m_pwszBuf ;
	DWORD m_dwBufSize ;
	DWORD m_dwCnt ;
	DWORD* m_pdwSize ;
	void Append( const WCHAR* pwsz ) ;
	void Append( WCHAR wc ) ;

protected:
	typedef struct _MF_MODE{
		int iMF; //���������W��
		int iMode; //�����W���̕���
		int iByte; //�ǂݍ��݃o�C�g��
		//=�I�y���[�^�[�̏���
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

	//�����T�C�Y
	STRING_SIZE m_emStrSize;
protected:
	void InitPSISI(void);
	BOOL Analyze( const BYTE* pbSrc, DWORD dwSrcSize, DWORD* pdwReadSize );

	BOOL IsSmallCharMode(void);

	//���䕄��
	BOOL C0( const BYTE* pbSrc, DWORD* pdwReadSize );
	BOOL C1( const BYTE* pbSrc, DWORD* pdwReadSize );
	BOOL GL( const BYTE* pbSrc, DWORD* pdwReadSize );
	BOOL GR( const BYTE* pbSrc, DWORD* pdwReadSize );
	//�V���O���V�t�g
	BOOL SS2( const BYTE* pbSrc, DWORD* pdwReadSize );
	BOOL SS3( const BYTE* pbSrc, DWORD* pdwReadSize );
	//�G�X�P�[�v�V�[�P���X
	BOOL ESC( const BYTE* pbSrc, DWORD* pdwReadSize );
	//�Q�o�C�g�����ϊ�
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
		//1�o�C�g�ڃ`�F�b�N
		if( pbSrc[dwReadSize] <= 0x20 ){
			//C0����R�[�h
			bRet = C0( pbSrc+dwReadSize, &dwReadBuff );
			dwReadSize += dwReadBuff;
			if( bRet == FALSE ){
				return FALSE;
			}else if( bRet == 2 ){
				bRet = TRUE;
				break;
			}
		}else if( pbSrc[dwReadSize] > 0x20 && pbSrc[dwReadSize] < 0x7F ){
			//GL�����̈�
			if( GL( pbSrc+dwReadSize, &dwReadBuff ) == FALSE ){
				return FALSE;
			}
			dwReadSize += dwReadBuff;
		}else if( pbSrc[dwReadSize] >= 0x7F && pbSrc[dwReadSize] <= 0xA0 ){
			//C1����R�[�h
			bRet = C1( pbSrc+dwReadSize, &dwReadBuff );
			dwReadSize += dwReadBuff;
			if( bRet == FALSE ){
				return FALSE;
			}else if( bRet == 2 ){
				bRet = TRUE;
				break;
			}
		}else if( pbSrc[dwReadSize] > 0xA0 && pbSrc[dwReadSize] < 0xFF ){
			//GR�����̈�
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
		//SP ��
		//�󔒂͕����T�C�Y�̉e������
		if( IsSmallCharMode() == FALSE ){
			Append( L"�@" ) ;
		}else{
			Append( L' ' ) ;
		}
		dwReadSize = 1;
		break;
	case 0x0D:
		//APR ���s
		Append( L"\r\n" ) ;
		dwReadSize = 1;
		break;
	case 0x0E:
		//LS1 GL��G1�Z�b�g
		m_GL = &m_G1;
		dwReadSize = 1;
		break;
	case 0x0F:
		//LS0 GL��G0�Z�b�g
		m_GL = &m_G0;
		dwReadSize = 1;
		break;
	case 0x19:
		//SS2 �V���O���V�t�g
		if( SS2( pbSrc+1, &dwReadBuff ) == FALSE ){
			return FALSE;
		}
		dwReadSize = 1+dwReadBuff;
		break;
	case 0x1D:
		//SS3 �V���O���V�t�g
		if( SS3( pbSrc+1, &dwReadBuff ) == FALSE ){
			return FALSE;
		}
		dwReadSize = 1+dwReadBuff;
		break;
	case 0x1B:
		//ESC �G�X�P�[�v�V�[�P���X
		if( ESC( pbSrc+1, &dwReadBuff ) == FALSE ){
			return FALSE;
		}
		dwReadSize = 1+dwReadBuff;
		break;
	default:
		//���T�|�[�g�̐���R�[�h
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
			//APB�AAPF�AAPD�AAPU
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
		//MSZ ���p�w��
		m_emStrSize = STR_MEDIUM;
		dwReadSize = 1;
		break;
	case 0x8A:
		//NSZ �S�p�w��
		m_emStrSize = STR_NORMAL;
		dwReadSize = 1;
		break;
	case 0x80:
		//BKF ������
		dwReadSize = 1;
		break;
	case 0x81:
		//RDF ������
		dwReadSize = 1;
		break;
	case 0x82:
		//GRF ������
		dwReadSize = 1;
		break;
	case 0x83:
		//YLF ������
		dwReadSize = 1;
		break;
	case 0x84:
		//BLF ������
		dwReadSize = 1;
		break;
	case 0x85:
		//MGF �����}�[���^
		dwReadSize = 1;
		break;
	case 0x86:
		//CNF �����V�A��
		dwReadSize = 1;
		break;
	case 0x87:
		//WHF ������
		dwReadSize = 1;
		break;
	case 0x88:
		//SSZ ���^�T�C�Y
		m_emStrSize = STR_SMALL;
		dwReadSize = 1;
		break;
	case 0x8B:
		//SZX �w��T�C�Y
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
		//COL �F�w��
		if( pbSrc[1] == 0x20 ){
			dwReadSize = 3;
		}else{
			dwReadSize = 2;
		}
		break;
	case 0x91:
		//FLC �t���b�V���O����
		//���T�|�[�g
		dwReadSize = 2;
		break;
	case 0x93:
		//POL �p�^�[���ɐ�
		//���T�|�[�g
		dwReadSize = 2;
		break;
	case 0x94:
		//WMM �������݃��[�h�ύX
		//���T�|�[�g
		dwReadSize = 2;
		break;
	case 0x95:
		//MACRO �}�N����`
		//���T�|�[�g
		{
		DWORD dwCount = 0;
		do{
			dwCount++;
		}while( pbSrc[dwCount] != 0x4F );
		dwReadSize = dwCount;
		}
		break;
	case 0x97:
		//HLC �͂ݐ���
		//���T�|�[�g
		dwReadSize = 2;
		break;
	case 0x98:
		//RPC �����J��Ԃ�
		//���T�|�[�g
		dwReadSize = 2;
		break;
	case 0x99:
		//SPL �A���_�[���C�� ���U�C�N�̏I��
		bRet = 2;
		dwReadSize = 1;
		break;
	case 0x9A:
		//STL �A���_�[���C�� ���U�C�N�̊J�n
		dwReadSize = 1;
		break;
	case 0x9D:
		//TIME ���Ԑ���
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
		//CSI �R���g���[���V�[�P���X
		if( CSI( pbSrc, &dwReadBuff ) == FALSE ){
			return FALSE;
		}
		dwReadSize = dwReadBuff;
	default:
		//���T�|�[�g�̐���R�[�h
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
		//�����R�[�h
		switch( m_GL->iMF ){
			case MF_JISX_KANA:
				//JISX X0201��0x7F�܂ł�ASCII�Ɠ���
			case MF_ASCII:
			case MF_PROP_ASCII:
				{
				if( IsSmallCharMode() == FALSE ){
					//�S�p�Ȃ̂Ńe�[�u������UNICODE�擾
					Append( AsciiTable[pbSrc[0]-0x21] ) ;
				}else{
					//���p�Ȃ̂ł��̂܂ܓ����
					Append( pbSrc[0] ) ;
				}
				dwReadSize = 1;
				}
				break;
			case MF_HIRA:
			case MF_PROP_HIRA:
				{
				//���p�Ђ炪��
				//�e�[�u������UNICODE�擾
				Append( HiraTable[pbSrc[0]-0x21] ) ;
				dwReadSize = 1;
				}
				break;
			case MF_KANA:
			case MF_PROP_KANA:
				{
				//���p�J�^�J�i
				//�e�[�u������UNICODE�擾
				Append( KanaTable[pbSrc[0]-0x21] ) ;
				dwReadSize = 1;
				}
				break;
			case MF_MACRO:
				//�}�N��
				//PSI/SI�ł͖��T�|�[�g
				dwReadSize = 1;
				break;
			case MF_KANJI:
			case MF_JIS_KANJI1:
			case MF_JIS_KANJI2:
			case MF_KIGOU:
				//����
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
			//�}�N��
			//PSI/SI�ł͖��T�|�[�g
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
		//�����R�[�h
		switch( m_GR->iMF ){
			case MF_ASCII:
			case MF_PROP_ASCII:
				{
				if( IsSmallCharMode() == FALSE ){
					//�S�p�Ȃ̂Ńe�[�u������UNICODE�擾
					Append( AsciiTable[(pbSrc[0]&0x7F)-0x21] ) ;
				}else{
					//���p�Ȃ̂ł��̂܂ܓ����
					Append( char(pbSrc[0]&0x7F) ) ;
				}
				dwReadSize = 1;
				}
				break;
			case MF_HIRA:
			case MF_PROP_HIRA:
				{
				//���p�Ђ炪��
				//�e�[�u������UNICODE�擾
				Append( HiraTable[(pbSrc[0]&0x7F)-0x21] ) ;
				dwReadSize = 1;
				}
				break;
			case MF_JISX_KANA:
				//JIX X0201��0x80�ȍ~�͔��p�J�i
			case MF_KANA:
			case MF_PROP_KANA:
				{
				//���p�J�^�J�i
				//�e�[�u������UNICODE�擾
				Append( KanaTable[(pbSrc[0]&0x7F)-0x21] ) ;
				dwReadSize = 1;
				}
				break;
			case MF_MACRO:
				//�}�N��
				//PSI/SI�ł͖��T�|�[�g
				dwReadSize = 1;
				break;
			case MF_KANJI:
			case MF_JIS_KANJI1:
			case MF_JIS_KANJI2:
			case MF_KIGOU:
				{
				//����
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
			//�}�N��
			//PSI/SI�ł͖��T�|�[�g
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
		Append( L"�E" ) ;
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
				//2�o�C�gDRCS
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
				//�����o�C�g�A���y����
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
				//2�o�C�gG�Z�b�g
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
			//2�o�C�gG�Z�b�g
			m_G0.iMF = pbSrc[1];
			m_G0.iMode = MF_MODE_G;
			m_G0.iByte = 2;
			dwReadSize = 2;
		}
	}else if( pbSrc[0] >= 0x28 && pbSrc[0] <= 0x2B ){
		if( pbSrc[1] == 0x20 ){
			//1�o�C�gDRCS
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
			//1�o�C�gG�Z�b�g
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
		//GL��G2�Z�b�g
		m_GL = &m_G2;
		dwReadSize = 1;
	}else if( pbSrc[0] == 0x6F ){
		//GL��G3�Z�b�g
		m_GL = &m_G3;
		dwReadSize = 1;
	}else if( pbSrc[0] == 0x7C ){
		//GR��G3�Z�b�g
		m_GR = &m_G3;
		dwReadSize = 1;
	}else if( pbSrc[0] == 0x7D ){
		//GR��G2�Z�b�g
		m_GR = &m_G2;
		dwReadSize = 1;
	}else if( pbSrc[0] == 0x7E ){
		//GR��G1�Z�b�g
		m_GR = &m_G1;
		dwReadSize = 1;
	}else{
		//���T�|�[�g
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
	//�ꎞ�I�ɓ���ւ���
	m_GL = &m_G2;
	//GL�����̈�
	BOOL bRet = GL( pbSrc, pdwReadSize );
	if( bRet != FALSE ){
		//���ɖ߂�
		m_GL = LastG;
		return bRet;
	}
	//���ɖ߂�
	m_GL = LastG;
	return TRUE;
}

BOOL CARIB8CharDecode::SS3( const BYTE* pbSrc, DWORD* pdwReadSize )
{
	MF_MODE* LastG;
	LastG = m_GL;
	//�ꎞ�I�ɓ���ւ���
	m_GL = &m_G3;
	//GL�����̈�
	BOOL bRet = GL( pbSrc, pdwReadSize );
	if( bRet != FALSE ){
		//���ɖ߂�
		m_GL = LastG;
		return bRet;
	}
	//���ɖ߂�
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
			//���T�|�[�g
			break;
		case 0x6E:
			//RCS
			//���T�|�[�g
			break;
		case 0x61:
			//ACPS
			//���T�|�[�g
			break;
		case 0x56:
			//SDF
			//���T�|�[�g
			break;
		case 0x5F:
			//SDP
			//���T�|�[�g
			break;
		case 0x57:
			//SSM
			//���T�|�[�g
			break;
		case 0x58:
			//SHS
			//���T�|�[�g
			break;
		case 0x59:
			//SVS
			//���T�|�[�g
			break;
		case 0x42:
			//GSM
			//���T�|�[�g
			break;
		case 0x5D:
			//GAA
			//���T�|�[�g
			break;
		case 0x5E:
			//SRC
			//���T�|�[�g
			break;
		case 0x62:
			//TCC
			//���T�|�[�g
			break;
		case 0x65:
			//CFS
			//���T�|�[�g
			break;
		case 0x63:
			//ORN
			//���T�|�[�g
			break;
		case 0x64:
			//MDF
			//���T�|�[�g
			break;
		case 0x66:
			//XCS
			//���T�|�[�g
			break;
		case 0x68:
			//PRA
			//���T�|�[�g
			break;
		case 0x54:
			//CCC
			//���T�|�[�g
			break;
		case 0x67:
			//SCR
			//���T�|�[�g
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

