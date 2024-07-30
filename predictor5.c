///////////////////////////////////////////////////////////////////////
////  Copyright 2020 by mars.     ʵ�־ֲ���ʷ��Ϣ                      //
///////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

// ���ͼ���������1
static inline UINT32 SatIncrement(UINT32 x, UINT32 max)
{
	if (x<max) return x + 1;
	return x;
}

// ���ͼ���������1
static inline UINT32 SatDecrement(UINT32 x)
{
	if (x>0) return x - 1;
	return x;
}

// The state is defined for Gshare, change for your design
// Gshare��֧Ԥ������״̬��Ϣ������Ҫ�����Լ�����ƽ��е���
UINT32 ghr;             // global history register  ȫ����ʷ�Ĵ���
UINT32 *pht;            // pattern history table    ģʽ��ʷ��
UINT32 *lht;            // local history table    	�ֲ���ʷ��
UINT32 historyLength;   // history length           ��ʷ����
UINT32 numPhtEntries;   // entries in pht           PHT�е�����
UINT32 numLhtEntries;   // entries in lht           LHT�е�����

#define PHT_CTR_MAX  3
#define PHT_CTR_INIT 2

#define HIST_LEN   17   // ȫ����ʷ�Ĵ������ȣ�ȡ17λ

#define TAKEN		'T'
#define NOT_TAKEN	'N'


void PREDICTOR_init(void)
{

	historyLength = HIST_LEN;
	ghr = 0;
	numPhtEntries = (1 << HIST_LEN);    		// ģʽ��ʷ����2^17��
	numLhtEntries = (1 << (HIST_LEN-14));    	// �ֲ���ʷ����2^14��

	pht = (UINT32 *)malloc(numPhtEntries * sizeof(UINT32));
	lht = (UINT32 *)malloc(numLhtEntries * sizeof(UINT32));

    // ��ģʽ��ʷ��ȫ����ʼ��ΪPHT_CTR_INIT
	for (UINT32 ii = 0; ii< numPhtEntries; ii++) {
		pht[ii] = PHT_CTR_INIT;
	}
	// ���ֲ���ʷ��ȫ����ʼ��ΪPHT_CTR_INIT
	for (UINT32 ii = 0; ii< numLhtEntries; ii++) {
		lht[ii] = PHT_CTR_INIT;
	}
}

// Gshare��֧Ԥ����
// ��PC�ĵ�17λ����ȫ����ʷ�Ĵ���������򣨼��ܣ���ȥ����PHT���õ���Ӧ�ı���״̬
// �����״̬��ֵ����һ�룬��Ԥ����ת
// �����״̬��ֵ����һ�룬��Ԥ�ⲻ��ת
char GetPrediction(UINT64 PC)
{
	UINT32 lhtIndex = (PC & 0x3FFF) % (numPhtEntries);			// ��ȡ����14λ��Ϊlht������
	UINT32 phtIndex = ((PC & 0x1C000) | lht[lhtIndex]) % (numPhtEntries);	// ��ȡ��ĩβ14λ����3λ����lhtֵ������
	UINT32 phtCounter = pht[phtIndex];

	if (phtCounter > (PHT_CTR_MAX / 2)) {
		return TAKEN;
	}
	else {
		return NOT_TAKEN;
	}
}

// Gshare��֧Ԥ����
// ���ݷ�ָ֧��ʵ��ִ�н���������¶�Ӧ�ı��ͼ�����
// ������Ϊ��ת�����Ӧ�ı��ͼ�����+1
// ������Ϊ����ת�����Ӧ�ı��ͼ�����-1
// ����ȫ����ʷ�Ĵ�����
// ���Ϊ��ת����1��λ��GHR�����λ
// ���Ϊ����ת����0��λ��GHR�����λ
void  UpdatePredictor(UINT64 PC, OpType opType, char resolveDir, char predDir, UINT64 branchTarget)
{

    opType = opType;
    predDir = predDir;
    branchTarget = branchTarget;
    
	UINT32 lhtIndex = (PC & 0x3FFF) % (numPhtEntries);			// ��ȡ����14λ��Ϊlht������
	UINT32 phtIndex = ((PC & 0x1C000) | lht[lhtIndex]) % (numPhtEntries);	// ��ȡ��ĩβ14λ����3λ����lhtֵ������
	UINT32 phtCounter = pht[phtIndex];
//	printf("PC=%016llx resolveDir=%c predDir=%c branchTarget=%016llx\n", PC, resolveDir, predDir, branchTarget);

	if (resolveDir == TAKEN) {
		pht[phtIndex] = SatIncrement(phtCounter, PHT_CTR_MAX);  // ������Ϊ��ת�����Ӧ�ı��ͼ�����+1
	}
	else {
		pht[phtIndex] = SatDecrement(phtCounter);  // ������Ϊ����ת�����Ӧ�ı��ͼ�����-1
	}

	// update the GHR
	ghr = (ghr << 1);

	if (resolveDir == TAKEN) {
		ghr = ghr | 0x1;
	}

	// update the LHT
	lht[lhtIndex] = (lht[lhtIndex] << 1);

	if (resolveDir == TAKEN) {
		lht[lhtIndex] = lht[lhtIndex] | 0x1;
	}
}

void PREDICTOR_free(void)
{
	free(pht);
	free(lht);
}