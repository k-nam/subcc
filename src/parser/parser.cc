#include <iostream>
#include <cstdarg> // for variable argument number
#include "parser.h"
#include "stopwatch.h"




namespace lib_calvin_parser {
using std::pair;
using std::cout;
using std::endl;

/******************** ContextFreeLanguage definitinos *******************/

ContextFreeLanguage::ContextFreeLanguage(int terminalEnd, int nonTerminalEnd):
  terminalEnd_(terminalEnd), nonTerminalEnd_(nonTerminalEnd),
  isFinal_(false) { }

ContextFreeLanguage::Production::Production(Production const &rhs):
key_(rhs.key_), head_(rhs.head_), body_(rhs.body_) { }

ContextFreeLanguage::Production::Production(Production &&rhs):
key_(rhs.key_), head_(rhs.head_), body_(std::move(rhs.body_)) { }

bool ContextFreeLanguage::Production::operator== (Production const &rhs) const {
  if (head_ == rhs.head_ && body_ == rhs.body_) {
    return true;
	} else {
		return false;
	}
}

bool ContextFreeLanguage::Production::operator< (Production const &rhs) const {
  if (head_ < rhs.head_) {
    return true;
	} else if (head_ == rhs.head_ && body_ < rhs.body_) {
    return true;
	} else {
		return false;
	}
}

int ContextFreeLanguage::add(int key, int inHead, int inSymbol, ...) {
  if (keyToProduction_.count(key) != 0) {
    cout << "The key was already used\n";
    exit(0);
  } 
  if (inHead <= terminalEnd_ || inHead >= nonTerminalEnd_) {
    cout << "add invalid input value\n";
    exit(0);
  }
  if (isFinal_) {
    cout << "Already finalized, can not modify\n";
    return 0;
  }
  productions_.push_back(Production(key, inHead));
  Production &curPro = productions_.back();
  keyToProduction_.insert(pair<int, Production> (key, curPro));
  va_list vl;
  va_start(vl, inSymbol);
  int curSymbol = inSymbol;
  int count = 0; // to prevent wrong input
  while (true) {
    if (count++ > 15) {
      cout << "language: invalid Production input \n";
      return -1;
    }
    if (curSymbol == kEndMarker)
      break;
    else { // nonnegative, in range, and not the reserved value
      if (curSymbol > 0 && curSymbol < nonTerminalEnd_ && 
          curSymbol != terminalEnd_) { 
        curPro.body_.push_back(curSymbol);        
      } else { // invalid value
        cout << productions_.size() - 1 << " th Production\n";
        cout << "got symbol " << curSymbol << endl;
        cout << "language Production inserting error\n";
				exit(0);
      }
    }
    curSymbol = va_arg(vl, int); // get next arg
  }
  va_end(vl);
  return static_cast<int>(productions_.size()) - 1;
}

void ContextFreeLanguage::finalize() {
  // Augmented grammar: insert S' -> S; -2 denotes end of Production body
  // Note that this is always the last Production.
  productions_.push_back(Production(-1, terminalEnd_));
  // The head of first inserted productions is always the start symbol
  productions_.back().body_.push_back(productions_[0].head_);
  calculateFirstSets();
  calculateFollowSets();
  isFinal_ = true;
  cout << "num of productions are " << productions_.size() - 1 << endl;
}


void ContextFreeLanguage::getFirst (vector<int> const &inSymbolstring, 
    int begin, int end, set<int> &firstSet) const {
  
  set<int> &result = firstSet;
  result.clear();
  int i, stringLength = (int)inSymbolstring.size();
  for (i = begin; i < end; ++i) {
    bool hasEpsilon = false;
    set<int> const &curFirst = firstSets_.find(inSymbolstring[i])->second;
    set<int>::const_iterator iter;
    // Add all of the first set of current symbol
    // Be careful of epsilon !!!
    for (iter = curFirst.begin(); iter != curFirst.end(); ++iter) {
      if (*iter != kEpsilonMarker) {
        //cout << "first set inserted " << *iter << endl;
        result.insert(*iter);
      }
      else
        hasEpsilon = true;
    }
    if (!hasEpsilon)
      break;
  }
  if (i == stringLength) // entire string can derive empty string!
    result.insert(kEpsilonMarker);
}

void ContextFreeLanguage::getFollow (
    int inSymbol, set<int> &followSet) const {
  if (inSymbol < terminalEnd_) 
    cout << "No follow set for a terminal!!\n";
  followSet = followSets_.find(inSymbol)->second;
}

// This method sets member firstSets_ correctly
void ContextFreeLanguage::calculateFirstSets() {
  // Initialize all to empty sets, and
  // For all tokens, first set is singleton of itself.
  for (int i = 0; i < terminalEnd_; ++i) { // include '$'
    firstSets_[i] = set<int>();
    firstSets_[i].insert(i);
  }
  for (int i = terminalEnd_; i < nonTerminalEnd_; ++i) {
    firstSets_[i] = set<int>();
  }
  // Start iterative expansion of first sets
  bool finished = false;
  while (!finished) {
    finished = true;
    for (unsigned i = 0; i < productions_.size(); ++i) {
      Production const &curPro = productions_[i];
      unsigned j;
      for (j = 0; j < curPro.body_.size(); ++j) {
        if (firstSets_.count(curPro.body_[j]) == 0) {
          cout << "i is " << i << "j is " << j;
          cout << "calcul firstSet error; symbol is " << 
            curPro.body_[j] << endl;
					exit(0);
        }
        bool hasEpsilon = false;
        set<int> const &curFirst = firstSets_[curPro.body_[j]];
        set<int>::const_iterator iter;
        // Add all elements of curFirst to first set of head nonterminal
        // Be careful of epsilon!!!
        for (iter = curFirst.begin(); iter != curFirst.end(); ++iter) {
          if (*iter != kEpsilonMarker) {
            if (firstSets_[curPro.head_].count(*iter) == 0) {
              finished = false;
              firstSets_[curPro.head_].insert(*iter);
              //cout << "Firstset: added " << *iter <<  " to " << 
               //   curPro.head_ << endl;
            }
          } else 
            hasEpsilon = true;
        }
        if (!hasEpsilon)
          break;
      }
      if (j == curPro.body_.size()) { // body can derive empty string!
        if (firstSets_[curPro.head_].count(kEpsilonMarker) == 0) {
          firstSets_[curPro.head_].insert(kEpsilonMarker);
          finished = false; // this is also an addition!
        }
      }
    }
  }
}

// No epsilon terminal in follow sets
void ContextFreeLanguage::calculateFollowSets() {
  // Initialize all to empty sets, and
  // For start symbol, add $ 
  for (int i = terminalEnd_; i < nonTerminalEnd_; ++i) {
    followSets_[i] = set<int>();
  }
  // S' -> S  $, terminal 0 is $
  followSets_[terminalEnd_].insert(kDollarMarker);
  // Start iterative expansion of follow sets
  bool finished = false;
  while (!finished) {
    finished = true;
    for (unsigned i = 0; i < productions_.size(); ++i) {
      Production const &curPro = productions_[i];
      unsigned j;
      // Special case for rightmost nonterminal symbol
      for (j = 0; j < curPro.body_.size(); ++j) {
        if (curPro.body_[j] < terminalEnd_) // skip terminals
          continue;
        if (followSets_.count(curPro.body_[j]) == 0) {
          cout << "i is " << i << "j is " << j;
          cout << "calcul followSet error; symbol is " << 
            curPro.body_[j] << endl;
					exit(0);
        }
        set<int>::const_iterator iter;
        // Get first set of remaining symbol string
        set<int> curFollow;
        if (j < curPro.body_.size() - 1) // Not rightmost
          getFirst(curPro.body_, j + 1, 
              (int)curPro.body_.size(), curFollow);
        else // rightmost 
          curFollow = followSets_[curPro.head_];
        for (iter = curFollow.begin(); iter != curFollow.end(); ++iter) {
          if (*iter != kEpsilonMarker) {
            if (followSets_[curPro.body_[j]].count(*iter) == 0) {
              finished = false;
              followSets_[curPro.body_[j]].insert(*iter);
              //cout << "Followset: added " << *iter <<  " to " << 
              //  curPro.body_[j] << endl;
            }
          } else { // skip epsilon
          }
        }
      }
    }
  }
}

/******************** SlrParserGenerator definitions *******************/

SlrParserGenerator::LR0Item::LR0Item(LR0Item const &rhs):
production_(rhs.production_), dot_(rhs.dot_) { }

SlrParserGenerator::LR0Item::LR0Item(LR0Item &&rhs):
production_(std::move(rhs.production_)), dot_(rhs.dot_) { }

bool SlrParserGenerator::LR0Item::operator< (LR0Item const &rhs) const {
  if (production_ < rhs.production_)
    return true;
  if (production_ == rhs.production_ && dot_ < rhs.dot_)
    return true;
  return false;
}

bool SlrParserGenerator::LR0Item::operator== (LR0Item const &rhs) const {
  if (production_ == rhs.production_ && dot_ == rhs.dot_)
    return true;
  return false;
}

void SlrParserGenerator::parse (int input, ...) const {
  vector<int> result;
  vector<int> inText;
  // endmarker_ is -2
  va_list vl;
  va_start(vl, input);
  while (input != kEndMarker) {
    inText.push_back(input);
    //cout << "pushing " << input << endl;
    input = va_arg(vl, int); // get next int
  }
  va_end(vl);
  parse(inText, result);
  vector<int>::const_reverse_iterator iter;
  for (iter = result.rbegin(); iter != result.rend(); ++iter) {
    cout << *iter << "  ";
  }
  cout << "\n\n";
}

void SlrParserGenerator::parse (vector<int> &inText, 
    vector<int> &result) const {
  cout << "parsing start\n"; 
  inText.push_back(kDollarMarker); // mark $ at the end
  vector<int> stack; // stack to store States
  size_t pointer = 0, textLength = inText.size(); // pointer to inText
  State curState = initialState_;
  stack.push_back(curState);
  while (pointer < textLength) {
    if (inText[pointer] < 0 || inText[pointer] >= language_.terminalEnd_) {
      cout << "parser input range error!\n";
      exit(0);
    }
    if (actionTable_[curState][inText[pointer]].empty()) {
      cout << " Parser: empty slot error\n";
      exit(0);
    }
    pair<Action, int> doThis = 
      *actionTable_[curState][inText[pointer]].begin();
    switch (doThis.first) {
      case Action::Shift:
        {
          cout << "Parser: shift " << inText[pointer] << " " << 
            "from state " << curState << " to state " <<
            doThis.second << endl;
          curState = doThis.second;
          stack.push_back(curState);
          pointer++; // shift one Token
          break;
        }
      case Action::Reduce: 
        {
          result.push_back(doThis.second); // write Production key
          Production const &curPro = 
            language_.getProduction(doThis.second);
          size_t bodySize = curPro.body_.size();
          // pop bodySize elements
          stack.resize(stack.size() - bodySize); 
          curState = stack.back(); // stack top 
          // Be careful of gotoTable index
          curState = gotoTable_[curState][
            curPro.head_ -language_.terminalEnd_];
          stack.push_back(curState);
          cout << "Parser: reduce with " << doThis.second << 
            " goto " << curState << endl;
          break;
        }
      case Action::Accept: 
        {
          cout << "Parser: accpeted\n";
          return;
        }
      default:
        {
          cout << "Parser: internal error\n";
          exit(0);
        }
    }
  }
}

void SlrParserGenerator::initialize() {
  // Empty state stack
  stack_.clear();
  // Goto initial state
  currentState_ = initialState_;
  stack_.push_back(currentState_);
}

pair<Action, int>
SlrParserGenerator::parse(int inToken) {
  //cout << "in parser's parse step inToken is " << inToken << endl;
  if (actionTable_[currentState_][inToken].empty()) { // empty slot
    cout << "Parser: empty slot error\n";
    return pair<Action, int>(Action::Error, 0);
  }
  pair<Action, int> doThis = *actionTable_[currentState_][inToken].begin();
  switch (doThis.first) {
    case Action::Shift:
      {
        //cout << "Parser: shift\n";
        currentState_ = doThis.second;
        stack_.push_back(currentState_);
        return doThis;
      }
    case Action::Reduce: 
      {
        //cout << "Parser: reduce " << doThis.second << endl;
        Production const &curPro = 
          language_.getProduction(doThis.second);
        size_t bodySize = curPro.body_.size();
        // pop bodySize elements
        stack_.resize(stack_.size() - bodySize); 
        currentState_ = stack_.back(); // stack top 
        // Be careful of gotoTable index
        currentState_ = 
          gotoTable_[currentState_][
          curPro.head_ - language_.terminalEnd_];
        stack_.push_back(currentState_);
        doThis.second = curPro.key_;
        return doThis;
      }
    case Action::Error: 
      {
        cout << "Parser: syntax error\n";
        return doThis;
      }
    case Action::Accept: 
      {
        cout << "Parser: accpeted\n";
        return doThis;
      }
    default:
      {
        cout << "SlrParserGenerator::parse internal error.\n";
        exit(0);
      }
  }
}


void SlrParserGenerator::build (bool doReduce) {
  // Get all productions and store them
  // The last Production is S' -> S
	for (int i = 0; i < language_.getNumProductions(); ++i) {
    Production const &curPro = language_.getProduction(i);
    productionToInt_[curPro] = i;
    headToProduction_[curPro.head_].push_back(curPro);
  }
  // Build LR(0) automaton
  vector<set<LR0Item>> stack; // stack for DFS of graph of states
  set<LR0Item> curItemSet, nextItemSet;
  State curState, nextState;
  //start wih { S' -> .S }
  LR0Item initialItem (
      headToProduction_.find(language_.terminalEnd_)->second[0]);
  curItemSet.insert(initialItem);
  getClosure(curItemSet, curItemSet);	
  initialState_ = makeNewState();
  lR0ItemSetToState_[curItemSet] = initialState_; 
  stack.push_back(curItemSet);	
  while (!stack.empty()) {		
    curItemSet = stack[stack.size() - 1];
    stack.pop_back();
    curState = lR0ItemSetToState_[curItemSet];	
    if (doReduce) {
      // set reduce Action 
      set<LR0Item>::const_iterator iter;
      for (iter = curItemSet.begin(); iter != curItemSet.end(); ++iter) {
        if (iter->dot_ == iter->production_.body_.size()) { // end dot
          int productionKey = productionToInt_[iter->production_];
          // DO NOT reduce with augmented grammar!!! 
          if (productionKey == language_.getNumProductions() - 1)
            continue;
          set<int> followSet;
          language_.getFollow(iter->production_.head_, followSet);
          set<int>::const_iterator follow;
          for (follow = followSet.begin(); follow != followSet.end();
              ++follow) {
            // followset should not have epsilon
            if (*follow == kEpsilonMarker) {
              cout << "Followset contains epsilon, error\n";
              return;
            }
            actionTable_[curState][*follow].insert (
              pair<Action, int>(Action::Reduce, productionKey));
          }
        }
      }
    }
    // set shift action and goto 
    for (int i = 0; i < language_.nonTerminalEnd_; ++i) {
      getMove(curItemSet, i, nextItemSet);
      getClosure(nextItemSet, nextItemSet);
      if (nextItemSet.empty())
        continue;
      if (lR0ItemSetToState_.count(nextItemSet) == 0) { // newly found
        nextState = makeNewState();
        lR0ItemSetToState_[nextItemSet] = nextState;
        stack.push_back(nextItemSet);
        //cout << " goto with " << i << " ";
        //cout << "new set size is " << nextItemSet.size() << endl;
        set<LR0Item>::const_iterator temp_iter;
        for (temp_iter = nextItemSet.begin(); 
            temp_iter != nextItemSet.end(); ++temp_iter) {  }
			} else {
        nextState = lR0ItemSetToState_[nextItemSet];
      } 
      // set transition
      if (i < language_.terminalEnd_) { // terminal: shift
        actionTable_[curState][i].insert (
            pair<Action, int>(Action::Shift, nextState));
      } else { // nonterminal: goto (be careful of index!)
        gotoTable_[curState][i - language_.terminalEnd_] = nextState;
        //cout << "Making goto with " << i << endl;

      }
    }
  }	
  // Mark accept action:  S' -> .S
  LR0Item finalItem(headToProduction_[language_.terminalEnd_][0]);
  finalItem.dot_ = 1; // S' -> S.
  // Find final item set and its State, and mark for accept
  map<set<LR0Item>, State>::const_iterator iter;
  bool check = false; // check if there are more than 1 final item set
  int finalState = -1;
  for (iter = lR0ItemSetToState_.begin(); iter != lR0ItemSetToState_.end();
      ++iter) {
    if (iter->first.count(finalItem) != 0) {
      if (check == true) {
        cout << "More than 1 accepting state error!!!\n";
        return;
      }
      check = true;
      finalState = iter->second;
    }
  }
  if (finalState == -1) {
    cout << "SlrParserGenerator::build internal error\n";
    return;
  }
  // 0 is terminal '$' (end of input text)
  actionTable_[finalState][0].insert (
      pair<Action, int> (Action::Accept, 0)); // int value doesn't matter
  cout << "SLR: num of total states are " << actionTable_.size() << endl;
}

// Revised to take precedence into account; eliminate conflicts
void SlrParserGenerator::checkConflict() {
  using namespace lib_calvin_parser;
  // Need only to check action table (goto table has no conflict)
  for (unsigned i = 0; i < actionTable_.size(); ++i) { // for all States
    for (int j = 0; j < language_.terminalEnd_; j++) { // for all terminals
      set<pair<Action, int>> &conflicts = actionTable_[i][j];
      bool problem = false;
      if (conflicts.size() < 2) // no conflict
        continue;
      if (conflicts.size() > 2) 
        problem = true;
      int numReduce = 0;
      int shiftPrece = -1, reducePrece = -1; // initialize to error value
      set<pair<Action, int>>::const_iterator iter;
      // Store Action for later job
      pair<Action, int> shiftAction, reduceAction;
      // Solution to choose
      Action solution;
      // See each Action and alert if not resolvable
      for (iter = conflicts.begin(); iter != conflicts.end(); ++iter) {
        if (iter->first == Action::Reduce) {
          reduceAction = *iter;
          numReduce++;
          reducePrece = getPrecedence (
              language_.getProduction(iter->second));
          if (numReduce > 1 || reducePrece == -1) {
            cout << "Production problem: ";
            problem = true;
          }
          //cout << "Reduce with Production " 
          //  << iter->second << "  ";
          continue;
        } 
        if (iter->first == Action::Shift) {
          shiftAction = *iter;
          shiftPrece = getPrecedence(j);
          if (shiftPrece == -1) {
            cout << "shift problem: ";
            problem = true;
          }
          //cout << "Shift  ";
          continue;
        } 
      }
      if (problem) { // unresolved conflict!!
        cout << "#### Unresolved conflict for terminal: " << j << endl;
        solution = Action::Shift; // default is shift when in conflict
      }
      // Determine solution and delete other Action from table
      if (shiftPrece > reducePrece) { // do shift
        solution = Action::Shift;
      } else if (shiftPrece < reducePrece) { // do reduce
        solution = Action::Reduce;
      } else if (association_.count(shiftPrece) == 0) { // not set: shift
        cout << "Assoc not set for terminal " << j << endl;
        solution = Action::Shift;
      } else {
        Association curAssoc = association_[shiftPrece];
        switch(curAssoc) {
          case Association::Left:
            solution = Action::Reduce;
            break;
          case Association::Right:
            solution = Action::Shift;
            break;
          case Association::None:
            solution = Action::Error;
            break;
          default:
            cout << "parser: resolving conflict internal error\n";
            exit(0);
        }
      }
      // Delete current Action 
      conflicts.clear();
      // Insert wanted action into table
      switch(solution) {
        case Action::Shift:
          conflicts.insert(shiftAction);
          break; 
        case Action::Reduce:
          conflicts.insert(reduceAction);
          break;
        case Action::Error:
          conflicts.insert(pair<Action, int>(Action::Error, 0));
          break;
        default:
          cout << "parser: internal error\n";
          exit(0);
      }
    }
  }
  // Inspect table
  int numShift = 0, numReduce = 0, numGoto = 0;
  for (unsigned i = 0; i < actionTable_.size(); ++i) { // for all States
    for (int j = 0; j < language_.terminalEnd_; j++) { // for all terminals
      set<pair<Action, int>> curSet = actionTable_[i][j];
      set<pair<Action, int>>::const_iterator pair_iter;
      for (pair_iter = curSet.begin(); pair_iter != curSet.end();
          ++pair_iter) {
        if (pair_iter->first == Action::Shift)
          numShift++;
        if (pair_iter->first == Action::Reduce)
          numReduce++;
      }
    }
  }
  for (unsigned i = 0; i < gotoTable_.size(); ++i) { // for all States
    for (int j = 0; j < language_.nonTerminalEnd_ - language_.terminalEnd_; 
        j++) { // for all terminals
      if (gotoTable_[i][j] != -1)
        numGoto++;
    }
  }
  cout << "numShift " << numShift << "  ";
  cout << "numReduce " << numReduce << "  ";
  cout << "numGoto " << numGoto << endl;
}

void SlrParserGenerator::getMove (set<LR0Item> const &source, int inSymbol,
    set<LR0Item> &result) const {
  
  set<LR0Item> target;
  set<LR0Item>::const_iterator iter;
  for (iter = source.begin(); iter != source.end(); ++iter) {
    if (iter->dot_ == iter->production_.body_.size()) // end dot
      continue;
    if (iter->production_.body_[iter->dot_] == inSymbol) { // right move
      LR0Item newItem(*iter); // copy
      newItem.dot_++; // move dot by one symbol
      target.insert(newItem);
      //cout << "LR0 getMove inserted one\n";
    }
  }
  result = target;
}

void SlrParserGenerator::getClosure (set<LR0Item> const &source, 
    set<LR0Item> &closure) const {
  closure = source;
  bool finished = false;
  while (!finished) {
		vector<LR0Item> items;
		for (auto iter = closure.begin(); iter != closure.end(); ++iter) {
			items.push_back(*iter);
		}
    finished = true;
    for (auto iter = items.begin(); iter != items.end(); ++iter) {
			// Skip end dot,
			if (iter->dot_ == iter->production_.body_.size())
				continue;
      int curSymbol = iter->production_.body_[iter->dot_];
      // Skip terminal
      if (curSymbol <= language_.terminalEnd_) 
        continue;
      vector<Production> const &targetProductions = 
        headToProduction_.find(curSymbol)->second;
      // Caution !!! I directly insert newly found items
      // Not sure this won't cause problems yet.
			// 2013-9-28: above logic renders iterator invalid. Revised logic
			// but generally, set implementations doesn't invalidate iterators while insertion
      for (unsigned i = 0; i < targetProductions.size(); ++i) {
				if (closure.insert(targetProductions[i]).second) { // inserted!
          finished = false;
				}
      }
    }
  }
}

SlrParserGenerator::State 
SlrParserGenerator::makeNewState() {
  
  size_++;
  // Maintain appropriate table shape
  actionTable_.push_back (
      vector<set<pair<Action, int>>>(language_.terminalEnd_));
  // Set default as -1 (error)
  gotoTable_.push_back (
      vector<int>(language_.nonTerminalEnd_ - language_.terminalEnd_, -1));
  return size_ - 1;
}

int SlrParserGenerator::getPrecedence (Production inPro) const {
  int bodyLen = static_cast<int>(inPro.body_.size());
  for (int i = bodyLen - 1; i >= 0; i--) {
    if (inPro.body_[i] < language_.terminalEnd_) { // is a terminal
      //cout << "get prece in produc: terminal: " << inPro.body_[i] << endl;
      return getPrecedence(inPro.body_[i]);
    }
  }
  return -1; // not defined
}

/*********************** Lr1ParserGenerator definitions ******************/

Lr1ParserGenerator::LR1Item::LR1Item(LR1Item const &rhs):
LR0Item(rhs), lookahead_(rhs.lookahead_) { }

Lr1ParserGenerator::LR1Item::LR1Item(LR1Item &&rhs):
LR0Item(std::move(rhs)), lookahead_(rhs.lookahead_) { 
	compareCount_++;
}

bool Lr1ParserGenerator::LR1Item::operator< (LR1Item const &rhs) const {
	compareCount_++;
	//if (compareCount_ % 1000000 == 0) {
		//std::cout << "compareCount_: " << compareCount_  / 1000000 <<"\n";
	//}
	if (production_ < rhs.production_) {
    return true;
	} else if (production_ == rhs.production_) {
		if (dot_ < rhs.dot_) {
			return true;
		} else if (dot_ == rhs.dot_ && lookahead_ < rhs.lookahead_) {
			return true;
		} else{
			return false;
		}
	} else {
		return false;
	}
}

bool Lr1ParserGenerator::LR1Item::operator==(LR1Item const &rhs) const {
	return production_ == rhs.production_ && dot_ == rhs.dot_&& lookahead_ == rhs.lookahead_;

}

int Lr1ParserGenerator::LR1Item::compareCount_ = 0;

// Same logic as slr, only using LR1Item instead of LR0Item
void Lr1ParserGenerator::build () {
  // Build lookup data
  for (int i = 0; i < language_.getNumProductions(); ++i) {
    Production const &curPro = language_.getProduction(i);
    productionToInt_[curPro] = i;
    headToProduction_[curPro.head_].push_back(curPro);
  }
  // Build LR(1) automaton
  vector<set<LR1Item>> stack; // stack for DFS of graph of states
  set<LR1Item> curItemSet, nextItemSet;
  State curState, nextState;
  // Start wih { S' -> .S ,  $  }
  // LR1Item constructor takes a Production and lookahead
  LR1Item initialItem (
      headToProduction_.find(language_.terminalEnd_)->second[0], 
      kDollarMarker);
  curItemSet.insert(initialItem);
  getClosure(curItemSet, curItemSet);
  initialState_ = makeNewState();
  LR1ItemSetToState_[curItemSet] = initialState_; 
  stack.push_back(curItemSet);
  while (!stack.empty()) {
    curItemSet = stack[stack.size() - 1];
    stack.pop_back();
    curState = LR1ItemSetToState_[curItemSet];
    // set reduce Action
    set<LR1Item>::const_iterator iter;
    for (iter = curItemSet.begin(); iter != curItemSet.end(); ++iter) {
      if (iter->dot_ == iter->production_.body_.size()) { // end dot
        int productionKey = productionToInt_[iter->production_];
        // DO NOT reduce with augmented grammar!!! 
        if (productionKey == language_.getNumProductions() - 1)
          continue;
        // Reduce with lookahead Token
        //cout << "lookahead is " << iter->lookahead_ << endl;
        actionTable_[curState][iter->lookahead_].insert (
            pair<Action, int>(Action::Reduce, productionKey));
      }
    }
    // set shift action and goto 
    for (int i = 0; i < language_.nonTerminalEnd_; ++i) {
      getMove(curItemSet, i, nextItemSet);
      getClosure(nextItemSet, nextItemSet);
      if (nextItemSet.empty()) {
        continue;
      }
      if (LR1ItemSetToState_.count(nextItemSet) == 0) { // newly found
        nextState = makeNewState();
        LR1ItemSetToState_[nextItemSet] = nextState;
        stack.push_back(nextItemSet);
      } else {
        nextState = LR1ItemSetToState_[nextItemSet];
      } 
      // set transition
      if (i < language_.terminalEnd_) { // terminal: shift
        actionTable_[curState][i].insert (
            pair<Action, int>(Action::Shift, nextState));
      } else { // nonterminal: goto (be careful of index!)
        gotoTable_[curState][i - language_.terminalEnd_] = nextState;
        //cout << "Making goto with " << i << endl;
      }
    }
  }
  // Mark accept action:  find S' -> .S , $
  LR1Item finalItem (headToProduction_[language_.terminalEnd_][0], 
      kDollarMarker);
  finalItem.dot_ = 1; // S' -> S. , $
  // Find final item set and its State, and mark for accept
  map<set<LR1Item>, State>::const_iterator iter;
  bool check = false; // check if there are more than 1 final item set
  int finalState = -1;
  for (iter = LR1ItemSetToState_.begin(); iter != LR1ItemSetToState_.end();
      ++iter) {
    if (iter->first.count(finalItem) != 0) {
      if (check == true) {
        cout << "More than 1 accepting state error!!!\n";
        return;
      }
      check = true;
      finalState = iter->second;
    }
  }
  if (finalState == -1) {
    cout << "Lr1ParserGenerator::build internal error\n";
    return;
  }
  // 0 is terminal '$' (end of input text)
  actionTable_[finalState][0].insert (
      pair<Action, int> (Action::Accept, 0)); // int value doesn't matter
  cout << "LR1: num of total states are " << size_ << endl;
}

void Lr1ParserGenerator::getMove(set<LR1Item> const &source, int inSymbol,
    set<LR1Item> &result) const {
  
  set<LR1Item> target;
  set<LR1Item>::const_iterator iter;
  for (iter = source.begin(); iter != source.end(); ++iter) {
    if (iter->dot_ == iter->production_.body_.size()) // end dot
      continue;
    if (iter->production_.body_[iter->dot_] == inSymbol) { // right move
      LR1Item newItem(*iter); // copy
      newItem.dot_++; // move dot by one symbol
      target.insert(std::move(newItem));
      //cout << "LR1 getMove inserted one\n";
    }
  }
  result = target;
}

void Lr1ParserGenerator::getClosure(set<LR1Item> const &kernels, 
    set<LR1Item> &closure) const {
  
  closure = kernels;
  bool finished = false;
  while (!finished) {
		vector<LR1Item> items;
		for (auto iter = closure.begin(); iter != closure.end(); ++iter) {
			items.push_back(*iter);
		}
    finished = true;
    for (auto iter = items.begin(); iter != items.end(); ++iter) {
			// Skip end dot,
			if (iter->dot_ == iter->production_.body_.size())
				continue;
      int curSymbol = iter->production_.body_[iter->dot_];
      // Skip terminal
      if (curSymbol <= language_.terminalEnd_) 
        continue;
      // first set of the remaining symbol string is important
      set<int> firstSet;
      vector<int> remaining = iter->production_.body_;
      remaining.push_back(iter->lookahead_); 
      language_.getFirst(remaining, iter->dot_ + 1, static_cast<int>(remaining.size()), firstSet);
      vector<Production> const &targetProductions = 
        headToProduction_.find(curSymbol)->second;
      for (unsigned i = 0; i < targetProductions.size(); ++i) {
        set<int>::const_iterator iter2;
        for (iter2 = firstSet.begin(); iter2 != firstSet.end(); ++iter2) {
          // Consider lookaheads:  insert tokens in the first set
          LR1Item newItem(targetProductions[i], *iter2);
          if (closure.insert(std::move(newItem)).second) { // inserted!
            finished = false;
					}
        }
      }
    }
  }
}

/*********************** LalrParserGenerator definitions ******************/

// Unite LR1 item sets with same cores
void LalrParserGenerator::build () {
	cout << "LALR build() start\n";
  // Build canonical LR1 first
  Lr1ParserGenerator::build();  
  // Illegally initialize States and tables 
  vector<vector<set<pair<Action, int>>>> oldActionTable = actionTable_;
  vector<vector<int>> oldGotoTable = gotoTable_;
  actionTable_.clear();
  gotoTable_.clear();
  // Find item sets with same core and unite them
  vector<set<LR1Item>> unionArray; // index is State number
  map<State, State> oldStateToNewState;
  map<set<LR1Item>, State>::const_iterator iter;
  // Find each LR1 item set's union and give new state 
  for (iter = LR1ItemSetToState_.begin(); iter != LR1ItemSetToState_.end();
      ++iter) {
    unsigned i;
    for (i = 0; i < unionArray.size(); ++i) {
      if (isSameCore(unionArray[i], iter->first))
        break;
    }
    if (i == unionArray.size()) { // This item set is first discovered now
      unionArray.push_back(iter->first);
    }
    oldStateToNewState[iter->second] = i;
    //cout << "map old state " << iter->second << " to " << i << endl;
  }
  // Set new initial state !!!
  initialState_ = oldStateToNewState[initialState_];
  
  // Now size is reduced
  size_ = static_cast<int>(unionArray.size());
  cout << "lalr state num is " << size_ << endl;
  actionTable_.resize (size_, 
      vector<set<pair<Action, int>>>(language_.terminalEnd_));
  gotoTable_.resize (size_,   // set -1 as default
      vector<int>(language_.nonTerminalEnd_ - language_.terminalEnd_, -1));
  // Fill new tables: map each old state to its new state
  for (unsigned i = 0; i < oldActionTable.size(); ++i) { // i: old state
    int newState = oldStateToNewState[i];
    // Action table: write all down (may contain conflicts)
    for (int j = 0; j < language_.terminalEnd_; j++) {
      set<pair<Action, int>>::const_iterator iter2;
      for (iter2 = oldActionTable[i][j].begin(); 
          iter2 != oldActionTable[i][j].end(); iter2++) {
        pair<Action, int> curPair = *iter2;;
        // Shift to appropriate new state
        if (curPair.first == Action::Shift) 
          curPair.second = oldStateToNewState[curPair.second];
        actionTable_[newState][j].insert (curPair);
      }
    }
    // Goto Table
    for (int j = 0; 
        j < language_.nonTerminalEnd_ - language_.terminalEnd_; j++) {
      State oldState = oldGotoTable[i][j];
      if (oldState != -1)
        gotoTable_[newState][j] = oldStateToNewState[oldState];
    }
  }
  cout << "LALR(1) build complete\n";
}

// Build directly from LR0 table, using lookahead propagating
// Note: same kernel item in different LR0 item set are DIFFERENT kernels!
void LalrParserGenerator::buildFast () {
  // Build LR0 without setting reduce Action
  SlrParserGenerator::build(false);
  // Prepare data set: kernel = pair<State, LR0Item>
  typedef pair<State, LR0Item> kernel;
  vector<kernel> kernelArray; // list each kernel
  map<kernel, set<kernel>> propagates;
  map<kernel, set<int>> lookaheads; // final result (closure)
  // Find all kernels and list them
  map<set<LR0Item>, State>::const_iterator state_iter;
  for (state_iter = lR0ItemSetToState_.begin();
      state_iter != lR0ItemSetToState_.end(); ++state_iter) { // for all states
    set<LR0Item>::const_iterator item_iter;
    for (item_iter = state_iter->first.begin(); 
        item_iter != state_iter->first.end(); ++item_iter) { // for each items
      if (item_iter->dot_ != 0)  { // is a kernel
        kernelArray.push_back(kernel(state_iter->second, *item_iter));
      }
    }
	}
	
  // Note that augmented Production IS KERNEL ITEM !!!
  LR0Item initialItem (
      headToProduction_.find(language_.terminalEnd_)->second[0]);
  kernel initialKernel(initialState_, initialItem);
  kernelArray.push_back(initialKernel);
  // Calculate propagates and spontaneous lookaheads for all kernels
  // Base case is augmented Production S' -> .S (with $)
  lookaheads[initialKernel].insert(kDollarMarker);
  vector<kernel>::const_iterator kernel_iter;
  for (kernel_iter = kernelArray.begin(); 
      kernel_iter != kernelArray.end(); ++kernel_iter) {
    // Use dummy lookahead to test ($ can not be included in any body)
    LR1Item srcLR1Item(kernel_iter->second, kDollarMarker);
    set<LR1Item> srcLR1ItemSet, targetLR1Kernel;
    srcLR1ItemSet.insert(std::move(srcLR1Item)); // singleton
    Lr1ParserGenerator::getClosure(srcLR1ItemSet, srcLR1ItemSet);		
    // Calculate for all possible moves
    for (int i = 0; i < language_.nonTerminalEnd_; ++i) {
      Lr1ParserGenerator::getMove(srcLR1ItemSet, i, targetLR1Kernel);			
      if (targetLR1Kernel.empty())
        continue;
      State curState = kernel_iter->first, nextState;
      if (i < language_.terminalEnd_) { // a terminal
        pair<Action, int> const &curPair = *actionTable_[curState][i].begin();
        if (curPair.first != Action::Shift) {
          cout << "lalr buildFast error\n";
          return;
        }
        nextState = curPair.second;
      }
      else { // a nonterminal
        nextState = gotoTable_[curState][i - language_.terminalEnd_];
        if (nextState == -1) {
          cout << "lalr buildFast error\n";
          return;
        }
      }
      set<LR1Item>::const_iterator item_iter;
      for (item_iter = targetLR1Kernel.begin(); 
          item_iter != targetLR1Kernel.end(); ++item_iter) {
        // Process for all items in resulted kernel
        LR0Item targetLR0Item(item_iter->production_, item_iter->dot_);
        kernel target(nextState, targetLR0Item);
        if (item_iter->lookahead_ == kDollarMarker) { // lookahead = $
	        propagates[*kernel_iter].insert(std::move(target));
        }
        else { // real Token lookahead
          lookaheads[target].insert(item_iter->lookahead_);
          //cout << "spontaneous " << item_iter->lookahead_ << endl;
        }
      }
    }
	}
  
  // Calculate closure
  bool finished = false;
  while (!finished) {
    finished = true;
    for (kernel_iter = kernelArray.begin(); 
        kernel_iter != kernelArray.end(); ++kernel_iter) {
      set<kernel>::const_iterator targetKernel;
      for (targetKernel = propagates[*kernel_iter].begin(); 
          targetKernel != propagates[*kernel_iter].end(); 
          ++targetKernel) {
        set<int> toAdd = lookaheads[*kernel_iter];
        /*
        set<int>::const_iterator la_iter; // la: lookahead
        for (la_iter = toAdd.begin(); la_iter != toAdd.end(); ++la_iter) {
          if (lookaheads[*targetKernel].count(*la_iter) == 0) {
            lookaheads[*targetKernel].insert(*la_iter);
            finished = false;
          }
        }
        */
        size_t beforeNum = lookaheads[*targetKernel].size();
        lookaheads[*targetKernel].insert(toAdd.begin(), toAdd.end());
        size_t afterNum = lookaheads[*targetKernel].size();
        if (beforeNum < afterNum) {
          finished = false;
				}
      }
    }
	}

  // New reduce Action (reduce only for lookaheads)
  // CAUTION: due to epsilon Production, it is needed to close each kernel
  // Epsilon is the only case where non-kernel item can be used in reduce
  for (kernel_iter = kernelArray.begin(); 
      kernel_iter != kernelArray.end(); ++kernel_iter) {
    
    State curState          = kernel_iter->first;
    set<int> curLookaheads  = lookaheads[*kernel_iter];
    LR0Item LR0Kernel       = kernel_iter->second;
    set<LR1Item> closure; // LR1 closure for this kernel item
    set<int>::const_iterator la_iter; // lookahead iter
    // Attatch each lookahead to LR0 kernel item and insert
    for (la_iter = curLookaheads.begin(); 
        la_iter != curLookaheads.end(); ++la_iter) {
      
      LR1Item curItem(LR0Kernel, *la_iter);
      closure.insert(std::move(curItem));
    }
    Lr1ParserGenerator::getClosure(closure, closure);
    set<LR1Item>::const_iterator item_iter;
    for (item_iter = closure.begin(); item_iter != closure.end(); 
        ++item_iter) {
      if (item_iter->dot_ != item_iter->production_.body_.size()) 
        continue; // skip non right-end dot
      int productionKey = productionToInt_[item_iter->production_];
      // DO NOT reduce with augmented grammar!!! 
      if (productionKey == language_.getNumProductions() - 1)
        continue;
      // Reduce with lookahead Token
      //cout << "lookahead is " << iter->lookahead_ << endl;
      actionTable_[curState][item_iter->lookahead_].insert (
          pair<Action, int>(Action::Reduce, productionKey));
    }
	}
  cout << "LALR(1) buildFast complete\n";
}

bool LalrParserGenerator::isSameCore (set<LR1Item> const &lhs, 
    set<LR1Item> const &rhs) {
  
  set<LR1Item>::const_iterator left, right;
  set<LR0Item> leftSet, rightSet;
  for (left = lhs.begin(); left != lhs.end(); ++left) {
    LR0Item core(left->production_);
    core.dot_ = left->dot_;
    leftSet.insert(core);
  }
  for (right = rhs.begin(); right != rhs.end(); ++right) {
    LR0Item core(right->production_);
    core.dot_ = right->dot_;
    rightSet.insert(core);
  }
  if (leftSet == rightSet)
    return true;
  return false;
}

}

