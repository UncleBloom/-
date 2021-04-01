#pragma once
#include "Traverse.h"
#include "lexer.h"
#include "parsing.h"
#include "pre_process.h"
#include "print_file.h"

int main() {
  char source_file_name[100], mid_file_name[110];
  printf("源程序文件名称(不含文件后缀名.c)：\n");
  scanf("%s", source_file_name);
  sprintf(mid_file_name, "mid_%s.c", source_file_name);
  sprintf(source_file_name, "%s.c", source_file_name);
  if (!pre_process(source_file_name, mid_file_name))
    printf("文件打开失败！\n");
  FILE *mid_file = fopen(mid_file_name, "r");
  int line = 1;
  char token_text[50];
  Queue Q;
  iniQueue(Q);
  printf("\n**********进行词法分析和语法分析**********\n");
  p_treeNode program = Program(Q, mid_file, line, token_text);
  printf("\n**********格式化输出语法分析树**********\n");
  traverse(program, 0);
  printf("\n**********将代码格式化输出至文件%s**********\n", mid_file_name);
  fclose(mid_file);
  mid_file = fopen(mid_file_name, "w");
  if (!mid_file) {
    printf("文件打开失败\n");
    return 0;
  }
  f_traverse(mid_file, program, 0, true);

  return 0;
}