#include <vector>
#include <iostream>
#include <cstdlib>
#include <utility>
#include <ctime>
#include <set>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <fstream>
#include <iterator>
#include <queue>

const int SIZE = 10; 
const int MAX_COST = 100;
using namespace std;

double prob(int &cost,int max_cost = MAX_COST){
  double r = static_cast<double>(rand() % 10000)/10000;
  cost = (rand() % MAX_COST) + 1;
  //or add min cost then rand() % (MAX_COST - MIN_COST) + MIN_COST
  return r;
};

class orderByCost {
  public:
    bool operator() (const int& lhs, const int& rhs) const
    {
      return costs[lhs]<costs[rhs];}
    orderByCost(int* nCosts):costs(nCosts) {};
  private:
    int* costs;
};

struct orderELByCost {
    bool operator() (const tuple<int,int,int>& lhs, const tuple<int,int,int>& rhs) const
    {return get<2>(lhs) < get<2>(rhs);}
};

class UFDS {
  private:
    vector<int> p, rank, setSize;
    int n; //number of sets
  public:
    UFDS(int n):n(n) {
      for (int i = 0;i<n;i++){
        p.push_back(i);
        rank.push_back(0); 
        setSize.push_back(1);
      }
    }
    int findSet(int i) {
      if (p[i] == i)
        return i;
      else
        p[i] = findSet(p[i]);
        return p[i];
    };
    bool isSameSet(int i, int j) {
      return findSet(i) == findSet(j);
    }
    int numSets(){
      return n;
    }
    int sizeOfSet(int i) {
      return setSize[i];
    }
    void unionSets(int item1,int item2) {
      int p1 = findSet(item1);
      int p2 = findSet(item2);
      if (p1 == p2) {
        return;
      }
      if (rank[p1] < rank[p2]) {
        p[p1] = p2;
        setSize[p2] += setSize[p1];
      }
      else if (rank[p1] == rank[p2]) {
        p[p1] = p2;
        rank[p2] += 1;
        setSize[p2] += setSize[p1];
      }
      else {
        p[p2] = p1;
        setSize[p1] += setSize[p2];
      }
      --n;
    };
};

class Graph {  
  protected:
    int* find_shortest_path_tree(const int s) { //Uses Dijkstra.
      fill_n(spt,n,-1);
      int *costs = new int[n]; //Stores the current min cost so we can sort the set using this.
      int top; //Current vertex we are taking neighbours of
      orderByCost comparator(costs);
      set<int,orderByCost> open(comparator);//Stores the vertices we are currently checking.
      unordered_set<int> closed;
      costs[s] = 0; //Free to get to source.
      open.insert(s); //index
      while (!open.empty()) { //Gen all shortest paths then we don't run dijkstra too many times.
        top = *open.begin(); //Cheapest open vertex.
        open.erase(*open.begin()); //No longer open.
        closed.insert(top); //Moves to closed.
        
        for (int i = 0; i < AL[top].size();i++){ //For every connected vertex to the current one.
          if (closed.count(AL[top][i].first) == 0) { //Not yet closed.
            if (open.count(AL[top][i].first) == 0) { //not in open or closed i.e add to open.
              costs[AL[top][i].first] = costs[top] + AL[top][i].second;
              open.insert(AL[top][i].first); //New discovery - add it to open
              spt[AL[top][i].first] = top; //Parent in shortest path tree. 
            }
            else { //not done but in pq
              if (costs[AL[top][i].first] > costs[top] + AL[top][i].second) { //new path is cheaper
                open.erase(AL[top][i].first); //Remove and readd
                costs[AL[top][i].first] = costs[top] + AL[top][i].second; //New cheaper path.
                open.insert(AL[top][i].first);
                spt[AL[top][i].first] = top; //Parent in shortest path tree.     
              }
            }
          }
        }
      }
      return costs;
    }
  public:
    int n;
    vector<vector<pair<int,int>>> AL;
    int* spt;
    string name;
    Graph *MST;//Use Pointer to avoid infinite recursive types and memory clogging.
    vector<tuple<int,int,int>> EL;
    vector<vector<int>> AM;
    bool MSTGenned=false;
    
