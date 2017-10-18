/* Referred from open source HEFT implementation provided on-https://github.com/hackerkid/HEFT-Scheduling/blob/master/heft.cpp
 */
#include <iostream>
#include <vector>
#include <cstdio>
#include <string.h>
#include <utility>
#include <algorithm>
#include <limits.h>
using namespace std;

class processor_slot
{
	public:
		double start_time;
		double end_time;
		int task_no;

};

struct time_slot
{
	int processor;
	double start_time;
	int task_no;
};

int dropped_tasks[1000];
double rank[1000];
double avg[1000]; 	// avg computation time of i
int adj[1000][1000];	// communication cost matrix
double task_time[1000][50]; // task_time[i][j] = computaion cost of i on j 
int proc;	// number of processsors
int tasks;	// number of tasks
double eft[1000][50];
double est[1000][50];
double aft[1000];
double ast[1000];
vector <int> parent[1000]; // list of parents of i
int mini_processor[1000];	// processor on which i is scheduled
vector < pair <double, int> > rank_task; // sorted list of tasks acc to their ranks
int jobs; // number of jobs/workflows/dags 
int totaltasks = 0;
int totaledges = 0;
double deadline[1000];
double priority[1000];
int startindexe[1000];
int startindext[1000];
int type[1000];
int LFT[1000];
int scheduled[1000];


vector <processor_slot> processor_scheduler[100]; //schedule for the processor

double rec_rank(int i)
{
    double maxi = 0;
	double temp = 0;

    for (int j = 0; j < totaltasks; j++) {
        if(adj[i][j] != -1) {
        	//&& std::find(parent[i].begin(), parent[i].end(), j) != parent[i].end()
			temp = rec_rank(j);
			// cout<< "for j "<< j << " temp is" << temp;
            if (maxi < adj[i][j] + temp)
                maxi = adj[i][j] + temp;
            }

    }
    rank[i] = avg[i] + maxi;
    // cout<< "for process i "<< i << "maxi, avgi and rank are "<< maxi <<" "<< avg[i] <<" " << rank[i] << endl;
    return rank[i];
}

double est_cal(int i, int j)
{
	int comm;
	int par;
	double max_time;
	double temp = 0;;

	max_time = 0;

	for (int k = 0; k < parent[i].size(); k++) {
		par = parent[i][k];
		if(mini_processor[par] == j) {
			temp = aft[par];
		}

		else {
			temp = adj[par][i] + aft[par];
		}

		max_time = max(max_time, temp);
	}

	return max_time;

}
			
double eft_cal(int i, int j) // i is task, j is the processor
{
		if(processor_scheduler[j].empty()) {
			return est[i][j] + task_time[i][j];
		}
		double endtime;
		ok = 1;
        //cout << "Processor: " << j << " task: " << i << " est: " << est[i][j] << " procend: " << processor_scheduler[j][processor_scheduler[j].size()-1].end_time;

        endtime = max(processor_scheduler[j][processor_scheduler[j].size()-1].end_time, est[i][j]) + task_time[i][j];
        //cout << " eft: " << endtime << endl;
        return endtime;

}

double LFT_calc(int i)
{
	double lft = 0;
	double temp = 0;
	for(int j=0; j< totaltasks; j++)
	{
		if(adj[i][j] != -1 && i!=j && scheduled[j] == 1)
		{
			temp =  aft[j] - task_time[j][mini_processor[j]] - adj[i][j];
		}
		if(lft<temp && lft!=0)
			lft = temp;
	}
	return lft;
}

double buffer(int i)
{
	int temp = 0;
	double est_child =0;
	double idle_time = 0;
	int pro = mini_processor[i];
	for (int j = 0; j < processor_scheduler[pro].size(); j++)
	{
		if(processor_scheduler[pro][j].task_no == i && j!=processor_scheduler[pro].size())
			{
				idle_time = processor_scheduler[pro][j+1]. start_time - processor_scheduler[pro][j].end_time;
				break;
			}
		else idle_time = task_time[i][mini_processor[i]];
	}

	if(idle_time > 0)
	{
		for (int j = 0; j < totaltasks; j++)
		{
			temp = 0;
			if(adj[i][j] != -1 && i!=j)
				temp = buffer(j);
			temp += ast[j]- adj[i][j]; 
			if(est_child!=0 && temp < est_child)
				est_child = temp;
		}

        if (aft[i] + idle_time < est_child)
            {
                if(idle_time<0)
                    dropped_tasks[i] = 1;
                return idle_time;
            }
        else {
            if (est_child - aft[i] < 0)
                dropped_tasks[i] = 1;
            return est_child - aft[i];
        }
	}

	else return 0;
}

