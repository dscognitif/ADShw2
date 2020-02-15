/* recognizeExp.c, Gerard Renardel, 29 January 2014
 *
 * In this file a recognizer acceptExpression is definined that can recognize
 * arithmetical expressions generated by the following BNF grammar:
 *
 * <expression>  ::= <term> { '+'  <term> | '-' <term> }
 *
 * <term>       ::= <factor> { '*' <factor> | '/' <factor> }
 *
 * <factor>     ::= <number> | <identifier> | '(' <expression> ')'
 *
 * Input for the recognizer is the token list constructed by the scanner (in scanner.c).
 * For the recognition of a token list the method of *recursive descent* is used.
 * It relies on the use of three functions for the recognition and processing of
 * terms, factors and expressions, respectively.
 * These three functions are defined with mutual recursion, corresponding with the
 * structure of the BNF grammar.
 */

#include <stdio.h>  /* getchar, printf */
#include <stdlib.h> /* NULL */
#include "scanner.h"
#include "recognizeEq.h"

/* The functions acceptNumber, acceptIdentifier and acceptCharacter have as
 * (first) argument a pointer to an token list; moreover acceptCharacter has as
 * second argument a character. They check whether the first token
 * in the list is a number, an identifier or the given character, respectively.
 * When that is the case, they yield the value 1 and the pointer points to the rest of
 * the token list. Otherwise they yield 0 and the pointer remains unchanged.
 */

// Do not change this function 
int acceptNumber(List *lp) {
  if (*lp != NULL && (*lp)->tt == Number) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

// Do not change this function
int acceptIdentifier(List *lp) {
  if (*lp != NULL && (*lp)->tt == Identifier) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}


// Do not change this function
int acceptCharacter(List *lp, char c) {
  if (*lp != NULL && (*lp)->tt == Symbol && ((*lp)->t).symbol == c) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}


/* The functions acceptFactor, acceptTerm and acceptExpression have as
 * argument a pointer to a token list. They check whether the token list
 * has an initial segment that can be recognized as factor, term or expression, respectively.
 * When that is the case, they yield the value 1 and the pointer points to the rest of
 * the token list. Otherwise they yield 0 and the pointer remains unchanged.
 */

int returnValue (List *lp) {
  return *lp;
} 


int biggestExponent = 0;

// accepts exponent '^' character and a number succeeding it
int acceptExponent(List *lp) {
  if (acceptCharacter(lp, '^')) {
    return acceptNumber(lp);
    if (returnValue(lp) > biggestExponent) {
        biggestExponent = returnValue(lp);
    }
  }
  return 0;
}

int acceptTerm(List *lp) {
  if (acceptNumber(lp)) {
    if (acceptIdentifier(lp)) {
      return acceptExponent(lp);
      
    }
    return 1;
  } else {
    if (acceptIdentifier(lp)) {
      return acceptExponent(lp);
      
    }
    return 0;
  }
  return 0;
}


int acceptExpression(List *lp) {
  if (!acceptTerm(lp)) {
    return 0;
  }
  while (acceptCharacter(lp, '+') || acceptCharacter(lp, '-')) {
    if (!acceptTerm(lp)) {
      return 0;
    }
    
  
  } /* no + or -, so we reached the end of the expression */
  return 1;
  
}

// accepts equations of the the form <expression> = <expression>
int acceptEquation(List *lp) {
  if (!acceptExpression(lp)) {
    return 0;
  }
  if (acceptCharacter(lp, '=')) {
    if (!acceptExpression(lp)) {
      return 0;
    }
  
  }
  return 1;
  

}

/* The function recognizeExpressions demonstrates the recognizer. */
void recognizeExpressions() {
  char *ar;
  List tl, tl1;
  printf("give an equation: ");
  ar = readInput();
  while (ar[0] != '!') {
    tl = tokenList(ar);
    printf("the token list is ");
    printList(tl);
    tl1 = tl;
    if (acceptEquation(&tl1) && tl1 == NULL) {
      printf("this is an equation in 1 variable");
      printf("%s %d %s", " of", biggestExponent, "degrees");
    } else {
      printf("this is not an equation\n");
    }
    free(ar);
    freeTokenList(tl);
    printf("\ngive an equation: ");
    ar = readInput();
  }
  free(ar);
  printf("good bye\n");
}
