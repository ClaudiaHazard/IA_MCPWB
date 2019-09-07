#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <fstream>
#include <random>
#include <deque>
#include <algorithm>
using namespace std;

#define C 2   //Penalty excede capacity.
#define P 100   //Penalty excede capacity.
#define N 50  // Penalty types of milk.
#define NN 2   //Penalty types of milk.
#define Q 100   // Factor of number of wolves.
#define T 100000 //End criteria.
#define TW 10 // Criteria of repair, auto-adaptive.
#define SCAP 0.9 // Probability of scape.
#define S1 0.7 // Probability of type scape option.
#define R1 0.7 // Probability of type repair option.
#define O 0.1 // Probability to try delete
#define AD 0.4 // Porcentage of diference in size accepted by paths.

//g++ TabuSearch.cpp -o -Wall -std=gnu++11
//./-Wall Instancia.txt NAME SEED

// OBJECTS
////////
///
//Farm definition.
class farm{
  public:
    int id;
    int x_axe;
    int y_axe;
    char milk_type;
    int q_milk;
    bool take;
    float milk_val;
  farm(int i,int x, int y, char mt,int mq,bool ta,float mil){
    this->id=i;
    this->x_axe=x;
    this->y_axe=y;
    this->milk_type=mt;
    this->q_milk=mq;
    this->take=ta;
    this->milk_val=mil;
  }
};

class milk{
  public:
    float value;
    char milk_type;
    int quota;
  milk(char mt, float v, int qu){
    this->milk_type=mt;
    this->value=v;
    this->quota=qu;
  }
};

class truck{
  public:
    int number;
    int capacity;
    char milk_type;
    std::vector<farm> path;
  truck(int n, int ca, char mt, std::vector<farm> p){
    this->milk_type=mt;
    this->number=n;
    this->capacity=ca;
    this->path=p;
  }
};

class cluster{
  public:
    std::vector<farm> f;
    float x_axe;
    float y_axe;
    float initx;
    float inity;
    int capacity;
    int min;
    int max;
    float milk_val;
  cluster(std::vector<farm> ff,float x,float y,int c, int mi, int ma,int xx, int yy,float milk){
      this->f=ff;
      this->x_axe=x;
      this->y_axe=y;
      this->capacity=c;
      this->min=mi;
      this->max=ma;
      this->initx=xx;
      this->inity=yy;
      this->milk_val=milk;
  }
};

class clusters{
  public:
    std::vector<cluster> clu;
    std::vector<farm> graph;
  clusters(std::vector<cluster> c,std::vector<farm> gr){
    this->clu=c;
    this->graph=gr;
  }
};

class wolf{
  public:
    std::vector<truck> trucks;
    float ef;
    int c;
  wolf(std::vector<truck> t, int count){
    this->trucks=t;
    this->c=count;
  }
};

/////
////////////////

//FUNCTIONS
///////////

//////
//DISTANCE
////////////
float distanceCluster(farm f1, cluster f2){
  float xdistance,ydistance,distance;
  xdistance=(float)f1.x_axe-f2.x_axe;
  ydistance=(float)f1.y_axe-f2.y_axe;
  distance=pow(pow(xdistance,2)+pow(ydistance,2),0.5);
  return distance;
}



float distance(farm f1, farm f2){
  float xdistance,ydistance,distance;
  xdistance=f1.x_axe-f2.x_axe;
  ydistance=f1.y_axe-f2.y_axe;
  distance=pow(pow(xdistance,2)+pow(ydistance,2),0.5);
  return distance;
}

float finalDistance(std::vector<farm> f){
  farm fa1=f[0];
  farm fa2=f[0];
  float totaldistance,distan=0;
  for(unsigned int i=0; i<f.size()-1;i++){
    fa1=f.at(i);
    fa2=f.at(i+1);
    distan=distance(fa1,fa2);
    totaldistance+=distan;
    //std::cout<<i<<" "<<distan<<" "<<i+1<<" \n";
  }
  return totaldistance;
}

