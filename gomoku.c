#include "app.h"
#include "gomoku.h"


static const char koma[10][7]={
	{0,0,0,1,0,0,0},
	{0,1,1,1,1,1,0},
	{0,1,1,1,1,1,0},
	{1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1},
	{1,1,1,1,1,1,1},
	{0,1,1,1,1,1,0},
	{0,1,1,1,1,1,0},
	{0,0,0,1,0,0,0},
};


struct yomumoves{
	int pos;
	int result;
};


struct gv{
	struct yomumoves yomumoves[250];
	int buf[3500];
};

extern int work_buf[4000];

static struct gv *gv = (struct gv*)work_buf;
static int goban[274];					// ���C���̔�
static int yomiend;
//static int ban_color;
static int black, white;
static int teban, tekazu;
static int dir10[4]={1,15,16,17};


static int MCjoseki(int *ban);


/********************************************************************************/
/*		initban																	*/
/********************************************************************************/
static void initban(int *ban)
{
	int i, x, y;
	
	for(i=0; i<274; i++){
		ban[i] = ISHI_O;
	}
	for(y=1; y<=15; y++){
		for(x=1; x<=15; x++){
			ban[macropos(x,y)] = ISHI_N;
		}
	}
}
/********************************************************************************/
/*		cpyban1/2																*/
/********************************************************************************/
static void cpyban1(int *ban1, int *ban2)
{
	memcpy(ban1, ban2, 274*sizeof(int));
}

static void cpyban2(int *ban1, int *ban2)
{
	int i;
	
	for(i=0; i<274; i++){
		ban1[i] = ban2[i] & 3;
	}
}
/********************************************************************************/
/*		MCupdateban																*/
/*		ban �Ƀt���O��t����													*/
/********************************************************************************/
//		MCupdates1
//		return  0: �҂�����~�܂��Ă���
//		return -1: �Ď~��
//		return -2: �Q�ԋ�
//		return -3: �Q�ԋ󂫂̏H�~��
//		return  n: �P�ԋ󂫂� n �̎���
//		h:1 �P�ԋ󂫂� n �̎��Όオ�~�܂��Ă���
//		h:2 �P�ԋ󂫂� n �̎��Όオ�H�~��
//		h:0 �P�ԋ󂫂� n �̎��Ό�ɋ󂫂���

static int MCupdates1(int *ban, int p, int dir, int k, int *h)
{
	int i, n;
	
	*h = 0;
	if((ban[p+dir] & 3)){				/* �ׂ��΂��Ւ[							*/
		return 0;
	}
	if((ban[p+dir*2] & 3)){				/* �P�ԂƂтŐ΂��Ւ[					*/
		if((ban[p+dir*2] & 3) != k)
			return -1;					/* �Ď~��								*/
		n = 1;
		for(i=3; ; i++){
			if((ban[p+dir*i] & 3)==k)
				n++;
			else
				break;
		}
		if((ban[p+dir*i] & 3)){
			*h = 1;						/* �����Ɏ~�܂��Ă���					*/
		}
		else{							/* ��									*/
			if((ban[p+dir*(i+1)] & 3)==k)
				*h = 2;					/* �H�~��								*/
			else
				*h = 0;
		}
		return n;
	}
	//	�Q�ԂƂ�
	if((ban[p+dir*3] & 3)){				/* �Q�ԂƂтŐ΂��Ւ[					*/
		if((ban[p+dir*3] & 3) == k)
			return -3;					/* �Q�ԂƂт̏H�~��						*/
	}
	return -2;							/* �Q�ԂƂ�								*/
}

static int MCupdates2(int *ban, int p, int dir)
{
	int cnt, i;
	
	cnt = 0;
	for(i=1; ; i++){
		if((ban[p+dir*i] & 3)==ISHI_N){
			cnt++;
			if(cnt >= 3)
				return cnt;
		}
		else
			return cnt;
	}
}

static void MCupdatebans(int *ban, int p)
{
	int i, j, ren, p1, p2, h1, h2, hasi1, hasi2, tmp, flag3;
	
	for(i=0; i<4; i++){
		ren = 1;
		p1 = p;
		for(j=1; ; j++){
			if((ban[p+dir10[i]*j] & 3)==ISHI_S){
				ren++;
				p1 += dir10[i];
			}
			else
				break;
		}
		p2 = p;
		for(j=1; ; j++){
			if((ban[p-dir10[i]*j] & 3)==ISHI_S){
				ren++;
				p2 -= dir10[i];
			}
			else
				break;
		}
		if(ren >= 5){
			ban[p] |= ISHI_S5;
		}
		else if(ren==4){
			if((ban[p1+dir10[i]] & 3)==ISHI_N && (ban[p2-dir10[i]] & 3)==ISHI_N)
				ban[p] |= ISHI_S4X;
			else if((ban[p1+dir10[i]] & 3)==ISHI_N)
				ban[p] |= ISHI_S4;
			else if((ban[p2-dir10[i]] & 3)==ISHI_N)
				ban[p] |= ISHI_S4;
		}
		else{
			hasi1 = MCupdates1(ban, p1, dir10[i], ISHI_S, &h1);
			hasi2 = MCupdates1(ban, p2, -dir10[i], ISHI_S, &h2);
			flag3 = 0;
			for(j=0; j<2; j++){
				if(j==1){
					tmp = hasi1;
					hasi1 = hasi2;
					hasi2 = tmp;
					tmp = h1;
					h1 = h2;
					h2 = tmp;
				}
				if(ren==3){
					if(hasi1 >= 1)		/* �P�ԂƂтłP�ȏ�̎���				*/
						ban[p] |= ISHI_S4;
					else if(hasi1 < 0 && hasi2 < 0){	/* �҂�����~�܂��Ă��Ȃ��A���΂����΂ɂȂ�	*/
						if(hasi1 != -1 || hasi2 != -1)	/* �Ď~�߂łȂ�			*/
//							ban[p] |= ISHI_S3;
							flag3 = 1;
						ban[p] |= ISHI_S2;
					}
					if(MCupdates2(ban, p1, dir10[i])+MCupdates2(ban, p2, -dir10[i]) >= 2)
						ban[p] |= ISHI_S2;
				}
				else if(ren==2){
					if(hasi1 >= 2)		/* �P�ԂƂтłQ�ȏ�̎���				*/
						ban[p] |= ISHI_S4;
					else if(hasi1==1){		/* �P�ԂƂтłP�̎���					*/
						if(h1 != 1){		/* �P�ԂƂт̐悪�~�܂��Ă��Ȃ�			*/
							if(hasi2 != 0)	/* �҂�����~�܂��Ă��Ȃ�			*/
//								ban[p] |= ISHI_S3;
								flag3 = 1;
							ban[p] |= ISHI_S2;
						}
						if(hasi2 != 0)		/* �҂�����~�܂��Ă��Ȃ�			*/
							ban[p] |= ISHI_S2;
					}
					if(MCupdates2(ban, p1, dir10[i])+MCupdates2(ban, p2, -dir10[i]) >= 3)
						ban[p] |= ISHI_S2;
				}
				else if(ren==1){
					if(hasi1 >= 3)		/* �P�ԂƂтłR�ȏ�̎���				*/
						ban[p] |= ISHI_S4;
					else if(hasi1==2){		/* �P�ԂƂтłQ�̎���					*/
						if(h1 != 1){		/* �P�ԂƂт̐悪�~�܂��Ă��Ȃ�			*/
							if(hasi2 != 0)	/* �҂�����~�܂��Ă��Ȃ�			*/
//								ban[p] |= ISHI_S3;
								flag3 = 1;
							ban[p] |= ISHI_S2;
						}
						if(hasi2 != 0)		/* �P�ԂƂт̐悪�~�܂��Ă��Ȃ�			*/
							ban[p] |= ISHI_S2;
					}
					else if(hasi1==1){
						if((h1 != 1) && (hasi2 != 0))
							ban[p] |= ISHI_S2;
					}
				}
			}
			if(flag3){
				if(ban[0] & ISHI_S3)
					ban[p] |= ISHI_S33;
				ban[p] |= ISHI_S3;
			}
		}
	}
}

