//
// Created by 王一舟 on 2021/3/14.
//

#ifndef COURSE_DESIGN_TRAVERSE_H
#define COURSE_DESIGN_TRAVERSE_H
#include "parsing.h"

void printTabs(int tabs);
void printFunCall(p_treeNode fun_call);
void traverseExp(p_treeNode p);
void traverse(p_treeNode p, int tabs);

#endif //COURSE_DESIGN_TRAVERSE_H
