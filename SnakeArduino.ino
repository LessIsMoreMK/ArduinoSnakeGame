#include <LiquidCrystal.h>

enum ButtonType { up, down, left, right, none };

LiquidCrystal lcd(6, 7, 8, 9, 10, 11);
unsigned long time, timeNow;
int gameSpeed;
boolean skip, gameOver;
int olddir;
int selectedLevel,levels;
 
int adc_key_val[5] ={50, 200, 400, 600, 800 };
int NUM_KEYS = 5;
int adc_key_in;
int key=-1;
int oldkey=-1;
 
boolean x[16][80];
byte myChar[8];
byte nullChar[8] = { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
boolean special;

int btn;
 
struct partdef
{
  int row,column,dir;
  struct partdef *next;
};
typedef partdef part;
 
part *head, *tail;
int i,j,collected = 0 ;
long pc,pr;

void newPoint()
{
  part *p;
  p = tail;
  boolean newp = true;
  while (newp)
  {
    pr = random(16);
    pc = random(80);
    newp = false;
    while (p->next != NULL && !newp)
    {
      if (p->row == pr && p->column == pc) newp = true;
      p = p->next;
    }
  }
 
  if (collected < 13)
  {
    growSnake();
  }
}

void drawMatrix()
{
  int cc=0;
  if (!gameOver)
  {
  x[pr][pc] = true;

  for(int r=0;r<2;r++)
  {
    for(int c=0;c<16;c++)
    {
      special = false;
      for(int i=0;i<8;i++)
      {
        byte b=B00000;
        if (x[r*8+i][c*5+0]) {b+=B10000; special = true;}
        if (x[r*8+i][c*5+1]) {b+=B01000; special = true;}
        if (x[r*8+i][c*5+2]) {b+=B00100; special = true;}
        if (x[r*8+i][c*5+3]) {b+=B00010; special = true;}
        if (x[r*8+i][c*5+4]) {b+=B00001; special = true;}
        myChar[i] = b;
      }
      
      if (special)
      {
        lcd.createChar(cc, myChar);
        lcd.setCursor(c,r);
        lcd.write(byte(cc));
        cc++;
      }
      
      else
      {
        lcd.setCursor(c,r);
        lcd.print(' ');
      }
    }
  }
  }
}
 
void freeList()
{
  part *p,*q;
  p = tail;
  while (p!=NULL)
  {
    q = p;
    p = p->next;
    free(q);
  }
  head = tail = NULL;
}
 
void growSnake()
{
  part *p;
  p = (part*)malloc(sizeof(part));
  p->row = tail->row;
  p->column = tail->column;
  p->dir = tail->dir;
  p->next = tail;
  tail = p;
}
 
void moveHead()
{
  switch(head->dir) 
  {
    case up: head->row--; break;
    case down: head->row++; break;
    case right: head->column++; break;
    case left: head->column--; break;
    default : break;
  }
  if (head->column >= 80) head->column = 0;
  if (head->column < 0) head->column = 79;
  if (head->row >= 16) head->row = 0;
  if (head->row < 0) head->row = 15;
 
  part *p;
  p = tail;
  while (p != head && !gameOver) 
  {
    if (p->row == head->row && p->column == head->column) gameOver = true;
    p = p->next;
  }
 
  x[head->row][head->column] = true;
 
  if (head->row == pr && head->column == pc) 
  {
    collected++;
    if (gameSpeed < 25) gameSpeed+=3;
    newPoint();
  }
}
 
void moveAll()
{
  part *p;
  p = tail;
  x[p->row][p->column] = false;
  while (p->next != NULL)
  {
    p->row = p->next->row;
    p->column = p->next->column;
    p->dir = p->next->dir;
    p = p->next;
  }
  moveHead();
}
 
void createSnake(int n) // n = size of snake
{
  for (i=0;i<16;i++)
    for (j=0;j<80;j++)
      x[i][j] = false;
         
  part *p, *q;
  tail = (part*)malloc(sizeof(part));
  tail->row = 7;
  tail->column = 39 + n/2;
  tail->dir = left;
  q = tail;
  x[tail->row][tail->column] = true;
  for (i = 0; i < n-1; i++) 
  {
    p = (part*)malloc(sizeof(part));
    p->row = q->row;
    p->column = q->column - 1;
    x[p->row][p->column] = true;
    p->dir = q->dir;
    q->next = p;
    q = p;
  }
  if (n>1)
  {
    p->next = NULL;
    head  = p;
  }
  else
  {
    tail->next = NULL;
    head = tail;
  }
}

void setup()
{ 
  gameSpeed = 3;
  gameOver = false;
  lcd.begin(16, 2);
  Serial.begin(9600);
  
  pinMode(5, INPUT);
  pinMode(4, INPUT);

  freeList();
  createSnake(3);
  countDown(3);
}

void countDown(int time){
  for(int i = time; i > 0; i--){
    lcd.setCursor(7,0);
    lcd.print(i);
    delay(1000);
  }
  lcd.clear();
}

void resetGame(){
  delay(3000);
  lcd.clear();
  lcd.home();
  lcd.print("Score: ");
  lcd.setCursor(7,0);
  lcd.print(collected);
  delay(3000);
  lcd.clear();
  
  gameOver = false;
  freeList();
  createSnake(3);
  countDown(3);
}

void loop()
{
  
  int right = digitalRead(5);
  int left = digitalRead(4);
  
  if(right)
  {
    if(head->dir == 1  ) { head ->dir = 2; }
    else if(head->dir == 0 ) { head ->dir = 3; }
    else if(head->dir == 2 ) { head ->dir = 0; }
    else if(head->dir == 3 ) { head ->dir = 1; }
  }
  
  if(left)
  {
    if(head->dir == 1  ) { head ->dir = 3; }
    else if(head->dir == 0    ) { head ->dir = 2; }
    else if(head->dir == 2  ) { head ->dir = 1; }
    else if(head->dir == 3 ) { head ->dir = 0; }
  }

  delay(1000/gameSpeed);
  moveAll();
  drawMatrix();
  
  if(gameOver){
    resetGame();
  }
}
 