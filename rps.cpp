#include <iostream>
#include <vector>
#include <map>
using namespace std;

#define MAX_STATES 80

#define ROCK 0
#define PAPER 1
#define SCISSOR 2

static inline int toInt( char a )
{
    return ( a == 'R' ? ( 0 ) : ( a == 'P' ? 1 : ( a == 'S' ? 2 : -1) ) );
}

using Graph = vector< vector<int> >;
using Bitmask = long long int;

typedef struct fsm
{
    vector<char> State;
    Graph Transition;
}FSM;

void alwaysWin( FSM *Opponent,FSM *Result, int *myStates, int startState );
// Creates A 100% winning FSM, given the startState of another.
int nextStep( FSM *Opponent, int oppStates, Bitmask mask);
//Calculates the next best step.
void decisionMaker( FSM *Opponent, FSM *Result, int *myStates, int oppStates, Bitmask mask );


bool identicalStates( FSM *Opponent, int state1, int state2 )
{
    bool similar = Opponent->State[state1] == Opponent->State[state2];
    // Checks if the state is same for both.
    for( int i = 0; i < (int)Opponent->Transition.size(); i++ )
        if ( Opponent->Transition[state1][i] != Opponent->Transition[state2][i] )
            similar = false;
    // If any one of the states gives a different result, then similarity is not present.
    return similar;
}

void minimizeFSM( FSM *Opponent, int *oppStates )
{
    int isMinimized = 1, state1 = 0, state2 = 0;
    for ( int i = 0; i < *oppStates - 1; i++ )
        for ( int j = i + 1; j < *oppStates; j++ )
            if ( identicalStates( Opponent, i, j ) )
            {
                state1 = i;
                state2 = j;
                isMinimized = 0;
                for( int k = j ; k < *oppStates - 1; k++ )
                {
                    Opponent->State[k] = Opponent->State[k+1];
                    for ( int p = 0; p < 3; p++ ) 
                        Opponent->Transition[k][p] = Opponent->Transition[k + 1][p];  
                    
                }
                (*oppStates)--;
                Opponent->State.pop_back();
                Opponent->Transition.pop_back();

                for( int i = 0; i < *oppStates ; i++ )
                    for (int j = 0; j < 3; j++ )
                    {
                        if ( Opponent->Transition[i][j] == state2  )
                            Opponent->Transition[i][j] = state1 ;
                        else if ( Opponent->Transition[i][j] >= state2 )
                            Opponent->Transition[i][j]--;
                    }
            }
    // Deleting the repeated node.
    if ( isMinimized )
        return;
    else 
        minimizeFSM( Opponent, oppStates );
    
    return;
}
map<int,int>KnownMask;
map<int,int>Precalc;

string S = "RPS";

