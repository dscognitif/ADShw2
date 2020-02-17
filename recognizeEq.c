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
#include <math.h>
#include <string.h>

/* The functions acceptNumber, acceptIdentifier and acceptCharacter have as
 * (first) argument a pointer to an token list; moreover acceptCharacter has as
 * second argument a character. They check whether the first token
 * in the list is a number, an identifier or the given character, respectively.
 * When that is the case, they yield the value 1 and the pointer points to the rest of
 * the token list. Otherwise they yield 0 and the pointer remains unchanged.
 */

int acceptNumber(List *lp) {
  if (*lp != NULL && (*lp)->tt == Number) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

int acceptIdentifier(List *lp) {
  if (*lp != NULL && (*lp)->tt == Identifier) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

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

// stores the biggest exponent
int biggestExponent = 0;

// reads the value of the number and determine whether it is biggest exponent
int valueNumber(List *lp, double *wp) {
  if (*lp != NULL && (*lp)->tt == Number) {
    *wp = ((*lp)->t).number;
    *lp = (*lp)->next;
    // if the value of the number is bigger than set it as the biggestExponent
    if (*wp > biggestExponent) {
      biggestExponent = *wp;
    }
    return 1;
  }
  return 0;
}

// accepts exponent '^' character and a natural number succeeding it
int acceptExponent(List *lp) {
  double wp = 0;
  // checks whether there is an exponent
  if (acceptCharacter(lp, '^')) {
    // only accepts natural numbers
    if (acceptCharacter(lp, '-')) {
      return 0;
    } else {
      // calls the valueNumber to determine the value of the exponent
      return valueNumber(lp, &wp);
    }
  } else {
    return 1;
  }
  return 0;
}

// accepts terms in the form of <nat> | [ <nat>] <identifier> ['^' <nat>]
int acceptTerm(List *lp) {
  if (acceptNumber(lp)) {
    if (acceptIdentifier(lp)) {
      // if number and identifier then call acceptExponent to check whether there is an exponent
      return acceptExponent(lp);
    }
    return 1;
  } else {
    if (acceptIdentifier(lp)) {
      // if there is only identifier then check whether an exponent is inputted
      return acceptExponent(lp);
    }
    return 0;
  }
  return 0;
}

// accepts expressions of the form ['-'] <term> {'+' <term> | '-' <term>}
int acceptExpression(List *lp) {

  // checks whether it is a negative number
  if (acceptCharacter(lp, '-')) {
    // checks whether it is a valid term
    if (!acceptTerm(lp)) {
      return 0;
    }
  }
  else if (!acceptTerm(lp)) {
    return 0;
  }

  while (acceptCharacter(lp, '+') || acceptCharacter(lp, '-')) {
    // checks whether it is a valid term
    if (!acceptTerm(lp)) {
      return 0;
    }
  } /* no + or -, so we reached the end of the expression */
  return 1;
}

// accepts equations of the the form <expression> '=' <expression>
int acceptEquation(List *lp) {
  return (acceptExpression(lp) && acceptCharacter(lp,'=') && acceptExpression(lp));
}

// determines the amount of variables there are. returns 1 if there is 1 variable
int determineVariables(List lp) {
  
  // initialise string to store the value of the identifiers/variables
  char *string = NULL;
  // loop through list 
  while (lp != NULL) {
    if (lp->tt == Identifier) {
      if (string == NULL) {
        free(string);
        string = (lp)->t.identifier;
      } 
      // conditional to compare whether the variable stored is the same
      if (strcmp(string, lp->t.identifier)) {
        return 2;
      }
    }
      if (lp != NULL) {
        lp = (lp)->next;
      }
  }
    // if there are no variables
    if (string == NULL) {
      return 0;
    }

  return 1;
}


/* The function recognizeExpressions demonstrates the recognizer. */
void recognizeEquations() {
  char *ar;
  List tl, tl1;
  printf("give an equation: ");
  ar = readInput();
  while (ar[0] != '!') {
    tl = tokenList(ar);
    printList(tl);
    tl1 = tl;
    if (acceptEquation(&tl1) && tl1 == NULL) {
      // conditional if there is one variable
      if (determineVariables(tl) == 1) {
      printf("this is an equation in 1 variable");
    
        // if the exponent is 1 or 0
        if (biggestExponent == 0) {
          printf(" of degree 1");
        }
        else {
          printf("%s %s %d\n", " of", "degree", biggestExponent);
        }
        // resets the biggestExponent
        biggestExponent = 0;
      }
      // conditional if there are more than 1 variable
      else if (determineVariables(tl) != 1) {
        printf("this is an equation, but not in 1 variable\n");
      }
    }
    else {
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