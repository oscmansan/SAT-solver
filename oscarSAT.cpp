#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <math.h>
using namespace std;

#define UNDEF -1
#define TRUE 1
#define FALSE 0

uint numVars;
uint numClauses;
vector<vector<int> > clauses;
vector<int> model;
vector<int> modelStack;
uint indexOfNextLitToPropagate;
uint decisionLevel;

uint numDecisions;
uint numPropagations;

vector<pair<vector<int>,vector<int> > > locations;
vector<int> pfreqs;
vector<int> nfreqs;
vector<pair<int,int> > scores;


void readClauses() {
  // Skip comments
  char c = cin.get();
  while (c == 'c') {
    while (c != '\n') c = cin.get();
    c = cin.get();
  }  
  
  // Read "cnf numVars numClauses"
  string aux;
  cin >> aux >> numVars >> numClauses;
  
  clauses.resize(numClauses);
  locations.resize(numVars+1);
  pfreqs.resize(numVars+1);
  nfreqs.resize(numVars+1);
  
  // Read clauses
  for (uint i = 0; i < numClauses; ++i) {
    int lit;
    while (cin >> lit and lit != 0) {
      clauses[i].push_back(lit);
      
      if (lit > 0) {
	locations[lit].first.push_back(i);
	pfreqs[lit] += 1;
      }
      else {
	locations[-lit].second.push_back(i);
	nfreqs[-lit] += 1;
      }
    }
  }  
  
  scores.resize(numVars+1);
  for (int i = 0; i <= numVars; ++i) {
      int k = 2;
      int score = (pfreqs[i]+nfreqs[i])*pow(2,k) + pfreqs[i]+nfreqs[i];
      scores[i] = pair<int,int>(score,i);
  }
      
  sort(scores.begin(),scores.end(),greater<pair<int,int> >());
}



int currentValueInModel(int lit) {
  if (lit >= 0) return model[lit];
  else {
    if (model[-lit] == UNDEF) return UNDEF;
    else return 1 - model[-lit]; // returns the value after aplying the negation
  }
}


void setLiteralToTrue(int lit) {
  modelStack.push_back(lit);
  if (lit > 0) model[lit] = TRUE;
  else model[-lit] = FALSE;		
}


bool propagateGivesConflict () {
  while (indexOfNextLitToPropagate < modelStack.size()) {
    ++numPropagations;
    
    int lit = modelStack[indexOfNextLitToPropagate];
    ++indexOfNextLitToPropagate;
    
    int *locs;
    int size;
    if (lit > 0) {
      locs = &locations[lit].second[0];
      size = locations[lit].second.size();
    }
    else {
      locs = &locations[-lit].first[0];
      size = locations[-lit].first.size();
    }
    
    for (uint i = 0; i < size; ++i) {
      bool someLitTrue = false;
      int numUndefs = 0;
      int lastLitUndef = 0;
      int j = locs[i];
      for (uint k = 0; not someLitTrue and k < clauses[j].size(); ++k){
	int val = currentValueInModel(clauses[j][k]);
	if (val == TRUE) someLitTrue = true;
	else if (val == UNDEF){ ++numUndefs; lastLitUndef = clauses[j][k]; }
      }
      if (not someLitTrue and numUndefs == 0) return true; // conflict! all lits false
      else if (not someLitTrue and numUndefs == 1) setLiteralToTrue(lastLitUndef); // if there's more than one literal undefinded, unit propagation can't be done
    }    
  }
  return false;
}


void backtrack() {
  uint i = modelStack.size() -1;
  int lit = 0;
  while (modelStack[i] != 0){ // 0 is the DL mark
    lit = modelStack[i];
    model[abs(lit)] = UNDEF;
    modelStack.pop_back();
    --i;
  }
  // at this point, lit is the last decision
  modelStack.pop_back(); // remove the DL mark
  --decisionLevel;
  indexOfNextLitToPropagate = modelStack.size();
  setLiteralToTrue(-lit);  // reverse last decision
}


// Heuristic for finding the next decision literal:
int getNextDecisionLiteral() {
  ++numDecisions;
  for (uint i = 0; i < scores.size(); ++i) {
    int lit = scores[i].second;
    if (model[lit] == UNDEF) return lit;
  }
  return 0; // returns 0 when all literals are defined
}

void checkmodel() {
  for (int i = 0; i < numClauses; ++i){
    bool someTrue = false;
    for (int j = 0; not someTrue and j < clauses[i].size(); ++j)
      someTrue = (currentValueInModel(clauses[i][j]) == TRUE);
    if (not someTrue) {
      cout << "Error in model, clause is not satisfied:";
      for (int j = 0; j < clauses[i].size(); ++j) cout << clauses[i][j] << " ";
      cout << endl;
      exit(1);
    }
  }  
}

int main() { 
  readClauses(); // reads numVars, numClauses and clauses
  model.resize(numVars+1,UNDEF);
  indexOfNextLitToPropagate = 0;  
  decisionLevel = 0;
  numDecisions = 0;
  numPropagations = 0;
  
  // Take care of initial unit clauses, if any
  for (uint i = 0; i < numClauses; ++i)
    if (clauses[i].size() == 1) {
      int lit = clauses[i][0];
      int val = currentValueInModel(lit);
      if (val == FALSE) {
	cout << "UNSATISFIABLE" << endl; 
	cout << "decisions: " << numDecisions << endl; 
	cout << "propagations: " << numPropagations << endl;
	return 10;
      }
      else if (val == UNDEF) setLiteralToTrue(lit);
    }
  
  // DPLL algorithm
  while (true) {
    while ( propagateGivesConflict() ) {
      if ( decisionLevel == 0) { 
	cout << "UNSATISFIABLE" << endl;
	cout << "decisions: " << numDecisions << endl; 
	cout << "propagations: " << numPropagations << endl;
	return 10;
      }
      backtrack();
    }
    int decisionLit = getNextDecisionLiteral();
    if (decisionLit == 0) { 
      checkmodel(); 
      cout << "SATISFIABLE" << endl;
      cout << "decisions: " << numDecisions << endl; 
      cout << "propagations: " << numPropagations << endl;
      return 20;
    }
    // start new decision level:
    modelStack.push_back(0);  // push mark indicating new DL
    ++indexOfNextLitToPropagate;
    ++decisionLevel;
    setLiteralToTrue(decisionLit);    // now push decisionLit on top of the mark
  }
}  