//////
//////////////////////
////
//MILK
///////////


float valmilk(char type,std::vector<milk> milks){
  for(unsigned i=0;i<milks.size();i++){
    if(milks[i].milk_type==type){
      return milks[i].value;
    }
  }
}




char blendMilk(char t1,char t2){
  if(t1!='-' && t2!='-'){
    if(t1 == t2){
      return t1;
    }
    if(t1>t2){
        return t1;
    }
    else{
      return t2;
    }
  }
  else{
    if(t1=='-'){
      return t2;
    }
    else{
      return t1;
    }
  }
}

char finalMilk(std::vector<farm> f){
  farm fa1=f[0];
  farm fa2=f[0];
  char type='-';
  bool flag=0;
  if(f.size()>1){
    for(unsigned int i=0; i<f.size()-1;i++){
      fa1=f.at(i);
      if(fa1.take==1 && flag==0){
        type=fa1.milk_type;
        flag=1;
      }
      fa2=f.at(i+1);
      if (fa2.take==1){
        type=blendMilk(type,fa2.milk_type);
      }
    }
  }
  else{
    fa1=f.at(0);
    if(fa1.milk_type!='-' && fa1.take==1){
      type=fa1.milk_type;
      flag==1;
    }
  }
  return type;
}

int finalQMilk(std::vector<farm> f){
  farm fa=f[0];
  int total=0;
  for(unsigned int i=0; i<f.size();i++){
    fa = f.at(i);
    if(fa.take==1){
      total+=fa.q_milk;
    }
  }
  return total;
}

////
////////////////


/////////SHOW////////
void showFarm(std::vector<farm> f){
  farm fa=f[0];
  for(unsigned int i=0; i<f.size();i++){
    fa=f.at(i);
    std::cout <<"id: " <<fa.id << " x: "<< fa.x_axe<< " y: " << fa.y_axe << " type: " << fa.milk_type << " quantity: " << fa.q_milk << "\n";
  }
}

void showTruck(std::vector<truck> t){
  truck tr=t[0];
  for(unsigned int i=0; i<t.size();i++){
    tr=t.at(i);
    std::cout <<"number: " <<tr.number << " capacity: "<< tr.capacity<< " milk type: " << tr.milk_type << "\n";
    showFarm(tr.path);
  }
}

void showPath(std::vector<truck> t){
  truck tr=t[0];
  unsigned int b=0;
  std::vector<farm> pa =std::vector<farm>();
  pa=t[0].path;
  for(unsigned int i=0; i<t.size();i++){
    tr=t.at(i);
    std::cout <<"number: " <<tr.number << " capacity: "<< tr.capacity<< " milk type: " << tr.milk_type <<"   final milk:" << finalQMilk(tr.path)<< "\n";
    pa=tr.path;
    for(b=0; b<pa.size();b++){
      std::cout <<"id: " <<pa[b].id << " milk type: "<< pa[b].milk_type<< " quantity: " << pa[b].q_milk << "\n";
    }
  }

}

void showMilk(std::vector<milk> m){
  milk mi=m[0];
  for(unsigned int i=0; i<m.size();i++){
    mi=m.at(i);
    std::cout <<"milk type: " <<mi.milk_type << " value: "<< mi.value<< " quota: " << mi.quota <<"\n";
  }
}

void showClusters(clusters kclust){
  std::vector<cluster> clus =std::vector<cluster>();
  clus=kclust.clu;
  cluster c=clus[0];
  showFarm(kclust.graph);
  for(unsigned int i=0; i<clus.size();i++){
      c=clus.at(i);
      std::cout <<"x: " <<c.x_axe << " y: "<< c.y_axe<< " capacity: " << c.capacity <<"\n";
  }
}

void showCluster(std::vector<cluster> clus){
  cluster c=clus[0];
  std::vector<farm> f =std::vector<farm>();
  for(unsigned int i=0; i<clus.size();i++){
      c=clus.at(i);
      std::cout <<"x: " <<c.x_axe << " y: "<< c.y_axe<< " capacity: " << c.capacity <<"\n";
      showFarm(c.f);
  }
}



