PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
COMMIT;

create table x_label(
iIndex integer primary key,
iPos integer,
iVal integer);

/*
Axis_Soft_Para
���˶����û����õĿ��Ʋ����������
*/
create table label(
iIndex integer primary key,
cSymbol varchar,-- ���־
bUse integer, -- ��ʾʹ��ĳһ��
bRotateLabel integer,-- �Ƿ���ת��
iChibu integer,
bPosMonitor integer,-- ��դ�߼���־
fRasterLinear integer,-- ��դ��ϵ��
iPosPrecision integer,--��դ�����
bLimitNoUse Integer,-- ��ʾ��λ���ز�ʹ�ã�A/C�������ã����඼����ʹ��
iZeroMakeUp integer,-- ��λ����
iLowerPos integer,-- ����λ��ֵ
iTopPos integer, -- ����λ��ֵ
iLocateArea integer,-- ��������,0,1,2..
bDirectMotor integer -- ֱ�����);

/*
���˶�ʹ�õĲ���
Axis_Para_Common
*/
create table label_var(
iIndex integer primary key,-- 
iMachPos integer,-- ��еλ��
bDir integer, -- �µ��˶�����
iRelSet_0 integer,
iRelSet_1 integer,
iRelSet_2 integer,
iRelSet_3 integer,
iRelSet_4 integer,
iRelSet_5 integer);

create table total(
iIndex integer primary key,
filename varchar,
workindex integer,
prune varchar,
total varchar,
iAxisLabel integer);

create table elec_oral (
id int primary key,
iTimeMin int,         -- ��Сʱ��
iTimeMax int,           -- ���ʱ��
iRisePos int,          -- ����λ��
iSafePos int,          -- ��ȫλ��
iJudgePos int,         -- ����λ��
iRepeatCount int,      -- �ظ�����
iRepeatLen int,        -- �ظ�����
iBottomSleep int,      -- �ײ�ͣЪ
iMillServo int,        -- ϳ���ŷ�
iOpLenAll int,         -- �����
iOpHoleIndex int,      -- �ӹ��׺�
iOpHoleAll int,        -- �ӹ��ܿ���
bRotateValidate int,  -- ��ת��Ч
bContinueOp int,       -- �����ӹ�
filename varchar(40));

create table elec_page(
id int primary key,
iTon int,          -- ������
iToff int,         -- �����Ъ
iElecLow int,      -- �ӹ�����
iElecHigh int,     -- ��ѹ����
iServo int,        -- �ŷ�����
iFeedSense int,    -- ��������
iBackSense int,    -- ��������
iOpLen int,        -- �ӹ����\��������
iCap int,          -- �ӹ�����
iRotSpeed int,     -- R���ٶ�
elecid int, -- elec_oral id
foreign key(elecid) references elec_oral(id));