time_slot find_satisfiable_resource(int i)
{
	time_slot abc;
	abc.processor = -1;
	abc.start_time = -1;
	double time_gap = 0;
	int temp =0;
	for (int j = 0; j < proc; j++)
	{
		time_gap = 0;
		for (int k = processor_scheduler[j].size()-1 ; k >= 0 ; k--)
		{
			// temp = 0;
			if(processor_scheduler[j][k].start_time >= est[i][j] && processor_scheduler[j][k].start_time <= deadline[i])
				time_gap = buffer(k);
			if(time_gap + processor_scheduler[j][k].start_time - processor_scheduler[j][k-1].end_time >= task_time[i][j])
				{
					abc.processor = j;
					abc.start_time = processor_scheduler[j][k-1].end_time;
					abc.task_no = k;
					return abc;
				}
		}
	}

	return abc;
}

int movetask(int i)
{
	processor_slot temporary;
	int temp = 0;
	int est_child =0;
	int idle_time = 0;
	int pro = mini_processor[i];
	for (int j = 0; j < processor_scheduler[pro].size(); j++)
	{
		if(processor_scheduler[pro][j].task_no == i && j!=processor_scheduler[pro].size())
			{
				idle_time = processor_scheduler[pro][j+1]. start_time - processor_scheduler[pro][j].end_time;
				break;
			}
		else idle_time = task_time[i][mini_processor[i]];
	}

	if(idle_time > 0)
	{
		for (int j = 0; j < totaltasks; j++)
		{
			temp = 0;
			if(adj[i][j] != -1 && i!=j)
				temp = movetask(j);
			temp = ast[j]- adj[i][j]; 
			if(est_child!=0 && temp < est_child)
				est_child = temp;
		}

		if (aft[i] + idle_time < est_child)
		{
			for (int j = 0; j < processor_scheduler[pro].size(); j++)
			{
				if(processor_scheduler[pro][j].task_no == i)
				{
					processor_scheduler[pro][j].start_time = ast[i]+idle_time;
					processor_scheduler[pro][j].end_time = aft[i]+idle_time;
					aft[i] += idle_time;
					ast[i] += idle_time;
					return idle_time;
				}

			}

		}
		else 
		{
			for (int j = 0; j < processor_scheduler[pro].size(); j++)
				{
					if(processor_scheduler[pro][j].task_no == i)
					{
						processor_scheduler[pro][j].start_time = ast[i]+ est_child - aft[i];
						processor_scheduler[pro][j].end_time = aft[i]+ est_child - aft[i];
						aft[i] += est_child - aft[i];
						ast[i] += est_child - aft[i];
						return est_child - aft[i];;
					}

				}
		}
	}

	// else return 0;
}


void reschedule(int i)
{
	int next_task;
	processor_slot temp,temp2;
	time_slot pqr;
	for(int j=0; j< totaltasks; j++)
	{
		if(adj[j][i] != -1 && i!=j)
		{
			if(aft[j] > deadline[j])
				reschedule(j);
		}
	}

	pqr = find_satisfiable_resource(i);
	if(pqr.processor !=-1)
	{
		mini_processor[i] = pqr.processor;
		temp.start_time = pqr.start_time;
		temp.task_no = i;
		temp.end_time = pqr.start_time + task_time[i][pqr.processor];
		std::vector<processor_slot>::iterator v;
		for(v = processor_scheduler[pqr.processor].begin(); v < processor_scheduler[pqr.processor].end(); v++)
		{
			temp2=*v;
			if(temp2.task_no == pqr.task_no)
			{
				processor_scheduler[pqr.processor].insert(v,temp);
				next_task = pqr.task_no;
				break;
			}
		}
		//movetask(next_task);
	}

}

void heft()
{
	int mini;
	mini = INT_MAX;
	int t;
	double mini_eft;
	double length = -1;

	processor_slot temp;
	processor_slot temp2;
	t = rank_task[0].second;

	for (int i = 0; i < proc; i++) {
		est[rank_task[0].second][i] = 0;
		if(mini > task_time[t][i]) {
			mini = task_time[t][i];
			mini_processor[t] = i;
		}

	}

	temp.start_time = 0;
	temp.end_time = mini;
	temp.task_no = t;

	processor_scheduler[mini_processor[t]].push_back(temp);
	ast[rank_task[0].second] = 0;
	aft[rank_task[0].second] = mini;
	scheduled[rank_task[0].second] = 1;

	int i;
	for (int h = 1; h < totaltasks; h++) {
		i = rank_task[h].second;

		for (int j = 0; j < proc; j++) {
			est[i][j] = est_cal(i, j);
		    //cout<<" for task "<< i << " on processor "<< j <<" est is "<<est[i][j] << endl;

		}
	
		mini_eft = INT_MAX;
		int processor_no = 0;

		for (int j = 0; j < proc; j++) {
			eft[i][j] = eft_cal(i, j);
			//cout<<" for task "<< i << " on processor "<< j <<" eft is "<<eft[i][j] << endl;
			if(eft[i][j] < mini_eft) {
				mini_eft = eft[i][j];
				processor_no = j;
			}
			else if(eft[i][j] == mini_eft)
			{
				for(std::vector<int>::iterator xyz = parent[i].begin(); xyz != parent[i].end(); ++xyz)
				if(j == mini_processor[*xyz])
				{
					// cout << *xyz;
					processor_no = j;
				}
			}
		}
		
		mini_processor[i] = processor_no;
		
		if(type[i] == 1 && deadline[i] < mini_eft)
			reschedule(i);

		temp2.start_time = mini_eft - task_time[i][processor_no];
		ast[i] = mini_eft - task_time[i][processor_no];
		temp2.end_time = mini_eft;
		aft[i] = mini_eft;
		temp2.task_no = i;
		length = max(mini_eft, length);

		processor_scheduler[processor_no].push_back(temp2);

	}

	for (int i = 0; i < proc; i++) {
		cout << "On processor" << i << endl;
		for (int j = 0; j < processor_scheduler[i].size(); j++) {
			cout << " Task:" << processor_scheduler[i][j].task_no << " priority => " << priority[processor_scheduler[i][j].task_no] << " start time " << processor_scheduler[i][j].start_time << " end time " << processor_scheduler[i][j].end_time << endl;
		}
		cout << endl;

	}

	cout << length << endl;
}
	