/////////////////////////////////
////////
//SOME HELP
////////////////////
///CHANGE
bool theres(float val,  std::vector<farm> p){
  for(unsigned int i=0; i<p.size();i++){
    if(val<=p[i].milk_val){
      return true;
    }
  }
  return false;
}

int theresquant(float val,  std::vector<farm> p){
  int c=0;
  c=0;
  for(unsigned int i=0; i<p.size();i++){
    if(val<=p[i].milk_val){
      c+=1;
    }
  }
  return c;
}

int firstdifferent(float val,  std::vector<farm> p){
  for(unsigned int i=0; i<p.size();i++){
    if(val>p[i].milk_val){
      return i;
    }
  }
}

int firstgood(float val,  std::vector<farm> p){
  for(unsigned int i=0; i<p.size();i++){
    if(val<=p[i].milk_val){
      return i;
    }
  }
}
//Change to trucks
std::vector<truck> change2(std::vector<truck> trucks, int val){
  std::vector<farm> p1,p2=std::vector<farm>();
  farm farmAux=farm(0,0,0,'a',0,0,0);
  int tr1,tr2,node1,node2;
  std::mt19937 gen(std::rand());
  std::uniform_int_distribution<int> dis(0, 10);
  dis = std::uniform_int_distribution<int>(0, trucks.size()-1);
  tr1=dis(gen);
  tr2=dis(gen);
  while(tr1==tr2 && theres(val,trucks[tr1].path) && theres(val,trucks[tr2].path)){
    tr2=dis(gen);
  }
  if(theresquant(val,trucks[tr1].path)>theresquant(val,trucks[tr2].path) && theresquant(val,trucks[tr1].path)!=trucks[tr1].path.size()){
    node1=firstdifferent(val,trucks[tr1].path);
    node2=firstgood(val,trucks[tr2].path);
  }
  if(theresquant(val,trucks[tr2].path)>=theresquant(val,trucks[tr1].path) && theresquant(val,trucks[tr2].path)!=trucks[tr2].path.size()){
    node1=firstdifferent(val,trucks[tr2].path);
    node2=firstgood(val,trucks[tr1].path);
  }
  if(theresquant(val,trucks[tr1].path)==trucks[tr1].path.size()&& theresquant(val,trucks[tr2].path)==trucks[tr2].path.size()){
    return trucks;
  }
  p1=trucks[tr1].path;
  p2=trucks[tr2].path;
  farmAux=p1[node1];
  p1[node1]=p2[node2];
  p2[node2]=farmAux;
  trucks[tr1].path=p1;
  trucks[tr2].path=p2;
  return trucks;
}

std::vector<truck> change1(std::vector<truck> trucks){
  std::vector<farm> p1,p2=std::vector<farm>();
  farm farmAux=farm(0,0,0,'a',0,0,0);
  int tr1,tr2,node1,node2;
  std::mt19937 gen(std::rand());
  std::uniform_int_distribution<int> dis(0, 10);
  dis = std::uniform_int_distribution<int>(0, trucks.size()-1);
  tr1=dis(gen);
  tr2=dis(gen);
  while(tr1==tr2){
    tr2=dis(gen);
  }
  if(trucks[tr1].path.size()-2==0 || trucks[tr2].path.size()-2==0){
    return trucks;
  }
  dis=std::uniform_int_distribution<int>(1, trucks[tr1].path.size()-2);
  node1=dis(gen);
  dis=std::uniform_int_distribution<int>(1, trucks[tr2].path.size()-2);
  node2=dis(gen);
  p1=trucks[tr1].path;
  p2=trucks[tr2].path;
  farmAux=p1[node1];
  p1[node1]=p2[node2];
  p2[node2]=farmAux;
  trucks[tr1].path=p1;
  trucks[tr2].path=p2;
  return trucks;
}



