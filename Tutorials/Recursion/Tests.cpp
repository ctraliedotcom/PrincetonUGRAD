#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int example1(int x){
if(x>2){
return example1(x-1) + 2*x;}
else{
return 2;}
}

int example2(int x, int y){
if(x > y){
return example2(x-2,y+1) + 3;}
else if(x < y){
return example2(x+1,y-3) + 2;}
else{
return 5;}
}

long hanoi(long l){
     if(l>1){
             return 2*hanoi(l-1) + 1;
             }
     else{
          return 1;
          }
     }


int main(){
 cout<<hanoi(15);
 system("pause");
 return 0;   
}


/*
int main(){
 char a=0x0;
for(int i=0; i<256; i++){ 
 a++;
 cout<<a<<"  ";
}
 system("pause");
 return 0;   
}
*/