char Beater( char State )
{
    char winn = '\0';
    
    if ( State == 'R' ) winn = 'P';
    else if ( State == 'P') winn = 'S';
    else if ( State == 'S') winn = 'R';
    
    return winn;
}
int main( )
{
    int oppStates = 0, myStates = 0;
    cin >> oppStates;
    
    Bitmask mask = ( 1 << ( oppStates ) ) - 1;

    FSM Opponent, Result;

    for( int i = 0; i < oppStates; i++ )
    {
        vector<int> temp;
        char S; int rok,pap,sci;
        cin >> S >> rok >> pap >> sci;
        temp.push_back(rok-1); temp.push_back(pap-1); temp.push_back(sci-1);
        Opponent.State.push_back(S);
        Opponent.Transition.emplace_back(temp);
    }
    
    minimizeFSM( &Opponent, &oppStates);
    decisionMaker( &Opponent, &Result, &myStates, oppStates, mask);
    minimizeFSM( &Result, &myStates);
    
    cout << myStates + 1<< '\n';
    for ( int i = 0; i <= myStates; i++ )
    {  
        cout << Result.State[i] << " ";
        for ( auto v : Result.Transition[i] )
            cout << v + 1 << " ";
        cout << '\n';
    }

    return 0;
}
void alwaysWin( FSM *Opponent,FSM *Result, int *myStates, int startState )
{
    int resultState = *myStates;
    map<int, int> LoopDetect;
    
    if ( Precalc.count(startState) == 0 )
    {
        Precalc[startState] = resultState + 1;
    }
    else
    {
        int prevEncounter = Precalc[startState];
        Result->State.emplace_back( Beater( Opponent->State[startState] ) );
        vector<int>bak (3,prevEncounter - 2);
        Result->Transition.emplace_back( bak );
        return;
    }

    while ( LoopDetect.count(startState) == 0 )
    {
        vector<int>temp (3,resultState);
        LoopDetect[startState] = resultState;
        
        Result->Transition.emplace_back(temp);
        Result->State.emplace_back( Beater( Opponent->State[startState] ) );

        char opponentPlay = Opponent->State[startState];

        if ( opponentPlay == 'R' )
        {
            startState = Opponent->Transition[startState][PAPER];
            Result->Transition[resultState][ROCK] = resultState + 1;
        }
        else if ( opponentPlay == 'P' )
        {
            startState = Opponent->Transition[startState][SCISSOR];
            Result->Transition[resultState][PAPER] = resultState + 1;
        }
        else if ( opponentPlay == 'S' )
        {
            startState = Opponent->Transition[startState][ROCK];
            Result->Transition[resultState][SCISSOR] = resultState + 1;
        }


        resultState++;
    }

    int prevMeet = LoopDetect[startState];

    resultState--;
    for( int k = 0; k < 3; k++ ) 
        Result->Transition[resultState][k] = prevMeet;

    *myStates = resultState;
}

int nextStep( FSM *Opponent, int oppStates, Bitmask mask)
{
    int r = 0, p = 0, s = 0;

    for ( int i = 0; i < oppStates; i++ )
    {
        Bitmask curr = ( 1 << i );
        if ( curr & mask )
        {
            if ( Opponent->State[i] == 'R' ) r++;
            else if ( Opponent->State[i] == 'P') p++;
            else s++;
        }
    }
    
    if ( r > s && r > p )
    {
        return PAPER;
    }
    else if ( s > p && s > r )
    {
        return ROCK;
    }
    else if ( p > r && p > s )
    {
        return SCISSOR;
    }
    else if ( p == s )
    {
        if ( p > r ) return SCISSOR;
        if ( p < r ) return PAPER;
    }
    else if ( p == r )
    {
        if ( p > s ) return PAPER;
        if ( p < s ) return ROCK;
    }
    else if ( r == s )
    {
        if ( r > p ) return ROCK;
        if ( r < p ) return SCISSOR;
    }
    else
   {
        // All the probs are equal.
        for( int i = 0; i < oppStates; i++ )
        {
            Bitmask cat = ( 1 << i );
            if ( cat & mask )
            {
                int next = Opponent->Transition[i][ROCK] ;
                if ( Precalc.count( next ) != 0 )
                    r++;
            }
        }
        for( int i = 0; i < oppStates; i++ )
        {
            Bitmask cat = ( 1 << i );
            if ( cat & mask )
            {
                int next = Opponent->Transition[i][PAPER] ;
                if ( Precalc.count( next ) != 0 )
                    p++;
            }
        }
        for( int i = 0; i < oppStates; i++ )
        {
            Bitmask cat = ( 1 << i );
            if ( cat & mask )
            {
                int next = Opponent->Transition[i][SCISSOR];
                if ( Precalc.count( next ) != 0 )
                    s++;
            }
        }
        if ( r > s && r > p )
        {
            return PAPER;
        }
        else if ( s > p && s > r )
        {
            return ROCK;
        }
        else if ( p > r && p > s )
        {
            return SCISSOR;
        }
        else if ( p == s )
        {
            if ( p > r ) return SCISSOR;
            if ( p < r ) return PAPER;
        }
        else if ( p == r )
        {
            if ( p > s ) return PAPER;
            if ( p < s ) return ROCK;
        }
        else if ( r == s )
        {
            if ( r > p ) return ROCK;
            if ( r < p ) return SCISSOR;
        }
        else
        {
            // Depth : 2
            for( int i = 0; i < oppStates; i++ )
            {
                Bitmask cat = ( 1 << i );
                if ( cat & mask )
                {
                    int next = Opponent->Transition[i][ROCK];
                    for ( int y = 0; y < 3; y++ )
                        if ( Precalc.count( Opponent->Transition[next][y] ) != 0 )
                            r++;
                }
            }
            for( int i = 0; i < oppStates; i++ )
            {
                Bitmask cat = ( 1 << i );
                if ( cat & mask )
                {
                    int next = Opponent->Transition[i][SCISSOR];
                    for ( int y = 0; y < 3; y++ )
                        if ( Precalc.count( Opponent->Transition[next][y] ) != 0 )
                            s++;
                }
            }
            for( int i = 0; i < oppStates; i++ )
            {
                Bitmask cat = ( 1 << i );
                if ( cat & mask )
                {
                    int next = Opponent->Transition[i][PAPER];
                    for ( int y = 0; y < 3; y++ )
                        if ( Precalc.count( Opponent->Transition[next][y] ) != 0 )
                            p++;
                }
            }

            if ( r > s && r > p )
            {
                return PAPER;
            }
            else if ( s > p && s > r )
            {
                return ROCK;
            }
            else if ( p > r && p > s )
            {
                return SCISSOR;
            }
            else if ( p == s )
            {
                if ( p > r ) return SCISSOR;
                if ( p < r ) return PAPER;
            }
            else if ( p == r )
            {
                if ( p > s ) return PAPER;
                if ( p < s ) return ROCK;
            }
            else if ( r == s )
            {
                if ( r > p ) return ROCK;
                if ( r < p ) return SCISSOR;
            }
            else  
                return ROCK;
        }
    }
    return ( ( KnownMask.size() + Precalc.size() )%3 );
}