//Add a truck to other path.
std::vector<truck> adding2(std::vector<truck> trucks, int val){
  std::vector<farm> p1,p2=std::vector<farm>();
  farm farmAux=farm(0,0,0,'a',0,0,0);
  int tr1,tr2,node1,node2;
  std::mt19937 gen(std::rand());
  std::uniform_int_distribution<int> dis(0, 10);
  dis = std::uniform_int_distribution<int>(0, trucks.size()-1);
  tr1=dis(gen);
  tr2=dis(gen);
  while(tr1==tr2 && theres(val,trucks[tr1].path) && theres(val,trucks[tr2].path)){
    tr2=dis(gen);
  }
  if(abs(trucks[tr1].path.size()-trucks[tr2].path.size())>round(trucks[tr1].path.size()+trucks[tr2].path.size())*AD){
    return trucks;
  }
  if(theresquant(val,trucks[tr1].path)>theresquant(val,trucks[tr2].path) && theresquant(val,trucks[tr1].path)!=trucks[tr1].path.size()){
    node1=firstdifferent(val,trucks[tr1].path);
    node2=firstgood(val,trucks[tr2].path);
  }
  if(theresquant(val,trucks[tr2].path)>=theresquant(val,trucks[tr1].path) && theresquant(val,trucks[tr2].path)!=trucks[tr2].path.size()){
    node1=firstdifferent(val,trucks[tr2].path);
    node2=firstgood(val,trucks[tr1].path);
  }
  if(theresquant(val,trucks[tr1].path)==trucks[tr1].path.size()&& theresquant(val,trucks[tr2].path)==trucks[tr2].path.size()){
    return trucks;
  }

  p1=trucks[tr1].path;
  p2=trucks[tr2].path;
  p1.insert(p1.end()-1,p2[node2]);
  p2.erase(p2.begin()+node2);
  trucks[tr1].path=p1;
  trucks[tr2].path=p2;
  return trucks;
}


std::vector<truck> adding1(std::vector<truck> trucks){
  std::vector<farm> p1,p2=std::vector<farm>();
  farm farmAux=farm(0,0,0,'a',0,0,0);
  int tr1,tr2,node1,node2;
  std::mt19937 gen(std::rand());
  std::uniform_int_distribution<int> dis(0, 10);
  dis = std::uniform_int_distribution<int>(0, trucks.size()-1);
  tr1=dis(gen);
  tr2=dis(gen);
  while(tr1==tr2){
    tr2=dis(gen);
  }
  if(abs(trucks[tr1].path.size()-trucks[tr2].path.size())>round(trucks[tr1].path.size()+trucks[tr2].path.size())*AD){
    return trucks;
  }
  dis=std::uniform_int_distribution<int>(1, trucks[tr1].path.size()-2);
  node1=dis(gen);
  dis=std::uniform_int_distribution<int>(1, trucks[tr2].path.size()-2);
  node2=dis(gen);
  p1=trucks[tr1].path;
  p2=trucks[tr2].path;
  p1.insert(p1.end()-1,p2[node2]);
  p2.erase(p2.begin()+node2);
  trucks[tr1].path=p1;
  trucks[tr2].path=p2;
  return trucks;
}


////2OPT

std::vector<farm> Opt(std::vector<farm> f){
  int n1,n2=0;
  std::vector<farm> finalAux=std::vector<farm>();
  finalAux=f;
  n1=0;
  n2=0;
  farm f1=farm(0,0,0,'a',0,0,0);
  farm f2=f1;
  std::mt19937 gen2(std::rand());
  std::uniform_int_distribution<int> dis2(1, f.size()-2);
  if(f.size()>4){
    while(abs(n1-n2)<2){
      n1=dis2(gen2);
      n2=dis2(gen2);
    }
    if(n1<n2){
      std::reverse(finalAux.begin()+n1,finalAux.begin()+n2);
      f=finalAux;
    }
    else{
      std::reverse(std::begin(finalAux)+n2,std::begin(finalAux)+n1);
      f=finalAux;
    }
  }
  return f;
}

