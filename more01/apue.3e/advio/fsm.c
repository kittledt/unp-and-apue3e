/*******************
	fsm :  read from tty11 and write to tty12, read from tty2 and write to tty1.
	1, fsm_st   fsm data struct 
	2, fsm_driver(state)  -----> define  state A --> state A/B/C/D. 
	3, relay(fd1, fd2)  ----   init  state,  and drive(if not Terminal state, loop drive )
	4, fsm states enum


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE        1024
#define TTY1        "/dev/tty11"
#define TTY2        "/dev/tty12"

/* ״̬���ĸ���״̬ */
enum
{
    STATE_R=1,
    STATE_W,
    STATE_Ex,
    STATE_T
};

/* ״̬�������ݲ�ͬ��������Ʋ�ͬ�ĳ�Ա */
struct fsm_st
{
    int state; // ״̬����ǰ��״̬
    int sfd; // ��ȡ����Դ�ļ�������
    int dfd; // д���Ŀ���ļ�������
    char buf[BUFSIZE]; // ����
    int len; // һ�ζ�ȡ����ʵ��������
    int pos; // buf ��ƫ���������ڼ�¼���д�� n ���ֽ�ʱÿ��ѭ��д��������
    char *errstr; // ������Ϣ
 };
 
 /* ״̬������ */
static void fsm_driver(struct fsm_st *fsm)
{
    int ret;

    switch(fsm->state)
   {
         case STATE_R: // ��̬
            fsm->len = read(fsm->sfd,fsm->buf,BUFSIZE);
            if(fsm->len == 0) // �������ļ�ĩβ����״̬������ T̬
               fsm->state = STATE_T;
            else if(fsm->len < 0) // ��ȡ�����쳣
            {
               if(errno == EAGAIN) // ����Ǽٴ���Ƶ� ��̬�����¶�һ��
                    fsm->state = STATE_R;
                else // ����������Ƶ� �쳣̬
                 {
                     fsm->errstr = "read()";
                     fsm->state = STATE_Ex;
                 }
            }
            else // �ɹ���ȡ�������ݣ���״̬���Ƶ� д̬
            {
                fsm->pos = 0;
                fsm->state = STATE_W;
            }
             break;

       case STATE_W: // д̬
             ret = write(fsm->dfd,fsm->buf+fsm->pos,fsm->len);
            if(ret < 0) // д������쳣
             {
                 if(errno == EAGAIN) // ����Ǽٴ���ٴ��Ƶ� д̬��������д��һ��
                    fsm->state = STATE_W;
                 else // ����������Ƶ� �쳣̬
                 {
                      fsm->errstr = "write()";
                      fsm->state = STATE_Ex;
                  }
              }
              else // �ɹ�д��������
              {
                  fsm->pos += ret;
                  fsm->len -= ret;
                  if(fsm->len == 0) // �����������������ȫд��ȥ�˾ͽ�״̬������ ��̬����ʼ��һ�ֶ�ȡ
                      fsm->state = STATE_R;
                  else // ���û�н�������������ȫд��ȥ����ô״̬����Ȼ�Ƶ� д̬���´μ���д��ûд������ݣ�ʵ�֡����д�� n ���ֽڡ�
                      fsm->state = STATE_W;
             }
  
             break;
     
         case STATE_Ex: // �쳣̬����ӡ�쳣����״̬���Ƶ� T̬
             perror(fsm->errstr);
             fsm->state = STATE_T;
             break;
  
          case STATE_T: // ����̬������������н���̬û��ʲô��Ҫ�������飬���Կ���
             /*do sth */
             break;
         default: // ����ܿ��ܷ���������Ȳ���Ԥ�ϵ������Ϊ�˱����쳣����ֱ����ɱ
             abort();
     }
 
 }
  
 /* �ƶ�״̬�� */
 static void relay(int fd1,int fd2)
 {
     int fd1_save,fd2_save;    
     // ��Ϊ�Ƕ� tty1 д tty2���� tty2 д tty1���������������״̬��ֱ��ȡ��Ϊ fsm12 �� fsm21
     struct fsm_st fsm12,fsm21;
 
     fd1_save = fcntl(fd1,F_GETFL);
     // ʹ��״̬������ IO һ�㶼���÷���������ʽ������״̬��������
     fcntl(fd1,F_SETFL,fd1_save|O_NONBLOCK);
	fd2_save = fcntl(fd2,F_GETFL);
     fcntl(fd2,F_SETFL,fd2_save|O_NONBLOCK);
 
     //������״̬��֮ǰ��״̬������ ��̬
    fsm12.state = STATE_R;
    // ����״̬���ж�д����Դ��Ŀ�꣬����״̬���Ķ�д�ӿھ�ͳһ�ˡ���״̬�����治�ùܵ����� ��tty1 дtty2 ���� ��tty2 дtty1 �ˣ���ֻ��Ҫ֪���� ��src дdes �Ϳ����ˡ�
     fsm12.sfd = fd1;
     fsm12.dfd = fd2;
 
     // ͬ��
     fsm21.state = STATE_R;
     fsm21.sfd = fd2;
    fsm21.dfd = fd1;


     // ��ʼ��״̬����ֻҪ���� T̬ ��һֱ��
     while(fsm12.state != STATE_T || fsm21.state != STATE_T)
     {
         // ����״̬������������״̬����ʼ����
         fsm_driver(&fsm12);
         fsm_driver(&fsm21);
     }
 
     fcntl(fd1,F_SETFL,fd1_save);
     fcntl(fd2,F_SETFL,fd2_save);
     
 }
 
 int main()
 {
     int fd1,fd2;
 
     // �����������ǽ��豸 tty1 �Է���������ʽ��Ҳû��ϵ����Ϊ�ƶ�״̬��֮ǰ�������趨�ļ�������Ϊ��������ʽ
     fd1 = open(TTY1,O_RDWR);
     if(fd1 < 0)
     {
        perror("open()");
         exit(1);
     }
     write(fd1,"TTY1\n",5);
     
     fd2 = open(TTY2,O_RDWR|O_NONBLOCK);
     if(fd2 < 0)
     {
         perror("open()");
         exit(1);
    }
    write(fd2,"TTY2\n",5);


    relay(fd1,fd2);	//call realy  for fsm
 
 
    close(fd1);
    close(fd2);

 
    exit(0);

}