#define SETFLAG(flag) {if(ban[p] & flag)ban[p] |= ISHI_KINSYU;else ban[p] |= flag;}

static void MCupdatebank(int *ban, int p)
{
	int i, j, ren, p1, p2, hasi1, hasi2, h1, h2, tmp, flag3;
	
	for(i=0; i<4; i++){
		ren = 1;
		p1 = p;
		for(j=1; ; j++){
			if((ban[p+dir10[i]*j] & 3)==ISHI_K){
				ren++;
				p1 += dir10[i];
			}
			else
				break;
		}
		p2 = p;
		for(j=1; ; j++){
			if((ban[p-dir10[i]*j] & 3)==ISHI_K){
				ren++;
				p2 -= dir10[i];
			}
			else
				break;
		}
		hasi1 = MCupdates1(ban, p1, dir10[i], ISHI_K, &h1);
		hasi2 = MCupdates1(ban, p2, -dir10[i], ISHI_K, &h2);
		if(ren > 5)
			ban[p] |= ISHI_KINSYU;
		else if(ren==5)
			ban[p] |= ISHI_K5;
		else if(ren==4){
			if(hasi1 < 0 && hasi2 < 0)
				SETFLAG(ISHI_K4X)
			else if(hasi1 < 0 || hasi2 < 0)	/* �~�܂��Ă��Ȃ�(�H�~�ߊ܂�)			*/
				SETFLAG(ISHI_K4)
		}
		else{
			flag3 = 0;
			for(j=0; j<2; j++){
				if(j==1){
					tmp = hasi1;
					hasi1 = hasi2;
					hasi2 = tmp;
					tmp = h1;
					h1 = h2;
					h2 = tmp;
				}
				if(ren==3){
					if(hasi1==1)		/* �P�ԂƂтłP�̎���					*/
						SETFLAG(ISHI_K4)
					else if(hasi1 < 0 && hasi2 < 0){	/* �~�܂��Ă��Ȃ�		*/
						if(hasi1==-2 || hasi2==-2)		/* �Q�ԂЂ炫			*/
							flag3 = 1;
						ban[p] |= ISHI_K2;
					}
					if(MCupdates2(ban, p1, dir10[i])+MCupdates2(ban, p2, -dir10[i]) >= 2)
						ban[p] |= ISHI_K2;
				}
				else if(ren==2){
					if(hasi1==2)		/* �P�ԂƂтłQ�̎���					*/
						SETFLAG(ISHI_K4)
					else if(hasi1==1){	/* �P�ԂƂтłP�̎���					*/
						if(h1==0){		/* �󂫂���								*/
							if(hasi2 < 0)	/* �~�܂��Ă��Ȃ�					*/
								flag3 = 1;
							ban[p] |= ISHI_K2;
						}
					}
					if(MCupdates2(ban, p1, dir10[i])+MCupdates2(ban, p2, -dir10[i]) >= 3)
						ban[p] |= ISHI_K2;
				}
				else if(ren==1){
					if(hasi1==3)		/* �P�ԂƂтłR�̎���					*/
						SETFLAG(ISHI_K4)
					else if(hasi1==2){	/* �P�ԂƂтłQ�̎���					*/
						if(h1==0){		/* �󂫂���								*/
							if(hasi2 < 0)	/* �~�܂��Ă��Ȃ�					*/
								flag3 = 1;
							ban[p] |= ISHI_K2;
						}
						if(hasi2 != 0)
							ban[p] |= ISHI_K2;
					}
					else if(hasi1==1){
						if((h1 != 1) && (hasi2 != 0))
							ban[p] |= ISHI_K2;
					}
				}
			}
			if(flag3)
				SETFLAG(ISHI_K3);
		}
	}
}

static void MCmakeban(int *ban)
{
	int p;
	
	cpyban2(ban, ban);					/* �t���O�N���A							*/
	for(p=17; p<256; p++){
		if(ban[p]==ISHI_N){
			MCupdatebank(ban, p);
			MCupdatebans(ban, p);
		}
	}
}

