#include<stdio.h>
#include<ncurses.h>
#include<stdlib.h>
#define CTRL(c) ((c) & 037)

/* Global value of current cursor position */
int row, col;

int main(int argc,char **argv)
{
   extern void perror(), exit();
   int i, n, l;
   int c;
   int line = 0;
   FILE *fd;

   if (argc != 2)
   {
       fprintf(stderr, "Usage: %s file\n", argv[0]);
       exit(1);
   }

   fd = fopen(argv[1], "r");
   if (fd == NULL)
   {
       perror(argv[1]);
       exit(2);
   }

   initscr();
   cbreak();
   nl();
   noecho();
   idlok(stdscr, TRUE);
   keypad(stdscr, TRUE);

   /* Read in the file */
   while ((c = getc(fd)) != EOF)
   {
       if (c == '\n')
           line++;
       if (line > LINES - 2)
           break;
       addch(c);
   }
   fclose(fd);

   move(0,0);
   refresh();
   edit();

   /* Write out the file */
   fd = fopen(argv[1], "w");
   for (l = 0; l < LINES - 1; l++)
   {
       n = len(l);
       for (i = 0; i < n; i++)
           putc(mvinch(l, i) & A_CHARTEXT, fd);
       putc('\n', fd);
   }
   fclose(fd);

   endwin();
   return 0;
}

len(int lineno)
{
   int linelen = COLS - 1;

   while (linelen >= 0 && mvinch(lineno, linelen) == ' ')
       linelen--;
   return linelen + 1;
}
edit()
{
   int c;

   while(1)
   {
       move(row, col);
       int k,j,len;
       j=0;
       k=0;
       char *ch1;
       refresh();
       c = getch();
       char ch;
       /* Editor commands */
       switch (c)
       {

       /* hjkl and arrow keys: move cursor
        * in direction indicated */
       case 'a':
       case KEY_LEFT:
           if (col > 0)
               col--;
           else
               flash();
           break;

       case 's':
       case KEY_DOWN:
           if (row < LINES - 1)
               row++;
           else
               flash();
           break;

       case 'w':
       case KEY_UP:
           if (row > 0)
               row--;
           else
               flash();
           break;

       case 'd':
       case KEY_RIGHT:
           if (col < COLS - 1)
               col++;
           else
               flash();
           break;
       /* i: enter input mode */
       case KEY_IC:
       case 'i':
           input();
           break;

       /* x: delete current character */
       case KEY_DC:
       case 'x':
           delch();
           break;

       /* o: open up a new line and enter input mode */
       case KEY_IL:
       case 'o':
           move(++row, col = 0);
           insertln();
           input();
           break;

       /* d: delete current line */
       case KEY_DL:
       case 'g':
           deleteln();
           break;

       /* ^L: redraw screen */
       case KEY_CLEAR:
       case CTRL('L'):
           wrefresh(curscr);
           break;

       /* w: write and quit */
       case 'z':
           return;
       /* q: quit without writing */
       case 'q':
           endwin();
           exit(2);
      case 'e': //clear everything from the input file
           clear();
           break; 
      case 'c':ch=inch();
               break;
       case 'p':move(row,col);   //paste
                 insch(ch);
                 break;
       case 'm': ch=inch();        //copying a word
                 len=0;
                 while(ch!=' ')
                 {
                  move(row,++col);
		  len++;
		  ch=inch(); 
                }
                 col=col-len;                 
		 move(row,col);
                 ch1=(char *)malloc(len);
                 j=0;
                 while(j<len)
                 {
                   ch=inch();
                   ch1[j++]=ch;
                   move(row,++col);
                 }
                 break;  
       case 'v': k=len-1;          //pasting a word
                 while(k>=0)
                 {
                  insch(ch1[k--]);
                 }
                  break;
       case 't':ch=inch();         //deleting a word
                while(ch!=' ')
                {
                 ch=inch();
                 delch();
                }
                break;
       default:
           flash();
           break;
       }
   }
}

/*
* Insert mode: accept characters and insert them.
* End with ctrl+D or
*/

input()
{
   int c;
   char ch;
   standout();
   mvaddstr(LINES - 1, COLS - 20, "INPUT MODE");
   standend();
   move(row, col);
   refresh();
   for (;;)
   {
       c = getch();
       if (c == CTRL('D') || c == KEY_EIC)
           break;
      else if(c=='\n')
        { 
         /* row++;col=0;
          move(row,col);
          */col=0;row++;
           move(row,col);
           insertln();              
          }
      else if(c==CTRL('b'))
       move(++row,col);
      else if(c==CTRL('u'))
       move(--row,col);
      else if(c==CTRL('l'))
       move(row,--col);
      else if(c==CTRL('r'))
       move(row,++col);   
      else if(c=='M')
             {
              if(col==0)
              {
               row--;
               col=len(row);
              }
              move(row,--col);
             delch();
            }
      else 
      { 
        insch(c);
        move(row,++col);
        refresh();
      }
   }
   move(LINES - 1, COLS - 20);
   clrtoeol();
   move(row, col);
   refresh();
}
