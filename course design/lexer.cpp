//
// Created by 王一舟 on 2021/3/14.
//

#include "lexer.h"

/**
 * @description:将关键字和枚举类型常量值编成哈希表，用于查找对应
 * @rule: 第一个字符的ASCII码对10求模，值对应数组第一个分量下标
 * 每行最后设置哨兵none，枚举类型值对应为IDENT
 */
KeyWord_name_value KeyWordHashTable[10][10] = {
    // ascii码 mod 10 ==0:d,n,x
    {{"double", DOUBLE}, {"do", DO}, {"default", DEFAULT}, {"none", IDENT}},
    // ascii码 mod 10 ==1:e,o,y
    {{"enum", ENUM}, {"extern", EXTERN}, {"else", ELSE}, {"none", IDENT}},
    // ascii码 mod 10 ==2:f,p,z
    {{"float", FLOAT}, {"for", FOR}, {"none", IDENT}},
    // ascii码 mod 10 ==3:g,q
    {{"goto", GOTO}, {"none", IDENT}},
    // ascii码 mod 10 ==4:h,r
    {{"register", REGISTER}, {"return", RETURN}, {"none", IDENT}},
    // ascii码 mod 10 ==5:i,s
    {{"short", SHORT},
     {"int", INT},
     {"struct", STRUCT},
     {"sighed", SIGNED},
     {"static", STATIC},
     {"if", IF},
     {"switch", SWITCH},
     {"sizeof", SIZEOF},
     {"none", IDENT}},
    // ascii码 mod 10 ==6:j,t
    {{"typedef", TYPEDEF}, {"none", IDENT}},
    // ascii码 mod 10 ==7:a,k,u
    {{"auto", AUTO}, {"union", UNION}, {"unsigned", UNSIGNED}, {"none", IDENT}},
    // ascii码 mod 10 ==8:b,l,v
    {{"long", LONG},
     {"volatile", VOLATILE},
     {"void", VOID},
     {"break", BREAK},
     {"none", IDENT}},
    // ascii码 mod 10 ==9:c,m,w
    {{"char", CHAR},
     {"const", CONST},
     {"case", CASE},
     {"while", WHILE},
     {"continue", CONTINUE},
     {"none", IDENT}},
}; // end of KeyWordHashTable

/**
 * @description: 检查字符串是否为关键字
 * @param {string}字符串
 * @return {enum token_kind}对应枚举类型常量值
 * @call: (none)
 * @called_by: get_token
 */
int isKeyWord(char str[]) {
  int line = str[0] % 10;
  for (int column = 0;; column++) {
    if (strcmp(str, KeyWordHashTable[line][column].name) == 0 ||
        strcmp("none", KeyWordHashTable[line][column].name) == 0)
      return KeyWordHashTable[line][column].value;
  }
} // end of isKeyWord

/**
 * @description: 判断字符是否为数字
 * @param {char} c
 * @return {bool}是数字返回true，否则返回false
 * @call: (none)
 * @called_by: get_token
 */
bool isNum(char c) {
  if ((c >= '0' && c <= '9') || c == '.' || c == '-')
    return true;
  else
    return false;
} // end of isNum

/**
 * @description: 判断字符是否为字母
 * @param {char} c
 * @return {bool}是字母返回true，不是返回false
 * @call: (none)
 * @called_by:get_token
 */
bool isLetter(char c) {
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
    return true;
  else
    return false;
} // end of isLetter

/**
 * @description: 对源程序进行词法分析,并打印种类码和自身值
 * @param {FILE} *p 文件指针
 * @param {int} &line_num 当前行数，用于报错
 * @param {char} token_text 单词的自身值
 * @return {int} 单词的种类码(包括报错符)，或EOF
 * @call: enum token_kind isKeyWord;
 *        bool isNum;
 *        bool isLetter
 * @called_by: functions in file "parser.cpp"
 */
