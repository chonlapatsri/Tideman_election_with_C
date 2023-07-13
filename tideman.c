#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
        //printf("%s ", candidates[i]);
    }
    //printf("\n");

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // TODO
    // Check if voter votes a candidate in the list, if yes rank it and return true
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i; // i is index of a candidate
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // TODO
    // Record pereference of a voter in global array preference (keep adding up one voter by another)
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
            //printf("%s > %s by %i, ", candidates[ranks[i]], candidates[ranks[j]], preferences[ranks[i]][ranks[j]]);

        }
    }
    //printf("\n");
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // adding counterparts together to see whether more votre prefre i over j which give +ve int, while giveing -ve for the opposite
            int diff = preferences[i][j] - preferences[j][i];
            // printf("%s vs %s the differnece is %i\n", candidates[i], candidates[j], diff);

            // use if-else to assign winner and loser of each pair;  only count pairs if there are winners and losers
            if (diff > 0)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (diff < 0)
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
            // printf("pair %i: (%s & %s)\n", pair_count, candidates[pairs[pair_count - 1].winner], candidates[pairs[pair_count - 1].loser]);
        }
    }
    // printf("\n");
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // TODO
    for (int i = 0; i < pair_count; i++)
    {
        // create dummies
        int max = 0;
        int max_pair = i;

        // Use selection sort due to small number of pairs (max is only 36)

        // this loop to collect the highest win in the remianing range
        for (int j = i; j < pair_count; j++)
        {
            int win_by = preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[j].loser][pairs[j].winner];
            if (win_by < 0)
            {
                win_by = win_by * -1;
            }

            if (win_by > max)
            {
                max = win_by;
                max_pair = j;
                // printf("The highest pair is the %i element\n", j);
            }
        }

        // if the highest win is located after the ith element then switch their places
        if (i < max_pair)
        {
            // Store values of pair[i]
            int winner_i = pairs[i].winner;
            int loser_i = pairs[i].loser;

            // Sort the maximum win found to the ith location
            pairs[i].winner = pairs[max_pair].winner;
            pairs[i].loser = pairs[max_pair].loser;

            // Re-assign value to of where maximum win used to be located with values of ith element before swap
            pairs[max_pair].winner = winner_i;
            pairs[max_pair].loser = loser_i;

            // printf("switch %i element with %i element\n", i, max_pair);
        }
    }
    //for (int i = 0; i < pair_count; i++)
    // {
    // printf("The pair#%i is (%s & %s), with difference of %i\n", i, candidates[pairs[i].winner], candidates[pairs[i].loser], preferences[pairs[i].winner][pairs[i].loser] - preferences[pairs[i].loser][pairs[i].winner]);
    // }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // TODO
    for (int i = 0; i < pair_count; i++)
    {
        int winner = pairs[i].winner;
        // printf("for pair %i, winner is %s, loser is %s\n", i, candidates[winner], candidates[pairs[i].loser]);

        // check the entire row of the winner of the current pair to see if there has already been a line which the winner of this pair is on the losing end
        for (int j = 0; j < candidate_count; j++)
        {
            // check if there is true on the opposite direction, which mean current ith candidate has lost to one of the other candidates
            if (locked[j][winner] == true)
            {

                // check if jth candidate is the loser of current pair, if yes it will create cycle
                if (j == pairs[i].loser)
                {
                    locked[pairs[i].winner][pairs[i].loser] = false;
                    break;
                }

                // if not a cycle yet, then we should also check the entire row of the winner of the existed line who has beated the winner of the ith pair
                else
                {
                    winner = j; // the winner of ith pair is defeated and jth candidate is now the undefeated so check him instead
                    j = -1; // assign j to -1 so it restart at 0 when it returns to governing for loop
                }
            }
            // if the entire row(s) has been checked and found no cycle, create the line between the ith pair's winner and loser.
            else if (j == candidate_count - 1)
            {
                locked[pairs[i].winner][pairs[i].loser] = true;
            }
        }
    }
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            // check if a link indicating that ith candidate has ever lost jth candidate existed, check both ways becasue false can be misleading (as it is default value of locked[])
            if (locked[i][j] == false && locked[j][i] != false)
            {
                break;
            }
            // if ith candidate went through all other candidates but suffered no lost (except those losses that would create cycle), declare hime the winner
            else if (j == candidate_count - 1)
            {
                string winner = candidates[i];
                printf("%s\n", winner);
                break;
            }
        }
    }
    return;
}