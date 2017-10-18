#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
using namespace std;

#define MIN_PER_RANK 1 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 5
#define MIN_RANKS 3    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 5
#define PERCENT 30     /* Chance of having an Edge.  */
#define MAX_COMMCOST 10
#define MAX_COMPCOST 15
#define MAX_JOBS 15
#define PROCESSORS 10

struct edge{
    int node1;
    int node2;
    int cost;
};



vector<edge> totaledges;
vector<int> totalnodes;

int main (void)
{
    int i, j, k,nodes = 0;
    int wght_edge,prev_nodes=0;
    edge temp;
    vector<edge> edges;
    srand (time (NULL));
    cout << MAX_JOBS << " " << PROCESSORS << endl;

    for (int m=0;m<MAX_JOBS;m++){
        int ranks = MIN_RANKS + (rand () % (MAX_RANKS - MIN_RANKS + 1));
        int wght_node[100] = {};
        nodes = 0;
        edges.clear();

        //cout << "digraph {" << endl;
        for (i = 0; i < ranks; i++)
        {
            /* New nodes of 'higher' rank than all nodes generated till now.  */
            int new_nodes = MIN_PER_RANK + (rand () % (MAX_PER_RANK - MIN_PER_RANK + 1));

            /* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
            for (j = 0; j < nodes; j++) {
                for (k = 0; k < new_nodes; k++) {
                    if(!wght_node[j]){
                        wght_node[j] = (rand() % MAX_COMPCOST) + 1;
                        //cout << "node " << j << " = " << wght_node[j] << endl;
                    }
                    if(!wght_node[k+nodes]){
                        wght_node[k+nodes] = (rand() % MAX_COMPCOST) + 1;
                        //cout << "node " << k+nodes << " = " << wght_node[k+nodes] << endl; 
                    }
                    if ( (rand () % 100) < PERCENT){
                        wght_edge = (rand() % MAX_COMMCOST) + 1; 
                        temp.node1 = j+prev_nodes;
                        temp.node2 = k+nodes+prev_nodes;
                        temp.cost = wght_edge;
                        edges.push_back(temp);
                        //cout << j << "->" << k+nodes << "," << wght_edge << endl; /* An Edge.  */
                     }
                }
             }
          nodes += new_nodes; /* Accumulate into old node set.  */
        }
        prev_nodes +=nodes;
        
        for(i =0;i< nodes;i++) {
            totalnodes.push_back(wght_node[i]);
        }

        totaledges.insert(totaledges.end(),edges.begin(),edges.end());
        
        cout << nodes << " " << edges.size() << endl;
    }

    for(i = 0; i < totalnodes.size(); i++){
        for(j=0;j<PROCESSORS;j++){
            cout << (0.8+j*0.4/(PROCESSORS-1))*totalnodes[i] << " ";
        }
        cout << endl;
    }

    for(vector<edge>::iterator it = totaledges.begin(); it != totaledges.end(); ++it){
         temp = *it;
         cout << temp.node1 << " " << temp.node2 << " " << temp.cost << endl;
    }

    //cout << "}" << endl;
    /*
    cout << nodes << " " << 3 << endl;
    for(i = 0; i < nodes; i++){
        cout << 0.8*wght_node[i] << " " << wght_node[i] << " " << 1.2*wght_node[i] << endl;
    }

    cout << totaledges.size() << endl;
    for(vector<edge>::iterator it = totaledges.begin(); it != totaledges.end(); ++it){
        temp = *it;
        cout << temp.node1 << " " << temp.node2 << " " << temp.cost << endl;
    }*/
    return 0;
}