void decisionMaker( FSM *Opponent, FSM *Result, int *myStates, int oppStates, Bitmask mask )
{
    int first = -1, second = -1, numStates = *myStates;
    
    for( int i = 0; i < oppStates ; i++ )
    {
        long long check = 1 << i;
        if ( ( check & mask ) && ( first != -1 ) )
        {
            second = i;
            break;
        }
        else if ( check & mask )
            first = i;
    }
    if ( second == -1 )
    {
        alwaysWin( Opponent, Result, myStates, first);
        return;
    }

    Bitmask MASK[3] = {0};

    int decision = nextStep( Opponent, oppStates, mask );
    // Gets the state which will definitely break down the possibilities.
    Result->State.emplace_back( S[decision] );
    // Adding the decision trasnsition to the veector of states.
    
    for ( int i = 0; i < oppStates; i++ )
    {
        long long check = ( 1 << i );

        if ( check & mask )
        {
            int after = Opponent->Transition[i][decision];

            Bitmask nextState = ( 1 << after );
            
            if ( Opponent->State[i] == 'R' )
                MASK[ROCK] |= nextState;
            else if ( Opponent->State[i] == 'P' )
                MASK[PAPER] |= nextState;
            else if ( Opponent->State[i] == 'S' )
                MASK[SCISSOR] |= nextState;
            else cout << "Weird Decision.\n";

        }
    }

    vector<int>temp(3,numStates);
    Result->Transition.emplace_back( temp );

    for ( int i = 0; i < 3; i++)
    {
        if ( MASK[i] == mask )
        {
            MASK[i] = 0;
            return;
        }
    }

    for( int i = 0; i < 3; i++ )
    {
        if ( MASK[i] != 0 )
        {
            if ( KnownMask.count(MASK[i]) != 0 )
            {
                int prevMeet = KnownMask[MASK[i]];
                Result->Transition[numStates][i] = prevMeet;
                
                continue;
            }
            else
            {
                KnownMask[MASK[i]] = *myStates + 1;
                Result->Transition[numStates][i] = ++(*myStates);
                decisionMaker( Opponent, Result, myStates, oppStates, MASK[i] );
            }
        }
    }
    return;
}