int main()
{
    int x;
    int y;
    int z;
    int n;
    float ans;
    int p;
	double temp;
	int edges_count;
    clock_t begin, end;
    double time_spent;
	begin = clock();
    memset(adj, -1, sizeof(adj));

	cout << "Enter the number of workflows\n";
	cin >> jobs;

	int edgesj[jobs];
	int tasksj[jobs];
	double ccr[jobs];
	int tempe = 0;
	int tempt = 0;
	double tempcomm = 0;
	double tempcomp = 0;

	cout<< "Enter number of processors \n";
	cin >> proc;
	cout << "Enter number of tasks and edges in each workflow \n";
	for (int q=0; q< jobs; q++)
		cin >> tasksj[q] >> edgesj[q];

	for(int q =0; q<jobs ; q++)
	{
		startindexe[q] = tempe;
		startindext[q] = tempt;
		tempe = tempe + edgesj[q];
		tempt = tempt + tasksj[q];
		type[tempt-1] = 1;
		// cout<<"start index of tasks and edges for workflow "<<q<<" is "<< startindext[q] <<" "<<startindexe [q] <<endl;
	}

	totaltasks = tempt;
	totaledges = tempe;

	// cout << "total tasks and edges are "<<tempt<<" "<<tempe<<endl;

	cout<< "Enter the time a tasks takes on each processor\n";
    for (int i = 0; i < totaltasks; i++) {
		temp = 0;
    	for (int j = 0; j < proc; j++) {
			cin >> task_time[i][j];
            //cout << "task,proc " << i << " "<< j << " time: "<< task_time[i][j] << endl; 
			temp += task_time[i][j];
		}
		avg[i] = (double) temp / (double) proc;
	}

	cout<<"Enter the edges in the format of parent, child, communication cost \n";
	for (int i = 0; i < totaledges; i++) {
		cin >> x >> y >> z;
		adj[x][y] = z;
		parent[y].push_back(x);
	}

	cout << "Enter the deadline for each workflow\n";
	for (int i=0; i < jobs; i++)
		cin >> deadline[startindext[i+1]-1];
	
	
	    
	for (int i = totaltasks-1; i >= 0; i--) {
        rec_rank(i);
        // cout<< "for task "<<i<<" rank is "<<rank[i]<<endl;
    }

    //calculating Communication to computation ratio (CCR)
	for(int i=0; i<jobs; i++)
	{
		tempcomp = 0;
		tempcomm = 0;
		for(int q= startindext[i]; q<startindext[i] + tasksj[i]; q++)
		{
			tempcomp += avg[q];
			deadline[q] = deadline[startindext[i+1]-1]-rank[q]+avg[q]; 
			priority[q] = rank[q]-deadline[startindext[i+1]-1];
		}
		for(int j=startindexe[i]; j<startindexe[i] + edgesj[i]; j++)
			for(int  q= startindexe[i]; q<startindexe[i] + edgesj[i]; q++)
				if(adj[j][q] != -1)
					tempcomm += adj[j][q];
		ccr[i] = tempcomm/tempcomp;
		// cout<< "CCR for workflow "<< i << " is "<< ccr[i]<< endl;
	}

	for(int i=0; i<totaltasks; i++)
	{
		// cout << "Priority of task "<< i << " is: "<< priority[i]<< endl;
	}

	// cout << "Deadlines are \n";
	// for (int i=0; i < totaltasks; i++)
	// 	cout << deadline[i] << " ";
	// cout << endl;

    for (int i = 0; i < totaltasks; i++) {
		rank_task.push_back(make_pair(priority[i], i));
    }
    
	sort(rank_task.rbegin(), rank_task.rend());
  
	heft();
    int dt=0;
    for(int l = 0; l<totaltasks;l++){
        dt+=dropped_tasks[l];
    }
    cout << "Dropped tasks: " << dt << endl;
    end = clock();
    time_spent = (double)(end - begin);
    cout << time_spent << endl;
    return 0;

}
