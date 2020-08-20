PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
COMMIT;

create table x_label(
iIndex integer primary key,
iPos integer,
iVal integer);

/*
Axis_Soft_Para
轴运动中用户设置的控制参数软件参数
*/
create table label(
iIndex integer primary key,
cSymbol varchar,-- 轴标志
bUse integer, -- 标示使用某一轴
bRotateLabel integer,-- 是否旋转轴
iChibu integer,
bPosMonitor integer,-- 光栅尺监测标志
fRasterLinear integer,-- 光栅尺系数
iPosPrecision integer,--光栅尺误差
bLimitNoUse Integer,-- 标示限位开关不使用，A/C可以设置，其余都必须使用
iZeroMakeUp integer,-- 零位补偿
iLowerPos integer,-- 软限位下值
iTopPos integer, -- 软限位上值
iLocateArea integer,-- 所属区域,0,1,2..
bDirectMotor integer -- 直驱电机);

/*
轴运动使用的参数
Axis_Para_Common
*/
create table label_var(
iIndex integer primary key,-- 
iMachPos integer,-- 机械位置
bDir integer, -- 新的运动方向
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
iTimeMin int,         -- 最小时限
iTimeMax int,           -- 最大时限
iRisePos int,          -- 回升位置
iSafePos int,          -- 安全位置
iJudgePos int,         -- 起判位置
iRepeatCount int,      -- 重复次数
iRepeatLen int,        -- 重复长度
iBottomSleep int,      -- 底部停歇
iMillServo int,        -- 铣削伺服
iOpLenAll int,         -- 总深度
iOpHoleIndex int,      -- 加工孔号
iOpHoleAll int,        -- 加工总孔数
bRotateValidate int,  -- 旋转有效
bContinueOp int,       -- 继续加工
filename varchar(40));

create table elec_page(
id int primary key,
iTon int,          -- 脉冲宽度
iToff int,         -- 脉宽间歇
iElecLow int,      -- 加工电流
iElecHigh int,     -- 高压电流
iServo int,        -- 伺服给定
iFeedSense int,    -- 进给灵敏
iBackSense int,    -- 回退灵敏
iOpLen int,        -- 加工深度\修整长度
iCap int,          -- 加工电容
iRotSpeed int,     -- R轴速度
elecid int, -- elec_oral id
foreign key(elecid) references elec_oral(id));