std::vector<truck> OPTtrucks(std::vector<truck> trucks){
  std::mt19937 gen(std::rand());
  std::uniform_int_distribution<int> dis(0, trucks.size()-1);
  std::vector<truck> tsAux=trucks;
  int n=dis(gen);
  tsAux[n].path=Opt(trucks[n].path);
  return tsAux;
}
////PENALTY
float penal(farm f,cluster c){
  if(c.capacity-f.q_milk<0){
    return P*pow(f.q_milk-c.capacity,C);
  }
  return 0;
}

float penal2(farm f,cluster c){
  if(c.milk_val!=f.milk_val){
    return N*(1-pow(c.milk_val-f.milk_val,NN));
  }
  return 0;
}

////CENTROID CALCULATION
std::vector<float> centr(cluster cl,farm f){
  std::vector<float> cord = std::vector<float>();
  std::vector<farm> nodes = std::vector<farm>();
  float x,y,n=0;
  nodes=cl.f;
  n=(float)nodes.size();
  x=((cl.x_axe+(f.x_axe/n))*(n/n+1.0));
  y=((cl.y_axe+(f.y_axe/n))*(n/n+1.0));
  cord.push_back(x);
  cord.push_back(y);
  return cord;
}

///CLUSTERS

std::vector<cluster> kMeans(clusters kclust){
  std::vector<farm> f = std::vector<farm>();
  std::vector<farm> graph = kclust.graph;
  std::vector<float> cord = std::vector<float>();
  std::vector<cluster> finalCluster=kclust.clu;
  std::vector<cluster> clus=kclust.clu;
  farm node = farm(0,0,0,'a',0,0,0);
  int n,count1,count2=0;
  float dist=std::numeric_limits<float>::max();
  float dist2=std::numeric_limits<float>::max();
  float d,penalize,penalize2=0;
  for(unsigned int i=0; i<graph.size();i++){
    dist=std::numeric_limits<float>::max();
    for(unsigned int b=0; b<clus.size();b++){ // Find better cluster for this node.
        d=distanceCluster(graph[i],finalCluster[b]);
        penalize=penal(graph[i],finalCluster[b]); //Penalize for capacity exceed.
        penalize2=penal2(graph[i],finalCluster[b]); // Penalize for blending.
        d+=penalize;
        d+=penalize2;
        if(d<dist and penalize==0){ //If the capacity is not violated, we prefer this cluster.
            dist=d;
            count1=b;
            d=0;
        }
        if(d<dist2 and penalize!=0){
            dist2=d;
            count2=b;
            d=0;
        }
      }
      //Append the node to best cluster.
      if(dist==std::numeric_limits<float>::max()){ //If in all clusters the minimum capacity is violated.
          cord=centr(finalCluster[count2],graph[i]);
          finalCluster[count2].x_axe=cord[0];
          finalCluster[count2].y_axe=cord[1];
          f=finalCluster[count2].f;
          f.push_back(graph[i]);
          finalCluster[count2].f=f;
          finalCluster[count2].capacity-=graph[i].q_milk;
      }
      else{ // Add to the best cluster without exceed the capacity.
          cord=centr(finalCluster[count1],graph[i]);
          finalCluster[count1].x_axe=cord[0];
          finalCluster[count1].y_axe=cord[1];
          f=finalCluster[count1].f;
          f.push_back(graph[i]);
          finalCluster[count1].f=f;
          finalCluster[count1].capacity-=graph[i].q_milk;
    }
  }
  return finalCluster;
}


