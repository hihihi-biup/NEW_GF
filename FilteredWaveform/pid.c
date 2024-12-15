
#include <stdio.h> 
#include<math.h>

//定义PID 的结构体 
struct _pid
{
  int pv;         // integer that contains the process value 过程量 
  int sp;         // integer that contains the set point   设定值 
  float integral; // 积分值 －－ 偏差累计值 
  float p_gain;
  float i_gain;
  float d_gain;
  int deadband;   //死区 
  int last_error;
};

struct _pid warm, *pid;
int process_point, set_point, dead_band;
float p_gain, i_gain, d_gain, integral_val, new_integ;

/* 
pid_init——该函数初始化
_pid结构中指向进程变量的指针
以及设定值。*pv和*sp是整数指针。
 */

void pid_init( struct _pid *warm, int process_point, int set_point )
{
  struct _pid *pid;
  pid = warm;
  pid->pv = process_point;
  pid->sp = set_point;
}

/*
pid_tune——设置比例增益（p_gain），积分增益（i_gain），
导数增益（d_gain）和死频带（dead_band）
一个pid控制结构_pid。

 */

void pid_tune( struct _pid *pid,
  float p_gain,
  float i_gain,
  float d_gain,
  int dead_band )
{
  pid->p_gain = p_gain;
  pid->i_gain = i_gain;
  pid->d_gain = d_gain;
  pid->deadband = dead_band;
  pid->integral = integral_val;
  pid->last_error = 0;
}

/*
pid_setinteg——为pid方程的积分项设置一个新值。
这对于在启动时设置pid控制器的初始输出是有用的。

 设定输出初始值 
 */

void pid_setinteg( struct _pid *pid, float new_integ )
{
  pid->integral = new_integ;
  pid->last_error = 0;
}

/*
 
pid_bumpless——无峰转移算法。
当突然改变设定值时，或重新启动PID方程时
在长时间的停顿之后，方程的导数可能会引起颠簸
在控制器输出中。这个函数将帮助消除凸起。
*pv中的进程值应该在使用该函数之前更新。
 */

void pid_bumpless( struct _pid *pid )
{
  pid->last_error = ( pid->sp ) - ( pid->pv );  //设定值与反馈值偏差 
}

/* 
pid_calc——对_pid结构* a执行PID计算。
该函数使用pid的位置形式
方程，并包含一个积分发条预防算法。
用了矩形积分，所以这个函数必须
在一致的时间基础上重复进行精确控制。
RETURN VALUE pid循环的新输出值。
 */

float pid_calc( struct _pid *pid )
{
  int err;
  float pterm, dterm, result, ferror;
  
  // 计算偏差 
  err = ( pid->sp ) - ( pid->pv );
  
  // 判断是否大于死区 
  if ( abs( err ) > pid->deadband )
  {
    ferror = (float) err;   //do integer to float conversion only once 数据类型转换 
      
    // 比例项 
    pterm = pid->p_gain * ferror;
    
    if ( pterm > 100 || pterm < -100 )
    {
      pid->integral = 0.0;
    }
    else
    {
      // 积分项 
      pid->integral += pid->i_gain * ferror;
      
      // 输出为0－－100% 
      // 如果计算结果大于100，则等于100 
      if ( pid->integral > 100.0 )
      {
        pid->integral = 100.0;
      }
      // 如果计算结果小于0.0，则等于0 
      else if ( pid->integral < 0.0 )
        pid->integral = 0.0;
      
    }
    
    // 微分项 
    dterm = ( (float) ( err - pid->last_error ) ) * pid->d_gain;
    
    result = pterm + pid->integral + dterm;
  }
  else
    result = pid->integral; // 在死区范围内，保持现有输出 
      
  // 保存上次偏差 
  pid->last_error = err;
  
  // 输出PID值(0-100) 
  return ( result );
}

void main( void )
{
  float display_value;
  int count = 0;
  pid = &warm;
  
  // printf("Enter the values of Process point, Set point, P gain, I gain, D gain \n"); 
  // scanf("%d%d%f%f%f", &process_point, &set_point,&p_gain, &i_gain, &d_gain); 
  
  // 初始化参数 
  process_point = 30;
  set_point = 40;
  p_gain = (float) ( 5.2 );
  i_gain = (float) ( 0.77 );
  d_gain = (float) ( 0.18 );
  dead_band = 2;
  integral_val = (float) ( 0.01 );
  
  printf(    "The  values  of  Process  point,  Set  point,  P  gain, I gain, D gain \n" );
  printf( " %6d %6d %4f %4f %4f\n", process_point, set_point, p_gain, i_gain, d_gain );
  printf( "Enter the values of Process point\n" );
  while ( count <= 20 )
  {
    scanf( "%d", &process_point );
    
    // 设定PV,SP 值 
    pid_init( &warm, process_point, set_point );
    
    // 初始化PID 参数值 
    pid_tune( &warm, p_gain, i_gain, d_gain, dead_band );
    
    // 初始化PID 输出值 
    pid_setinteg( &warm, 0.0 );
    
    //pid_setinteg(&warm,30.0); 
    //Get input value for process point 
    pid_bumpless( &warm );
    
    // how to display output 
    display_value = pid_calc( &warm );
    
    printf( "%f\n", display_value );
    //printf("\n%f%f%f%f",warm.pv,warm.sp,warm.igain,warm.dgain); 
    
    count++;
  }
}