static void MCupdateban(int *ban, int pos)
{
	int i, j;
	
	if((ban[pos] & 3)==ISHI_N){
		ban[pos] &= 3;				/* �t���O�N���A							*/
		MCupdatebank(ban, pos);
		MCupdatebans(ban, pos);
	}
	for(i=0; i<4; i++){
		for(j=1; ; j++){
			if((ban[pos+dir10[i]*j] & 3)==ISHI_O)
				break;
			if((ban[pos+dir10[i]*j] & 3)==ISHI_N){
				ban[pos+dir10[i]*j] &= 3;			/* �t���O�N���A							*/
				MCupdatebank(ban, pos+dir10[i]*j);
				MCupdatebans(ban, pos+dir10[i]*j);
			}
		}
		for(j=-1; ; j--){
			if((ban[pos+dir10[i]*j] & 3)==ISHI_O)
				break;
			if((ban[pos+dir10[i]*j] & 3)==ISHI_N){
				ban[pos+dir10[i]*j] &= 3;			/* �t���O�N���A							*/
				MCupdatebank(ban, pos+dir10[i]*j);
				MCupdatebans(ban, pos+dir10[i]*j);
			}
		}
	}
}
/********************************************************************************/
/*		judge																	*/
/********************************************************************************/
static int judge(int *ban, int k)
{
	int i, ten, b;
	
	ten = 0;
	if(k==ISHI_K){							// ����
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & ISHI_KINSYU){
				ten -= 1000;
			}
			if(b & ISHI_K5){
				ten += 100000;
			}
			if(b & ISHI_K4X){
				ten += 10000;
			}
			if(b & ISHI_K4){
				ten += 1000;
			}
			if(b & ISHI_K3){
				ten += 200;
			}
			if(b & ISHI_K2){
				ten += 10;
			}
			if(b & ISHI_S5){
				ten -= 10000;
			}
			if(b & ISHI_S4X){
				ten -= 5000;
			}
			if(b & ISHI_S4){
				ten -= 500;
			}
			if(b & ISHI_S33){
				ten -= 500;
			}
			if(b & ISHI_S3){
				ten -= 100;
			}
			if(b & ISHI_S2){
				ten -= 10;
			}
		}
	}
	else{									// ����
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & ISHI_KINSYU){
				ten += 1000;
			}
			if(b & ISHI_K5){
				ten -= 10000;
			}
			if(b & ISHI_K4X){
				ten -= 5000;
			}
			if(b & ISHI_K4){
				ten -= 500;
			}
			if(b & ISHI_K3){
				ten -= 100;
			}
			if(b & ISHI_K2){
				ten -= 10;
			}
			if(b & ISHI_S5){
				ten += 100000;
			}
			if(b & ISHI_S4X){
				ten += 10000;
			}
			if(b & ISHI_S4){
				ten += 1000;
			}
			if(b & ISHI_S33){
				ten += 1000;
			}
			if(b & ISHI_S3){
				ten += 200;
			}
			if(b & ISHI_S2){
				ten += 10;
			}
		}
	}
	return ten;
}
/********************************************************************************/
/*		makeallmoves															*/
/********************************************************************************/
static void makeallmoves(int *ban, int k, struct yomumoves *moves, int *poscnt)
{
	int i, j, b;
	
	*poscnt = 0;
	if(k==ISHI_K){							// ����
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & 3){
				continue;
			}
			if(b & ISHI_KINSYU){
			}
			else if(b & ISHI_K5){
				moves[0].pos = i;
				moves[0].result = 100000;
				*poscnt = 1;
				return;
			}
			else if(b & ISHI_S5){
				moves[0].pos = i;
				moves[0].result = 1000;
				*poscnt = 1;
				return;
			}
			else if(b & ISHI_K4X){
				for(j=17; j<256; j++){		// ���̂S�A���L�邩�ǂ������ׂ�
					if(ban[j] & ISHI_S5){
						break;
					}
				}
				if(j==256){					// ���̂S�A�͖�������
					moves[0].pos = i;
					moves[0].result = 100000;
					*poscnt = 1;
					return;
				}
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 210;
			}
			else if(b & ISHI_S4X){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 200;
			}
			else if(b & ISHI_K4){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 60;
			}
			else if(b & ISHI_S4){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 50;
			}
			else if(b & ISHI_K3){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 40;
			}
			else if(b & ISHI_S33){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 30;
			}
			else if(b & ISHI_S3){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 20;
			}
			else if(b & ISHI_K2){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 10;
			}
			else if(b & ISHI_S2){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 5;
			}
		}
	}
	else{									// ����
		for(i=17; i<256; i++){
			b = ban[i];
			if(b & 3){
				continue;
			}
			if(b & ISHI_S5){
				moves[0].pos = i;
				moves[0].result = 100000;
				*poscnt = 1;
				return;
			}
			else if(b & ISHI_K5){
				moves[0].pos = i;
				moves[0].result = 1000;
				*poscnt = 1;
				return;
			}
			else if(b & ISHI_S4X){
				for(j=17; j<256; j++){		// ���̂S�A���L�邩�ǂ������ׂ�
					if(ban[j] & ISHI_K5){
						break;
					}
				}
				if(j==256){					// ���̂S�A�͖�������
					moves[0].pos = i;
					moves[0].result = 100000;
					*poscnt = 1;
					return;
				}
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 210;
			}
			else if(b & ISHI_K4X){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 200;
			}
			else if(b & ISHI_S4){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 60;
			}
			else if(b & ISHI_K4){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 50;
			}
			else if(b & ISHI_S33){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 40;
			}
			else if(b & ISHI_S3){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 30;
			}
			else if(b & ISHI_K3){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 20;
			}
			else if(b & ISHI_S2){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 10;
			}
			else if(b & ISHI_K2){
				moves[(*poscnt)].pos      = i;
				moves[(*poscnt)++].result = 5;
			}
		}
	}
}
/********************************************************************************/
/*		kosyu																	*/
/*		top �Ď��2�����_���C������ �_�� 500, 490								*/
/********************************************************************************/
int kosyu_eva(int *ban1, int k)			// 3�A 4�A�����鐔���J�E���g����
{
	int i, ten;
	
	ten = 0;
	for(i=17; i<256; i++){
		if(ban1[i] & ISHI_KINSYU){
			ten -= 10;
			continue;
		}
		if(ban1[i] & (ISHI_K3 | ISHI_K4 | ISHI_K4X | ISHI_K5)){
			ten++;
		}
		if(ban1[i] & (ISHI_S3 | ISHI_S33 | ISHI_S4 | ISHI_S4X | ISHI_S5)){
			ten--;
		}
	}
	if(k==ISHI_K)
		return ten;
	return -ten;
}

void kosyu(int *ban, int k, struct yomumoves *yomumoves, int poscnt)
{
	int ban1[274];
	int i, ten, top_i, top_ten, second_i, second_ten;
	
	if(yomumoves[0].result >= 1000){			// 5�A�܂���5�A�~�߂�������
		return;
	}
	top_i = top_ten = second_i = second_ten = -100;
	for(i=0; i< poscnt; i++){
		cpyban1(ban1, ban);
		ban1[yomumoves[i].pos] = k;
		MCupdateban(ban1, yomumoves[i].pos);
		ten = kosyu_eva(ban1, k);
		if(ten > top_ten){
			second_ten = top_ten;
			second_i   = top_i;
			top_ten    = ten;
			top_i      = i;
		}
		else if(ten > second_ten){
			second_ten = ten;
			second_i   = i;
		}
	}
	if(top_i >= 0){
		yomumoves[top_i].result = 500;
	}
	if(second_i >= 0){
		yomumoves[second_i].result = 490;
	}

}
/********************************************************************************/
/*		MCyomu																	*/
/********************************************************************************/
#define TEN_INFINIT	100000000

static int MCcompk(const void *elem1, const void *elem2)
{
	return ((struct yomumoves *)elem2)->result - ((struct yomumoves *)elem1)->result;
}