clusters createClusters(std::vector<farm> graph,int num,int cap){
  std::vector<int> ids=std::vector<int>();
  std::mt19937 gen(std::rand());
  std::uniform_int_distribution<int> dis(1, graph.size()-1);
  std::vector<cluster> clust=std::vector<cluster>();
  std::vector<farm> fa=std::vector<farm>();
  cluster clu=cluster(fa,0,0,0,0,0,0,0,0);
  clusters Kclust=clusters(clust,graph);
  std::vector<farm> newGraph=graph;
  int i,c,m=0;
  c=0;
  while(c<num){
    i=dis(gen);
    if(std::find(ids.begin(), ids.end(), newGraph[i].id)==ids.end()){
      fa=std::vector<farm>();
      clu=cluster(fa,0,0,0,0,0,0,0,0);
      fa.push_back(newGraph[i]);
      clu.f=fa;
      clu.x_axe=(float)newGraph[i].x_axe;
      clu.y_axe=(float)newGraph[i].y_axe;
      clu.initx=(float)newGraph[i].x_axe;
      clu.inity=(float)newGraph[i].y_axe;
      clu.milk_val=newGraph[i].milk_val;
      clu.capacity=cap;
      clust.push_back(clu);
      c+=1;
      ids.push_back(newGraph[i].id);
      newGraph.erase(newGraph.begin() + i);
      dis = std::uniform_int_distribution<int>(1, newGraph.size()-1);
    }
  }
  newGraph.erase(newGraph.begin());
  Kclust.clu=clust;
  Kclust.graph=newGraph;
  c=0;
  return Kclust;

}
///RESTRICTION
///////

float restriction(std::vector<truck> trucks,std::vector<milk> milks){
  float d,q,total,valu,val,aux=0.0;
  valu=0;
  std::vector<farm> Tpath = std::vector<farm>();
  truck t = truck(0,0,0,Tpath);
  char type;
  char finale=0;
  std::vector<farm> f =std::vector<farm>();
  for(unsigned int i=0; i<trucks.size();i++){
    t=trucks.at(i);
    f=t.path;
    q=(float)finalQMilk(f);
    type=finalMilk(f);
    for(unsigned int b=0;b<milks.size();b++){
        if(milks[b].milk_type==type){
          milks[b].quota-=q;
        }
    }
  }
  for(unsigned int b=0;b<milks.size();b++){
      if(milks[b].quota>0){
        if(milks[b].value>valu){
          finale=milks[b].value;
          val=milks[b].value;
        }
      }
  }
  return finale;
}


/////

std::vector<truck> addfails(std::vector<truck> trucks,farm f,std::vector<milk> milks){
  std::vector<farm> fa=std::vector<farm>();
  std::vector<truck> tru=trucks;
  truck tr = truck(0,0,0,fa);
  for(unsigned int i=0; i<trucks.size();i++){
    tr=trucks.at(i);
    if(tr.capacity>=f.q_milk){
      fa=tr.path;
      f.take=1;
      fa.insert( fa.end() -1, f);
      tr.capacity=tr.capacity-f.q_milk;
      tr.path=fa;
      tru[i]=tr;
      tru[i].milk_type=finalMilk(tru[i].path);
      return tru;
    }
  }
}

////////////////
///////


/////
//CREATING SOLUTIONS
///////////////////

std::vector<truck> createPath(farm start,std::vector<truck> trucks,std::vector<cluster> clust,std::vector<milk> milks){
  int n,c=0;
  float val;
  char type;
  farm fr=farm(0,0,0,'a',0,0,0);
  farm frAux=farm(0,0,0,'a',0,0,0);
  std::vector<farm> fails=std::vector<farm>();
  std::vector<truck> finish=std::vector<truck>();
  std::vector<farm> farms=std::vector<farm>();
  cluster cl=cluster(fails,0,0,0,0,0,0,0,0);
  truck truckAux=truck(0,0,0,farms);
  farm pro=start;
  int flag=0;
  for(unsigned int i=0; i<clust.size();i++){
    cl=clust.at(i);
    farms=cl.f;
    for(unsigned int b=0; b<farms.size();b++){
      fr=farms.at(b);
      if(b==0){
        trucks[i].path.push_back(pro);
        flag=1;
      }
      frAux=fr;
      frAux.take=1;
      truckAux=trucks[i];
      truckAux.path.push_back(frAux);
      type=finalMilk(truckAux.path);
      val=valmilk(type,milks);
      if(distance(fr,truckAux.path[truckAux.path.size()-2])<val*fr.q_milk){
        if(trucks[i].capacity>=fr.q_milk){
          trucks[i].capacity=trucks[i].capacity-fr.q_milk;
          fr.take=1;
          trucks[i].path.push_back(fr);
        }
        else{
          fails.push_back(fr);
        }
      }
      else{
        fails.push_back(fr);
      }
    }
    trucks[i].path.push_back(pro);
    trucks[i].milk_type=finalMilk(trucks[i].path);
  }
  for(unsigned int i=0;i<fails.size();i++){
    finish=addfails(trucks,fails[i],milks);
    trucks=finish;
  }
  type='z';
  return trucks;
}

