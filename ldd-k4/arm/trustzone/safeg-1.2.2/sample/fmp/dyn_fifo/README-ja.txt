
                SafeG User Samples: FMP FIFO Sample

          2013 (C) Daniel Sangorrin, Shinya Honda, ���c �M�� 
          
--------
�T�v
--------

���̃v���O������ Dynamic SafeG syscall��p���āC2��OS�Ԃ�FIFO�ʐM��
�s���T���v���v���O�����ł���D

���L�� Altera SoC ��Ώۂɐ������鑼�̃^�[�Q�b�g�ł����l�̕��@�Ŏ��s��
�\�ł���D

------------------
�r���h
------------------
�E�f�B���N�g���\��
���̂悤�ȃf�B���N�g���\���ɂ���D
  ./fmp    <-- Altera SoC �����̊ȈՃp�b�P�[�W��W�J
  ./safeg-1.xxx
    ./app
    ./doc
    ./monitor
    ./patches


�E�R���t�B�M�����[�V�����̐ݒ�
SafeG�̃g�b�v�t�H���_�� Makefile.config ���^�[�Q�b�g�ɍ��킹�Đݒ肷��D
------------
MONITOR_TARGET=socfpga
------------

�ESecure����ASP�̃r���h���@
./trust �ňȉ������
$ cd ./trust
$ make

�ENon-Secure�̃r���h���@
./non-trust �ňȉ������
$ cd ./non-trust
$ make

�ȏ�D