static int MCyomuk(int *ban, int k, int *pos, int depth, int alpha, int beta, int buf_ptr)
{
	int i, max, ret, mv, poscnt;
	int ban1[274];
	struct yomumoves *ymoves = (struct yomumoves*)&gv->buf[buf_ptr];
	int s = macroinv(k);
	
	if(depth >= yomiend)
		return judge(ban, k);
	makeallmoves(ban, k, gv->yomumoves, &poscnt);
	if(yomiend-depth >= 3){
		kosyu(ban, k, gv->yomumoves, poscnt);
	}
	memcpy(ymoves, gv->yomumoves, poscnt*sizeof(struct yomumoves));
	qsort(ymoves, poscnt, sizeof(struct yomumoves), MCcompk);
	if(poscnt >= 6){
		ret = ymoves[5].result;
		for(i=6; i<poscnt; i++){
			if(ret > ymoves[i].result){
				break;
			}
		}
		poscnt = i;
	}
	max = -TEN_INFINIT;
	for(i=0; i<poscnt; i++){
		if(ymoves[i].result==100000){		// 5�A����
			*pos = ymoves[i].pos;
			return 100000;
		}
		cpyban1(ban1, ban);
		ban1[ymoves[i].pos] = k;
		MCupdateban(ban1, ymoves[i].pos);

		ret = -MCyomuk(ban1, s, &mv, depth+1, -beta, -alpha, buf_ptr+poscnt*2);

//		if(depth==0 && tekazu < 10){
//			ret += MCrnd(500)-250;		// ����
//		}
		if(ret >= beta){
			return (ret+1);
		}
		if(max < ret){
			max = ret;
			*pos = ymoves[i].pos;
			if(max > alpha)
				alpha = max;
		}
	}
	return max;
}


static int MCyomu(int *ban, int k, int *pos)
{
	int ret, p, tmp;
	
	for(p=17; p<256; p++){
		tmp = ban[p] & 3;
		if(tmp!=ISHI_O && tmp!=ISHI_N){
			break;
		}
	}
	if(p==256){								// �΂Ȃ�
		*pos = macropos(8,8);
		return 0;
	}
    
    *pos = MCjoseki(ban);
    if(*pos){
        if((ban[*pos]&3)==0){
            return 0;
        }
    }
    
	//		�ʏ�ǂ�
	MCmakeban(ban);
//    yomiend = 5;
	*pos = 0;
	
	ret = MCyomuk(ban, k, pos, 0, -TEN_INFINIT, TEN_INFINIT, 0);
	
	if(k != ISHI_K)
		ret = -ret;
	return ret;
}
/********************************************************************************/
/*		dispban																	*/
/********************************************************************************/
static void dispban_null()
{
	int x, y, xx, yy;
	
	//		�w�i�F�h��Ԃ�
	lcd_clearA(0,0, 7*15-1, 10*15-1, YELLOW_D);
	
	//		����
	for(x=3; x<=3+7*14; x++){
		for(y=5; y<=5+10*14;y+=10){
			pset(x,y, BLACK);
		}
	}
	
	//		�c��
	for(y=5; y<=5+10*14; y++){
		for(x=3; x<=3+7*14; x+=7){
			pset(x,y, BLACK);
		}
	}
	
	//		��
	for(yy=4; yy<=12; yy+=4){
		for(xx=4; xx<=12; xx+=4){
			x = 3+(xx-1)*7;
			y = 5+(yy-1)*10;
			pset(x-1,y-1, BLACK);
			pset(x-1,y+1, BLACK);
			pset(x+1,y-1, BLACK);
			pset(x+1,y+1, BLACK);
		}
	}
}

static void disp_koma(int x, int y, int text_color, int back_color)
{
	int xx, yy;
	
	for(yy=0; yy<10; yy++){
		for(xx=0; xx<7; xx++){
			if(koma[yy][xx]==0){			// back
				pset((x-1)*7+xx,(y-1)*10+yy, back_color);
			}
			else{							// �~
				pset((x-1)*7+xx,(y-1)*10+yy, text_color);
			}
		}
	}
}
/********************************************************************************/
/*		human_input																*/
/********************************************************************************/
static int human_input(int *ban, int pos)
{
	int x, y, sw, xx, yy;
	
	if(pos){
		x = macrox(pos);
		y = macroy(pos);
	}
	else{
		x = y = 8;
	}
	for(;;){
		for(yy=0; yy<10; yy++){
			for(xx=0; xx<7; xx++){
				if(pget((x-1)*7+xx, (y-1)*10+yy)==YELLOW_D){
					pset((x-1)*7+xx, (y-1)*10+yy, CYAN);
				}
			}
		}

		sw = sw_updown();

		if(sw & A_SW){
			return macropos(x,y);
		}
		
		for(yy=0; yy<10; yy++){
			for(xx=0; xx<7; xx++){
				if(pget((x-1)*7+xx, (y-1)*10+yy)==CYAN){
					pset((x-1)*7+xx, (y-1)*10+yy, YELLOW_D);
				}
			}
		}
		if(sw & UP){
			if(--y < 1) y = 1;
		}
		else if(sw & DOWN){
			if(++y > 15) y = 15;
		}
		else if(sw & RIGHT){
			if(++x > 15) x = 15;
		}
		else if(sw & LEFT){
			if(--x < 1) x = 1;
		}
	}
}
/********************************************************************************/
/*		game																	*/
/********************************************************************************/
static void game()
{
	int pos, pos_mae, player, ten;
	char buf[20];
	
	lcd_clear(WHITE);
	dispban_null();							// ��̔Ղ�`��
	initban(goban);
	teban = ISHI_K;
	tekazu = 1;
	pos = pos_mae = 0;
	ten = 0;
	for(;;){
		//		�P��O�̎�̃J�[�\��������
		if(pos_mae){
			if(teban==ISHI_K){
				disp_koma(macrox(pos_mae), macroy(pos_mae), BLACK, YELLOW_D);
			}
			else{
				disp_koma(macrox(pos_mae), macroy(pos_mae), WHITE, YELLOW_D);
			}
		}
		//		����̎���J�[�\���t���ŕ\������
		if(pos){
			if(teban==ISHI_S){
				disp_koma(macrox(pos), macroy(pos), BLACK, CYAN);
			}
			else{
				disp_koma(macrox(pos), macroy(pos), WHITE, CYAN);
			}
		}
        pos_mae = pos;
		sprintf(buf, "%d", ten);
		str_disp(105, 150-16, buf);
		player = teban==ISHI_K ? black : white;
		if(player==0){						// cpu
			str_disp(105, 10, "CPU   ");
			ten = MCyomu(goban, teban, &pos);
			
//			buzzer(1000, 50);
            buzzer_div = 8;                 // 1KHz
            buzzer_ms = 50;
			MCmakeban(goban);
			//		��������
			if(teban==ISHI_K && (goban[pos] & ISHI_K5)){
				disp_koma(macrox(pos), macroy(pos), BLACK, CYAN);
//				buzzer(500,1000);
                buzzer_div = 16;            // 500Hz
                buzzer_ms = 1000;
				for(;;)
					;
			}
			if(teban==ISHI_S && (goban[pos] & ISHI_S5)){
				disp_koma(macrox(pos), macroy(pos), WHITE, CYAN);
                buzzer_div = 16;            // 500Hz
                buzzer_ms = 1000;
//				buzzer(500,1000);
				for(;;)
					;
			}
			goban[pos] = teban;
		}
		else{								// human
			str_disp(105, 10, "human ");
			for(;;){
				pos = human_input(goban, pos);
				if((goban[pos]&3) || (teban==ISHI_K && (goban[pos]&ISHI_KINSYU))){
                    buzzer_div = 16;        // 500Hz
                    buzzer_ms = 1000;
//					buzzer(500, 1000);
					continue;
				}
				if(tekazu==1 && pos!=macropos(8,8)){
                    buzzer_div = 16;        // 500Hz
                    buzzer_ms = 1000;
//					buzzer(500, 1000);
					continue;
				}
				break;
			}
			if((teban==ISHI_K && (goban[pos]&ISHI_K5)) || (teban==ISHI_S && (goban[pos]&ISHI_S5))){
                buzzer_div = 8;             // 1KHz
                buzzer_ms = 1000;
//				buzzer(1000, 1000);
				str_cdisp(60, "�M���̏���");
				sw_updown();
				return;
			}
			goban[pos] = teban;
		}
		tekazu++;
		teban = macroinv(teban);
	}
}
/********************************************************************************/
/*		gomoku_main																*/
/********************************************************************************/
static const struct menu gomoku_menu[] = {
	{0, 30,  1,"���F�b�o�t�A���F�b�o�t"},
	{0, 50,  2,"���F�l�ԁA�@���F�l��"},
	{0, 70,  3,"���F�b�o�t�A���F�l��"},
	{0, 90,  4,"���F�l�ԁA�@���F�b�o�t"},
	{0,0,0,0}
};