///////
///////////////////
////


/////////
//EVALUATION FUNCTION
//////////////////

float evfunct(std::vector<truck> trucks,std::vector<milk> milks){
  float d,q,total,val,aux=0.0;
  total=0;
  std::vector<farm> Tpath = std::vector<farm>();
  truck t = truck(0,0,0,Tpath);
  char type;
  std::vector<farm> f =std::vector<farm>();
  for(unsigned int i=0; i<trucks.size();i++){
    t=trucks.at(i);
    f=t.path;
    d=(float)finalDistance(f);
    q=(float)finalQMilk(f);
    type=finalMilk(f);
    val=valmilk(type,milks);
    //std::cout<<"Valores "<<q*val<<"\n";
    aux=q*val-d;
    //std::cout<<"Valores2 "<<d<<"\n";
    total+=aux;
  }
  return total;
}

/////////
///DELETE
/////////////////
std::vector<farm> deleting(std::vector<farm> p){
  int nu=0;
  std::vector<farm> farmAux =std::vector<farm>();
  std::mt19937 gen(std::rand());
  std::uniform_int_distribution<int> dis(0, 10);
  dis = std::uniform_int_distribution<int>(1, p.size()-2);
  nu=dis(gen);
  farmAux=p;
  farmAux.erase(farmAux.begin()+nu);
  return farmAux;
}

std::vector<truck> deletingTruck(std::vector<truck> trucks,std::vector<milk> milks,int Nodes){
  std::vector<farm> p1,p2=std::vector<farm>();
  std::vector<truck> trucksAux=std::vector<truck>();
  farm farmAux=farm(0,0,0,'a',0,0,0);
  int tr1;
  std::mt19937 gen(std::rand());
  std::uniform_int_distribution<int> dis(0, 10);
  dis = std::uniform_int_distribution<int>(0, trucks.size()-1);
  tr1=dis(gen);
  trucksAux=trucks;
  if(trucks[tr1].path.size()-1<round(Nodes*0.3)){
    return trucks;
  }
  trucksAux[tr1].path=deleting(trucks[tr1].path);
  if(evfunct(trucksAux,milks)>evfunct(trucks,milks)){
    return trucksAux;
  }
  return trucks;
}



//////////////
//// WOLVES

std::vector<wolf> Allwolves(int n,std::vector<farm> graph,int nT,int minor,std::vector<milk> milks,std::vector<truck> trucks){
  std::vector<cluster> newcluster = std::vector<cluster>();
  std::vector<wolf> wolves=std::vector<wolf>();
  std::vector<truck> trucksAux=std::vector<truck>();
  trucksAux=trucks;
  wolf w=wolf(trucks,0);
  clusters Kclust=clusters(newcluster,graph);
  for(unsigned int i=0;i<n;i++){
    Kclust=createClusters(graph,nT,minor);
    newcluster=kMeans(Kclust);
    trucksAux=createPath(graph[0],trucks,newcluster,milks);
    w.trucks=trucksAux;
    w.ef=evfunct(trucksAux,milks);
    wolves.push_back(w);
  }
  return wolves;
}

///////////////////////////////
////
//MAIN
///////////////////////