token_info get_token(FILE *p, int &line_num, char token_text[]) {
  char c;
  token_info this_token;
  token_text[0] = '\0'; //重置token_text变量
  while ((c = fgetc(p)) == ' ' || c == '\t' || c == '\n')
  //过滤空白符
  {
    if (c == '\n') //遇到换行符，行数自增
      line_num += 1;
  }
  // printf("*****ch:%c\n", c);

  if (isLetter(c))
  //以字母开头，应该为关键字或标识符
  {
    do { //读取当前标识符串并存入字符串变量token_text中
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
    } while (isLetter(c = fgetc(p)) || isNum(c));
    ungetc(c, p); //将多读的字符退回缓冲区
    if (isKeyWord(token_text) != IDENT) {
      printf("关键字\t\t\t%s\n", token_text);
    } else {
      printf("标识符\t\t\t%s\n", token_text);
    }
    this_token.line = line_num;
    this_token.token_kind = isKeyWord(token_text);
    strcpy(this_token.token_text, token_text);
    return this_token;
  } // end of if(isLetter)

  if (isNum(c))
  //以数字开头，应该是整型、浮点型常量
  {
    bool isFloat = false;
    bool isHex = false; //十六进制
    if (c == '.')
      isFloat = true;
    do {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      if (c == '.' || (!isHex && (c == 'e' || c == 'E')))
        isFloat = true;
      if (c == 'x' || c == 'X')
        isHex = true;
      if ((c >= 'a' && c <= 'e') || (c >= 'A' && c <= 'E')) {
        if (!isHex)
          break;
      }
    } while (isNum(c = fgetc(p)) || (c >= 'a' && c <= 'e') ||
             (c >= 'A' && c <= 'E') || c == 'x' || c == 'X');
    ungetc(c, p);
    c = fgetc(p);

    /**********************处理常量后缀**********************/
    if (c == 'u' || c == 'U')
    //可能为UNSIGNED_CONST、UNSIGNED_LONG_CONST
    {
      //浮点型变量不能有'u'、'U'后缀
      if (isFloat) {
        printf("错误的浮点型常量\t%s(in line %d)\n", token_text, line_num);
        this_token.line = line_num;
        this_token.token_kind = ERROR_TOKEN;
        strcpy(this_token.token_text, token_text);
        return this_token;
      }
      if ((c = fgetc(p)) == 'l' || c == 'L') {
        printf("无符号长整型常量\t%s\n", token_text);
        this_token.line = line_num;
        this_token.token_kind = UNSIGNED_LONG_CONST;
        strcpy(this_token.token_text, token_text);
        return this_token;
      } else {
        ungetc(c, p);
        printf("无符号整型常量\t%s\n", token_text);
        this_token.line = line_num;
        this_token.token_kind = UNSIGNED_CONST;
        strcpy(this_token.token_text, token_text);
        return this_token;
      }
    } // end of if
    else if (c == 'l' || c == 'L')
    //可能为LONG_CONST、UNSIGNED_LONG_CONST、LONG_DOUBLE_CONST
    {
      if ((c = fgetc(p)) == 'u' || c == 'U') {
        //浮点型变量不能有'u'、'U'后缀
        if (isFloat) {
          printf("错误的浮点型常量\t%s(in line %d)\n", token_text, line_num);
          this_token.line = line_num;
          this_token.token_kind = ERROR_TOKEN;
          strcpy(this_token.token_text, token_text);
          return this_token;
        } else {
          printf("无符号整型常量\t%s\n", token_text);
          this_token.line = line_num;
          this_token.token_kind = UNSIGNED_LONG_CONST;
          strcpy(this_token.token_text, token_text);
          return this_token;
        }
      } // end of if
      else {
        ungetc(c, p);
        if (isFloat) {
          printf("长浮点型常量\t\t%s\n", token_text);
          this_token.line = line_num;
          this_token.token_kind = LONG_DOUBLE_CONST;
          strcpy(this_token.token_text, token_text);
          return this_token;
        } else {
          printf("长整型常量\t\t%s\n", token_text);
          this_token.line = line_num;
          this_token.token_kind = LONG_CONST;
          strcpy(this_token.token_text, token_text);
          return this_token;
        }
      } // end of else
    }   // end of if (c == 'l' || c == 'L')
    else if (c == 'f' || c == 'F')
    //为单精度浮点型常量
    {
      printf("浮点型常量\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = FLOAT_CONST;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } // end of if

    //若常量无后缀
    else {

      ungetc(c, p);
      if (isFloat) {
        printf("双精度浮点型常量\t%s\n", token_text);
        this_token.line = line_num;
        this_token.token_kind = DOUBLE_CONST;
        strcpy(this_token.token_text, token_text);
        return this_token;
      } else {
        printf("整型常量\t\t\t%s\n", token_text);
        this_token.line = line_num;
        this_token.token_kind = INT_CONST;
        strcpy(this_token.token_text, token_text);
        return this_token;
      }
    } // end of else
  }   // end of if(isNum)

  /**********************判断特殊符号**********************/
  token_text[strlen(token_text) + 1] = '\0';
  token_text[strlen(token_text)] = c;
  switch (c) {
  case '#':
    token_text[strlen(token_text) + 1] = '\0';
    token_text[strlen(token_text)] = c;
    printf("未知的标识符\t\t%s(in line %d)\n", token_text, line_num);
    this_token.line = line_num;
    this_token.token_kind = ERROR_TOKEN;
    strcpy(this_token.token_text, token_text);
    return this_token;
  case '+':
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("加等号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = PLUS_EQ;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("加号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = PLUS;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '-':
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("减等号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = MINUS_EQ;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("减号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = MINUS;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '*':
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("乘等号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = MULTIPLY_EQ;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("乘号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = MULTIPLY;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '/':
    //不考虑转义
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("除等号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = DIVIDE_EQ;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("除等号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = DIVIDE;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
  case '%':
    //未考虑类似：printf("%d",number)的情形
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("模等号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = MOD_EQ;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("求模号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = MOD;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '=':
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("相等号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = EQUAL;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("赋值号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = ASSIGN;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '!':
    //未考虑逻辑非运算
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("不等号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = UNEQUAL;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("错误的标识符\t\t%s(in line %d)\n", token_text, line_num);
      this_token.line = line_num;
      this_token.token_kind = ERROR_TOKEN;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '<':
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("小于等于号\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = LESS_OR_EQUAL;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("小于号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = LESS;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '>':
    if ((c = fgetc(p)) == '=') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("大于等于号\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = MORE_OR_EQUAL;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("大于号\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = MORE;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '&':
    //未考虑按位与运算
    if ((c = fgetc(p)) == '&') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("逻辑与\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = AND;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("错误的标识符\t\t%s(in line %d)\n", token_text, line_num);
      this_token.line = line_num;
      this_token.token_kind = ERROR_TOKEN;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '|':
    //未考虑按位或运算
    if ((c = fgetc(p)) == '|') {
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
      printf("逻辑或\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = OR;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      ungetc(c, p);
      printf("错误的标识符\t\t%s(in line %d)\n", token_text, line_num);
      this_token.line = line_num;
      this_token.token_kind = ERROR_TOKEN;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case ',':
    printf("逗号\t\t\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = COMMA;
    strcpy(this_token.token_text, token_text);
    return this_token;
    break;
  case ';':
    printf("分号\t\t\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = SEMICOLON;
    strcpy(this_token.token_text, token_text);
    return this_token;
    break;
  case '(':
    printf("左圆括号\t\t\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = LPARENTHESE;
    strcpy(this_token.token_text, token_text);
    return this_token;
    break;
  case ')':
    printf("右圆括号\t\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = RPARENTHESE;
    strcpy(this_token.token_text, token_text);
    return this_token;
    break;
  case '[':
    printf("左方括号\t\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = LSUBSCRIPT;
    strcpy(this_token.token_text, token_text);
    return this_token;
    break;
  case ']':
    printf("右方括号\t\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = RSUBSCRIPT;
    strcpy(this_token.token_text, token_text);
    return this_token;
    break;
  case '{':
    printf("左花括号\t\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = LCURLYBRACE;
    strcpy(this_token.token_text, token_text);
    return this_token;
    break;
  case '}':
    printf("右花括号\t\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = RCURLYBRACE;
    strcpy(this_token.token_text, token_text);
    return this_token;
    break;
  case '\'':
    do {
      c = fgetc(p);
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
    } while (c != '\'');
    if (strlen(token_text) == 3 ||
        (strlen(token_text) == 4 && token_text[1] == '\\')) {
      printf("字符常量\t\t\t%s\n", token_text);
      this_token.line = line_num;
      this_token.token_kind = CHAR_CONST;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      printf("错误的字符常量\t\t%s(in line %d)\n", token_text, line_num);
      this_token.line = line_num;
      this_token.token_kind = ERROR_TOKEN;
      strcpy(this_token.token_text, token_text);
      return this_token;
    }
    break;
  case '\"':
    do {
      c = fgetc(p);
      if (c == '\n')
        line_num++;
      token_text[strlen(token_text) + 1] = '\0';
      token_text[strlen(token_text)] = c;
    } while (c != '\"');
    printf("字符串常量\t\t%s\n", token_text);
    this_token.line = line_num;
    this_token.token_kind = STRING_CONST;
    strcpy(this_token.token_text, token_text);
    return this_token;
  default:
    if (feof(p)) {
      printf("文件结束\n");
      this_token.line = line_num;
      this_token.token_kind = EOF;
      strcpy(this_token.token_text, token_text);
      return this_token;
    } else {
      printf("未知的标识符\t\t%s (in line %d)\n", token_text, line_num);
      this_token.line = line_num;
      this_token.token_kind = ERROR_TOKEN;
      strcpy(this_token.token_text, token_text);
      return this_token; //报错符号
    }
    break;
  } // end of switch(c)（特殊符号判断）
} // end of fun: int get_token;