static const struct menu level_menu[]={
	{0, 40,  5, "����"},
	{0, 60,  3, "�ア"},
	{0,0,0,0}
};

void gomoku_main()
{
	int cmd;
	
	lcd_clear(MAGENTA_D);
	str_cdisp(0, "�ܖڕ���");
	str_disp(0,25, "���F�O�O�֎�");
	str_disp(0,37, "���F�l�l�֎�");
	str_disp(0,49, "���F���A�֎�");
	str_disp(0,69, "���F�֎薳��");
	str_disp(0,81, "���F���A����");
	str_cdisp(106, "K.Fukumoto");
	sw_updown();
	
	lcd_clear(GRAY);
	cmd = menu_select(gomoku_menu);
	switch(cmd){
		case 1:							// cpu - cpu
			black = white = 0;
			break;
		case 2:							// human - human
			black = white = 1;
			break;
		case 3:							// cpu black
			black = 0;
			white = 1;
			break;
		case 4:							// cpu white
			black = 1;
			white = 0;
			break;
	}
	
	lcd_clear(GRAY);
	str_cdisp(10, "�����ݒ�");
	yomiend = menu_select(level_menu);
	
	game();
}
/********************************************************************/
/*		zyocnv1s		                                            */
/********************************************************************/
#define BANSIZE	15

static int  zyocnv1(int cnvflg, int pos)
{
    int  x, y, tmp;

    x = macrox(pos);
    y = macroy(pos);
    if(x<0 || x>BANSIZE || y<0 || y>BANSIZE)
    	return pos;

    if(cnvflg & 4)
		y = (BANSIZE+1)-y;
    if(cnvflg & 2)
		x = (BANSIZE+1)-x;
    if(cnvflg & 1){
		tmp = x;
		x   = y;
		y   = tmp;
    }
    return(macropos(x,y));
}
/****************************************************************************/
/*		zyocnv2                                                             */
/*		��ΔՈʒu�����Ոʒu�ɕϊ�����										*/
/****************************************************************************/
static int  zyocnv2(int cnvflg, int pos)
{
    int  x, y, tmp;

    x = macrox(pos);
    y = macroy(pos);
    if(x<0 || x>BANSIZE || y<0 || y>BANSIZE)
    	return pos;

    if(cnvflg & 1){
		tmp = x;
		x   = y;
		y   = tmp;
    }
    if(cnvflg & 2)
		x = (BANSIZE+1) - x;
    if(cnvflg & 4)
		y = (BANSIZE+1) - y;
    return(macropos(x,y));
}
/***************************************************************************/
/*		zyoban1                                                            */
/***************************************************************************/
static void zyoban1(int cnvflg, int ban1[], int ban2[])
{
    int  pos;

    cpyban1(ban1, ban2);
    for(pos=17; pos<256; pos++)
		ban1[zyocnv1(cnvflg, pos)] = ban2[pos];
}
/********************************************************************************/
/*		MCjoseki																*/
/********************************************************************************/
#define MCrndx		MCrnd(100)

