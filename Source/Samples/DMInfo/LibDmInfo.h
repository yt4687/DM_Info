///////////////////////////////////////
//
//  =====                             =====
//  ===== DMInfo (TVTest�p�v���O�C��) =====
//  ===== �f�W�I�̊y�ȏ���\������  =====
//  =====                             =====
//
// ======== ���p������ ====
// ���̃t�@�C����DMInfo�p�ɐV�K�ɍ쐬�������̂ł��B
// ���̃R�[�h�����p�\�t�g��V�F�A�E�F�A�ɍė��p���邱�Ƃ͕s�Ƃ��܂��B
// �t���[�\�t�g�ւ͂��̃R�[�h�����̂܂܁A���邢�͉��ς��čė��p���č\���܂��񂪁A
// �\���܂��񂪁A���̏ꍇ�A���p���������̃\�[�X�R�[�h������Ɠ��l�̏����Ŕz�z
// �p�b�P�[�W�ɓY�t���邱�Ƃ𗘗p�����Ƃ��܂��B
//
///////////////////////////////////////

//
//  �X�^�[�f�W�I�̊y�ȏ��ES����ȉ����擾����
//   �� �Ȗ�
//   �� �A�[�e�B�X�g��
//   �� �Ȃ̑�����
//   �� �Ȃ̐擪����̌o�ߎ���
//

				//
				// �Ȗ��E�A�[�e�B�X�g���̍ő啶����
				//
#define DI_NAME_LEN 1024

				//
				// ����Ԃ��\����
				//
typedef struct _DI_Info_ {
	WCHAR wcsTitle[DI_NAME_LEN+1] ;		// �Ȗ�
	WCHAR wcsArtist[DI_NAME_LEN+1] ;	// �A�[�e�B�X�g��
	DWORD dwDuration ;					// �Ȃ̑�����(�b)
	DWORD dwElapsed ;					// �Ȃ̐擪����̌o�ߎ���(�b)
	BOOL bIntermisson ;					// �ԑg�ԓ��A�y�ȏ��̖�����
} DI_Info ;

				//
				// ��Ԃ̏�����
				//   �A�v���N�����A�y�сA�`�����l���ύX���Ŋy�ȏ��ES
				//   �̘A������������ꍇ�ɁA���֐����ĂԕK�v����B
				//
extern void LibDI_Reset( void ) ;

				//
				// �y�ȏ��ES��������擾
				//   �Ȗ����������A��񂪕����p�P�b�g�Ɍׂ�ꍇ�A���
				//   �m�肷��ŏI�p�P�b�g�ȊO�ł�FALSE��Ԃ��B
				//   ���̎��ADI_Info�ɕԂ��f�[�^�͎g�p�s�B
				//
extern BOOL LibDI_GetInfo(
	const BYTE* pyPacket,				// �y�ȏ��ES�̃p�P�b�g(188�o�C�g)
	DI_Info* pInfo						// �y�ȏ���Ԃ��\����
) ;

