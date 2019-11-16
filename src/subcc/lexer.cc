/**  SubccLexer.cc **/
#include "lexer.h" 

#define CAT CAT<char>
#define OR OR<char>
#define STAR STAR<char>
#define PLUS PLUS<char>
#define ONEORZERO ONEORZERO<char>

namespace subcc
{

// Build lexical analyzer for subcc
void Lexer::build() {
  using namespace lib_calvin_lexer;
  /******** Configuring Lexer Generator *******/

  // As I don't have regex parser, I need to do some manual setting
  // prepare intermediate expressions
  c_string letterString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  c_string digitString("0123456789");
  c_string nonZeroDigitString("123456789");
  c_string underScoreString("_");
  c_string esc_a("\\a");
  c_string esc_b("\\b");
  c_string esc_f("\\f");
  c_string esc_n("\\n");
  c_string esc_r("\\r");
  c_string esc_v("\\v");
  c_string esc_sq("\\\'"); // single quote
  c_string esc_dq("\\\""); // double quote
  c_string esc_bs("\\\\"); // backslash
  c_string esc_null("\\0");
  
  shared_ptr<Node> whitespaceNode;
  shared_ptr<Node> letterNode, digitNode, nonZeroDigitNode;
  shared_ptr<Node> idNode;
  // both // and /* */
  shared_ptr<Node> cCommentNode, cppCommentNode, commentsNode;
  // ' can not be inside a char literal! ( \' is fine)
  shared_ptr<Node> escapeCharNode, charLiteralNode;
  // " or newline can not be inside a c_string literal! ( \" is fine)
  shared_ptr<Node> stringLiteralNode;
  shared_ptr<Node> decimalNode, hexNode;
  
  whitespaceNode = PLUS(ONEOF(c_string(" \t\n")));
  letterNode = ONEOF(letterString);
  digitNode = ONEOF(digitString);
  nonZeroDigitNode = ONEOF(nonZeroDigitString);
  idNode = CAT(OR(letterNode, LITERAL(underScoreString)), 
        STAR(OR(OR(letterNode, digitNode), LITERAL(underScoreString))));
  cppCommentNode = CAT(LITERAL(c_string("//")), 
      CAT(STAR(NOTOF(c_string("\n"))), 
        LITERAL(c_string("\n"))));
  // \/*([^*]|(*)+[^*/])*(*)+\/
  cCommentNode = CAT(LITERAL(c_string("/*")),
      CAT(STAR(OR(NOTOF(c_string("*")), 
        CAT(PLUS(LITERAL(c_string("*"))), NOTOF(c_string("/*"))))),
      CAT(PLUS(LITERAL(c_string("*"))), LITERAL(c_string("/")))));
  commentsNode = OR(cCommentNode, cppCommentNode);
  escapeCharNode = OR(LITERAL(esc_a), OR(LITERAL(esc_b), OR(LITERAL(esc_f),
            OR(LITERAL(esc_n), OR(LITERAL(esc_r), OR(LITERAL(esc_v),
              OR(LITERAL(esc_sq), OR(LITERAL(esc_dq), 
                  OR(LITERAL(esc_bs),
                    LITERAL(esc_null))))))))));
  charLiteralNode =   CAT(LITERAL(c_string("\'")),
            CAT(OR(NOTOF(c_string("\'")), escapeCharNode),
              LITERAL(c_string("\'"))));
  //  \"([^\"\n]|[\t\n\r...]|(\\\n))*\"
  stringLiteralNode = CAT(LITERAL(c_string("\"")),
            CAT(STAR(OR(OR(NOTOF(c_string("\"\n")), escapeCharNode),
                LITERAL(c_string("\\\n")))),
              LITERAL(c_string("\""))));
  decimalNode = OR(CAT(nonZeroDigitNode,
                CAT(STAR(digitNode), ONEORZERO(CAT(LITERAL(c_string(".")),
                PLUS(digitNode))))),
                LITERAL(c_string("0")));  
  hexNode   =   CAT(ONEORZERO(ONEOF(c_string("+-"))),
          CAT(LITERAL(c_string("0x")),
            PLUS(OR(digitNode, ONEOF(c_string("abcdefABCDEF"))))));

  // Insert each regex into NFA simulator
  // First entered regex has higher priority than later entered ones
  nfa.addRegularExpression(WHITESPACE, whitespaceNode);
  nfa.addRegularExpression(COMMENT, commentsNode); 
  nfa.addRegularExpression(SHORT, LITERAL(c_string("short")));
  nfa.addRegularExpression(INT, LITERAL(c_string("int")));
  nfa.addRegularExpression(STRUCT, LITERAL(c_string("struct")));
  nfa.addRegularExpression(SIZEOF, LITERAL(c_string("sizeof")));
  nfa.addRegularExpression(TYPEDEF, LITERAL(c_string("typedef")));
  nfa.addRegularExpression(IF, LITERAL(c_string("if")));
  nfa.addRegularExpression(ELSE, LITERAL(c_string("else")));
  nfa.addRegularExpression(WHILE, LITERAL(c_string("while")));
  nfa.addRegularExpression(SWITCH, LITERAL(c_string("switch")));
  nfa.addRegularExpression(CASE, LITERAL(c_string("case")));
  nfa.addRegularExpression(DEFAULT, LITERAL(c_string("default")));
  nfa.addRegularExpression(BREAK, LITERAL(c_string("break")));
  nfa.addRegularExpression(CONTINUE, LITERAL(c_string("continue")));
  nfa.addRegularExpression(RETURN, LITERAL(c_string("return"))); 
  nfa.addRegularExpression(CHAR, LITERAL(c_string("char"))); 
  nfa.addRegularExpression(VOID_, LITERAL(c_string("void"))); 
  nfa.addRegularExpression(ID, idNode);
  nfa.addRegularExpression(CHAR_LITERAL, charLiteralNode);
  nfa.addRegularExpression(STRING_LITERAL, stringLiteralNode);
  nfa.addRegularExpression(DECIMAL_LITERAL, decimalNode);
  nfa.addRegularExpression(HEXADECIMAL_LITERAL, hexNode);
  nfa.addRegularExpression(ARITH_PLUS, LITERAL(c_string("+")));
  nfa.addRegularExpression(ARITH_INCREMENT, LITERAL(c_string("++")));
  nfa.addRegularExpression(ARITH_MINUS, LITERAL(c_string("-")));
  nfa.addRegularExpression(ARITH_DECREMENT, LITERAL(c_string("--")));
  nfa.addRegularExpression(ARITH_MULTI, LITERAL(c_string("*")));
  nfa.addRegularExpression(ARITH_DIVIDE, LITERAL(c_string("/")));
  nfa.addRegularExpression(ARITH_MOD, LITERAL(c_string("%")));
  nfa.addRegularExpression(LOGIC_BIT_AND, LITERAL(c_string("&")));
  nfa.addRegularExpression(LOGIC_BIT_OR, LITERAL(c_string("|")));
  nfa.addRegularExpression(LOGIC_BIT_XOR, LITERAL(c_string("^")));
  nfa.addRegularExpression(LOGIC_BIT_NOT, LITERAL(c_string("~")));
  nfa.addRegularExpression(LOGIC_AND, LITERAL(c_string("&&")));
  nfa.addRegularExpression(LOGIC_OR, LITERAL(c_string("||")));
  nfa.addRegularExpression(LOGIC_NOT, LITERAL(c_string("!")));
  nfa.addRegularExpression(COMP_EQ, LITERAL(c_string("==")));
  nfa.addRegularExpression(COMP_GT, LITERAL(c_string(">")));
  nfa.addRegularExpression(COMP_LT, LITERAL(c_string("<")));
  nfa.addRegularExpression(COMP_GE, LITERAL(c_string(">=")));
  nfa.addRegularExpression(COMP_LE, LITERAL(c_string("<=")));
  nfa.addRegularExpression(COMP_NE, LITERAL(c_string("!=")));
  nfa.addRegularExpression(ASSIGN, LITERAL(c_string("=")));
  nfa.addRegularExpression(ASSIGN_PLUS, LITERAL(c_string("+=")));
  nfa.addRegularExpression(ASSIGN_MINUS, LITERAL(c_string("-=")));
  nfa.addRegularExpression(ASSIGN_MULTI, LITERAL(c_string("*=")));
  nfa.addRegularExpression(ASSIGN_DIVIDE, LITERAL(c_string("/=")));
  nfa.addRegularExpression(ASSIGN_MOD, LITERAL(c_string("%=")));
  nfa.addRegularExpression(ASSIGN_BIT_XOR, LITERAL(c_string("^=")));
  nfa.addRegularExpression(ASSIGN_BIT_AND, LITERAL(c_string("&=")));
  nfa.addRegularExpression(ASSIGN_BIT_OR, LITERAL(c_string("|=")));
  nfa.addRegularExpression(PUNC_PERIOD, LITERAL(c_string(".")));
  nfa.addRegularExpression(PUNC_COMMA, LITERAL(c_string(",")));
  nfa.addRegularExpression(PUNC_COLON, LITERAL(c_string(":")));
  nfa.addRegularExpression(PUNC_SEMICOLON, LITERAL(c_string(";")));
  nfa.addRegularExpression(DELIM_PAREN_OPEN, LITERAL(c_string("(")));
  nfa.addRegularExpression(DELIM_PAREN_CLOSE, LITERAL(c_string(")")));
  nfa.addRegularExpression(DELIM_BRACKET_OPEN, LITERAL(c_string("[")));
  nfa.addRegularExpression(DELIM_BRACKET_CLOSE, LITERAL(c_string("]")));
  nfa.addRegularExpression(DELIM_BRACE_OPEN, LITERAL(c_string("{")));
  nfa.addRegularExpression(DELIM_BRACE_CLOSE, LITERAL(c_string("}")));
	nfa.addRegularExpression(DEREFERENCE, LITERAL(c_string("*")));
	nfa.addRegularExpression(AMPERSAND, LITERAL(c_string("&")));
  nfa.buildAll();
  dfa.convertFrom(nfa);
}

// return -1 on no match (error), 
// return 0 on finish ( $ = 0 )
int Lexer::getNextToken() {
  // Start lexicalizing with given source file
  Token myToken;
  while (index_ < static_cast<int>(text_.size())) {
    dfa.getMatch(text_, index_, myToken); // get next Token
    for (int i = index_; i < index_ + myToken.length; ++i) {
      if (text_[i] == '\n') // count lines
        ++lineNum_;
    }
    index_ += myToken.length; // move pointer
    if (myToken.key == -1) { // lexical error: length is 1
      std::cout << lineNum_ << ": ";
      std::cout << "lexical error: " << text_[index_] << std::endl;
      continue; // continue until finding legal lexeme
    }
    if (myToken.key == 200 || myToken.key == 201) { // ingnore ws and comments
      continue;
    } else {
      // Store currently matched lexeme
      curLexeme_ = text_.substr(index_ - myToken.length, index_);
      return myToken.key;
    }
  } 
  // At the end of text

  return 0;
}
} // end namespace subcc