    Graph(const int size,string nName="Unnamed"):AL(vector<vector<pair<int,int>>>(size,vector<pair<int,int>>(0))),n(size),spt(new int[size]),name(nName),AM(vector<vector<int>>(size,vector<int>(size,0))) {} //Should have used delegation in later ones.

    Graph(const int size,const float density,const int max_cost=MAX_COST,string nName="Unnamed"):AL(vector<vector<pair<int,int>>>(size,vector<pair<int,int>>(0))),n(size),spt(new int[size]),name(nName),AM(vector<vector<int>>(size,vector<int>(size,0))){ //The 0 tells it to be empty
      int cost;
      for (int i = 0; i<size;i++) {
        for (int j=i;j<size;j++){
          if (prob(cost,max_cost)<density && i !=j) {
            //pair<int,int>* a = new pair<int,int>(j,cost);
            addEdge(i,j,cost);
            addEdge(j,i,cost);
          }
        }
      }
    };
    Graph (const string fileName,const string nName="Unnamed") { //Reads Graph in from file. File should be a simple text file with at the top 1 line containing the number of nodes and then subsequently lines containing triples of a,b,cost for each edge. Undirected edges should feature twice in the file - both ways.
      int cost,a,b;
      ifstream input(fileName);
      istream_iterator<int> start(input);
      n = *start;
      cout << n;
      AL = vector<vector<pair<int,int>>>(n,vector<pair<int,int>>(0));
      AM = vector<vector<int>>(n,vector<int>(n,0));
      spt = new int[n];
      name = nName; 

      start++;
      for (istream_iterator<int> end;start != end;start++) {
        a = *start; start++;
        b = *start; start++;
        cost = *start;
        addEdge(a,b,cost); 
      }
    }
    Graph(vector<tuple<int,int,int>> nEL,int size,string nName="Unnamed"):AL(vector<vector<pair<int,int>>>(size,vector<pair<int,int>>(0))),n(size),spt(new int[size]),name(nName),EL(nEL),AM(vector<vector<int>>(size,vector<int>(size,0))) { //Produce graph from EL
      for (auto edge : nEL) {
        addEdge(edge);
      }
    }
    //Copy constructor? 

    void print_SSSP(const int source,const int* dests,int nQueries) {
      int *costs = find_shortest_path_tree(source);
      int dest;
      cout << "Shortest paths from source " << source << " on graph " << name << endl;
      for (int i = 0;i<nQueries;i++) {
        dest = dests[i];
        cout << "Shortest path from " << source << " to " << dest << ":" << endl;
        int c = dest;
        vector<int> path;
        while (!(c == source || c==-1)) {
          path.push_back(c);
          c = spt[c];
        }
        if (c == -1) {
          cout << "\tUnreachable - No path exists" << endl;
        }
        else {
          cout << "\t" << source << ", ";
          for (int i=path.size()-1;i>=0;i--){
            cout << path[i] << ", ";
          }
          cout << endl;
          cout << "\t" << "The cost is " << costs[dest] << "." << endl;
        }
      }
      delete costs;
    }
    void addEdge(int i,int j,int cost) {
      AL[i].emplace_back(j,cost);//creates a pair 
      EL.emplace_back(i,j,cost);
      AM[i][j] = cost;
    }
    void addEdge(tuple<int,int,int> edge) {
      addEdge(get<0>(edge),get<1>(edge),get<2>(edge));
    }
    void addReverseEdge(tuple<int,int,int> edge){
      addEdge(get<1>(edge),get<0>(edge),get<2>(edge));
    }
    bool hasEdge(int i,int j) {
      return (AM[i][j] != 0);
    }
    int kruskalMST() {
      int cost = 0;
      if (MSTGenned)
        delete MST;
      MST = new Graph(n,"MST of " + name);
      sort(EL.begin(),EL.end(),orderELByCost());
      UFDS forest(n);
      for (auto edge : EL) {
        if (!forest.isSameSet(get<0>(edge),get<1>(edge))) {
          MST->addEdge(edge);
          MST->addReverseEdge(edge);
          forest.unionSets(get<0>(edge),get<1>(edge));
          cost += get<2>(edge); //Add both directions but only count once for cost
        }
        if (forest.numSets() == 1){
          break;
        }
      }
      if (forest.numSets() != 1) {
        cout << "Unconnected graph, MST creation failed";
        delete MST;
      }
      else 
        MSTGenned = true;
      return cost;
    }