int main(int args, char **argv) {
  int seed= atoi(argv[3]);
  std::srand(static_cast<unsigned int>(seed));
  std::vector<farm> graph = std::vector<farm>();
  std::vector<cluster> newcluster = std::vector<cluster>();
  std::vector<farm> path = std::vector<farm>();
  std::vector<truck> trucks = std::vector<truck>();
  std::vector<truck> emptyTrucks = std::vector<truck>();
  std::vector<truck> bestTrucks = std::vector<truck>();
  std::vector<milk> milks = std::vector<milk>();
  std::vector<farm> Tpath = std::vector<farm>();
  int minor=std::numeric_limits<int>::max();
  float best=-std::numeric_limits<float>::max();
  truck ntruck=truck(0,0,0,Tpath);
  milk nmilk=milk(0,0,0);
  farm fr=farm(0,0,0,'a',0,0,0);
  std::ifstream arch (argv[1], std::ifstream::in);
  int nT,tM,nF,id,x,y,quantity,s,equals,su=0;
  bool condition=false;
  std::string abc="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char type='a';
  float vM=0;
  int c=1,valu=0;
  bool criteria,find=false;
  const clock_t begin_time = clock();
   std::string Name=argv[2];
  arch>>nT;
  while(c<=nT && arch >> s){
    ntruck=truck(c,s,0,Tpath);
    trucks.push_back(ntruck);
    c+=1;
    if(s<minor){
      minor=s;
    }
  }
  arch >> tM;
  c=0;
  while(c<tM && arch >> s){
    nmilk=milk(abc[c],0,s);
    milks.push_back(nmilk);
    c+=1;
  }
  c=0;
  while(c<tM && arch >> vM){
    milks[c].value=vM;
    c+=1;
  }
  arch >> nF;
  while(arch >> id >> x >> y >> type >> quantity){
    fr=farm(id,x,y,type,quantity,0,valmilk(type,milks));
    graph.push_back(fr);
  }
  //WSA
  std::mt19937 gen(std::rand());
  std::uniform_real_distribution<> dis(0.0, 1.0);
  bestTrucks=trucks;
  emptyTrucks=trucks;
  //Initialize
  std::vector<wolf> wolves=Allwolves(Q*nF,graph,nT,minor,milks,trucks);
  while(criteria==false){
    for(int i=0;i<wolves.size();i++){
        //Search
        if(dis(gen)>O){
          trucks=OPTtrucks(wolves[i].trucks);
        }
        else{
          trucks=deletingTruck(wolves[i].trucks,milks,nF);
        }
        if(evfunct(trucks,milks)>wolves[i].ef){
        //Move to position
          wolves[i].trucks=trucks;
          if(restriction(wolves[i].trucks,milks)==0){
            wolves[i].ef=evfunct(trucks,milks);
          }
          else{
            wolves[i].c+=1;
          }
        }
        if(best<wolves[i].ef && restriction(wolves[i].trucks,milks)==0){
          // Save best solution.
          best=wolves[i].ef;
          bestTrucks=wolves[i].trucks;
          find=true;
          c=0;
          wolves[i].c=0;
        }
        else{
          c+=1;
          if(c>T and find==true){
            criteria=true;
          }
        //Scape
        if(dis(gen)>SCAP){
          if(dis(gen)>S1){
            wolves[i].trucks=change1(wolves[i].trucks);
          }
          else{
            wolves[i].trucks=adding1(wolves[i].trucks);
          }
        }
        }
      if(wolves[i].c>TW and find==false){
        valu=restriction(wolves[i].trucks,milks);
        if(dis(gen)>R1){
          wolves[i].trucks=change2(wolves[i].trucks,valu);
        }
        else{
          wolves[i].trucks=adding2(wolves[i].trucks,valu);
        }
      }
    }
  }
  //showPath(bestTrucks);
  vM=evfunct(bestTrucks,milks);
  //printf("%f\n",vM);
  ofstream file;
  file.open(Name+"value.txt", fstream::app);
  file << std::to_string(vM)+", ";
  file.close();
  file.open(Name+"time.txt",fstream::app);
  file << std::to_string(float( clock () - begin_time ) /  CLOCKS_PER_SEC)+", ";
  file.close();
}