static int MCjoseki(int *ban)
{
	int ban1[274], i, cnt, cnvflg, tmp;
	
	cnt = 0;
	for(i=0; i<274; i++){
		if(ban[i]==ISHI_K || ban[i]==ISHI_S)
			cnt++;
	}
	for(cnvflg=0; cnvflg<8; cnvflg++){
		zyoban1(cnvflg, ban1, ban);
		if(ban1[macropos(8,8)]==ISHI_K){	/* �V���ɍ��L						*/
			if(cnt==1){					/* �V���̐΂݂̂�����					*/
				if((tmp=MCrndx)<50)
					return zyocnv2(cnvflg, macropos(9,7));
				else
					return zyocnv2(cnvflg, macropos(8,7));
			}

			//����������
			//����������
			//����������
			//����������
			//����������
			if(ban1[macropos(9,7)]==ISHI_S){	/* �Ԑڂǂ�						*/
				if(cnt==2){
//					if(MCrndx < 70)
						return zyocnv2(cnvflg, macropos(9,9));
//					else
//						return 0;
				}

				//����������
				//����������
				//����������
				//����������
				//����������
				if(ban1[macropos(9,8)]==ISHI_K){
					if(cnt==3){
						tmp = MCrndx;
						if(tmp < 40)
							return zyocnv2(cnvflg, macropos(8,9));
						else if(tmp < 70)
							return zyocnv2(cnvflg, macropos(10,8));
						else
							return zyocnv2(cnvflg, macropos(7,8));
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(10,8)]==ISHI_S){

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(8,6)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(8,9));
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(7,8)]==ISHI_S){

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(10,7)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(10,9));
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(8,9)]==ISHI_S){

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(7,8)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(6,8));
						}
					}
				}

				//����������
				//����������
				//����������
				//����������
				//����������
				if(ban1[macropos(9,9)]==ISHI_K){	/* �Y�����					*/
					if(cnt==3){
						tmp = MCrndx;
						if(tmp < 40)
							return zyocnv2(cnvflg, macropos(7,7));
						else if(tmp < 60)
							return zyocnv2(cnvflg, macropos(10,10));
						else if(tmp < 80)
							return zyocnv2(cnvflg, macropos(8,9));
						else
							return zyocnv2(cnvflg, macropos(8,10));
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(7,7)]==ISHI_S){
						if(cnt==4){
							if(MCrndx < 70)
								return zyocnv2(cnvflg, macropos(10,9));
							else
								return zyocnv2(cnvflg, macropos(10,8));
						}

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(10,9)]==ISHI_K){
							if(cnt==5){
								tmp = MCrndx;
								if(tmp < 20)
									return zyocnv2(cnvflg, macropos(7,8));
								else if(tmp < 40)
									return zyocnv2(cnvflg, macropos(8,9));
								else if(tmp < 60)
									return zyocnv2(cnvflg, macropos(10,7));
								else if(tmp < 80)
									return zyocnv2(cnvflg, macropos(11,9));
								else
									return zyocnv2(cnvflg, macropos(8,6));
							}

							//����������
							//����������
							//����������
							//����������
							//����������
							if(ban1[macropos(7,8)]==ISHI_S){

								//����������
								//����������
								//����������
								//����������
								//����������
								if(ban1[macropos(10,7)]==ISHI_K){
									if(cnt==7){
										if(MCrndx < 70)
											return zyocnv2(cnvflg, macropos(7,9));
										else
											return 0;
									}

									//����������
									//����������
									//����������
									//����������
									//����������
									if(ban1[macropos(7,9)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(7,6));

										//����������
										//����������
										//����������
										//����������
										//����������
										if(ban1[macropos(7,6)]==ISHI_K){

											//����������
											//����������
											//����������
											//����������
											//����������
											//����������
											if(ban1[macropos(7,10)]==ISHI_S){

												//����������
												//����������
												//����������
												//����������
												//����������
												//����������
												if(ban1[macropos(7,11)]==ISHI_K){

													//����������
													//����������
													//����������
													//����������
													//����������
													//����������
													if(ban1[macropos(8,7)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(10,11));
													}
												}
											}
										}
									}

									//����������
									//����������
									//����������
									//����������
									//����������
									if(ban1[macropos(7,6)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(7,9));
									}
								}
							}

							//����������
							//����������
							//����������
							//����������
							//����������
							if(ban1[macropos(10,7)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(8,7));

								//����������
								//����������
								//����������
								//����������
								//����������
								if(ban1[macropos(8,7)]==ISHI_K){

									//����������
									//����������
									//����������
									//����������
									//����������
									if(ban1[macropos(9,8)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,10));
									}
								}
							}

							//������������
							//������������
							//������������
							//������������
							//������������
							if(ban1[macropos(11,9)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(8,7));

								//������������
								//������������
								//������������
								//������������
								//������������
								if(ban1[macropos(8,7)]==ISHI_K){

									//������������
									//������������
									//������������
									//������������
									//������������
									if(ban1[macropos(9,8)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(10,8));

										//������������
										//������������
										//������������
										//������������
										//������������
										if(ban1[macropos(10,8)]==ISHI_K){

											//������������
											//������������
											//������������
											//������������
											//������������
											if(ban1[macropos(10,10)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(8,9));
											}
										}
									}

									//������������
									//������������
									//������������
									//������������
									//������������
									if(ban1[macropos(10,8)]==ISHI_S){

										//������������
										//������������
										//������������
										//������������
										//������������
										if(ban1[macropos(8,6)]==ISHI_K){

											//������������
											//������������
											//������������
											//������������
											//������������
											if(ban1[macropos(8,9)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(7,6));
											}
										}
									}
								}
							}

							//����������
							//����������
							//����������
							//����������
							//����������
							if(ban1[macropos(8,9)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,8));

								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								if(ban1[macropos(10,8)]==ISHI_K){

									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									if(ban1[macropos(10,7)]==ISHI_S){

										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										if(ban1[macropos(11,7)]==ISHI_K){

											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											if(ban1[macropos(12,6)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(9,8));

												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												if(ban1[macropos(9,8)]==ISHI_K){

													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													if(ban1[macropos(11,8)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(10,10));
													}
												}
											}
										}
									}
								}
							}

							//��������������
							//��������������
							//��������������
							//��������������
							//��������������
							if(ban1[macropos(8,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(11,9));

								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								if(ban1[macropos(11,9)]==ISHI_K){

									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									if(ban1[macropos(12,9)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,10));
									}

									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									if(ban1[macropos(8,9)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(11,11));
									}
								}
							}

							//����������
							//����������
							//����������
							//����������
							//����������
							if(ban1[macropos(6,8)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,8));
							}
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(8,10)]==ISHI_S){
						if(cnt==4)
							return zyocnv2(cnvflg, macropos(7,7));

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(7,7)]==ISHI_K){

							//����������
							//����������
							//����������
							//����������
							//����������
							if(ban1[macropos(6,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(7,8));
							}
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(8,9)]==ISHI_S){
						if(cnt==4)
							return zyocnv2(cnvflg, macropos(7,7));

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(7,7)]==ISHI_K){

							//����������
							//����������
							//����������
							//����������
							//����������
							if(ban1[macropos(6,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,10));

								//������������
								//������������
								//������������
								//������������
								//������������
								//������������
								if(ban1[macropos(10,10)]==ISHI_K){

									//������������
									//������������
									//������������
									//������������
									//������������
									//������������
									if(ban1[macropos(11,11)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,10));

										//������������
										//������������
										//������������
										//������������
										//������������
										//������������
										if(ban1[macropos(8,10)]==ISHI_K){
											if(cnt==9)
												return zyocnv2(cnvflg, macropos(7,11));

											//������������
											//������������
											//������������
											//������������
											//������������
											//������������
											if(ban1[macropos(7,11)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(9,11));

												//������������
												//������������
												//������������
												//������������
												//������������
												//������������
												if(ban1[macropos(9,11)]==ISHI_K){

													//������������
													//������������
													//������������
													//������������
													//������������
													//������������
													if(ban1[macropos(9,10)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(10,8));

														//������������
														//������������
														//������������
														//������������
														//������������
														//������������
														if(ban1[macropos(10,8)]==ISHI_K){

															//������������
															//������������
															//������������
															//������������
															//������������
															//������������
															if(ban1[macropos(7,8)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(10,11));

																//������������
																//������������
																//������������
																//������������
																//������������
																//������������
																if(ban1[macropos(10,11)]==ISHI_K){

																	//������������
																	//������������
																	//������������
																	//������������
																	//������������
																	//������������
																	if(ban1[macropos(10,9)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(12,8));
																	}
																}
															}
														}
													}

													//������������
													//������������
													//������������
													//������������
													//������������
													//������������
													if(ban1[macropos(7,9)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(9,8));

														//������������
														//������������
														//������������
														//������������
														//������������
														//������������
														if(ban1[macropos(9,8)]==ISHI_K){

															//������������
															//������������
															//������������
															//������������
															//������������
															//������������
															if(ban1[macropos(11,10)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(10,8));

																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																if(ban1[macropos(10,8)]==ISHI_K){

																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	if(ban1[macropos(11,8)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(11,7));

																		//��������������
																		//��������������
																		//��������������
																		//��������������
																		//��������������
																		//��������������
																		if(ban1[macropos(11,7)]==ISHI_K){

																			//��������������
																			//��������������
																			//��������������
																			//��������������
																			//��������������
																			//��������������
																			if(ban1[macropos(12,6)]==ISHI_S){
																				if(cnt==18)
																					return zyocnv2(cnvflg, macropos(9,5));
																			}
																		}
																	}
																}
															}
														}
													}

													//������������
													//������������
													//������������
													//������������
													//������������
													//������������
													//������������
													if(ban1[macropos(10,12)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(9,8));

														//������������
														//������������
														//������������
														//������������
														//������������
														//������������
														//������������
														if(ban1[macropos(9,8)]==ISHI_K){

															//������������
															//������������
															//������������
															//������������
															//������������
															//������������
															//������������
															if(ban1[macropos(11,10)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(10,8));
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(10,10)]==ISHI_S){

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(7,7)]==ISHI_K){

							//������������
							//������������
							//������������
							//������������
							//������������
							//������������
							if(ban1[macropos(8,10)]==ISHI_S){

								//������������
								//������������
								//������������
								//������������
								//������������
								//������������
								if(ban1[macropos(6,6)]==ISHI_K){

									//������������
									//������������
									//������������
									//������������
									//������������
									//������������
									if(ban1[macropos(5,5)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(6,8));

										//������������
										//������������
										//������������
										//������������
										//������������
										//������������
										if(ban1[macropos(6,8)]==ISHI_K){

											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											if(ban1[macropos(9,10)]==ISHI_S){

												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												if(ban1[macropos(11,10)]==ISHI_K){

													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													if(ban1[macropos(7,8)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(6,7));
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

			//����������
			//����������
			//����������
			//����������
			//����������
			else if(ban1[macropos(8,7)]==ISHI_S){	/* ���ڂǂ�					*/
				if(cnt==2){
//					if(MCrndx < 70)
						return zyocnv2(cnvflg, macropos(9,7));
//					else
//						return 0;
				}

				//����������
				//����������
				//����������
				//����������
				//����������
				if(ban1[macropos(9,8)]==ISHI_K){
					if(cnt==3){
						tmp = MCrndx;
						if(tmp < 40)
							return zyocnv2(cnvflg, macropos(9,9));
						if(tmp < 70)
							return zyocnv2(cnvflg, macropos(7,8));
						else
							return zyocnv2(cnvflg, macropos(10,8));
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(7,8)]==ISHI_S){

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(9,6)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(9,9));
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(10,8)]==ISHI_S){

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(7,7)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(7,9));
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(9,9)]==ISHI_S){

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(7,8)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(6,8));
						}
					}
				}

				//����������
				//����������
				//����������
				//����������
				//����������
				if(ban1[macropos(9,7)]==ISHI_K){	/* �Ԍ����					*/
					if(cnt==3){
						tmp = MCrndx;
						if(tmp < 35)
							return zyocnv2(cnvflg, macropos(7,9));
						else if(tmp < 70)
							return zyocnv2(cnvflg, macropos(9,8));
						else
							return zyocnv2(cnvflg, macropos(9,9));
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(7,9)]==ISHI_S){
						if(cnt==4){
							return zyocnv2(cnvflg, macropos(10,6));
						}

						//������������
						//������������
						//������������
						//������������
						//������������
						//������������
						if(ban1[macropos(10,6)]==ISHI_K){

							//������������
							//������������
							//������������
							//������������
							//������������
							//������������
							if(ban1[macropos(9,9)]==ISHI_S){
								if(cnt==6){
									tmp = MCrndx;
									if(tmp<50)
										return zyocnv2(cnvflg, macropos(9,5));
									else
										return zyocnv2(cnvflg, macropos(9,6));
								}

								//������������
								//������������
								//������������
								//������������
								//������������
								//������������
								if(ban1[macropos(9,5)]==ISHI_K){

									//������������
									//������������
									//������������
									//������������
									//������������
									//������������
									if(ban1[macropos(8,9)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(10,9));
									}
								}
							}

							//������������
							//������������
							//������������
							//������������
							//������������
							//������������
							if(ban1[macropos(9,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,5));

								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								if(ban1[macropos(10,5)]==ISHI_K){

									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									if(ban1[macropos(11,5)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(10,8));

										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										if(ban1[macropos(10,8)]==ISHI_K){

											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											if(ban1[macropos(10,7)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(11,8));

												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												if(ban1[macropos(11,8)]==ISHI_K){

													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													if(ban1[macropos(9,8)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(11,9));

														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														if(ban1[macropos(11,9)]==ISHI_K){

															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															if(ban1[macropos(12,10)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(11,10));
															}
														}
													}
												}
											}
										}
									}

									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									if(ban1[macropos(10,7)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,5));

										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										if(ban1[macropos(8,5)]==ISHI_K){

											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											if(ban1[macropos(9,5)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(9,4));

												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												if(ban1[macropos(9,4)]==ISHI_K){

													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													if(ban1[macropos(10,4)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(8,3));

														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														if(ban1[macropos(8,3)]==ISHI_K){

															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															if(ban1[macropos(11,6)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(7,6));

																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																if(ban1[macropos(7,6)]==ISHI_K){

																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	if(ban1[macropos(10,3)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(7,4));
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(10,6)]==ISHI_S){
						if(cnt==4){
							return zyocnv2(cnvflg, macropos(7,9));
						}

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(7,9)]==ISHI_K){
							if(cnt==5)
								return zyocnv2(cnvflg, macropos(9,8));

							//������������
							//������������
							//������������
							//������������
							//������������
							//������������
							if(ban1[macropos(9,8)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,9));

								//������������
								//������������
								//������������
								//������������
								//������������
								//������������
								if(ban1[macropos(10,9)]==ISHI_K){
									if(cnt==7)
										return zyocnv2(cnvflg, macropos(11,6));

									//������������
									//������������
									//������������
									//������������
									//������������
									//������������
									if(ban1[macropos(11,6)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,9));

										//������������
										//������������
										//������������
										//������������
										//������������
										//������������
										if(ban1[macropos(8,9)]==ISHI_K){

											//������������
											//������������
											//������������
											//������������
											//������������
											//������������
											if(ban1[macropos(9,9)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(7,11));

												//������������
												//������������
												//������������
												//������������
												//������������
												//������������
												if(ban1[macropos(7,11)]==ISHI_K){

													//������������
													//������������
													//������������
													//������������
													//������������
													//������������
													if(ban1[macropos(9,6)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(8,6));

														//������������
														//������������
														//������������
														//������������
														//������������
														//������������
														if(ban1[macropos(8,6)]==ISHI_K){

															//������������
															//������������
															//������������
															//������������
															//������������
															//������������
															if(ban1[macropos(7,8)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(6,10));

																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																if(ban1[macropos(6,10)]==ISHI_K){

																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	if(ban1[macropos(5,11)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(6,9));
																	}
																
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}

							//������������
							//������������
							//������������
							//������������
							//������������
							//������������
							//������������
							if(ban1[macropos(8,6)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(7,6));

								//������������
								//������������
								//������������
								//������������
								//������������
								//������������
								//������������
								if(ban1[macropos(7,6)]==ISHI_K){

									//������������
									//������������
									//������������
									//������������
									//������������
									//������������
									//������������
									if(ban1[macropos(7,8)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(6,10));
									}
								}
							}
						}
					}

					//����������
					//����������
					//����������
					//����������
					//����������
					if(ban1[macropos(9,8)]==ISHI_S){
						if(cnt==4){
							return zyocnv2(cnvflg, macropos(10,9));
						}

						//����������
						//����������
						//����������
						//����������
						//����������
						if(ban1[macropos(10,9)]==ISHI_K){
							if(cnt==5){
								tmp = MCrndx;
								if(tmp < 30)
									return zyocnv2(cnvflg, macropos(7,9));
								else if(tmp < 55)
									return zyocnv2(cnvflg, macropos(8,9));
								else if(tmp <80)
									return zyocnv2(cnvflg, macropos(9,9));
								else
									return zyocnv2(cnvflg, macropos(10,10));
							}

							//������������
							//������������
							//������������
							//������������
							//������������
							if(ban1[macropos(10,10)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(11,9));

								//������������
								//������������
								//������������
								//������������
								//������������
								if(ban1[macropos(11,9)]==ISHI_K){

									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									if(ban1[macropos(10,6)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(11,8));

										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										if(ban1[macropos(11,8)]==ISHI_K){

											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											if(ban1[macropos(12,9)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(9,10));

												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������1
												if(ban1[macropos(9,10)]==ISHI_K){

													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������1
													if(ban1[macropos(8,11)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(11,7));

														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														if(ban1[macropos(11,7)]==ISHI_K){

															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															if(ban1[macropos(11,6)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(10,8));

																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																if(ban1[macropos(10,8)]==ISHI_K){

																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	if(ban1[macropos(12,10)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(12,6));

																		//����������������
																		//����������������
																		//����������������
																		//����������������
																		//����������������
																		//����������������
																		//����������������
																		if(ban1[macropos(12,6)]==ISHI_K){

																			//����������������
																			//����������������
																			//����������������
																			//����������������
																			//����������������
																			//����������������
																			//����������������
																			if(ban1[macropos(13,5)]==ISHI_S){
																				if(cnt==18)
																					return zyocnv2(cnvflg, macropos(12,7));

																				//����������������
																				//����������������
																				//����������������
																				//����������������
																				//����������������
																				//����������������
																				//����������������
																				if(ban1[macropos(12,7)]==ISHI_K){

																					//����������������
																					//����������������
																					//����������������
																					//����������������
																					//����������������
																					//����������������
																					//����������������
																					if(ban1[macropos(13,6)]==ISHI_S){
																						if(cnt==20)
																							return zyocnv2(cnvflg, macropos(13,7));

																						//����������������
																						//����������������
																						//����������������
																						//����������������
																						//����������������
																						//����������������
																						//����������������
																						if(ban1[macropos(13,7)]==ISHI_K){

																							//����������������
																							//����������������
																							//����������������
																							//����������������
																							//����������������
																							//����������������
																							//����������������
																							if(ban1[macropos(10,7)]==ISHI_S){
																								if(cnt==22)
																									return zyocnv2(cnvflg, macropos(9,9));

																								//����������������
																								//����������������
																								//����������������
																								//����������������
																								//����������������
																								//����������������
																								//����������������
																								if(ban1[macropos(9,9)]==ISHI_K){

																									//����������������
																									//����������������
																									//����������������
																									//����������������
																									//����������������
																									//����������������
																									//����������������
																									if(ban1[macropos(8,10)]==ISHI_S){
																										if(cnt==24)
																											return zyocnv2(cnvflg, macropos(8,9));
																									}
																								}
																							}
																						}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}

							//����������
							//����������
							//����������
							//����������
							//����������
							if(ban1[macropos(9,9)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,6));

								//������������
								//������������
								//������������
								//������������
								//������������
								//������������
								if(ban1[macropos(10,6)]==ISHI_K){

									//������������
									//������������
									//������������
									//������������
									//������������
									//������������
									if(ban1[macropos(11,5)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(10,10));

										//������������
										//������������
										//������������
										//������������
										//������������
										//������������
										if(ban1[macropos(10,10)]==ISHI_K){

											//������������
											//������������
											//������������
											//������������
											//������������
											//������������
											if(ban1[macropos(10,7)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(8,10));
											}
										}
									}
								}
							}

							//��������������
							//��������������
							//��������������
							//��������������
							//��������������
							if(ban1[macropos(7,9)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(11,9));

								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								if(ban1[macropos(11,9)]==ISHI_K){

									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									if(ban1[macropos(10,8)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(12,9));

										//����������������
										//����������������
										//����������������
										//����������������
										//����������������
										if(ban1[macropos(12,9)]==ISHI_K){

											//����������������
											//����������������
											//����������������
											//����������������
											//����������������
											if(ban1[macropos(13,9)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(10,10));
											}
										}
									}
								}
							}
						}
					}

					//��������������
					//��������������
					//��������������
					//��������������
					//��������������
					//��������������
					//��������������
					if(ban1[macropos(9,9)]==ISHI_S){
						if(cnt==4)
							return zyocnv2(cnvflg, macropos(7,9));

						//��������������
						//��������������
						//��������������
						//��������������
						//��������������
						//��������������
						//��������������
						if(ban1[macropos(7,9)]==ISHI_K){

							//��������������
							//��������������
							//��������������
							//��������������
							//��������������
							//��������������
							//��������������
							if(ban1[macropos(6,10)]==ISHI_S){
								if(cnt==6)
									return zyocnv2(cnvflg, macropos(10,6));

								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								//��������������
								if(ban1[macropos(10,6)]==ISHI_K){

									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									//��������������
									if(ban1[macropos(11,5)]==ISHI_S){
										if(cnt==8)
											return zyocnv2(cnvflg, macropos(8,6));

										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										//��������������
										if(ban1[macropos(8,6)]==ISHI_K){

											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											//��������������
											if(ban1[macropos(9,6)]==ISHI_S){
												if(cnt==10)
													return zyocnv2(cnvflg, macropos(7,8));

												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												//��������������
												if(ban1[macropos(7,8)]==ISHI_K){

													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													//��������������
													if(ban1[macropos(10,8)]==ISHI_S){
														if(cnt==12)
															return zyocnv2(cnvflg, macropos(6,8));

														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														//��������������
														if(ban1[macropos(6,8)]==ISHI_K){

															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															//��������������
															if(ban1[macropos(5,8)]==ISHI_S){
																if(cnt==14)
																	return zyocnv2(cnvflg, macropos(8,10));

																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																//��������������
																if(ban1[macropos(8,10)]==ISHI_K){

																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	//��������������
																	if(ban1[macropos(9,11)]==ISHI_S){
																		if(cnt==16)
																			return zyocnv2(cnvflg, macropos(6,4));
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