    int primMST() {
      int cost = 0;
      if (MSTGenned) //If already generated delete and restart - we might have added edges in the mean time and in between time.
        delete MST;
      MST = new Graph(n,"MST of " + name);
      tuple<int,int,int> topEdge;
      priority_queue<tuple<int,int,int>,vector<tuple<int,int,int>>,orderELByCost> nextBest;
      unordered_set<int> verticesInMST; verticesInMST.insert(0);
      for (auto candidate : AL[0]) {
        nextBest.push(tuple<int,int,int>(0,candidate.first,-candidate.second));
      } 
      while (verticesInMST.size() < n && !nextBest.empty()){ //While MST not built and we still have edges to add
        topEdge = nextBest.top();nextBest.pop();
        if (verticesInMST.count(get<1>(topEdge)) != 0) //not yet in MST so valid edge
          continue;//We already added this vertex. lazy deletion
        MST->addEdge(get<0>(topEdge),get<1>(topEdge),-get<2>(topEdge));
        MST->addEdge(get<1>(topEdge),get<0>(topEdge),-get<2>(topEdge));
        verticesInMST.insert(get<1>(topEdge));
        cost -= get<2>(topEdge);
        for (auto candidate : AL[get<1>(topEdge)]) {
          nextBest.push(tuple<int,int,int>(get<1>(topEdge),candidate.first,-candidate.second));
        }
      }
      if (verticesInMST.size() != n) {
        cout << "Unconnected graph, MST creation failed";
        delete MST;
      }
      MSTGenned = true;
      return cost;       
    }

    int primMaxST() {
      int cost = 0;
      if (MSTGenned) //If already generated delete and restart - we might have added edges in the mean time and in between time.
        delete MST;
      MST = new Graph(n,"MaxST of " + name);
      tuple<int,int,int> topEdge;
      priority_queue<tuple<int,int,int>,vector<tuple<int,int,int>>,orderELByCost> nextBest;
      unordered_set<int> verticesInMST; verticesInMST.insert(0);
      for (auto candidate : AL[0]) {
        nextBest.push(tuple<int,int,int>(0,candidate.first,candidate.second));
      } 
      while (verticesInMST.size() < n && !nextBest.empty()){ //While MST not built and we still have edges to add
        topEdge = nextBest.top();nextBest.pop();
        if (verticesInMST.count(get<1>(topEdge)) != 0) //not yet in MST so valid edge
          continue;//We already added this vertex. lazy deletion
        MST->addEdge(get<0>(topEdge),get<1>(topEdge),get<2>(topEdge));
        MST->addEdge(get<1>(topEdge),get<0>(topEdge),get<2>(topEdge));
        verticesInMST.insert(get<1>(topEdge));
        cost += get<2>(topEdge);
        for (auto candidate : AL[get<1>(topEdge)]) {
          nextBest.push(tuple<int,int,int>(get<1>(topEdge),candidate.first,candidate.second));
        }
      }
      if (verticesInMST.size() != n) {
        cout << "Unconnected graph, MST creation failed";
        delete MST;
      }
      MSTGenned = true;
      return cost;       
    }
   ~Graph () {
      if (MSTGenned)
        delete MST;
      delete[] spt; //1 new implies 1 delete :). I wanted to delete AL but it seems that goes automatically.

   }
};
ostream& operator << (ostream &o,const Graph &a) {//Make a friend 
  o << endl << "Printout of graph " << a.name << endl;
  o << "Human Readable:" << endl;
  int i;
  int j;
  cout << "Graph has " << a.n << " nodes numbered 0 to " << a.n-1 << endl;
  for (i = 0;i<a.n;i++){
    for(j = 0;j<a.AL[i].size();j++) {
      o << "Node " << i << " is connected to node " << a.AL[i][j].first << " with weight " << a.AL[i][j].second << endl;  
    }
  }
  o << "CS Academy:" << endl;
  o << "Node Count: " << a.n << endl;
  for (i = 0;i<a.n;i++){
    for(j = 0;j<a.AL[i].size();j++) {
      o << i << " " << a.AL[i][j].first << " "<< a.AL[i][j].second << endl;  
    }
  }  
  o << endl;
  return o;
}